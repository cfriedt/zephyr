/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <pthread.h>
#include <stddef.h>

#include <zephyr/ztest.h>

#define PTHREAD_SCOPE_INVALID (-1)
#define SCHED_INVALID (-1)

extern void create_thread_common_entry(const pthread_attr_t *attrp, bool expect_success, bool joinable,
    void *(*entry)(void *arg), void *arg);
extern void can_create_thread(const pthread_attr_t *attrp);

pthread_attr_t attr;

ZTEST(xsi_realtime_threads, test_pthread_attr_getscope)
{
	int contentionscope = PTHREAD_SCOPE_INVALID;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zexpect_equal(pthread_attr_getscope(NULL, NULL), EINVAL);
			zexpect_equal(pthread_attr_getscope(NULL, &contentionscope), EINVAL);
			zexpect_equal(pthread_attr_getscope(&attr, &contentionscope), EINVAL);
		}
		zexpect_equal(pthread_attr_getscope(&attr, NULL), EINVAL);
	}

    zexpect_ok(pthread_attr_init(&attr));
	zexpect_ok(pthread_attr_getscope(&attr, &contentionscope));
	zexpect_equal(contentionscope, PTHREAD_SCOPE_SYSTEM);
}

ZTEST(xsi_realtime_threads, test_pthread_attr_setscope)
{
	int contentionscope = PTHREAD_SCOPE_INVALID;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zexpect_equal(pthread_attr_setscope(NULL, PTHREAD_SCOPE_INVALID), EINVAL);
			zexpect_equal(pthread_attr_setscope(NULL, PTHREAD_SCOPE_SYSTEM), EINVAL);
			zexpect_equal(pthread_attr_setscope(&attr, PTHREAD_SCOPE_INVALID), EINVAL);
		}
		zexpect_equal(pthread_attr_setscope(&attr, 3), EINVAL);
	}

    /* valid cases */
    zexpect_ok(pthread_attr_init(&attr));
	zexpect_equal(pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS), ENOTSUP);
	zexpect_ok(pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM));
	zexpect_ok(pthread_attr_getscope(&attr, &contentionscope));
	zexpect_equal(contentionscope, PTHREAD_SCOPE_SYSTEM);
}

ZTEST(xsi_realtime_threads, test_pthread_attr_getinheritsched)
{
	int inheritsched = PTHREAD_SCOPE_INVALID;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zexpect_equal(pthread_attr_getinheritsched(NULL, NULL), EINVAL);
			zexpect_equal(pthread_attr_getinheritsched(NULL, &inheritsched), EINVAL);
			zexpect_equal(pthread_attr_getinheritsched(&attr, &inheritsched), EINVAL);
		}
		zexpect_equal(pthread_attr_getinheritsched(&attr, NULL), EINVAL);
	}

    zexpect_ok(pthread_attr_init(&attr));
	zexpect_ok(pthread_attr_getinheritsched(&attr, &inheritsched));
	zexpect_equal(inheritsched, PTHREAD_INHERIT_SCHED);
}

static void *inheritsched_entry(void *arg)
{
	int prio;
	int inheritsched;
	int pprio = POINTER_TO_INT(arg);

	zexpect_ok(pthread_attr_getinheritsched(&attr, &inheritsched));

	prio = k_thread_priority_get(k_current_get());

	if (inheritsched == PTHREAD_INHERIT_SCHED) {
		/*
		 * There will be numerical overlap between posix priorities in different scheduler
		 * policies so only check the Zephyr priority here. The posix policy and posix
		 * priority are derived from the Zephyr priority in any case.
		 */
		zexpect_equal(prio, pprio, "actual priority: %d, expected priority: %d", prio,
			      pprio);
		return NULL;
	}

	/* inheritsched == PTHREAD_EXPLICIT_SCHED */
	int act_prio;
	int exp_prio;
	int act_policy;
	int exp_policy;
	struct sched_param param;

	/* get the actual policy, param, etc */
	zexpect_ok(pthread_getschedparam(pthread_self(), &act_policy, &param));
	act_prio = param.sched_priority;

	/* get the expected policy, param, etc */
	zexpect_ok(pthread_attr_getschedpolicy(&attr, &exp_policy));
	zexpect_ok(pthread_attr_getschedparam(&attr, &param));
	exp_prio = param.sched_priority;

	/* compare actual vs expected */
	zexpect_equal(act_policy, exp_policy, "actual policy: %d, expected policy: %d", act_policy,
		      exp_policy);
	zexpect_equal(act_prio, exp_prio, "actual priority: %d, expected priority: %d", act_prio,
		      exp_prio);

	return NULL;
}

static void test_pthread_attr_setinheritsched_common(bool inheritsched)
{
	int prio;
	int policy;
	struct sched_param param;

	extern int zephyr_to_posix_priority(int priority, int *policy);

	prio = k_thread_priority_get(k_current_get());
	zexpect_not_equal(prio, K_LOWEST_APPLICATION_THREAD_PRIO);

	/*
	 * values affected by inheritsched are policy / priority / contentionscope
	 *
	 * we only support PTHREAD_SCOPE_SYSTEM, so no need to set contentionscope
	 */
	prio = K_LOWEST_APPLICATION_THREAD_PRIO;
	param.sched_priority = zephyr_to_posix_priority(prio, &policy);

	zexpect_ok(pthread_attr_setschedpolicy(&attr, policy));
	zexpect_ok(pthread_attr_setschedparam(&attr, &param));
	zexpect_ok(pthread_attr_setinheritsched(&attr, inheritsched));
	create_thread_common_entry(&attr, true, true, inheritsched_entry,
				   UINT_TO_POINTER(k_thread_priority_get(k_current_get())));
}

ZTEST(xsi_realtime_threads, test_pthread_attr_setinheritsched)
{
	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zexpect_equal(pthread_attr_setinheritsched(NULL, PTHREAD_EXPLICIT_SCHED), EINVAL);
			zexpect_equal(pthread_attr_setinheritsched(NULL, PTHREAD_INHERIT_SCHED), EINVAL);
			zexpect_equal(pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED), EINVAL);
		}
		zexpect_equal(pthread_attr_setinheritsched(&attr, 3), EINVAL);
	}

	/* valid cases */
    zexpect_ok(pthread_attr_init(&attr));
	test_pthread_attr_setinheritsched_common(PTHREAD_INHERIT_SCHED);
	test_pthread_attr_setinheritsched_common(PTHREAD_EXPLICIT_SCHED);
}

ZTEST(pthread_attr, test_pthread_attr_getschedpolicy)
{
	int policy = SCHED_INVALID;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getschedpolicy(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getschedpolicy(NULL, &policy), EINVAL);
			zassert_equal(pthread_attr_getschedpolicy(&attr, &policy), EINVAL);
		}
		zassert_equal(pthread_attr_getschedpolicy(&attr, NULL), EINVAL);
	}

	/* only check to see that the function succeeds and sets policy */
    zexpect_ok(pthread_attr_init(&attr));
	zassert_ok(pthread_attr_getschedpolicy(&attr, &policy));
	zassert_not_equal(SCHED_INVALID
        , policy);
}

ZTEST(pthread_attr, test_pthread_attr_setschedpolicy)
{
	int policy = SCHED_OTHER;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setschedpolicy(NULL, SCHED_INVALID), EINVAL);
			zassert_equal(pthread_attr_setschedpolicy(NULL, policy), EINVAL);
			zassert_equal(
				pthread_attr_setschedpolicy(&attr, policy),
				EINVAL);
		}
		zassert_equal(pthread_attr_setschedpolicy(&attr, SCHED_INVALID), EINVAL);
	}

    zexpect_ok(pthread_attr_init(&attr));
	zassert_ok(pthread_attr_setschedpolicy(&attr, SCHED_OTHER));
	/* read back the same policy we just wrote */
	policy = SCHED_INVALID;
	zassert_ok(pthread_attr_getschedpolicy(&attr, &policy));
	zassert_equal(policy, SCHED_OTHER);

	can_create_thread(&attr);
}

ZTEST(xsi_realtime_threads, test_pthread_getschedparam)
{
    int policy;
    struct sched_param param;

	{
        /* degenerate cases */
        zassert_not_ok(pthread_getschedparam((int)(uintptr_t)NULL, NULL, NULL));
        zassert_not_ok(pthread_getschedparam((int)(uintptr_t)NULL, NULL, &param));
        zassert_not_ok(pthread_getschedparam((int)(uintptr_t)NULL, &policy, NULL));
        zassert_not_ok(pthread_getschedparam((int)(uintptr_t)NULL, &policy, &param));
        zassert_not_ok(pthread_getschedparam(pthread_self(), NULL, NULL));
        zassert_not_ok(pthread_getschedparam(pthread_self(), NULL, &param));
        zassert_not_ok(pthread_getschedparam(pthread_self(), &policy, NULL));
    }

    zassert_ok(pthread_getschedparam(pthread_self(), &policy, &param));
}

ZTEST(xsi_realtime_threads, test_pthread_mutex_getprioceiling)
{
    zassert_equal(pthread_mutex_getprioceiling(NULL, NULL), ENOSYS);
}

ZTEST(xsi_realtime_threads, test_pthread_mutex_setprioceiling)
{
    zassert_equal(pthread_mutex_setprioceiling(NULL, 0, NULL), ENOSYS);
}

ZTEST(xsi_realtime_threads, test_pthread_mutexattr_getprioceiling)
{
    zassert_equal(pthread_mutexattr_getprioceiling(NULL, NULL), ENOSYS);
}

ZTEST(xsi_realtime_threads, test_pthread_mutexattr_getprotocol)
{
    ztest_test_fail();
}

ZTEST(xsi_realtime_threads, test_pthread_mutexattr_setprioceiling)
{
    zassert_equal(pthread_mutexattr_setprioceiling(NULL, 0), ENOSYS);
}

ZTEST(xsi_realtime_threads, test_pthread_mutexattr_setprotocol)
{
	ztest_test_fail();
}

ZTEST(xsi_realtime_threads, test_pthread_setschedparam)
{
	ztest_test_fail();
}

ZTEST(xsi_realtime_threads, test_pthread_setschedprio)
{
	ztest_test_fail();
}

static void after(void *data)
{
	ARG_UNUSED(data);

	(void)pthread_attr_destroy(&attr);
}

ZTEST_SUITE(xsi_realtime_threads, NULL, NULL, NULL, after, NULL);
