/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <ff.h>
#include <zephyr/fs/fs.h>
#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

static FATFS _fs;

static struct fs_mount_t _mnt = {
	.type = FS_FATFS,
	.mnt_point = "/",
	.fs_data = &_fs,
};

void *setup(void)
{
	int ret;

	memset(&_fs, 0, sizeof(_fs));

	ret = fs_mount(&_mnt);
	zassert_ok(ret, "mount failed: %d", ret);

	ret = fs_mkdir("/tmp");
	zassert_ok(ret, "mkdir failed: %d", ret);

	return NULL;
}

void teardown(void *arg)
{
	ARG_UNUSED(arg);
	(void)fs_unmount(&_mnt);
}
