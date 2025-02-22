/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <zephyr/ztest.h>

ZTEST(xsi_advanced_realtime, test_posix_cputime)
{
	clockid_t clock_id;
	struct timespec ts = {0};

	{
		/* degenerate cases */
		zexpect_not_ok(clock_getcpuclockid(-1, NULL));
		zexpect_not_ok(clock_getcpuclockid(-1, &clock_id));
		zexpect_not_ok(clock_getcpuclockid(0, NULL));
	}

	clock_id = -1;
	zexpect_ok(clock_getcpuclockid(0, &clock_id));
	zexpect_not_equal(clock_id, -1);

	zexpect_ok(clock_gettime(clock_id, &ts));
}
