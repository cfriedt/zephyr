/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/kernel.h>
#include <zephyr/posix/time.h>
#include <zephyr/posix/sys/time.h>
#include <zephyr/sys/util.h>

#define _rtc DEVICE_DT_GET(DT_CHOSEN(zephyr_rtc))

static int uptime_to_timespec(struct timespec *ts)
{
	bool carry;
	uint64_t ticks;

	ticks = k_uptime_get();
	ts->tv_sec = ticks / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
	ts->tv_nsec = k_ticks_to_ns_floor32(ticks - ts->tv_sec * CONFIG_SYS_CLOCK_TICKS_PER_SEC);
	carry = ts->tv_nsec >= NSEC_PER_SEC;
	ts->tv_sec += carry;
	ts->tv_nsec -= carry * NSEC_PER_SEC;

	return 0;
}

int clock_gettime(clockid_t clock_id, struct timespec *ts)
{
	switch (clock_id) {
	case CLOCK_MONOTONIC:
		return uptime_to_timespec(ts);

	case CLOCK_REALTIME: {
		int ret;

		ret = rtc_get_timespec(_rtc, ts);
		if (ret == 0) {
			return 0;
		}

		errno = -ret;
		return -1;
	}

	default:
		errno = EINVAL;
		return -1;
	}
}

/**
 * @brief Set the time of the specified clock.
 *
 * See IEEE 1003.1.
 *
 * Note that only the `CLOCK_REALTIME` clock can be set using this
 * call.
 */
int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
	int ret;
	struct rtc_time t = {
		.tm_sec = tp->tv_sec,
		.tm_nsec = tp->tv_nsec,
	};

	if (clock_id != CLOCK_REALTIME || rtc_set_time(_rtc, &t) != 0) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

/**
 * @brief Get current real time.
 *
 * See IEEE 1003.1
 */
int gettimeofday(struct timeval *tv, void *tz)
{
	int res;
	struct timespec ts;

	/* As per POSIX, "if tzp is not a null pointer, the behavior
	 * is unspecified."  "tzp" is the "tz" parameter above. */
	ARG_UNUSED(tz);

	res = clock_gettime(CLOCK_REALTIME, &ts);
	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;

	return 0;
}
