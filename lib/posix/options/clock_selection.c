/*
 * Copyright (c) 2023 Meta
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "posix_clock.h"
#include "posix_internal.h"

#include <stddef.h>
#include <time.h>
#include <errno.h>

#include <zephyr/sys/clock.h>
#include <zephyr/toolchain.h>

int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
		    struct timespec *rmtp)
{
	int ret;

	if (rqtp == NULL) {
		errno = EFAULT;
		return -1;
	}

	ret = sys_clock_nanosleep(sys_clock_from_clockid((int)clock_id), flags, rqtp, rmtp);
	if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *ZRESTRICT att,
			      clockid_t *ZRESTRICT clock_id)
{
	struct posix_condattr *const attr = (struct posix_condattr *)att;

	if ((attr == NULL) || !attr->initialized) {
		return EINVAL;
	}

	*clock_id = attr->clock;

	return 0;
}

static inline bool clockid_is_valid_for_condattr(clockid_t clock_id)
{
	if (clock_id == CLOCK_REALTIME) {
		return true;
	}

#if defined(_POSIX_MONOTONIC_CLOCK) && defined(CLOCK_MONOTONIC)
	if (clock_id == CLOCK_MONOTONIC) {
		return true;
	}
#endif

	return false;
}

int pthread_condattr_setclock(pthread_condattr_t *att, clockid_t clock_id)
{
	struct posix_condattr *const attr = (struct posix_condattr *)att;

	if (!clockid_is_valid_for_condattr(clock_id)) {
		return -EINVAL;
	}

	if ((attr == NULL) || !attr->initialized) {
		return EINVAL;
	}

	attr->clock = clock_id;

	return 0;
}
