/*
 * Copyright (c) 2018-2023 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <pthread.h>

#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

#define PTHREAD_INVALID -1

static void *create_thread1(void *p1)
{
	/* do nothing */
	return NULL;
}

ZTEST(posix_threads_base, test_pthread_descriptor_leak)
{
	pthread_t pthread1;

	/* If we are leaking descriptors, then this loop will never complete */
	for (size_t i = 0; i < CONFIG_POSIX_THREAD_THREADS_MAX * 2; ++i) {
		zassert_ok(pthread_create(&pthread1, NULL, create_thread1, NULL),
			   "unable to create thread %zu", i);
		zassert_ok(pthread_join(pthread1, NULL), "unable to join thread %zu", i);
	}
}

ZTEST(posix_threads_base, test_sched_getparam)
{
	struct sched_param param;
	int rc = sched_getparam(0, &param);
	int err = errno;

	zassert_true((rc == -1 && err == ENOSYS));
}

ZTEST(posix_threads_base, test_sched_getscheduler)
{
	int rc = sched_getscheduler(0);
	int err = errno;

	zassert_true((rc == -1 && err == ENOSYS));
}
ZTEST(posix_threads_base, test_sched_setparam)
{
	struct sched_param param = {
		.sched_priority = 2,
	};
	int rc = sched_setparam(0, &param);
	int err = errno;

	zassert_true((rc == -1 && err == ENOSYS));
}

ZTEST(posix_threads_base, test_sched_setscheduler)
{
	struct sched_param param = {
		.sched_priority = 2,
	};
	int policy = 0;
	int rc = sched_setscheduler(0, policy, &param);
	int err = errno;

	zassert_true((rc == -1 && err == ENOSYS));
}

ZTEST(posix_threads_base, test_sched_rr_get_interval)
{
	struct timespec interval = {
		.tv_sec = 0,
		.tv_nsec = 0,
	};
	int rc = sched_rr_get_interval(0, &interval);
	int err = errno;

	zassert_true((rc == -1 && err == ENOSYS));
}

ZTEST(posix_threads_base, test_pthread_equal)
{
	zassert_true(pthread_equal(pthread_self(), pthread_self()));
	zassert_false(pthread_equal(pthread_self(), (pthread_t)4242));
}

ZTEST(posix_threads_base, test_pthread_set_get_concurrency)
{
	/* EINVAL if the value specified by new_level is negative */
	zassert_equal(EINVAL, pthread_setconcurrency(-42));

	/*
	 * Note: the special value 0 indicates the implementation will
	 * maintain the concurrency level at its own discretion.
	 *
	 * pthread_getconcurrency() should return a value of 0 on init.
	 */
	zassert_equal(0, pthread_getconcurrency());

	for (int i = 0; i <= CONFIG_MP_MAX_NUM_CPUS; ++i) {
		zassert_ok(pthread_setconcurrency(i));
		/* verify parameter is saved */
		zassert_equal(i, pthread_getconcurrency());
	}

	/* EAGAIN if the a system resource to be exceeded */
	zassert_equal(EAGAIN, pthread_setconcurrency(CONFIG_MP_MAX_NUM_CPUS + 1));
}

static void cleanup_handler(void *arg)
{
	bool *boolp = (bool *)arg;

	*boolp = true;
}

static void *test_pthread_cleanup_entry(void *arg)
{
	bool executed[2] = {0};

	pthread_cleanup_push(cleanup_handler, &executed[0]);
	pthread_cleanup_push(cleanup_handler, &executed[1]);
	pthread_cleanup_pop(false);
	pthread_cleanup_pop(true);

	zassert_true(executed[0]);
	zassert_false(executed[1]);

	return NULL;
}

ZTEST(posix_threads_base, test_pthread_cleanup)
{
	pthread_t th;

	zassert_ok(pthread_create(&th, NULL, test_pthread_cleanup_entry, NULL));
	zassert_ok(pthread_join(th, NULL));
}

static bool testcancel_ignored;
static bool testcancel_failed;

static void *test_pthread_cancel_fn(void *arg)
{
	zassert_ok(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));

	testcancel_ignored = false;

	/* this should be ignored */
	pthread_testcancel();

	testcancel_ignored = true;

	/* this will mark it pending */
	zassert_ok(pthread_cancel(pthread_self()));

	/* enable the thread to be cancelled */
	zassert_ok(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));

	testcancel_failed = false;

	/* this should terminate the thread */
	pthread_testcancel();

	testcancel_failed = true;

	return NULL;
}

ZTEST(posix_threads_base, test_pthread_testcancel)
{
	pthread_t th;

	zassert_ok(pthread_create(&th, NULL, test_pthread_cancel_fn, NULL));
	zassert_ok(pthread_join(th, NULL));
	zassert_true(testcancel_ignored);
	zassert_false(testcancel_failed);
}

static void *test_pthread_setschedprio_fn(void *arg)
{
	int policy;
	int prio = 0;
	struct sched_param param;
	pthread_t self = pthread_self();

	zassert_equal(pthread_setschedprio(self, PRIO_INVALID), EINVAL, "EINVAL was expected");
	zassert_equal(pthread_setschedprio(PTHREAD_INVALID, prio), ESRCH, "ESRCH was expected");

	zassert_ok(pthread_setschedprio(self, prio));
	param.sched_priority = ~prio;
	zassert_ok(pthread_getschedparam(self, &policy, &param));
	zassert_equal(param.sched_priority, prio, "Priority unchanged");

	return NULL;
}

ZTEST(posix_threads_base, test_pthread_setschedprio)
{
	pthread_t th;

	zassert_ok(pthread_create(&th, NULL, test_pthread_setschedprio_fn, NULL));
	zassert_ok(pthread_join(th, NULL));
}
