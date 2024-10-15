/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_TESTS_POSIX_THREADS_BASE_SRC__MAIN_H_
#define ZEPHYR_TESTS_POSIX_THREADS_BASE_SRC__MAIN_H_

#include <pthread.h>
#include <stdbool.h>

#include <zephyr/kernel.h>

#define STATIC_THREAD_STACK_SIZE (MAX(1024, PTHREAD_STACK_MIN + CONFIG_TEST_EXTRA_STACK_SIZE))

extern bool attr_valid;
extern pthread_attr_t attr;
extern struct z_thread_stack_element static_thread_stack[];

#endif /* ZEPHYR_TESTS_POSIX_THREADS_BASE_SRC__MAIN_H_ */
