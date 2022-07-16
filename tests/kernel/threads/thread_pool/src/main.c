/*
 * Copyright (c) 2022, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

extern k_thread_stack_t *k_thread_stack_alloc(size_t stack_size, int options);
extern int k_thread_stack_free(k_thread_stack_t *stack);

static void common(size_t stack_size, bool expect_fail)
{
	int options = 0;
	k_thread_stack_t *stack;

	if (IS_ENABLED(CONFIG_USERSPACE)) {
		options |= K_USER;
	}

	stack = k_thread_stack_alloc(stack_size, options);
	if (expect_fail) {
		zassert_is_null(stack, NULL);
		return;
	}
	
	zassert_not_null(stack, NULL);
	zassert_ok(k_thread_stack_free(stack), NULL);
}

ZTEST(test_dynamic_thread_stacks, size_0) {
	common(0, false);
}

ZTEST(test_dynamic_thread_stacks, size_2048)
{
	common(2048, false);
}

ZTEST(test_dynamic_thread_stacks, size_max)
{
	common(SIZE_MAX, true);
}

ZTEST_SUITE(test_dynamic_thread_stacks, NULL, NULL, NULL, NULL, NULL);
