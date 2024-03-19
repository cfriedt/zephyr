/*
 * Copyright (c) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <threads.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/posix/pthread.h>
#include <zephyr/posix/sched.h>

struct thrd_trampoline_arg {
	thrd_start_t func;
	void *arg;
};

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	typedef void *(*pthread_func_t)(void *arg);

	int ret;
	pthread_func_t pfunc = (pthread_func_t)func;

	ret = pthread_create(thr, NULL, pfunc, arg);
	printk("pthread_create returned %d\n", ret);

	switch (ret) {
	case 0:
		return thrd_success;
	case EAGAIN:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

int thrd_equal(thrd_t lhs, thrd_t rhs)
{
	int rv = pthread_equal(lhs, rhs);
	printk("pthread_equal returned %d\n", rv);

	return rv;
}

thrd_t thrd_current(void)
{
	return pthread_self();
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining)
{
	int rv = nanosleep(duration, remaining);
	printk("nanosleep returned %d\n", rv);
	return rv;
}

void thrd_yield(void)
{
	(void)sched_yield();
}

FUNC_NORETURN void thrd_exit(int res)
{
	pthread_exit(INT_TO_POINTER(res));

	CODE_UNREACHABLE;
}

int thrd_detach(thrd_t thr)
{
	int rv = pthread_detach(thr);
	printk("pthread_detach returned %d\n", rv);

	switch (rv) {
	case 0:
		return thrd_success;
	default:
		return thrd_error;
	}
}

int thrd_join(thrd_t thr, int *res)
{
	int rv;
	void *ret;

	rv = pthread_join(thr, &ret);
	printk("pthread_join returned %d\n", rv);

	switch (rv) {
	case 0:
		if (res != NULL) {
			*res = POINTER_TO_INT(ret);
		}
		return thrd_success;
	default:
		return thrd_error;
	}
}
