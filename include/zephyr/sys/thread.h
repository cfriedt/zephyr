/*
 * Copyright (c) The Zephyr Project contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYR_SYS_THREAD_H_
#define ZEPHYR_INCLUDE_ZEPHYR_SYS_THREAD_H_

#include <stdint.h>

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

__syscall int sys_thread_create(struct k_thread **thread, k_thread_stack_t *stack, size_t stack_size,
			     sys_thread_entry entry, void *p1, void *p2, void *p3, int prio,
			     uint32_t options);

#ifdef __cplusplus
}
#endif

#include <zephyr/syscalls/sys/thread.h>

#endif /* ZEPHYR_INCLUDE_ZEPHYR_SYS_THREAD_H_ */
