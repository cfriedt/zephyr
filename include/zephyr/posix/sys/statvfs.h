/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_POSIX_SYS_STATVFS_H_
#define ZEPHYR_POSIX_SYS_STATVFS_H_

#include <zephyr/types.h>
#include <zephyr/posix/posix_types.h>

#define ST_RDONLY 0x0001
#define ST_NOSUID 0x0002

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __FSBLKCNT_T_DEFINED
typedef unsigned long fsblkcnt_t;
#endif

#ifndef __FSFILCNT_T_DEFINED
typedef unsigned long fsfilcnt_t;
#endif

struct statvfs {
	unsigned long f_bsize;
	unsigned long f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;
	fsfilcnt_t f_files;
	fsfilcnt_t f_ffree;
	fsfilcnt_t f_favail;
	unsigned long f_fsid;
	unsigned long f_flag;
	unsigned long f_namemax;
};

int fstatvfs(int fildes, struct statvfs *buf);
int statvfs(const char *ZRESTRICT path, struct statvfs *ZRESTRICT buf);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_POSIX_SYS_STATVFS_H_ */
