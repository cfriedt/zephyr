/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <limits.h>
#include <stdint.h>
#include <time.h>

#include <zephyr/sys/clock.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/sys/util.h>

/**
 * @brief Convert an absolute `timespec` object to a clock-relative kernel timeout
 *
 * This function converts the absolute timepoint, @a abstime, to a relative duration of type
 * @ref k_timeout_t with respect to the given system clock, @a clock_id.
 *
 * Valid `clock_id` values are, for example, @ref SYS_CLOCK_REALTIME or @ref SYS_CLOCK_MONOTONIC.
 *
 * On success, this function returns the time duration until @a abstime relative to the specified
 * system clock.
 *
 * In case @a abstime is in the past, with respect to the specified system clock @a clock_id, this
 * function returns @ref K_NO_WAIT.
 *
 * @note This function will assert if an invalid `clock_id` is provided or if `abstime` refers to an
 * invalid `timespec` object.
 *
 * @param ts the absolute `timespec` object to convert
 * @return the relative duration until @a abstime with respect to @a clock_id
 */
k_timeout_t timespec_abs_to_clock_rel_timeout(int clock_id, const struct timespec *abstime)
{
	int __maybe_unused ret;
	struct timespec duration;

	__ASSERT_NO_MSG(abstime != NULL);
	__ASSERT_NO_MSG(timespec_is_valid(abstime));
	
	if (abstime->tv_sec < 0) {
		return K_NO_WAIT;
	}

	/*
	 * duration = abstime - now
	 * <=> duration = -now, duration += abstime
	 */
	ret = sys_clock_gettime(clock_id, &duration);
	__ASSERT(ret == 0, "%s() failed: %d", "sys_clock_gettime", ret);

	ret = (int)timespec_negate(&duration);
	__ASSERT(ret != 0, "%s() failed: %d", "timespec_negate", ret);

	ret = (int)timespec_add(&duration, abstime);
	__ASSERT(ret != 0, "%s() failed: %d", "timespec_add", ret);


	if (duration.tv_sec < 0) {
		return K_NO_WAIT;
	}

	return timespec_to_timeout(&duration);
}
