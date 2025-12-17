/*
 * Copyright (c) 2025, Friedt Professional Engineering Services, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/elastipool.h>
#include <zephyr/sys/hash_map.h>
#include <zephyr/toolchain.h>

#define SYS_ELASTIPOOL_STRIDE_BITS (sizeof(unsigned long) * BITS_PER_BYTE)

/* like find_lsb_set() but operates on longs rather than fixed-size uint32_t's */
static ALWAYS_INLINE unsigned int find_lsb_set_l(unsigned long op)
{
#ifdef CONFIG_TOOLCHAIN_HAS_BUILTIN_FFS
	return __builtin_ffsl(op);
#else
	if (op == 0) {
		return 0;
	}

	for (size_t i = 0; i < sizeof(op) * BITS_PER_BYTE; ++i, op >>= 1) {
		if ((op & 1) != 0) {
			return i + 1;
		}
	}

	return -1;
#endif
}

static int sys_elastipool_bmp_alloc(unsigned long *bmp, size_t n)
{
	int bit = -1;
	size_t m = 0;
	int n_longs = DIV_ROUND_UP(n, SYS_ELASTIPOOL_STRIDE_BITS);

	/* cycle through longs that do not require any truncation */
	for (unsigned long *end = &bmp[n_longs - 1]; bmp < end;
	     ++bmp, m += SYS_ELASTIPOOL_STRIDE_BITS) {
		unsigned long l = ~(*bmp);

		if (l == 0) {
			continue;
		}

		bit = find_lsb_set_l(l) - 1;
		*bmp |= BIT(bit);

		return bit + m;
	}

	/* the last long might require truncation, unless n is an even multiple of stride */
	unsigned long mask = (unsigned long)BIT64_MASK(n - m);
	unsigned long l = ~(*bmp) & mask;

	if (l == 0) {
		/* there are no blocks available to allocate */
		return -ENOMEM;
	}

	bit = find_lsb_set_l(l) - 1;
	*bmp |= BIT(bit);

	return bit + m;
}

static int sys_elastipool_bmp_check(unsigned long *bmp, size_t bit, size_t num_bits)
{
	if (bit >= num_bits) {
		return -EINVAL;
	}

	size_t stride = SYS_ELASTIPOOL_STRIDE_BITS;
	size_t i = bit / stride;

	bmp += i;
	bit -= i * stride;

	if (((*bmp) & (unsigned long)BIT64(bit)) == 0) {
		/* block was never allocated */
		return -EINVAL;
	}

	return bit;
}

static int sys_elastipool_bmp_free(unsigned long *bmp, size_t bit, size_t num_bits)
{
	int ret;

	ret = sys_elastipool_bmp_check(bmp, bit, num_bits);
	if (ret < 0) {
		return ret;
	}

	size_t w = bit >> LOG2(SYS_ELASTIPOOL_STRIDE_BITS);

	bmp = &bmp[w];

	/* get the bit inside of the word */
	bit &= LOG2(SYS_ELASTIPOOL_STRIDE_BITS);
	bmp[w] &= (unsigned long)~BIT64(bit);

	return 0;
}

int sys_elastipool_api_alloc_dyn(const struct sys_elastipool *pool, void **obj)
{
	int ret;

	__ASSERT_NO_MSG(pool != NULL);
	__ASSERT_NO_MSG(obj != NULL);

	if (pool->data->pool_size == pool->config->max_obj) {
		return -ENOMEM;
	}

	*obj = pool->api->heap_alloc(NULL, pool->config->obj_size, pool->config->obj_align);
	if (*obj == NULL) {
		return -ENOMEM;
	}

	uint64_t kv = (uint64_t)(uintptr_t)*obj;

	ret = sys_hashmap_insert(pool->config->map, kv, kv, NULL);
	if (ret < 0) {
		pool->api->heap_alloc(*obj, 0, 0);
		*obj = NULL;
		return ret;
	}

	pool->data->pool_size++;
	return 0;
}

int sys_elastipool_api_alloc_elastic(const struct sys_elastipool *pool, void **obj)
{
	if (sys_elastipool_api_alloc_static(pool, obj) == 0) {
		return 0;
	}

	return sys_elastipool_api_alloc_dyn(pool, obj);
}

int sys_elastipool_api_alloc_static(const struct sys_elastipool *pool, void **obj)
{
	__ASSERT_NO_MSG(pool != NULL);
	__ASSERT_NO_MSG(obj != NULL);

	if (pool->data->pool_size == pool->config->max_obj) {
		return -ENOMEM;
	}

	int ret = sys_elastipool_bmp_alloc(pool->config->bmp, pool->config->min_obj);

	if (ret < 0) {
		return ret;
	}

	++pool->data->pool_size;
	*obj = pool->config->storage +
	       ret * ROUND_UP(pool->config->obj_size, pool->config->obj_align);

	return 0;
}

int sys_elastipool_api_free_dyn(const struct sys_elastipool *pool, const void *obj)
{
	__ASSERT_NO_MSG(pool != NULL);
	__ASSERT_NO_MSG(obj != NULL);

	if (pool->data->pool_size == 0) {
		return -EINVAL;
	}

	if (!sys_hashmap_remove(pool->config->map, (uint64_t)(uintptr_t)obj, NULL)) {
		return -EINVAL;
	}

	(void)pool->api->heap_alloc(obj, 0, 0);

	return 0;
}

int sys_elastipool_api_free_elastic(const struct sys_elastipool *pool, const void *obj)
{
	if (sys_elastipool_api_free_static(pool, obj) == 0) {
		return 0;
	}

	return sys_elastipool_api_free_dyn(pool, obj);
}

int sys_elastipool_api_free_static(const struct sys_elastipool *pool, const void *obj)
{
	__ASSERT_NO_MSG(pool != NULL);
	__ASSERT_NO_MSG(obj != NULL);

	size_t block_size = ROUND_UP(pool->config->obj_size, pool->config->obj_align);
	size_t bit = (uintptr_t)((uint8_t *)obj - pool->config->storage) / block_size;

	int ret = sys_elastipool_bmp_free(pool->config->bmp, bit, pool->config->min_obj);

	if (ret < 0) {
		return ret;
	}

	--pool->data->pool_size;
	return 0;
}

static void test_hashmap_clear_callback(uint64_t key, uint64_t value, void *cookie)
{
	ARG_UNUSED(value);

	const struct sys_elastipool *pool = (const struct sys_elastipool *)cookie;

	(void)pool->api->heap_alloc((void *)(uintptr_t)key, 0, 0);
}

void sys_elastipool_api_clear_dyn(const struct sys_elastipool *pool)
{
	sys_hashmap_clear(pool->config->map, test_hashmap_clear_callback, (void *)pool);
	pool->data->pool_size = 0;
}

void sys_elastipool_api_clear_elastic(const struct sys_elastipool *pool)
{
	sys_elastipool_api_clear_static(pool);
	sys_elastipool_api_clear_dyn(pool);
}

void sys_elastipool_api_clear_static(const struct sys_elastipool *pool)
{
	memset(pool->config->bmp, 0,
	       pool->config->min_obj * ROUND_UP(pool->config->obj_size, pool->config->obj_align));
	pool->data->pool_size = 0;
}

void *sys_elastipool_api_heap_alloc(const void *ptr, size_t size, size_t align)
{
	/* newlib doesn't declare this by default */
	void *aligned_alloc(size_t alignment, size_t size);

	if (size == 0) {
		free((void *)ptr);
		return NULL;
	}

	return aligned_alloc(align, size);
}

int sys_elastipool_init(struct sys_elastipool *pool, struct sys_elastipool_api *api,
			struct sys_elastipool_config *config, struct sys_elastipool_data *data,
			size_t obj_size, size_t obj_align, size_t min_obj, size_t max_obj,
			uint8_t *storage, unsigned long *bmp, struct sys_hashmap *map,
			sys_elastipool_api_heap_alloc_t allocator)
{
	if ((obj_size == 0) || (min_obj == 0) || (max_obj < min_obj) ||
	    ((obj_align != 0) && !IS_POWER_OF_TWO(obj_align)) || (api == NULL) ||
	    (config == NULL)) {
		return -EINVAL;
	}

	if ((min_obj > 0) && ((storage == NULL) || (bmp == NULL))) {
		return -EINVAL;
	}

	if ((max_obj > min_obj) && (map == NULL)) {
		return -EINVAL;
	}

	if (obj_align == 0) {
		obj_align = 1;
	}

	*config = (struct sys_elastipool_config){
		.obj_size = obj_size,
		.obj_align = obj_align,
		.min_obj = min_obj,
		.max_obj = max_obj,
		.storage = storage,
		.bmp = bmp,
		.map = map,
	};
	*data = (struct sys_elastipool_data){0};
	*api = (struct sys_elastipool_api){
		.alloc = SYS_ELASTIPOOL_ALLOC_FN(min_obj, max_obj),
		.check = SYS_ELASTIPOOL_CHECK_FN(min_obj, max_obj),
		.clear = SYS_ELASTIPOOL_CLEAR_FN(min_obj, max_obj),
		.free = SYS_ELASTIPOOL_FREE_FN(min_obj, max_obj),
		.heap_alloc = (allocator == NULL) ? SYS_ELASTIPOOL_HEAP_ALLOC_FN(min_obj, max_obj)
						  : allocator,
	};

	*pool = (struct sys_elastipool){
		.api = api,
		.config = config,
		.data = data,
	};

	return 0;
}

int sys_elastipool_api_check_dyn(const struct sys_elastipool *pool, const void *ptr)
{
	__ASSERT_NO_MSG(pool != NULL);

	if (!sys_hashmap_contains_key(pool->config->map, (uint64_t)(uintptr_t)ptr)) {
		return -EINVAL;
	}

	return 0;
}

int sys_elastipool_api_check_elastic(const struct sys_elastipool *pool, const void *ptr)
{
	__ASSERT_NO_MSG(pool != NULL);

	if (sys_elastipool_api_check_static(pool, ptr) == 0) {
		return 0;
	}
	return sys_elastipool_api_check_dyn(pool, ptr);
}

int sys_elastipool_api_check_static(const struct sys_elastipool *pool, const void *ptr)
{
	int ret;

	__ASSERT_NO_MSG(pool != NULL);

	int bit = (((uint8_t *)ptr) - pool->config->storage) /
		  ROUND_UP(pool->config->obj_size, pool->config->obj_align);

	ret = sys_elastipool_bmp_check(pool->config->bmp, (size_t)bit, pool->config->min_obj);
	if (ret < 0) {
		return ret;
	}

	return 0;
}
