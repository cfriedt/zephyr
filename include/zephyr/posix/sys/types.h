/*
 * Copyright (c) 2024, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_POSIX_SYS_TYPES_H_
#define ZEPHYR_INCLUDE_POSIX_SYS_TYPES_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pthread_attr;
struct pthread_barrierattr;
struct pthread_condattr;
struct pthread_mutexattr;
struct pthread_once;

typedef signed long blkcnt_t;
typedef signed long blksize_t;
typedef unsigned long clock_t;
typedef unsigned long clockid_t;
typedef int16_t dev_t;
typedef uint64_t fsblkcnt_t;
typedef uint32_t fsfilcnt_t;
typedef uint16_t gid_t;
typedef uint32_t id_t;
typedef uint16_t ino_t;
typedef signed long key_t;
typedef uint32_t mode_t;
typedef uint16_t nlink_t;
typedef signed long off_t;
typedef int32_t pid_t;
typedef struct pthread_attr pthread_attr_t;
typedef uint32_t pthread_barrier_t;
typedef struct pthread_barrierattr pthread_barrierattr_t;
typedef uint32_t pthread_cond_t;
typedef struct pthread_condattr pthread_condattr_t;
typedef uint32_t pthread_key_t;
typedef uint32_t pthread_mutex_t;
typedef struct pthread_mutexattr pthread_mutexattr_t;
typedef struct pthread_once pthread_once_t;
typedef uint32_t pthread_rwlock_t;
typedef uint32_t pthread_rwlockattr_t;
typedef uint32_t pthread_spinlock_t;
typedef uint32_t pthread_t;
/* size_t defined by stddef.h */
typedef long int ssize_t;
typedef long int suseconds_t;
typedef signed long time_t;
typedef long unsigned int timer_t;
typedef uint32_t trace_attr_t;
typedef uint32_t trace_event_id_t;
typedef uint32_t trace_event_set_t;
typedef uint32_t trace_id_t;
typedef uint16_t uid_t;
typedef unsigned long int useconds_t;

struct pthread_attr {
	void *stack;
	uint32_t details[2];
};
struct pthread_barrierattr {
	int pshared;
};
struct pthread_condattr {
	clockid_t clock;
};
struct pthread_mutexattr {
	int type;
};
struct pthread_once {
	uint8_t flag;
};

#define __off_t off_t
#define _fpos_t off_t

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_POSIX_SYS_TYPES_H_ */
