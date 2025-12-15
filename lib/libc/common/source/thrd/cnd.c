/*
 * Copyright (c) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <threads.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/timeutil.h>

static inline int sys_cond_init(struct k_condvar *condvar, void *opts)
{
	return -ENOSYS;
}

static inline int sys_cond_destroy(struct k_condvar *condvar)
{
	return -ENOSYS;
}

int cnd_broadcast(cnd_t *cond)
{
	if (k_condvar_broadcast(*cond) >= 0) {
		return thrd_success;
	}

	return thrd_error;
}

void cnd_destroy(cnd_t *cond)
{
	(void)sys_cond_destroy(*cond);
}

int cnd_init(cnd_t *cond)
{
	switch (sys_cond_init(*cond, NULL)) {
	case 0:
		return thrd_success;
	case ENOMEM:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

int cnd_signal(cnd_t *cond)
{
	switch (k_condvar_signal(*cond)) {
	case 0:
		return thrd_success;
	case ENOMEM:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

int cnd_timedwait(cnd_t *restrict cond, mtx_t *restrict mtx,
		  const struct timespec *restrict time_point)
{
	struct timespec ts;
	struct timespec duration = *time_point;

	/* convert time_point to duration */
	if ((sys_clock_gettime(SYS_CLOCK_REALTIME, &ts) < 0) || !timespec_sub(&duration, &ts)) {
		return thrd_error;
	}

	switch (k_condvar_wait(*cond, *mtx, timespec_to_timeout(&duration, NULL))) {
	case 0:
		return thrd_success;
	case ETIMEDOUT:
		return thrd_timedout;
	default:
		return thrd_error;
	}
}

int cnd_wait(cnd_t *cond, mtx_t *mtx)
{
	switch (k_condvar_wait(*cond, *mtx, K_FOREVER)) {
	case 0:
		return thrd_success;
	default:
		return thrd_error;
	}
}
