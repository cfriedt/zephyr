/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYS__PTHREADTYPES_H_
#define _SYS__PTHREADTYPES_H_

#if defined(_POSIX_THREADS)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/posix/sys/sched.h>
typedef uint32_t pthread_t;

#define PTHREAD_SCOPE_SYSTEM  0
#define PTHREAD_SCOPE_PROCESS 1

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
#define PTHREAD_INHERIT_SCHED  0
#define PTHREAD_EXPLICIT_SCHED 1
#endif

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

typedef struct {
	void *stack;
	uint32_t details[2];
} pthread_attr_t;

#if defined(_POSIX_THREAD_PROCESS_SHARED)
#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1
#endif

#if defined(_POSIX_THREAD_PRIO_PROTECT)
#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2
#endif

/* _UNIX98_THREAD_MUTEX_ATTRIBUTES */
#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    3

typedef uint32_t pthread_mutex_t;
typedef struct {
	unsigned char type: 2;
	bool initialized: 1;
#if defined(_POSIX_THREAD_PROCESS_SHARED)
	bool pshared: 1;
#endif
#if defined(_POSIX_THREAD_PRIO_PROTECT)
	/* reserved */
#endif
} pthread_mutexattr_t;
#define _PTHREAD_MUTEX_INITIALIZER (-1)

typedef uint32_t pthread_cond_t;
typedef struct {
	unsigned long clock;
#if defined(_POSIX_THREAD_PROCESS_SHARED)
	bool pshared: 1;
#endif
} pthread_condattr_t;
#define _PTHREAD_COND_INITIALIZER (-1)

typedef uint32_t pthread_key_t;
typedef struct {
	bool flag: 1;
} pthread_once_t;
#define _PTHREAD_ONCE_INIT {0}

#if defined(_POSIX_BARRIERS)
typedef uint32_t pthread_barrier_t;
typedef struct {
#if defined(_POSIX_THREAD_PROCESS_SHARED)
	bool pshared: 1;
#endif
} pthread_barrierattr_t;
#endif

#if defined(_POSIX_SPIN_LOCKS)
typedef uint32_t pthread_spinlock_t;
#endif

#if defined(_POSIX_READER_WRITER_LOCKS)
typedef uint32_t pthread_rwlock_t;
typedef struct {
#if defined(_POSIX_THREAD_PROCESS_SHARED)
	bool pshared: 1;
#endif
} pthread_rwlockattr_t;
#define _PTHREAD_RWLOCK_INITIALIZER (-1)
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SYS__PTHREADTYPES_H_ */
