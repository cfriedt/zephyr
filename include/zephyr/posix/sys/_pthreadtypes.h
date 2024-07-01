/*
 * Copyright (c) 2018 Intel Corporation
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_POSIX_TYPES_H_
#define ZEPHYR_INCLUDE_POSIX_TYPES_H_

/* exclude external libc sys/_pthreadtypes.h */
#define _SYS__PTHREADTYPES_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_POSIX_THREADS) || (_POSIX_C_SOURCE >= 199506L) || defined(__DOXYGEN__)

#if !defined(CONFIG_ARCMWDT_LIBC)
typedef int pid_t;
#endif

#include <zephyr/posix/sys/sched.h>

/* Pthread */
typedef uint32_t pthread_t;

/* Pthread scope */
#define PTHREAD_SCOPE_PROCESS 1
#define PTHREAD_SCOPE_SYSTEM  0

/* Pthread inherit scheduler */
#define PTHREAD_INHERIT_SCHED  0
#define PTHREAD_EXPLICIT_SCHED 1

/* Pthread detach/joinable */
#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_CREATE_JOINABLE 0

typedef struct {
	void *stack;
	uint32_t details[2];
} pthread_attr_t;

#if defined(_POSIX_THREAD_PROCESS_SHARED) || defined(__DOXYGEN__)
#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1
#endif

#if defined(_POSIX_THREAD_PRIO_PROTECT) || defined(__DOXYGEN__)
#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2
#endif

/* Mutex type */
#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

/* Mutex */
typedef uint32_t pthread_mutex_t;
typedef struct {
	unsigned char type: 2;
	bool initialized: 1;
} pthread_mutexattr_t;

/**
 * @brief Declare a mutex as initialized
 *
 * Initialize a mutex with the default mutex attributes.
 */
#define PTHREAD_MUTEX_INITIALIZER (-1)

/* Condition variables */
typedef uint32_t pthread_cond_t;
typedef struct {
	unsigned int clock;
} pthread_condattr_t;

/**
 * @brief Declare a condition variable as initialized
 *
 * Initialize a condition variable with the default condition variable attributes.
 */
#define PTHREAD_COND_INITIALIZER (-1)

typedef uint32_t pthread_key_t;
typedef struct {
	bool flag;
} pthread_once_t;

/* clang-format off */
/* Passed to pthread_once */
#define PTHREAD_ONCE_INIT {0}
/* clang-format on */

#endif

#if defined(_POSIX_BARRIERS) || defined(__DOXYGEN__)
/* Barrier */
typedef uint32_t pthread_barrier_t;
typedef struct {
	int pshared;
} pthread_barrierattr_t;
#endif

#if defined(_POSIX_SPINLOCKS) || defined(__DOXYGEN__)
typedef uint32_t pthread_spinlock_t;
#endif

#if defined(_POSIX_READER_WRITER_LOCKS) || defined(__DOXYGEN__)
typedef uint32_t pthread_rwlock_t;
typedef uint32_t pthread_rwlockattr_t;
#endif

#ifdef __cplusplus
}
#endif

#endif	/* ZEPHYR_INCLUDE_POSIX_TYPES_H_ */
