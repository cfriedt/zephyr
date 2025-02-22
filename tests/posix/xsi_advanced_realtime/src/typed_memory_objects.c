/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <zephyr/ztest.h>

ZTEST(xsi_advanced_realtime, test_posix_mem_offset)
{
	/* int posix_mem_offset(const void *ZRESTRICT addr, size_t len, off_t *ZRESTRICT off,
			 size_t *ZRESTRICT contig_len, int *ZRESTRICT fildes); */

	{
		/* degenerate cases */
		zexpect_not_ok(posix_mem_offset(NULL, 0, NULL, NULL, NULL));
	}

	zexpect_ok(posix_mem_offset(NULL, 0, NULL, NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_typed_mem_get_info)
{
	/* int posix_typed_mem_get_info(int fildes,
	   struct posix_typed_mem_info *info); */

	{
		/* degenerate cases */
		zexpect_not_ok(posix_typed_mem_get_info(-1, NULL));
	}

	zexpect_ok(posix_typed_mem_get_info(-1, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_typed_mem_open)
{
	/* int posix_typed_mem_open(const char *name, int oflag, int tflag); */

	{
		/* degenerate cases */
		zexpect_not_ok(posix_typed_mem_get_info(-1, NULL));
	}

	zexpect_ok(posix_typed_mem_get_info(-1, NULL));
}
