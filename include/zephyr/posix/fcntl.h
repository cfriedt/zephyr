/*
 * Copyright (c) 2018 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_POSIX_FCNTL_H_
#define ZEPHYR_POSIX_FCNTL_H_

#include <zephyr/sys/fdtable.h>

#define O_APPEND   ZVFS_O_APPEND
#define O_CREAT    ZVFS_O_CREAT
#define O_EXCL     ZVFS_O_EXCL
#define O_NONBLOCK ZVFS_O_NONBLOCK
#define O_TRUNC    ZVFS_O_TRUNC

#define O_ACCMODE (ZVFS_O_RDONLY | ZVFS_O_RDWR | ZVFS_O_WRONLY)

#define O_RDONLY ZVFS_O_RDONLY
#define O_RDWR   ZVFS_O_RDWR
#define O_WRONLY ZVFS_O_WRONLY

#define F_DUPFD ZVFS_F_DUPFD
#define F_GETFL ZVFS_F_GETFL
#define F_SETFL ZVFS_F_SETFL

#if _POSIX_ADVISORY_INFO >= 200112L
#define POSIX_FADV_DONTNEED   0
#define POSIX_FADV_NOREUSE    1
#define POSIX_FADV_NORMAL     2
#define POSIX_FADV_RANDOM     3
#define POSIX_FADV_SEQUENTIAL 4
#define POSIX_FADV_WILLNEED   5
#endif

#ifdef __cplusplus
extern "C" {
#endif

int fcntl(int fildes, int cmd, ...);
int open(const char *name, int flags, ...);
#if _POSIX_ADVISORY_INFO >= 200112L
int posix_fadvise(int fd, off_t offset, off_t len, int advice);
int posix_fallocate(int fd, off_t offset, off_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_POSIX_FCNTL_H_ */
