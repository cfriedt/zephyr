/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_PTHREADTYPES_H_
#define ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_PTHREADTYPES_H_

#define _SYS__TYPES_H

#include <stdint.h>

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

// typedef int64_t _TIME_T_;
// typedef int32_t _SUSECONDS_T_;

typedef int pid_t;

typedef unsigned long clockid_t;
#define __clockid_t_defined

typedef unsigned long timer_t;
#define __timer_t_defined

typedef long _fpos_t;
typedef long _off_t;

/* Thread attributes */
typedef struct pthread_attr {
	void *stack;
	uint32_t details[2];
} pthread_attr_t;

typedef uint32_t pthread_t;
typedef uint32_t pthread_spinlock_t;

/* Semaphore */
typedef struct k_sem sem_t;

/* Mutex */
typedef uint32_t pthread_mutex_t;

typedef struct pthread_mutexattr {
	unsigned char type: 2;
	bool initialized: 1;
} pthread_mutexattr_t;

/* Condition variables */
typedef uint32_t pthread_cond_t;

struct pthread_condattr {
	clockid_t clock;
};

typedef struct pthread_condattr pthread_condattr_t;

/* Barrier */
typedef uint32_t pthread_barrier_t;

typedef struct pthread_barrierattr {
	int pshared;
} pthread_barrierattr_t;

typedef uint32_t pthread_rwlockattr_t;

typedef uint32_t pthread_rwlock_t;

struct pthread_once {
	bool flag;
};

typedef uint32_t pthread_key_t;
typedef struct pthread_once pthread_once_t;

struct sched_param {
	int sched_priority;
};

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_TYPES_H_ */
