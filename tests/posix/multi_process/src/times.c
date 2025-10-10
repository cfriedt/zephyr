/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/times.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

ZTEST(posix_multi_process, test_times)
{
	static const struct {
		const char *name;
		size_t offset;
	} fields[] = {
		{
			.name = "utime",
			.offset = offsetof(struct tms, tms_utime),
		},
		{
			.name = "stime",
			.offset = offsetof(struct tms, tms_stime),
		},
		{
			.name = "cutime",
			.offset = offsetof(struct tms, tms_cutime),
		},
		{
			.name = "cstime",
			.offset = offsetof(struct tms, tms_cstime),
		},
	};
	struct tms test_tms[2] = {};

	zexpect_not_equal(times(&test_tms[0]), -1);
	k_msleep(MSEC_PER_SEC);
	zexpect_not_equal(times(&test_tms[1]), -1);

	ARRAY_FOR_EACH(fields, i) {
		const char *name = fields[i].name;
		size_t offset = fields[i].offset;

		clock_t t0 = *(clock_t *)((uint8_t *)&test_tms[0] + offset);
		clock_t t1 = *(clock_t *)((uint8_t *)&test_tms[1] + offset);

		zexpect_true(t1 >= t0, "time moved backward for tms_%s: t0: %d t1: %d", name, t0,
			     t1);
	}
}
