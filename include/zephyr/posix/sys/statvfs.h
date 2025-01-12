/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_POSIX_SYS_STATVFS_H_
#define ZEPHYR_INCLUDE_POSIX_SYS_STATVFS_H_

#include <zephyr/posix/posix_types.h>
#include <zephyr/toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_FSBLKCNT_T_DECLARED) && !defined(__fsblkcnt_t_defined)
typedef unsigned long fsblkcnt_t;
typedef unsigned long fsfilcnt_t;
#define _FSBLKCNT_T_DECLARED
#define __fsblkcnt_t_defined
#endif

#define ST_RDONLY 0x0001
#define ST_NOSUID 0x0002

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

#endif /* ZEPHYR_INCLUDE_POSIX_SYS_STATVFS_H_ */
