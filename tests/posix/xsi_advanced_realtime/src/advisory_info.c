/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <zephyr/ztest.h>

ZTEST(xsi_advanced_realtime, test_posix_fadvise)
{
	int ret;
	static const uint8_t valid_fadv[] = {
		POSIX_FADV_DONTNEED, POSIX_FADV_NOREUSE,    POSIX_FADV_NORMAL,
		POSIX_FADV_RANDOM,   POSIX_FADV_SEQUENTIAL, POSIX_FADV_WILLNEED,
	};

	fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	zassert_not_equal(fd, -1, "%s() failed: %d", "open", errno);

	{
		/* degenerate cases */
		zexpect_not_ok(posix_fadvise(-1, -1, -1, -1));
		zexpect_not_ok(posix_fadvise(-1, -1, -1, POSIX_FADV_DONTNEED));
		zexpect_not_ok(posix_fadvise(-1, -1, 0, -1));
		zexpect_not_ok(posix_fadvise(-1, -1, 0, POSIX_FADV_NOREUSE));
		zexpect_not_ok(posix_fadvise(-1, 0, -1, -1));
		zexpect_not_ok(posix_fadvise(-1, 0, -1, POSIX_FADV_NORMAL));
		zexpect_not_ok(posix_fadvise(-1, 0, 0, -1));
		zexpect_not_ok(posix_fadvise(-1, 0, 0, POSIX_FADV_RANDOM));
		zexpect_not_ok(posix_fadvise(fd, -1, -1, -1));
		zexpect_not_ok(posix_fadvise(fd, -1, -1, POSIX_FADV_SEQUENTIAL));
		zexpect_not_ok(posix_fadvise(fd, -1, 0, -1));
		zexpect_not_ok(posix_fadvise(fd, -1, 0, POSIX_FADV_WILLNEED));
		zexpect_not_ok(posix_fadvise(fd, 0, -1, -1));
		zexpect_not_ok(posix_fadvise(fd, 0, -1, POSIX_FADV_DONTNEED));
		zexpect_not_ok(posix_fadvise(fd, 0, 0, -1));
	}

	ARRAY_FOR_EACH_PTR(valid_fadv, fadv) {
		ret = posix_fadvise(fd, 0, 0, *fadv);
		zexpect_ok(ret, "%s() failed: %d", "posix_fadvise", ret);
	}
}

ZTEST(xsi_advanced_realtime, test_posix_fallocate)
{
	struct stat st = {0};

	fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	zassert_not_equal(fd, -1, "%s() failed: %d", "open", errno);

	{
		/* degenerate cases */
		zexpect_not_ok(posix_fallocate(-1, -1, -1));
		zexpect_not_ok(posix_fallocate(-1, -1, 0));
		zexpect_not_ok(posix_fallocate(-1, 0, -1));
		zexpect_not_ok(posix_fallocate(-1, 0, 0));
		zexpect_not_ok(posix_fallocate(fd, -1, -1));
		zexpect_not_ok(posix_fallocate(fd, -1, 0));
		zexpect_not_ok(posix_fallocate(fd, 0, -1));
	}

	/* allocate space */
	zexpect_ok(posix_fallocate(fd, 0, 32));
	zassume_ok(fsync(fd));

	zassume_ok(stat(TEST_FILE, &st));
	zexpect_equal(st.st_size, 32);

	/* deallocate space */
	zexpect_ok(ftruncate(fd, 0));
	zassume_ok(fsync(fd));

	zassume_ok(stat(TEST_FILE, &st));
	zexpect_equal(st.st_size, 0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"

ZTEST(xsi_advanced_realtime, test_posix_memalign)
{
	void *memptr = NULL;

	{
		/* degenerate cases */
		if (false) {
			/* causes a crash in at least one libc */
			zexpect_not_ok(posix_memalign(NULL, 0, 0));
			zexpect_not_ok(posix_memalign(NULL, 0, 8));
			zexpect_not_ok(posix_memalign(NULL, 8, 0));
			zexpect_not_ok(posix_memalign(NULL, 8, 8));
			zexpect_not_ok(posix_memalign(&memptr, 0, 0));
			zexpect_not_ok(posix_memalign(&memptr, 0, 8));
			zexpect_not_ok(posix_memalign(&memptr, 8, 0));
		}
	}

	zexpect_ok(posix_memalign(&memptr, 8, 8));
	zexpect_not_equal(memptr, NULL);
	zexpect_equal((uintptr_t)memptr % 8, 0);
	if (memptr != NULL) {
		free(memptr);
	}
}

#pragma GCC diagnostic pop

ZTEST(xsi_advanced_realtime, test_posix_madvise)
{
	int ret;
	static uint32_t buf[4];
	static const uint8_t valid_advice[] = {
		POSIX_MADV_DONTNEED,   POSIX_MADV_NORMAL,   POSIX_MADV_RANDOM,
		POSIX_MADV_SEQUENTIAL, POSIX_MADV_WILLNEED,
	};

	{
		/* degenerate cases */
		zexpect_not_ok(posix_madvise(NULL, 0, -1));
		zexpect_not_ok(posix_madvise(NULL, 0, POSIX_MADV_DONTNEED));
		zexpect_not_ok(posix_madvise(NULL, sizeof(buf), -1));
		zexpect_not_ok(posix_madvise(NULL, sizeof(buf), POSIX_MADV_NORMAL));
		zexpect_not_ok(posix_madvise(buf, 0, -1));
		zexpect_not_ok(posix_madvise(buf, 0, POSIX_MADV_RANDOM));
		zexpect_not_ok(posix_madvise(buf, sizeof(buf), -1));
	}

	ARRAY_FOR_EACH_PTR(valid_advice, advice) {
		ret = posix_madvise(buf, sizeof(buf), *advice);
		zexpect_ok(ret, "%s() failed: %d", "posix_madvise", ret);
	}
}
