/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/kernel/obj_core.h>
#include <zephyr/sys/elastipool.h>
#include <zephyr/sys/hash_map.h>

#define __k_elastipool_define_common(_name, _alignment, _element_size, _num_pool_elements,         \
				     _allow_allocation, _allocator, ...)                           \
	static uint8_t __noinit                                                                    \
		__aligned(_alignment) _name##_pool[(_element_size) * (_num_pool_elements)];        \
	static unsigned long _name##_bitset[DIV_ROUND_UP((_num_elements), BITS_PER_LONG)];         \
	__VA_ARG__ const struct k_elastipool _name = {                                             \
		.element_size = _element_size,                                                     \
		.num_pool_elements = _num_pool_elements,                                           \
		.allow_allocation = _allow_allocation,                                             \
		.allocator = _allocator,                                                           \
		.pool = _name##_pool,                                                              \
		.bitset = _name##_bitset,                                                          \
	};                                                                                         \
	BUILD_ASSERT((_element_size) % (_alignment) == 0,                                          \
		     "element size is not a multiple of alignment");                               \
	BUILD_ASSERT((_num_pool_elements > 0) || (_allow_allocation), "Invalid pool "              \
								      "configuration")

#define K_ELASTIPOOL_DEFINE(_name, _alignment, _element_size, _num_pool_elements,                  \
			    _allow_allocation, _allocator)                                         \
	__k_elastipool_define_common(_name, _alignment, _element_size, _num_pool_elements,         \
				     _allow_allocation, _allocator)

#define K_ELASTIPOOL_DEFINE_STATIC(_name, _element_size, _num_pool_elements, _allow_allocation,    \
				   _allocator)                                                     \
	__k_elastipool_define_common(_name, _alignment, _element_size, _num_pool_elements,         \
				     _allow_allocation, _allocator, static)

K_ELASTIPOOL_DEFINE_STATIC(thread_stack_pool, CONFIG_DYNAMIC_THREAD_STACK_SIZE,
			   CONFIG_DYNAMIC_THREAD_POOL_SIZE, CONFIG_DYNAMIC_THREAD_POOL_ALLOW_ALLOC);

static void *z_object_alloc(enum k_objects otype, size_t size)
{
	ARG_UNUSED(otype);
	ARG_UNUSED(size);

	return NULL;
}

void *z_impl_k_object_alloc(enum k_objects otype)
{
	return z_object_alloc(otype, 0);
}

void *z_impl_k_object_alloc_size(enum k_objects otype, size_t size)
{
	return z_object_alloc(otype, size);
}

void k_object_free(void *obj)
{
	ARG_UNUSED(obj);
}

struct k_object *k_object_find(const void *obj)
{
	ARG_UNUSED(obj);

	return NULL;
}
