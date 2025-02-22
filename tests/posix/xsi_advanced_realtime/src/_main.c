/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <fcntl.h>
#include <unistd.h>

#include <ff.h>
#include <zephyr/fs/fs.h>
#include <zephyr/ztest.h>

int fd;

static FATFS fat_fs;

static struct fs_mount_t fatfs_mnt = {
	.type = FS_FATFS,
	.mnt_point = FATFS_MNTP,
	.fs_data = &fat_fs,
};

static void test_mount(void *arg)
{
	int res;

	ARG_UNUSED(arg);

	fd = -1;
	res = fs_mount(&fatfs_mnt);
	zassert_ok(res, "Error mounting fs [%d]\n", res);
}

static void test_unmount(void *arg)
{
	int res;

	ARG_UNUSED(arg);

	if (fd >= 0) {
		res = close(fd);
		__ASSERT(res == 0, "Error closing file [%d]", res);
		fd = -1;
	}

	res = fs_unmount(&fatfs_mnt);
	__ASSERT(res == 0, "Error unmounting fs [%d]", res);
}

ZTEST_SUITE(xsi_advanced_realtime, NULL, NULL, test_mount, test_unmount, NULL);
