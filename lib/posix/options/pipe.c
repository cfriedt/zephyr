/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/sys/fdtable.h>
#include <zephyr/sys/ring_buffer.h>

#include <zephyr/kernel.h>
#include <zephyr/posix/fcntl.h>
#include <zephyr/zvfs/eventfd.h>
#include <zephyr/sys/bitarray.h>
#include <zephyr/sys/fdtable.h>

struct pipe_obj {
	struct k_poll_signal read_sig;
	struct k_poll_signal write_sig;
	struct k_spinlock lock;
	struct ring_buf buf;
	int flags;
};

typedef int (*pipe_rw_op_t)(struct pipe_obj *p, uint8_t *data, size_t size);

static ssize_t pipe_rw_op(void *obj, void *buf, size_t size, pipe_rw_op_t op);

K_MEM_SLAB_DEFINE_STATIC(pipe_slab, CONFIG_POSIX_PIPE_BUF, CONFIG_POSIX_PIPE_MAX,
			 alignof(uint32_t));
static struct pipe_obj pipes[CONFIG_POSIX_PIPE_MAX];
static const struct fd_op_vtable pipe_read_vtable;
static const struct fd_op_vtable pipe_write_vtable;

static inline bool pipe_is_blocking(struct pipe_obj *p)
{
	return (p->flags & O_NONBLOCK) == 0;
}

static int pipe_poll_prepare(struct pipe_obj *p, struct zsock_pollfd *pfd,
			     struct k_poll_event **pev, struct k_poll_event *pev_end)
{
	return zvfs_poll_prepare(pfd, pev, pev_end, &p->read_sig, &p->write_sig);
}

static int pipe_poll_update(struct pipe_obj *p, struct zsock_pollfd *pfd, struct k_poll_event **pev)
{
	return zvfs_poll_update(pfd, pev, ring_buf_size_get(&p->buf) > 0,
				ring_buf_space_get(&p->buf) > 0);
}

static int pipe_read_locked(struct pipe_obj *p, uint8_t *data, size_t size)
{
	size_t avail = ring_buf_size_get(&p->buf);

	if (avail == 0) {
		/* would block / try again */
		return -EAGAIN;
	}

	size = MIN(size, avail);
	ring_buf_get(&p->buf, data, size);
	avail = ring_buf_size_get(&p->buf);

	if (avail == 0) {
		k_poll_signal_reset(&p->read_sig);
	}

	k_poll_signal_raise(&p->write_sig, 0);

	return size;
}

static int pipe_write_locked(struct pipe_obj *p, uint8_t *data, size_t size)
{
	size_t avail = ring_buf_space_get(&p->buf);

	size = MIN(size, avail);
	ring_buf_put(&p->buf, data, size);
	avail = ring_buf_space_get(&p->buf);

	if (avail == 0) {
		k_poll_signal_reset(&p->write_sig);
	}

	k_poll_signal_raise(&p->read_sig, 0);

	return size;
}

static ssize_t pipe_read_op(void *obj, void *buf, size_t size)
{
	pipe_rw_op_t op = pipe_read_locked;

	return pipe_rw_op(obj, buf, size, op);
}

static ssize_t pipe_write_op(void *obj, const void *buf, size_t size)
{
	pipe_rw_op_t op = pipe_write_locked;

	return pipe_rw_op(obj, (void *)buf, size, op);
}

static int pipe_close_op(void *obj)
{
	struct k_mutex *lock = NULL;
	struct k_condvar *cond = NULL;
	struct pipe_obj *p = (struct pipe_obj *)obj;

	if (k_is_in_isr()) {
		errno = EWOULDBLOCK;
		return -1;
	}

	if (!z_get_obj_lock_and_cond(obj, &pipe_read_vtable, &lock, &cond)) {
		if (!z_get_obj_lock_and_cond(obj, &pipe_write_vtable, &lock, &cond)) {
			__ASSERT(false, "z_get_obj_lock_and_cond() failed");
		}
	}

	k_mutex_lock(lock, K_FOREVER);
	k_mem_slab_free(&pipe_slab, &p->buf.buffer);
	/* when closing a pipe, broadcast to all waiters */
	k_condvar_broadcast(cond);
	k_mutex_unlock(lock);

	return 0;
}

static int pipe_ioctl_op(void *obj, unsigned int request, va_list args)
{
	int ret;
	struct pipe_obj *p = (struct pipe_obj *)obj;

	K_SPINLOCK(&p->lock) {
		switch (request) {
		case F_GETFL:
			ret = p->flags;
			break;

		case F_SETFL: {
			int flags;

			flags = va_arg(args, int);
			p->flags |= flags;
			ret = 0;
		} break;

		case ZFD_IOCTL_POLL_PREPARE: {
			struct zsock_pollfd *pfd;
			struct k_poll_event **pev;
			struct k_poll_event *pev_end;

			pfd = va_arg(args, struct zsock_pollfd *);
			pev = va_arg(args, struct k_poll_event **);
			pev_end = va_arg(args, struct k_poll_event *);

			ret = pipe_poll_prepare(obj, pfd, pev, pev_end);
		} break;

		case ZFD_IOCTL_POLL_UPDATE: {
			struct zsock_pollfd *pfd;
			struct k_poll_event **pev;

			pfd = va_arg(args, struct zsock_pollfd *);
			pev = va_arg(args, struct k_poll_event **);

			ret = pipe_poll_update(obj, pfd, pev);
		} break;

		default:
			errno = EOPNOTSUPP;
			ret = -1;
			break;
		}
	}

	return ret;
}

static const struct fd_op_vtable pipe_read_vtable = {
	.read = pipe_read_op,
	.close = pipe_close_op,
	.ioctl = pipe_ioctl_op,
};

static const struct fd_op_vtable pipe_write_vtable = {
	.write = pipe_write_op,
	.close = pipe_close_op,
	.ioctl = pipe_ioctl_op,
};

/* common to both pipe_read_op() and pipe_write_op() */
static ssize_t pipe_rw_op(void *obj, void *buf, size_t size, pipe_rw_op_t op)
{
	int err;
	ssize_t ret;
	k_spinlock_key_t key;
	struct pipe_obj *p = obj;
	struct k_mutex *lock = NULL;
	struct k_condvar *cond = NULL;

	if (buf == NULL) {
		errno = EFAULT;
		return -1;
	}

	key = k_spin_lock(&p->lock);

	if (!pipe_is_blocking(p)) {
		/*
		 * Handle the non-blocking case entirely within this scope
		 */
		ret = op(p, buf, size);
		if (ret < 0) {
			errno = -ret;
			ret = -1;
		}

		goto unlock_spin;
	}

	/*
	 * Handle the blocking case below
	 */
	__ASSERT_NO_MSG(pipe_is_blocking(p));

	if (k_is_in_isr()) {
		/* not covered by the man page, but necessary in Zephyr */
		errno = EWOULDBLOCK;
		ret = -1;
		goto unlock_spin;
	}

	if (z_get_obj_lock_and_cond(obj, &pipe_read_vtable, &lock, &cond)) {
		if (z_get_obj_lock_and_cond(obj, &pipe_write_vtable, &lock, &cond)) {
			__ASSERT(false, "z_get_obj_lock_and_cond() failed");
		}
	}
	__ASSERT_NO_MSG(lock != NULL);
	__ASSERT_NO_MSG(cond != NULL);

	/* do not hold a spinlock when taking a mutex */
	k_spin_unlock(&p->lock, key);
	err = k_mutex_lock(lock, K_FOREVER);
	__ASSERT(err == 0, "k_mutex_lock() failed: %d", err);

	while (true) {
		/* retake the spinlock */
		key = k_spin_lock(&p->lock);

		ret = op(p, buf, size);
		if (ret == -EAGAIN) {
			/* not an error in blocking mode. break and try again */
		} else if (ret < 0) {
			errno = -ret;
			ret = -1;
			goto unlock_mutex;
		} else {
			/* success! */
			goto unlock_mutex;
		}

		/* do not hold a spinlock when taking a mutex */
		k_spin_unlock(&p->lock, key);

		/* wait for a write or close */
		k_condvar_wait(cond, lock, K_FOREVER);
	}

unlock_mutex:
	k_spin_unlock(&p->lock, key);
	/* only wake a single waiter */
	k_condvar_signal(cond);
	k_mutex_unlock(lock);

	goto out;

unlock_spin:
	k_spin_unlock(&p->lock, key);

out:
	return ret;
}

int pipe(int fd[2])
{
	int ret;
	uint8_t *buf;
	size_t offset;
	struct pipe_obj *p;

	/* fd[0] is the read-end of the pipe */
	fd[0] = z_reserve_fd();
	/* fd[1] is the write-end of the pipe */
	fd[1] = z_reserve_fd();
	if ((fd[0] < 0) || (fd[1] < 0)) {
		errno = EMFILE;
		ret = -1;
		goto out_err;
	}

	if (k_mem_slab_alloc(&pipe_slab, (void **)&buf, K_NO_WAIT) < 0) {
		errno = ENFILE;
		ret = -1;
		goto out_err;
	}

	offset = (buf - (uint8_t *)pipe_slab.buffer) / CONFIG_POSIX_PIPE_BUF;

	p = &pipes[offset];
	*p = (struct pipe_obj){
		.read_sig = K_POLL_SIGNAL_INITIALIZER(p->read_sig),
		.write_sig = K_POLL_SIGNAL_INITIALIZER(p->write_sig),
	};

	ring_buf_init(&p->buf, CONFIG_POSIX_PIPE_BUF, buf);

	z_finalize_fd(fd[0], p, &pipe_write_vtable);
	z_finalize_fd(fd[1], p, &pipe_read_vtable);

	ret = 0;
	goto out;

out_err:
	if (fd[0] >= 0) {
		z_free_fd(fd[0]);
	}
	if (fd[1] >= 0) {
		z_free_fd(fd[1]);
	}

out:
	return ret;
}
