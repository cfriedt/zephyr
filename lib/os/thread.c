/*
 * Copyright (c) The Zephyr Project contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/thread.h>

K_MEM_SLAB_DEFINE_STATIC(sys_thread_pool, sizeof(struct k_thread), CONFIG_DYNAMIC_THREAD_POOL_SIZE, sizeof(void *));

int z_impl_sys_thread_create(struct k_thread **thread, k_thread_stack_t *stack, size_t stack_size,
			     sys_thread_entry entry, void *p1, void *p2, void *p3, int prio,
			     uint32_t options)
{
	int ret;

	ret = k_mem_slab_alloc(&sys_thread_pool, thread, K_NO_WAIT);
	if (ret < 0) {
		return ret;
	}

	return -ENOSYS;
}

#ifdef CONFIG_USERSPACE
static inline void z_vrfy_sys_thread_create(struct k_thread **thread, k_thread_stack_t *stack, size_t stack_size,
			     sys_thread_entry entry, void *p1, void *p2, void *p3, int prio,
			     uint32_t options)
{
	K_OOPS(K_SYSCALL_MEMORY_WRITE(thread, sizeof(struct k_thread *)));

	if (stack != NULL) {
		K_OOPS(K_SYSCALL_MEMORY_WRITE(stack, stack_size));
	}

	return z_impl_sys_thread_create(thread, stack, stack_size, entry, p1, p2, p3, prio, options);
}
#include <zephyr/syscalls/sys_thread_create_mrsh.c>
#endif /* CONFIG_USERSPACE */
