/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include <zephyr/ztest.h>

#define TEST_FD0_FN "/tmp/foo.txt"

static int fd[2];

ZTEST(posix_fd_mgmt, test_dup)
{
	{
		/* degenerate cases */
		errno = 0;
		fd[1] = dup(-1);
		zexpect_equal(fd[1], -1);
		zexpect_equal(errno, EBADF);
	}

	fd[1] = dup(fd[0]);
	zexpect_true(fd[1] >= 0, "%s() failed: %d", "dup", errno);
	zexpect_true(fd[1] != fd[0], "fd[1] (%d) == fd[0] (%d)", fd[1], fd[0]);
}

ZTEST(posix_fd_mgmt, test_dup2)
{
	{
		/* degenerate cases */
		errno = 0;
		zexpect_equal(dup2(-1, -1), -1);
		zexpect_equal(errno, EBADF);
		errno = 0;
		zexpect_equal(dup2(fd[0], -1), -1);
		zexpect_equal(errno, EBADF);
		errno = 0;
		zexpect_equal(dup2(-1, fd[0]), -1);
		zexpect_equal(errno, EBADF);
	}

	fd[1] = dup(fd[0]);
	zexpect_true(fd[1] >= 0, "%s() failed: %d", "dup", errno);
	zexpect_true(fd[1] != fd[0], "fd[1] (%d) == fd[0] (%d)", fd[1], fd[0]);
}

ZTEST(posix_fd_mgmt, test_fcntl)
{
	zexpect_equal(fcntl(-1, -1), -1);
}

ZTEST(posix_fd_mgmt, test_fseeko)
{
	zexpect_equal(fseeko(NULL, 0, SEEK_SET), -1);
}

ZTEST(posix_fd_mgmt, test_ftello)
{
	zexpect_equal(ftello(NULL), -1);
}

ZTEST(posix_fd_mgmt, test_ftruncate)
{
	zexpect_equal(ftruncate(-1, -1), -1);
}

ZTEST(posix_fd_mgmt, test_lseek)
{
	zexpect_equal(lseek(-1, 0, SEEK_SET), -1);
}

void *setup(void);
static void before(void *arg)
{
	ARRAY_FOR_EACH(fd, i) {
		if (i == 0) {
			fd[i] = open(TEST_FD0_FN, O_RDWR, O_CREAT | O_TRUNC);
		} else {
			fd[i] = -1;
		}
	}
}

static void after(void *arg)
{
	ARRAY_FOR_EACH(fd, i) {
		if (fd[i] != -1) {
			(void)close(fd[i]);
			fd[i] = -1;
		}
	}
}
void teardown(void *arg);

ZTEST_SUITE(posix_fd_mgmt, NULL, setup, before, after, teardown);
