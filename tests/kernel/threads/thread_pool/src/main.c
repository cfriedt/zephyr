/*
 * Copyright (c) 2022, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <ztest.h>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#define POOL_SIZE  CONFIG_THREAD_POOL_TEST_POOL_SIZE
#define STACK_SIZE (CONFIG_THREAD_POOL_TEST_STACK_SIZE + CONFIG_TEST_EXTRA_STACK_SIZE)
#ifdef CONFIG_USERSPACE
#define STACK_OBJ_SIZE Z_THREAD_STACK_SIZE_ADJUST(STACK_SIZE)
#else
#define STACK_OBJ_SIZE Z_KERNEL_STACK_SIZE_ADJUST(STACK_SIZE)
#endif

K_THREAD_POOL_DEFINE(test_pool, POOL_SIZE, STACK_SIZE);

/* Note: need to jump through serious hoops to circumvent gcc -Warray-bounds warnings */
#define BAD_TID_AT(offs) ((k_tid_t)((uint8_t *)test_pool_thread + offs))
#define BAD_TID_BEFORE	 BAD_TID_AT(-sizeof(struct k_thread))
#define BAD_TID_MIDDLE	 BAD_TID_AT(+0x42)
#define BAD_TID_AFTER	 BAD_TID_AT(+POOL_SIZE * sizeof(struct k_thread))

ZTEST(thread_pool, test_index)
{
	const size_t N = POOL_SIZE;

	BUILD_ASSERT(sizeof(*test_pool_thread) != 0x42);

	zassert_equal(-ESRCH, k_thread_pool_index(&test_pool, BAD_TID_BEFORE), NULL);
	zassert_equal(-ESRCH, k_thread_pool_index(&test_pool, BAD_TID_MIDDLE), NULL);
	zassert_equal(-ESRCH, k_thread_pool_index(&test_pool, BAD_TID_AFTER), NULL);

	for (int i = 0; i < N; ++i) {
		zassert_equal(i, k_thread_pool_index(&test_pool, &test_pool_thread[i]), NULL);
	}
}

ZTEST(thread_pool, test_stack)
{
	const size_t N = POOL_SIZE;
	const size_t stack_obj_size = sizeof(test_pool_stack) / N;

	printk("sizeof(test_pool_stack): %zu\n", sizeof(test_pool_stack));
	printk("STACK_OBJ_SIZE: %zu\n", (size_t)STACK_OBJ_SIZE);
	zassert_equal(0, sizeof(test_pool_stack) % STACK_OBJ_SIZE, NULL);

	zassert_is_null(k_thread_pool_stack(&test_pool, BAD_TID_BEFORE), NULL);
	zassert_is_null(k_thread_pool_stack(&test_pool, BAD_TID_MIDDLE), NULL);
	zassert_is_null(k_thread_pool_stack(&test_pool, BAD_TID_AFTER), NULL);

	for (int i = 0; i < N; ++i) {
		zassert_equal(test_pool.stack + i * stack_obj_size,
			      (uint8_t *)k_thread_pool_stack(&test_pool, &test_pool_thread[i]),
			      NULL);
	}
}

ZTEST(thread_pool, test_alloc)
{
	k_tid_t tid;
	const size_t N = POOL_SIZE;

	for (int i = 0; i < N; ++i) {
		zassert_ok(k_thread_pool_alloc(&test_pool, &tid), NULL);
	}

	zassert_equal(-ENOSPC, k_thread_pool_alloc(&test_pool, &tid), NULL);
}

ZTEST(thread_pool, test_free)
{
	k_tid_t tid;
	const size_t N = POOL_SIZE;

	zassert_equal(-ESRCH, k_thread_pool_free(&test_pool, BAD_TID_BEFORE), NULL);

	for (int i = 0; i < N; ++i) {
		zassert_equal(-EFAULT, k_thread_pool_free(&test_pool, &test_pool_thread[i]), NULL);
	}

	zassert_ok(k_thread_pool_alloc(&test_pool, &tid), NULL);
	zassert_ok(k_thread_pool_free(&test_pool, tid), NULL);
}

static void func(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);
}

ZTEST(thread_pool, test_spawn_respawn)
{
	k_tid_t tid[POOL_SIZE];
	const size_t N = POOL_SIZE;

	for (size_t iter = 0; iter < 2; ++iter) {
		for (size_t i = 0; i < N; ++i) {
			zassert_ok(k_thread_pool_alloc(&test_pool, &tid[i]), NULL);
			k_thread_create(tid[i], k_thread_pool_stack(&test_pool, tid[i]), STACK_SIZE,
					func, NULL, NULL, NULL, K_LOWEST_APPLICATION_THREAD_PRIO, 0,
					K_NO_WAIT);
		}

		for (int i = 0; i < N; ++i) {
			zassert_ok(k_thread_join(tid[i], K_MSEC(100)), NULL);
			k_thread_pool_free(&test_pool, tid[i]);
		}
	}
}

static void after_fn(void *unused)
{
	const size_t N = POOL_SIZE;

	ARG_UNUSED(unused);

	sys_bitarray_free(test_pool.bitmap, N, 0);
}

ZTEST_SUITE(thread_pool, NULL, NULL, NULL, after_fn, NULL);

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
