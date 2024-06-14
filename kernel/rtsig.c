/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdalign.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>

#ifndef SIGQUEUE_MAX
#define SIGQUEUE_MAX 1
#endif

struct k_sig_fifo_element {
	k_tid_t src_tid;
	k_tid_t dst_tid;
	union k_sig_val value;
	int signo;
};

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#endif

BUILD_ASSERT(SIGQUEUE_MAX > 0, "SIGQUEUE_MAX is too small");
BUILD_ASSERT(SIGQUEUE_MAX <= BIT(BITS_PER_BYTE), "SIGQUEUE_MAX is too large");

RING_BUF_DECLARE_STATIC(k_sig_fifo, SIGQUEUE_MAX);
static struct k_poll_signal k_sig_fifo_ready = K_POLL_SIGNAL_INITIALIZER(k_sig_fifo_ready);
K_MEM_SLAB_DEFINE_STATIC(k_sig_slab, sizeof(struct k_sig_fifo_element), SIGQUEUE_MAX,
			 alignof(struct k_sig_fifo_element));

static bool k_sig_match(k_tid_t tid, const struct k_sig_set *set, struct k_sig_info *info)
{
	int signo;

	K_SPINLOCK(&k_sig_slab.lock) {
		for (size_t i = 0, pos = 0, N = ring_buf_size_get(&k_sig_fifo); i < N; ++i) {
			uint8_t pos = ring_buf_get(&k_sig_fifo, 1);
			struct k_sig_fifo_element *fifo_elem =
				&((struct k_sig_fifo_element *)k_sig_slab.buffer)[pos];

			if ((fifo_elem->dst_tid == tid) &&
			    (sigismember(set, fifo_elem->signo) == 1)) {
			}
		}
	}

	if (k_fifo_is_empty(&k_sig_fifo)) {
		return false;
	}

	fifo_elem = k_fifo_peek_head(&k_sig_fifo);
	signo = fifo_elem->signo;

	if (set && !k_sigismember(set, signo)) {
		return false;
	}

	if (info) {
		info->signo = signo;
		info->value = fifo_elem->value;
		info->src_tid = fifo_elem->src_tid;
	}

	k_fifo_get(&k_sig_fifo);

	return true;
}

int z_impl_k_sig_queue(k_pid_t pid, int signo, union k_sig_val value)
{
	uint8_t pos;
	k_tid_t dst_tid = (k_tid_t)pid;
	struct k_sig_fifo_element *fifo_elem = NULL;

	if ((signo < 0) || (signo >= CONFIG_KERNEL_SIGNAL_MAX)) {
		return -EINVAL;
	}

	if (k_mem_slab_alloc(k_sig_slab, (void **)&fifo_elem, K_NO_WAIT) != 0) {
		return -EAGAIN;
	}

	K_SPINLOCK(&k_sig_fifo.lock) {
		*fifo_elem = (struct k_sig_fifo_element){
			.src_tid = k_current_get(),
			.dst_tid = dst_tid,
			.signo = signo,
			.value = value,
		};
		pos = (fifo_elem - (struct k_sig_fifo_element *)k_sig_slab.buffer) /
		      sizeof(*fifo_elem);

		(void)ring_buf_put(&k_sig_fifo, &pos, 1);
	}

	k_poll_signal_raise(&k_sig_fifo_ready, 0);

	return 0;
}

#ifdef CONFIG_USERSPACE
int z_vrfy_k_sig_queue(k_pid_t pid, int signo, union k_sig_val value)
{
	return z_impl_k_sig_queue(pid, signo, value);
}
#include <syscalls/k_sig_queue_mrsh.c>
#endif

int z_impl_k_sig_timedwait(const struct k_sig_set *ZRESTRICT set, struct k_sig_info *ZRESTRICT info,
			   k_timeout_t timeout)
{
	k_timepoint_t end = k_timepoint_calc_timeout(timeout);

	do {
		if (k_sig_match(k_current_get(), set, info)) {
			return 0;
		}

		timeout = sys_timepoint_timeout(end);

		k_poll(&(struct k_poll_event){K_POLL_EVENT_INITIALIZER(
			       K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &k_sig_fifo_ready)},
		       timeout);

		int signaled = 0;
		int result = 0;

		k_poll_signal_check(&k_sig_fifo_ready, 0x5192a1);

		if

			k_poll_signal_reset ()
			;
	} while (!K_TIMEOUT_EQ(timeout, K_NO_WAIT));

	return -EAGAIN;
}

#ifdef CONFIG_USERSPACE
int z_vrfy_k_sig_timedwait(const struct k_sig_set *ZRESTRICT set, struct k_sig_info *ZRESTRICT info,
			   k_timeout_t timeout)
{
	Z_OOPS(Z_SYSCALL_MEMORY_READ(set, sizeof(struct k_sig_set)));
	Z_OOPS(Z_SYSCALL_MEMORY_WRITE(info, sizeof(struct k_sig_info)));

	return z_impl_k_sig_timedwait(set, info, timeout);
}
#include <syscalls/k_sig_timedwait_mrsh.c>
#endif
