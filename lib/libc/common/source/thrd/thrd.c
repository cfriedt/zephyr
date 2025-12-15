/*
 * Copyright (c) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdint.h>
#include <threads.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/clock.h>

typedef void (*sys_thread_entry)(void *p1, void *p2, void *p3);

static void sys_thread_trampoline(thrd_start_t func, void *p1, void *p2)
{
	(void)k_thread_exit(func(p1));
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	k_thread_entry_t entry = (k_thread_entry_t)sys_thread_trampoline;

	switch (sys_thread_create(thr, NULL, CONFIG_DYNAMIC_THREAD_STACK_SIZE, entry, func, arg,
				  NULL, k_thread_priority_get(k_current_get()),
				  arch_is_user_context() ? K_USER : 0)) {
	case 0:
		return thrd_success;
	case -EAGAIN:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

int thrd_equal(thrd_t lhs, thrd_t rhs)
{
	return lhs == rhs;
}

thrd_t thrd_current(void)
{
	return (thrd_t)k_current_get();
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining)
{
	if (sys_clock_nanosleep(SYS_CLOCK_REALTIME, 0, duration, remaining) != 0) {
		return thrd_error;
	}

	return thrd_success;
}

void thrd_yield(void)
{
	k_yield();
}

FUNC_NORETURN void thrd_exit(int res)
{
	k_thread_exit(res);

	CODE_UNREACHABLE; /* LCOV_EXCL_LINE */
}

int thrd_detach(thrd_t thr)
{
	switch (k_thread_detach(thr)) {
	case 0:
		return thrd_success;
	default:
		return thrd_error;
	}
}

int thrd_join(thrd_t thr, int *res)
{
	intptr_t ret;

	switch (k_thread_rejoin(thr, K_FOREVER, &ret)) {
	case 0:
		if (res != NULL) {
			*res = POINTER_TO_INT(ret);
		}
		return thrd_success;
	default:
		return thrd_error;
	}
}
