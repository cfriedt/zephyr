/*
 * Copyright (c) 2024, Meta
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <pthread.h>
#include <stdbool.h>

#include <zephyr/ztest.h>

static void before(void *arg)
{
	ARG_UNUSED(arg);

	zassert_ok(pthread_attr_init(&attr));
	/* TODO: pthread_attr_init() should be sufficient to initialize a thread by itself */
	zassert_ok(pthread_attr_setstack(&attr, &static_thread_stack, STATIC_THREAD_STACK_SIZE));
	attr_valid = true;
}

static void after(void *arg)
{
	ARG_UNUSED(arg);

	if (attr_valid) {
		(void)pthread_attr_destroy(&attr);
		attr_valid = false;
	}
}

ZTEST_SUITE(posix_threads_base, NULL, NULL, before, after, NULL);
