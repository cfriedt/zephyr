/*
 * Copyright (c) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_LIB_LIBC_COMMON_INCLUDE_MACHINE__THREADS_H_
#define ZEPHYR_LIB_LIBC_COMMON_INCLUDE_MACHINE__THREADS_H_

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ONCE_FLAG_INIT {0}

typedef struct k_condvar *cnd_t;
typedef struct k_mutex *mtx_t;
typedef struct k_thread *thrd_t;
typedef intptr_t tss_t;
typedef atomic_t once_flag;

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_LIBC_COMMON_INCLUDE_MACHINE__THREADS_H_ */
