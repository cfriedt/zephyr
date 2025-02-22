/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_MMAN_H_
#define ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_MMAN_H_

#include <stddef.h>
#include <sys/types.h>

#include <zephyr/toolchain.h>

#define PROT_NONE  0x0
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

#define MAP_SHARED  0x1
#define MAP_PRIVATE 0x2
#define MAP_FIXED   0x4

/* for Linux compatibility */
#define MAP_ANONYMOUS 0x20

#define MS_SYNC       0x0
#define MS_ASYNC      0x1
#define MS_INVALIDATE 0x2

#define MAP_FAILED ((void *)-1)

#define MCL_CURRENT 0
#define MCL_FUTURE  1

#if _POSIX_ADVISORY_INFO >= 200112L
#define POSIX_MADV_DONTNEED   0
#define POSIX_MADV_NORMAL     1
#define POSIX_MADV_RANDOM     2
#define POSIX_MADV_SEQUENTIAL 3
#define POSIX_MADV_WILLNEED   4
#endif

#if _POSIX_TYPED_MEMORY_OBJECTS >= 200112L
#define POSIX_TYPED_MEM_ALLOCATE        0
#define POSIX_TYPED_MEM_ALLOCATE_CONTIG 1
#define POSIX_TYPED_MEM_MAP_ALLOCATABLE 2
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if _POSIX_TYPED_MEMORY_OBJECTS >= 200112L
struct posix_typed_mem_info {
	size_t posix_tmi_length;
};
#endif

#if _POSIX_MEMLOCK_RANGE >= 200112L
int mlock(const void *addr, size_t len);
#endif
#if _POSIX_MEMLOCK >= 200112L
int mlockall(int flags);
#endif
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
#if (_POSIX_SYNCHRONIZED_IO >= 200112L) || (defined(_XOPEN_REALTIME) && (_XOPEN_REALTIME != -1))
int msync(void *addr, size_t length, int flags);
#endif
#if _POSIX_MEMLOCK_RANGE >= 200112L
int munlock(const void *addr, size_t len);
#endif
#if _POSIX_MEMLOCK >= 200112L
int munlockall(void);
#endif
int munmap(void *addr, size_t len);
#if _POSIX_ADVISORY_INFO >= 200112L
int posix_madvise(void *addr, size_t len, int advice);
#endif
#if _POSIX_TYPED_MEMORY_OBJECTS >= 200112L
int posix_mem_offset(const void *ZRESTRICT addr, size_t len, off_t *ZRESTRICT off,
		     size_t *ZRESTRICT contig_len, int *ZRESTRICT fildes);
int posix_typed_mem_get_info(int fildes, struct posix_typed_mem_info *info);
int posix_typed_mem_open(const char *name, int oflag, int tflag);
#endif
#if _POSIX_SHARED_MEMORY_OBJECTS >= 200112L
int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_MMAN_H_ */
