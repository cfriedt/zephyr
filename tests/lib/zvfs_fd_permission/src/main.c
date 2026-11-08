/*
 * Copyright (c) 2026 Eleven October
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/fdtable.h>
#include <zephyr/sys/zvfs.h>
#include <zephyr/ztest.h>
#include <zephyr/ztest_error_hook.h>

#define ATTACKER_STACK_SIZE 4096

enum operation {
	OP_POLL,
	OP_READ,
	OP_WRITE,
	OP_CLOSE,
	OP_IOCTL,
	OP_LSEEK,
	OP_FSTAT,
	OP_FSYNC,
	OP_FTRUNCATE,
	OP_DUP,
	OP_DUP2,
	OP_FCNTL,
};

enum reuse_operation {
	REUSE_POLL,
	REUSE_DUP,
	REUSE_DUP2,
};

struct backend {
	unsigned int calls;
};

struct attempt {
	int started;
	int returned;
	int rc;
	int duplicate_fd;
	short revents;
};

struct reuse_request {
	int fd;
	int operation;
	int newfd;
	atomic_t sequence;
	atomic_t completed;
	int stop;
};

static struct backend source;
static struct backend destination;
static struct k_thread attacker_thread;
static struct k_thread reuse_thread;
static struct k_thread new_owner_thread;
static K_THREAD_STACK_DEFINE(attacker_stack, ATTACKER_STACK_SIZE);
static K_THREAD_STACK_DEFINE(reuse_stack, ATTACKER_STACK_SIZE);
static K_THREAD_STACK_DEFINE(new_owner_stack, ATTACKER_STACK_SIZE);
ZTEST_BMEM static struct attempt attempt;
ZTEST_BMEM static struct reuse_request reuse_request;

static ssize_t backend_read(void *obj, void *buf, size_t size)
{
	struct backend *backend = obj;

	ARG_UNUSED(buf);
	backend->calls++;
	return (ssize_t)size;
}

static ssize_t backend_write(void *obj, const void *buf, size_t size)
{
	struct backend *backend = obj;

	ARG_UNUSED(buf);
	backend->calls++;
	return (ssize_t)size;
}

static int backend_close(void *obj)
{
	struct backend *backend = obj;

	backend->calls++;
	return 0;
}

static int backend_ioctl(void *obj, unsigned int request, va_list args)
{
	struct backend *backend = obj;
	int rc = 0;

	backend->calls++;
	TOOLCHAIN_DISABLE_WARNING(TOOLCHAIN_WARNING_DEPRECATED_DECLARATIONS)
	switch (request) {
	case ZFD_IOCTL_POLL_PREPARE:
		ARG_UNUSED(va_arg(args, struct zvfs_pollfd *));
		ARG_UNUSED(va_arg(args, struct k_poll_event **));
		ARG_UNUSED(va_arg(args, struct k_poll_event *));
		rc = -EALREADY;
		break;
	case ZFD_IOCTL_POLL_UPDATE: {
		struct zvfs_pollfd *pfd = va_arg(args, struct zvfs_pollfd *);

		ARG_UNUSED(va_arg(args, struct k_poll_event **));
		pfd->revents = pfd->events & ZVFS_POLLIN;
		break;
	}
	default:
		break;
	}
	TOOLCHAIN_ENABLE_WARNING(TOOLCHAIN_WARNING_DEPRECATED_DECLARATIONS)

	return rc;
}

static const struct fd_op_vtable backend_vtable = {
	.read = backend_read,
	.write = backend_write,
	.close = backend_close,
	.ioctl = backend_ioctl,
};

static unsigned int backend_calls(void)
{
	return source.calls + destination.calls;
}

static int ioctl_call(int fd, unsigned long request, ...)
{
	va_list args;
	int rc;

	va_start(args, request);
	rc = zvfs_ioctl(fd, request, args);
	va_end(args);

	return rc;
}

static void attacker(void *p1, void *p2, void *p3)
{
	int fd = POINTER_TO_INT(p1);
	enum operation operation = POINTER_TO_INT(p2);
	int other_fd = POINTER_TO_INT(p3);
	char buffer[4] = { 0 };
	struct zvfs_pollfd pfd = {
		.fd = fd,
		.events = ZVFS_POLLIN,
	};
	struct zvfs_stat stat;
	int value;
	int rc = -EINVAL;

	ARG_UNUSED(other_fd);
	attempt.started = 1;
	ztest_set_fault_valid(true);
	switch (operation) {
	case OP_POLL:
		rc = zvfs_poll(&pfd, 1, 0);
		attempt.revents = pfd.revents;
		break;
	case OP_READ:
		rc = (int)zvfs_read(fd, buffer, sizeof(buffer));
		break;
	case OP_WRITE:
		rc = (int)zvfs_write(fd, buffer, sizeof(buffer));
		break;
	case OP_CLOSE:
		rc = zvfs_close(fd);
		break;
	case OP_IOCTL:
		rc = ioctl_call(fd, ZFD_IOCTL_FIONREAD, &value);
		break;
	case OP_LSEEK:
		rc = (int)zvfs_lseek(fd, 0, ZVFS_SEEK_SET);
		break;
	case OP_FSTAT:
		rc = zvfs_fstat(fd, &stat);
		break;
	case OP_FSYNC:
		rc = zvfs_fsync(fd);
		break;
	case OP_FTRUNCATE:
		rc = zvfs_ftruncate(fd, 0);
		break;
	case OP_DUP:
		rc = zvfs_dup(fd, 0);
		break;
	case OP_DUP2:
		rc = zvfs_dup2(fd, other_fd);
		break;
	case OP_FCNTL:
		rc = zvfs_fcntl(fd, ZVFS_F_SETFD, ZVFS_FD_CLOEXEC);
		break;
	}
	attempt.rc = rc;
	attempt.returned = 1;
}

static void expect_denied(int fd, int other_fd, enum operation operation)
{
	unsigned int calls_before = backend_calls();
	int join_rc;

	attempt.started = 0;
	attempt.returned = 0;
	attempt.rc = -999;
	attempt.revents = 0;
	k_thread_create(&attacker_thread, attacker_stack,
			K_THREAD_STACK_SIZEOF(attacker_stack), attacker,
			INT_TO_POINTER(fd), INT_TO_POINTER(operation),
			INT_TO_POINTER(other_fd), K_PRIO_PREEMPT(1), K_USER, K_NO_WAIT);
	join_rc = k_thread_join(&attacker_thread, K_SECONDS(5));
	zassert_equal(join_rc, 0, "operation %d thread did not terminate", operation);
	zassert_equal(attempt.started, 1, "operation %d did not start", operation);
	zassert_equal(attempt.returned, 0, "unauthorized operation %d returned %d",
		      operation, attempt.rc);
	zassert_equal(backend_calls(), calls_before,
		      "unauthorized operation %d reached the backend", operation);
}

static void reuse_worker(void *p1, void *p2, void *p3)
{
	int last_sequence = 0;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	while (true) {
		int sequence = atomic_get(&reuse_request.sequence);
		struct zvfs_pollfd pfd = {
			.fd = reuse_request.fd,
			.events = ZVFS_POLLIN,
		};

		if (sequence == last_sequence) {
			k_yield();
			continue;
		}
		last_sequence = sequence;
		if (reuse_request.stop) {
			return;
		}

		attempt.started = 1;
		ztest_set_fault_valid(true);
		attempt.duplicate_fd = reuse_request.fd;
		switch (reuse_request.operation) {
		case REUSE_DUP:
			attempt.duplicate_fd = zvfs_dup(reuse_request.fd, 0);
			break;
		case REUSE_DUP2:
			attempt.duplicate_fd = zvfs_dup2(reuse_request.fd, reuse_request.newfd);
			break;
		default:
			break;
		}
		pfd.fd = attempt.duplicate_fd;
		attempt.rc = zvfs_poll(&pfd, 1, 0);
		attempt.revents = pfd.revents;
		attempt.returned = 1;
		atomic_set(&reuse_request.completed, sequence);
	}
}

ZTEST_SUITE(zvfs_fd_permission, NULL, NULL, NULL, NULL, NULL);

ZTEST(zvfs_fd_permission, test_ungranted_fd_syscalls_are_denied)
{
	static const enum operation operations[] = {
		OP_POLL, OP_READ, OP_WRITE, OP_CLOSE, OP_IOCTL, OP_LSEEK,
		OP_FSTAT, OP_FSYNC, OP_FTRUNCATE, OP_DUP, OP_DUP2, OP_FCNTL,
	};
	char buffer[4] = { 0 };
	struct zvfs_pollfd pfd;
	int fd;
	int other_fd;
	int rc;
	unsigned int i;
	unsigned int calls_before;

	source.calls = 0;
	destination.calls = 0;
	fd = zvfs_alloc_fd(&source, &backend_vtable);
	other_fd = zvfs_alloc_fd(&destination, &backend_vtable);
	zassert_true(fd >= 0 && other_fd >= 0, "fd allocation failed");
	zassert_equal(zvfs_read(fd, buffer, sizeof(buffer)), sizeof(buffer),
		      "owner read failed");
	zassert_equal(zvfs_write(fd, buffer, sizeof(buffer)), sizeof(buffer),
		      "owner write failed");
	pfd = (struct zvfs_pollfd){ .fd = fd, .events = ZVFS_POLLIN };
	rc = zvfs_poll(&pfd, 1, 0);
	zassert_equal(rc, 1, "owner poll did not report ready");
	zassert_equal(pfd.revents, ZVFS_POLLIN, "owner readiness mismatch");
	calls_before = backend_calls();

	for (i = 0; i < ARRAY_SIZE(operations); i++) {
		expect_denied(fd, other_fd, operations[i]);
	}

	pfd = (struct zvfs_pollfd){ .fd = fd, .events = ZVFS_POLLIN };
	zassert_equal(zvfs_poll(&pfd, 1, 0), 1, "owner poll after attacks failed");
	zassert_equal(pfd.revents, ZVFS_POLLIN, "descriptor was changed by an attack");

	zvfs_free_fd(other_fd);
	zvfs_free_fd(fd);
}

ZTEST(zvfs_fd_permission, test_grant_dup_exhaustion_reuse)
{
	struct backend replacement = { 0 };
	struct backend filler = { 0 };
	void *kobj;
	void *target_kobj;
	int fd;
	int target_fd;
	int dup_fd;
	int dup2_fd;
	int filler_fd;
	int reused_fd;
	int join_rc;

	source.calls = 0;
	destination.calls = 0;
	fd = zvfs_alloc_fd(&source, &backend_vtable);
	target_fd = zvfs_alloc_fd(&destination, &backend_vtable);
	zassert_true(fd >= 0 && target_fd >= 0, "fd allocation failed");
	kobj = zvfs_test_fd_entry_kobj(fd);
	target_kobj = zvfs_test_fd_entry_kobj(target_fd);
	zassert_not_null(kobj, "fd has no kernel object");
	zassert_not_null(target_kobj, "dup2 target has no kernel object");
	reuse_request.fd = 0;
	reuse_request.operation = REUSE_POLL;
	reuse_request.newfd = 0;
	atomic_set(&reuse_request.sequence, 0);
	atomic_set(&reuse_request.completed, 0);
	reuse_request.stop = 0;
	attempt.started = 0;
	attempt.returned = 0;
	attempt.rc = -999;
	attempt.revents = 0;
	k_thread_create(&reuse_thread, reuse_stack, K_THREAD_STACK_SIZEOF(reuse_stack),
			reuse_worker, NULL, NULL, NULL, K_PRIO_PREEMPT(1), K_USER,
			K_NO_WAIT);
	k_object_access_grant(kobj, &reuse_thread);
	k_object_access_grant(target_kobj, &reuse_thread);
	reuse_request.fd = fd;
	atomic_set(&reuse_request.sequence, 1);
	for (int i = 0; i < 5000 && atomic_get(&reuse_request.completed) != 1; i++) {
		k_sleep(K_MSEC(1));
	}
	zassert_equal(atomic_get(&reuse_request.completed), 1, "granted thread did not run");
	zassert_equal(attempt.returned, 1, "granted poll did not return");
	zassert_equal(attempt.rc, 1, "granted poll did not report readiness");
	zassert_equal(attempt.revents, ZVFS_POLLIN, "granted readiness mismatch");

	attempt.returned = 0;
	attempt.duplicate_fd = -1;
	reuse_request.operation = REUSE_DUP;
	atomic_set(&reuse_request.sequence, 2);
	for (int i = 0; i < 5000 && atomic_get(&reuse_request.completed) != 2; i++) {
		k_sleep(K_MSEC(1));
	}
	zassert_equal(atomic_get(&reuse_request.completed), 2, "granted dup did not return");
	zassert_true(attempt.duplicate_fd >= 0, "granted dup failed");
	zassert_equal(attempt.rc, 1, "grantee could not poll duplicated descriptor");
	zassert_equal(attempt.revents, ZVFS_POLLIN, "duplicated descriptor readiness mismatch");
	dup_fd = attempt.duplicate_fd;

	attempt.returned = 0;
	attempt.duplicate_fd = -1;
	reuse_request.operation = REUSE_DUP2;
	reuse_request.newfd = target_fd;
	atomic_set(&reuse_request.sequence, 3);
	for (int i = 0; i < 5000 && atomic_get(&reuse_request.completed) != 3; i++) {
		k_sleep(K_MSEC(1));
	}
	zassert_equal(atomic_get(&reuse_request.completed), 3, "granted dup2 did not return");
	zassert_equal(attempt.duplicate_fd, target_fd, "dup2 returned the wrong descriptor");
	zassert_equal(attempt.rc, 1, "grantee could not poll dup2 descriptor");
	zassert_equal(attempt.revents, ZVFS_POLLIN, "dup2 descriptor readiness mismatch");
	dup2_fd = attempt.duplicate_fd;
	filler_fd = zvfs_alloc_fd(&filler, &backend_vtable);
	zassert_true(filler_fd >= 0, "filler fd allocation failed");
	zassert_equal(zvfs_alloc_fd(&replacement, &backend_vtable), -1,
		      "fd table did not report exhaustion");
	expect_denied(fd, target_fd, OP_POLL);
	expect_denied(dup_fd, dup2_fd, OP_POLL);
	expect_denied(dup2_fd, dup_fd, OP_POLL);

	zvfs_free_fd(fd);
	reused_fd = zvfs_alloc_fd(&replacement, &backend_vtable);
	zassert_equal(reused_fd, fd, "descriptor slot was not reused");
	attempt.started = 0;
	attempt.returned = 0;
	attempt.rc = -999;
	attempt.revents = 0;
	reuse_request.fd = reused_fd;
	reuse_request.operation = REUSE_POLL;
	atomic_set(&reuse_request.sequence, 4);
	join_rc = k_thread_join(&reuse_thread, K_SECONDS(2));
	if (join_rc != 0) {
		reuse_request.stop = 1;
		atomic_set(&reuse_request.sequence, 5);
		(void)k_thread_join(&reuse_thread, K_SECONDS(2));
	}
	zassert_equal(join_rc, 0, "stale grant survived descriptor reuse");
	zassert_equal(attempt.started, 1, "reused descriptor probe did not start");
	zassert_equal(attempt.returned, 0, "reused descriptor was accessible to stale owner");
	zvfs_free_fd(filler_fd);
	zvfs_free_fd(dup2_fd);
	zvfs_free_fd(dup_fd);

	kobj = zvfs_test_fd_entry_kobj(reused_fd);
	zassert_not_null(kobj, "reused fd has no kernel object");
	reuse_request.fd = reused_fd;
	reuse_request.operation = REUSE_POLL;
	reuse_request.stop = 0;
	atomic_set(&reuse_request.sequence, 0);
	atomic_set(&reuse_request.completed, 0);
	attempt.started = 0;
	attempt.returned = 0;
	attempt.rc = -999;
	attempt.revents = 0;
	k_thread_create(&new_owner_thread, new_owner_stack,
			K_THREAD_STACK_SIZEOF(new_owner_stack), reuse_worker,
			NULL, NULL, NULL, K_PRIO_PREEMPT(1), K_USER, K_NO_WAIT);
	k_object_access_grant(kobj, &new_owner_thread);
	atomic_set(&reuse_request.sequence, 1);
	for (int i = 0; i < 5000 && atomic_get(&reuse_request.completed) != 1; i++) {
		k_sleep(K_MSEC(1));
	}
	zassert_equal(atomic_get(&reuse_request.completed), 1, "new owner thread did not run");
	zassert_equal(attempt.returned, 1, "new owner could not use reused descriptor");
	zassert_equal(attempt.rc, 1, "new owner poll did not report readiness");
	zassert_equal(attempt.revents, ZVFS_POLLIN, "new owner readiness mismatch");
	reuse_request.stop = 1;
	atomic_set(&reuse_request.sequence, 2);
	zassert_equal(k_thread_join(&new_owner_thread, K_SECONDS(2)), 0,
		      "new owner thread did not terminate");

	zvfs_free_fd(reused_fd);
}
