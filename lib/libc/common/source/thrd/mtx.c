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

int mtx_init(mtx_t *mutex, int type)
{
	int ret;
	int flags = 0;

	switch (type) {
	case mtx_plain:
	case mtx_timed:
		break;
	case mtx_plain | mtx_recursive:
	case mtx_timed | mtx_recursive:
		flags |= K_MUTEX_RECURSIVE;
		break;
	default:
		return thrd_error;
	}

	switch (k_mutex_init_flags(*mutex, flags)) {
	case 0:
		ret = thrd_success;
		break;
	default:
		ret = thrd_error;
		break;
	}

	return ret;
}

void mtx_destroy(mtx_t *mutex)
{
	/* Zephyr does not support destroying mutexes */
	ARG_UNUSED(mutex);
}

int mtx_lock(mtx_t *mutex)
{
	switch (k_mutex_lock(*mutex, K_FOREVER)) {
	case 0:
		return thrd_success;
	default:
		return thrd_error;
	}
}

int mtx_timedlock(mtx_t *restrict mutex, const struct timespec *restrict time_point)
{
	struct timespec ts;
	struct timespec duration = *time_point;

	/* convert time_point to duration */
	if ((sys_clock_gettime(SYS_CLOCK_REALTIME, &ts) < 0) || !timespec_sub(&duration, &ts)) {
		return thrd_error;
	}

	switch (k_mutex_lock(*mutex, timespec_to_timeout(&duration, NULL))) {
	case 0:
		return thrd_success;
	case -EAGAIN:
		return thrd_timedout;
	default:
		return thrd_error;
	}
}

int mtx_trylock(mtx_t *mutex)
{
	switch (k_mutex_lock(*mutex, K_NO_WAIT)) {
	case 0:
		return thrd_success;
	case -EBUSY:
		return thrd_busy;
	default:
		return thrd_error;
	}
}

int mtx_unlock(mtx_t *mutex)
{
	switch (k_mutex_unlock(*mutex)) {
	case 0:
		return thrd_success;
	default:
		return thrd_error;
	}
}
