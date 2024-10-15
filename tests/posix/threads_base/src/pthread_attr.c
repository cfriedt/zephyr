/*
 * Copyright (c) 2024, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <pthread.h>

#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

#define BIOS_FOOD           0xB105F00D
#define SCHED_INVALID       4242
#define INVALID_DETACHSTATE 7373

bool attr_valid;
pthread_attr_t attr;
static const pthread_attr_t uninit_attr;
static bool detached_thread_has_finished;

/* TODO: this should be optional */
K_THREAD_STACK_DEFINE(static_thread_stack, STATIC_THREAD_STACK_SIZE);

static void *thread_entry(void *arg)
{
	bool joinable = (bool)POINTER_TO_UINT(arg);

	if (!joinable) {
		detached_thread_has_finished = true;
	}

	return NULL;
}

static void create_thread_common_entry(const pthread_attr_t *attrp, bool expect_success,
				       bool joinable, void *(*entry)(void *arg), void *arg)
{
	pthread_t th;

	if (!joinable) {
		detached_thread_has_finished = false;
	}

	if (expect_success) {
		zassert_ok(pthread_create(&th, attrp, entry, arg));
	} else {
		zassert_not_ok(pthread_create(&th, attrp, entry, arg));
		return;
	}

	if (joinable) {
		zassert_ok(pthread_join(th, NULL), "failed to join joinable thread");
		return;
	}

	/* should not be able to join detached thread */
	zassert_not_ok(pthread_join(th, NULL));

	for (size_t i = 0; i < 10; ++i) {
		k_msleep(2 * CONFIG_PTHREAD_RECYCLER_DELAY_MS);
		if (detached_thread_has_finished) {
			break;
		}
	}

	zassert_true(detached_thread_has_finished, "detached thread did not seem to finish");
}

static void create_thread_common(const pthread_attr_t *attrp, bool expect_success, bool joinable)
{
	create_thread_common_entry(attrp, expect_success, joinable, thread_entry,
				   UINT_TO_POINTER(joinable));
}

static inline void can_create_thread(const pthread_attr_t *attrp)
{
	create_thread_common(attrp, true, true);
}

static inline void cannot_create_thread(const pthread_attr_t *attrp)
{
	create_thread_common(attrp, false, true);
}

ZTEST(posix_threads_base, test_null_attr)
{
	/*
	 * This test can only succeed when it is possible to call pthread_create() with a NULL
	 * pthread_attr_t* (I.e. when we have the ability to allocate thread stacks dynamically).
	 */
	create_thread_common(NULL, IS_ENABLED(CONFIG_DYNAMIC_THREAD) ? true : false, true);
}

ZTEST(posix_threads_base, test_pthread_attr_static_corner_cases)
{
	pthread_attr_t attr1;

	Z_TEST_SKIP_IFDEF(CONFIG_DYNAMIC_THREAD);

	/*
	 * These tests are specifically for when dynamic thread stacks are disabled, so passing
	 * a NULL pthread_attr_t* should fail.
	 */
	cannot_create_thread(NULL);

	/*
	 * Additionally, without calling pthread_attr_setstack(), thread creation should fail.
	 */
	zassert_ok(pthread_attr_init(&attr1));
	cannot_create_thread(&attr1);
}

ZTEST(posix_threads_base, test_pthread_attr_init_destroy)
{
	/* attr has already been initialized in before() */

	if (false) {
		/* undefined behaviour */
		zassert_ok(pthread_attr_init(&attr));
	}

	/* cannot destroy an uninitialized attr */
	zassert_equal(pthread_attr_destroy((pthread_attr_t *)&uninit_attr), EINVAL);

	can_create_thread(&attr);

	/* can destroy an initialized attr */
	zassert_ok(pthread_attr_destroy(&attr), "failed to destroy an initialized attr");
	attr_valid = false;

	cannot_create_thread(&attr);

	if (false) {
		/* undefined behaviour */
		zassert_ok(pthread_attr_destroy(&attr));
	}

	/* can re-initialize a destroyed attr */
	zassert_ok(pthread_attr_init(&attr));
	/* TODO: pthread_attr_init() should be sufficient to initialize a thread by itself */
	zassert_ok(pthread_attr_setstack(&attr, &static_thread_stack, STATIC_THREAD_STACK_SIZE));
	attr_valid = true;

	can_create_thread(&attr);

	/* note: attr is still valid and is destroyed in after() */
}

ZTEST(posix_threads_base, test_pthread_attr_getguardsize)
{
	size_t guardsize;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getguardsize(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getguardsize(NULL, &guardsize), EINVAL);
			zassert_equal(pthread_attr_getguardsize(&uninit_attr, &guardsize), EINVAL);
		}
		zassert_equal(pthread_attr_getguardsize(&attr, NULL), EINVAL);
	}

	guardsize = BIOS_FOOD;
	zassert_ok(pthread_attr_getguardsize(&attr, &guardsize));
	zassert_not_equal(guardsize, BIOS_FOOD);
}

ZTEST(posix_threads_base, test_pthread_attr_setguardsize)
{
	size_t guardsize = CONFIG_POSIX_PTHREAD_ATTR_GUARDSIZE_DEFAULT;
	size_t sizes[] = {0, BIT_MASK(CONFIG_POSIX_PTHREAD_ATTR_GUARDSIZE_BITS / 2),
			  BIT_MASK(CONFIG_POSIX_PTHREAD_ATTR_GUARDSIZE_BITS)};

	/* valid value */
	zassert_ok(pthread_attr_getguardsize(&attr, &guardsize));

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setguardsize(NULL, SIZE_MAX), EINVAL);
			zassert_equal(pthread_attr_setguardsize(NULL, guardsize), EINVAL);
			zassert_equal(pthread_attr_setguardsize((pthread_attr_t *)&uninit_attr,
								guardsize),
				      EINVAL);
		}
		zassert_equal(pthread_attr_setguardsize(&attr, SIZE_MAX), EINVAL);
	}

	ARRAY_FOR_EACH(sizes, i) {
		zassert_ok(pthread_attr_setguardsize(&attr, sizes[i]));
		guardsize = ~sizes[i];
		zassert_ok(pthread_attr_getguardsize(&attr, &guardsize));
		zassert_equal(guardsize, sizes[i]);
	}
}

ZTEST(posix_threads_base, test_pthread_attr_getschedparam)
{
	struct sched_param param = {
		.sched_priority = BIOS_FOOD,
	};

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getschedparam(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getschedparam(NULL, &param), EINVAL);
			zassert_equal(pthread_attr_getschedparam(&uninit_attr, &param), EINVAL);
		}
		zassert_equal(pthread_attr_getschedparam(&attr, NULL), EINVAL);
	}

	/* only check to see that the function succeeds and sets param */
	zassert_ok(pthread_attr_getschedparam(&attr, &param));
	zassert_not_equal(BIOS_FOOD, param.sched_priority);
}

ZTEST(posix_threads_base, test_pthread_attr_setschedparam)
{
	struct sched_param param = {0};

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setschedparam(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_setschedparam(NULL, &param), EINVAL);
			zassert_equal(
				pthread_attr_setschedparam((pthread_attr_t *)&uninit_attr, &param),
				EINVAL);
		}
		zassert_equal(pthread_attr_setschedparam(&attr, NULL), EINVAL);
	}

	zassert_ok(pthread_attr_setschedparam(&attr, &param));

	can_create_thread(&attr);
}

ZTEST(posix_threads_base, test_pthread_attr_getstack)
{
	void *stackaddr = (void *)BIOS_FOOD;
	size_t stacksize = BIOS_FOOD;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getstack(NULL, NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getstack(NULL, NULL, &stacksize), EINVAL);
			zassert_equal(pthread_attr_getstack(NULL, &stackaddr, NULL), EINVAL);
			zassert_equal(pthread_attr_getstack(NULL, &stackaddr, &stacksize), EINVAL);
			zassert_equal(pthread_attr_getstack(&uninit_attr, &stackaddr, &stacksize),
				      EINVAL);
		}
		zassert_equal(pthread_attr_getstack(&attr, NULL, NULL), EINVAL);
		zassert_equal(pthread_attr_getstack(&attr, NULL, &stacksize), EINVAL);
		zassert_equal(pthread_attr_getstack(&attr, &stackaddr, NULL), EINVAL);
	}

	zassert_ok(pthread_attr_getstack(&attr, &stackaddr, &stacksize));
	zassert_not_equal(stackaddr, (void *)BIOS_FOOD);
	zassert_not_equal(stacksize, BIOS_FOOD);
}

ZTEST(posix_threads_base, test_pthread_attr_setstack)
{
	void *stackaddr;
	size_t stacksize;
	void *new_stackaddr;
	size_t new_stacksize;

	/* valid values */
	zassert_ok(pthread_attr_getstack(&attr, &stackaddr, &stacksize));

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setstack(NULL, NULL, 0), EACCES);
			zassert_equal(pthread_attr_setstack(NULL, NULL, stacksize), EINVAL);
			zassert_equal(pthread_attr_setstack(NULL, stackaddr, 0), EINVAL);
			zassert_equal(pthread_attr_setstack(NULL, stackaddr, stacksize), EINVAL);
			zassert_equal(pthread_attr_setstack((pthread_attr_t *)&uninit_attr,
							    stackaddr, stacksize),
				      EINVAL);
		}
		zassert_equal(pthread_attr_setstack(&attr, NULL, 0), EACCES);
		zassert_equal(pthread_attr_setstack(&attr, NULL, stacksize), EACCES);
		zassert_equal(pthread_attr_setstack(&attr, stackaddr, 0), EINVAL);
	}

	/* ensure we can create and join a thread with the default attrs */
	can_create_thread(&attr);

	/* set stack / addr to the current values of stack / addr */
	zassert_ok(pthread_attr_setstack(&attr, stackaddr, stacksize));
	can_create_thread(&attr);

	/* qemu_x86 seems to be unable to set thread stacks to be anything less than 4096 */
	if (!IS_ENABLED(CONFIG_X86)) {
		/*
		 * check we can set a smaller stacksize
		 * should not require dynamic reallocation
		 * size may get rounded up to some alignment internally
		 */
		zassert_ok(pthread_attr_setstack(&attr, stackaddr, stacksize - 1));
		/* ensure we read back the same values as we specified */
		zassert_ok(pthread_attr_getstack(&attr, &new_stackaddr, &new_stacksize));
		zassert_equal(new_stackaddr, stackaddr);
		zassert_equal(new_stacksize, stacksize - 1);
		can_create_thread(&attr);
	}

	if (IS_ENABLED(DYNAMIC_THREAD_ALLOC)) {
		/* ensure we can set a dynamic stack */
		k_thread_stack_t *stack;

		stack = k_thread_stack_alloc(2 * stacksize, 0);
		zassert_not_null(stack);

		zassert_ok(pthread_attr_setstack(&attr, (void *)stack, 2 * stacksize));
		/* ensure we read back the same values as we specified */
		zassert_ok(pthread_attr_getstack(&attr, &new_stackaddr, &new_stacksize));
		zassert_equal(new_stackaddr, (void *)stack);
		zassert_equal(new_stacksize, 2 * stacksize);
		can_create_thread(&attr);
	}
}

ZTEST(posix_threads_base, test_pthread_attr_getstacksize)
{
	size_t stacksize = BIOS_FOOD;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getstacksize(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getstacksize(NULL, &stacksize), EINVAL);
			zassert_equal(pthread_attr_getstacksize(&uninit_attr, &stacksize), EINVAL);
		}
		zassert_equal(pthread_attr_getstacksize(&attr, NULL), EINVAL);
	}

	zassert_ok(pthread_attr_getstacksize(&attr, &stacksize));
	zassert_not_equal(stacksize, BIOS_FOOD);
}

ZTEST(posix_threads_base, test_pthread_attr_setstacksize)
{
	size_t stacksize;
	size_t new_stacksize;

	/* valid size */
	zassert_ok(pthread_attr_getstacksize(&attr, &stacksize));

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setstacksize(NULL, 0), EINVAL);
			zassert_equal(pthread_attr_setstacksize(NULL, stacksize), EINVAL);
			zassert_equal(pthread_attr_setstacksize((pthread_attr_t *)&uninit_attr,
								stacksize),
				      EINVAL);
		}
		zassert_equal(pthread_attr_setstacksize(&attr, 0), EINVAL);
	}

	/* ensure we can spin up a thread with the default stack size */
	can_create_thread(&attr);

	/* set stack / addr to the current values of stack / addr */
	zassert_ok(pthread_attr_setstacksize(&attr, stacksize));
	/* ensure we can read back the values we just set */
	zassert_ok(pthread_attr_getstacksize(&attr, &new_stacksize));
	zassert_equal(new_stacksize, stacksize);
	can_create_thread(&attr);

	/* qemu_x86 seems to be unable to set thread stacks to be anything less than 4096 */
	if (!IS_ENABLED(CONFIG_X86)) {
		zassert_ok(pthread_attr_setstacksize(&attr, stacksize - 1));
		/* ensure we can read back the values we just set */
		zassert_ok(pthread_attr_getstacksize(&attr, &new_stacksize));
		zassert_equal(new_stacksize, stacksize - 1);
		can_create_thread(&attr);
	}

	if (IS_ENABLED(CONFIG_DYNAMIC_THREAD_ALLOC)) {
		zassert_ok(pthread_attr_setstacksize(&attr, 2 * stacksize));
		/* ensure we read back the same values as we specified */
		zassert_ok(pthread_attr_getstacksize(&attr, &new_stacksize));
		zassert_equal(new_stacksize, 2 * stacksize);
		can_create_thread(&attr);
	}
}

ZTEST(posix_threads_base, test_pthread_attr_large_stacksize)
{
	size_t actual_size;
	const size_t expect_size = BIT(CONFIG_POSIX_PTHREAD_ATTR_STACKSIZE_BITS);

	if (pthread_attr_setstacksize(&attr, expect_size) != 0) {
		TC_PRINT("Unable to allocate large stack of size %zu (skipping)\n", expect_size);
		ztest_test_skip();
		return;
	}

	zassert_ok(pthread_attr_getstacksize(&attr, &actual_size));
	zassert_equal(actual_size, expect_size);
}

ZTEST(posix_threads_base, test_pthread_attr_getdetachstate)
{
	int detachstate;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getdetachstate(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getdetachstate(NULL, &detachstate), EINVAL);
			zassert_equal(pthread_attr_getdetachstate(&uninit_attr, &detachstate),
				      EINVAL);
		}
		zassert_equal(pthread_attr_getdetachstate(&attr, NULL), EINVAL);
	}

	/* default detachstate is joinable */
	zassert_ok(pthread_attr_getdetachstate(&attr, &detachstate));
	zassert_equal(detachstate, PTHREAD_CREATE_JOINABLE);
	can_create_thread(&attr);
}

ZTEST(posix_threads_base, test_pthread_attr_setdetachstate)
{
	int detachstate = PTHREAD_CREATE_JOINABLE;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setdetachstate(NULL, INVALID_DETACHSTATE),
				      EINVAL);
			zassert_equal(pthread_attr_setdetachstate(NULL, detachstate), EINVAL);
			zassert_equal(pthread_attr_setdetachstate((pthread_attr_t *)&uninit_attr,
								  detachstate),
				      EINVAL);
		}
		zassert_equal(pthread_attr_setdetachstate(&attr, INVALID_DETACHSTATE), EINVAL);
	}

	/* read back detachstate just written */
	zassert_ok(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	zassert_ok(pthread_attr_getdetachstate(&attr, &detachstate));
	zassert_equal(detachstate, PTHREAD_CREATE_DETACHED);
	create_thread_common(&attr, true, false);
}
