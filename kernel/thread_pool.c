#include "kernel_internal.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(os, CONFIG_KERNEL_LOG_LEVEL);

int k_thread_pool_index(const struct k_thread_pool *pool, k_tid_t tid)
{
	int idx;
	uint8_t *elem = (uint8_t *)tid;
	uint8_t *begin = (uint8_t *)&pool->thread[0];
	uint8_t *end = (uint8_t *)&pool->thread[pool->pool_size];

	if (begin <= elem && elem < end && (elem - begin) % sizeof(pool->thread[0]) == 0) {
		idx = tid - pool->thread;
		return idx;
	}

	LOG_ERR("tid %p does not belong to pool %p", tid, pool);

	return -ESRCH;
}

k_thread_stack_t *k_thread_pool_stack(const struct k_thread_pool *pool, k_tid_t tid)
{
	int idx;
	size_t stack_obj_size;

#ifdef CONFIG_USERSPACE
	stack_obj_size = Z_THREAD_STACK_SIZE_ADJUST(pool->stack_size);
#else
	stack_obj_size = Z_KERNEL_STACK_SIZE_ADJUST(pool->stack_size);
#endif

	idx = k_thread_pool_index(pool, tid);
	if (idx < 0) {
		return NULL;
	}

	__ASSERT(idx < pool->pool_size, "index %zu exceeds pool_size %zu", idx, pool->pool_size);

	return (k_thread_stack_t *)&pool->stack[idx * stack_obj_size];
}

int k_thread_pool_alloc(struct k_thread_pool *pool, k_tid_t *tid)
{
	int ret;
	size_t idx;

	ret = sys_bitarray_alloc(pool->bitmap, 1, &idx);
	if (ret < 0) {
		LOG_ERR("alloc failed: %d", ret);
		return ret;
	}

	__ASSERT(idx < pool->pool_size, "index %zu exceeds pool_size %zu", idx, pool->pool_size);
	*tid = &pool->thread[idx];
	(*tid)->base.thread_state = _THREAD_DEAD;

	LOG_DBG("alloc thread %p", *tid);

	return 0;
}

int k_thread_pool_free(struct k_thread_pool *pool, k_tid_t tid)
{
	int ret;
	size_t idx;

	ret = k_thread_pool_index(pool, tid);
	if (ret < 0) {
		return ret;
	}

	idx = ret;
	__ASSERT(idx < pool->pool_size, "index %zu exceeds pool_size %zu", idx, pool->pool_size);

	if (tid->base.thread_state != _THREAD_DEAD) {
		LOG_ERR("thread %p is in use", tid);
		return -EBUSY;
	}

	ret = sys_bitarray_free(pool->bitmap, 1, idx);
	if (ret < 0) {
		LOG_ERR("free failed: %d", ret);
		return ret;
	}

	LOG_DBG("free thread %p", tid);

	return 0;
}
