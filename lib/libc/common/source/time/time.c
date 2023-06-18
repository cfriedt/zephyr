/*
 * Copyright (c) 2021 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <time.h>

#include <zephyr/drivers/rtc.h>
#include <zephyr/kernel.h>

time_t time(time_t *tloc)
{
	struct timespec ts = {0};
	int ret;

	ret = rtc_get_timespec(DEVICE_DT_GET(DT_CHOSEN(zephyr_rtc)), &ts);
	if (ret < 0) {
		errno = -ret;
		return (time_t)-1;
	}

	if (tloc) {
		*tloc = ts.tv_sec;
	}

	return ts.tv_sec;
}
