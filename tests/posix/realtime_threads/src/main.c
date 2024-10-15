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
#define PRIO_INVALID        -1

static bool attr_valid;
static pthread_attr_t attr;
static const pthread_attr_t uninit_attr;
static bool detached_thread_has_finished;

/* TODO: this should be optional */
K_THREAD_STACK_DEFINE(static_thread_stack, STATIC_THREAD_STACK_SIZE);

/* Test the internal priority conversion functions */
int zephyr_to_posix_priority(int z_prio, int *policy);
int posix_to_zephyr_priority(int priority, int policy);
ZTEST(posix_threads_base, test_pthread_priority_conversion)
{
	/*
	 *    ZEPHYR [-CONFIG_NUM_COOP_PRIORITIES, -1]
	 *                       TO
	 * POSIX(FIFO) [0, CONFIG_NUM_COOP_PRIORITIES - 1]
	 */
	for (int z_prio = -CONFIG_NUM_COOP_PRIORITIES, prio = CONFIG_NUM_COOP_PRIORITIES - 1,
		 p_prio, policy;
	     z_prio <= -1; z_prio++, prio--) {
		p_prio = zephyr_to_posix_priority(z_prio, &policy);
		zassert_equal(policy, SCHED_FIFO);
		zassert_equal(p_prio, prio, "%d %d\n", p_prio, prio);
		zassert_equal(z_prio, posix_to_zephyr_priority(p_prio, SCHED_FIFO));
	}

	/*
	 *  ZEPHYR [0, CONFIG_NUM_PREEMPT_PRIORITIES - 1]
	 *                      TO
	 * POSIX(RR) [0, CONFIG_NUM_PREEMPT_PRIORITIES - 1]
	 */
	for (int z_prio = 0, prio = CONFIG_NUM_PREEMPT_PRIORITIES - 1, p_prio, policy;
	     z_prio < CONFIG_NUM_PREEMPT_PRIORITIES; z_prio++, prio--) {
		p_prio = zephyr_to_posix_priority(z_prio, &policy);
		zassert_equal(policy, SCHED_RR);
		zassert_equal(p_prio, prio, "%d %d\n", p_prio, prio);
		zassert_equal(z_prio, posix_to_zephyr_priority(p_prio, SCHED_RR));
	}
}

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

ZTEST(posix_threads_base, test_pthread_attr_getschedpolicy)
{
	int policy = BIOS_FOOD;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getschedpolicy(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getschedpolicy(NULL, &policy), EINVAL);
			zassert_equal(pthread_attr_getschedpolicy(&uninit_attr, &policy), EINVAL);
		}
		zassert_equal(pthread_attr_getschedpolicy(&attr, NULL), EINVAL);
	}

	/* only check to see that the function succeeds and sets policy */
	zassert_ok(pthread_attr_getschedpolicy(&attr, &policy));
	zassert_not_equal(BIOS_FOOD, policy);
}

ZTEST(posix_threads_base, test_pthread_attr_setschedpolicy)
{
	int policy = SCHED_OTHER;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setschedpolicy(NULL, SCHED_INVALID), EINVAL);
			zassert_equal(pthread_attr_setschedpolicy(NULL, policy), EINVAL);
			zassert_equal(
				pthread_attr_setschedpolicy((pthread_attr_t *)&uninit_attr, policy),
				EINVAL);
		}
		zassert_equal(pthread_attr_setschedpolicy(&attr, SCHED_INVALID), EINVAL);
	}

	zassert_ok(pthread_attr_setschedpolicy(&attr, SCHED_OTHER));
	/* read back the same policy we just wrote */
	policy = SCHED_INVALID;
	zassert_ok(pthread_attr_getschedpolicy(&attr, &policy));
	zassert_equal(policy, SCHED_OTHER);

	can_create_thread(&attr);
}

ZTEST(posix_threads_base, test_pthread_attr_getscope)
{
	int contentionscope = BIOS_FOOD;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getscope(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getscope(NULL, &contentionscope), EINVAL);
			zassert_equal(pthread_attr_getscope(&uninit_attr, &contentionscope),
				      EINVAL);
		}
		zassert_equal(pthread_attr_getscope(&attr, NULL), EINVAL);
	}

	zassert_ok(pthread_attr_getscope(&attr, &contentionscope));
	zassert_equal(contentionscope, PTHREAD_SCOPE_SYSTEM);
}

ZTEST(posix_threads_base, test_pthread_attr_setscope)
{
	int contentionscope = BIOS_FOOD;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setscope(NULL, PTHREAD_SCOPE_SYSTEM), EINVAL);
			zassert_equal(pthread_attr_setscope(NULL, contentionscope), EINVAL);
			zassert_equal(pthread_attr_setscope((pthread_attr_t *)&uninit_attr,
							    contentionscope),
				      EINVAL);
		}
		zassert_equal(pthread_attr_setscope(&attr, 3), EINVAL);
	}

	zassert_equal(pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS), ENOTSUP);
	zassert_ok(pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM));
	zassert_ok(pthread_attr_getscope(&attr, &contentionscope));
	zassert_equal(contentionscope, PTHREAD_SCOPE_SYSTEM);
}

ZTEST(posix_threads_base, test_pthread_attr_getinheritsched)
{
	int inheritsched = BIOS_FOOD;

	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_getinheritsched(NULL, NULL), EINVAL);
			zassert_equal(pthread_attr_getinheritsched(NULL, &inheritsched), EINVAL);
			zassert_equal(pthread_attr_getinheritsched(&uninit_attr, &inheritsched),
				      EINVAL);
		}
		zassert_equal(pthread_attr_getinheritsched(&attr, NULL), EINVAL);
	}

	zassert_ok(pthread_attr_getinheritsched(&attr, &inheritsched));
	zassert_equal(inheritsched, PTHREAD_INHERIT_SCHED);
}

static void *inheritsched_entry(void *arg)
{
	int prio;
	int inheritsched;
	int pprio = POINTER_TO_INT(arg);

	zassert_ok(pthread_attr_getinheritsched(&attr, &inheritsched));

	prio = k_thread_priority_get(k_current_get());

	if (inheritsched == PTHREAD_INHERIT_SCHED) {
		/*
		 * There will be numerical overlap between posix priorities in different scheduler
		 * policies so only check the Zephyr priority here. The posix policy and posix
		 * priority are derived from the Zephyr priority in any case.
		 */
		zassert_equal(prio, pprio, "actual priority: %d, expected priority: %d", prio,
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
	zassert_ok(pthread_getschedparam(pthread_self(), &act_policy, &param));
	act_prio = param.sched_priority;

	/* get the expected policy, param, etc */
	zassert_ok(pthread_attr_getschedpolicy(&attr, &exp_policy));
	zassert_ok(pthread_attr_getschedparam(&attr, &param));
	exp_prio = param.sched_priority;

	/* compare actual vs expected */
	zassert_equal(act_policy, exp_policy, "actual policy: %d, expected policy: %d", act_policy,
		      exp_policy);
	zassert_equal(act_prio, exp_prio, "actual priority: %d, expected priority: %d", act_prio,
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
	zassert_not_equal(prio, K_LOWEST_APPLICATION_THREAD_PRIO);

	/*
	 * values affected by inheritsched are policy / priority / contentionscope
	 *
	 * we only support PTHREAD_SCOPE_SYSTEM, so no need to set contentionscope
	 */
	prio = K_LOWEST_APPLICATION_THREAD_PRIO;
	param.sched_priority = zephyr_to_posix_priority(prio, &policy);

	zassert_ok(pthread_attr_setschedpolicy(&attr, policy));
	zassert_ok(pthread_attr_setschedparam(&attr, &param));
	zassert_ok(pthread_attr_setinheritsched(&attr, inheritsched));
	create_thread_common_entry(&attr, true, true, inheritsched_entry,
				   UINT_TO_POINTER(k_thread_priority_get(k_current_get())));
}

ZTEST(posix_threads_base, test_pthread_attr_setinheritsched)
{
	/* degenerate cases */
	{
		if (false) {
			/* undefined behaviour */
			zassert_equal(pthread_attr_setinheritsched(NULL, PTHREAD_EXPLICIT_SCHED),
				      EINVAL);
			zassert_equal(pthread_attr_setinheritsched(NULL, PTHREAD_INHERIT_SCHED),
				      EINVAL);
			zassert_equal(pthread_attr_setinheritsched((pthread_attr_t *)&uninit_attr,
								   PTHREAD_INHERIT_SCHED),
				      EINVAL);
		}
		zassert_equal(pthread_attr_setinheritsched(&attr, 3), EINVAL);
	}

	/* valid cases */
	test_pthread_attr_setinheritsched_common(PTHREAD_INHERIT_SCHED);
	test_pthread_attr_setinheritsched_common(PTHREAD_EXPLICIT_SCHED);
}

ZTEST(posix_threads_base, test_pthread_attr_policy_and_priority_limits)
{
	int pmin = -1;
	int pmax = -1;
	struct sched_param param;
	static const int policies[] = {
		SCHED_FIFO,
		SCHED_RR,
		SCHED_OTHER,
		SCHED_INVALID,
	};
	static const char *const policy_names[] = {
		"SCHED_FIFO",
		"SCHED_RR",
		"SCHED_OTHER",
		"SCHED_INVALID",
	};
	static const bool policy_enabled[] = {
		CONFIG_NUM_COOP_PRIORITIES > 0,
		CONFIG_NUM_PREEMPT_PRIORITIES > 0,
		CONFIG_NUM_PREEMPT_PRIORITIES > 0,
		false,
	};
	static int nprio[] = {
		CONFIG_NUM_COOP_PRIORITIES,
		CONFIG_NUM_PREEMPT_PRIORITIES,
		CONFIG_NUM_PREEMPT_PRIORITIES,
		42,
	};
	const char *const prios[] = {"pmin", "pmax"};

	BUILD_ASSERT(!(SCHED_INVALID == SCHED_FIFO || SCHED_INVALID == SCHED_RR ||
		       SCHED_INVALID == SCHED_OTHER),
		     "SCHED_INVALID is itself invalid");

	ARRAY_FOR_EACH(policies, policy) {
		/* get pmin and pmax for policies[policy] */
		ARRAY_FOR_EACH(prios, i) {
			errno = 0;
			if (i == 0) {
				pmin = sched_get_priority_min(policies[policy]);
				param.sched_priority = pmin;
			} else {
				pmax = sched_get_priority_max(policies[policy]);
				param.sched_priority = pmax;
			}

			if (policy == 3) {
				/* invalid policy */
				zassert_equal(-1, param.sched_priority);
				zassert_equal(errno, EINVAL);
				continue;
			}

			zassert_not_equal(-1, param.sched_priority,
					  "sched_get_priority_%s(%s) failed: %d",
					  i == 0 ? "min" : "max", policy_names[policy], errno);
			zassert_ok(errno, "sched_get_priority_%s(%s) set errno to %s",
				   i == 0 ? "min" : "max", policy_names[policy], errno);
		}

		if (policy != 3) {
			/* this will not work for SCHED_INVALID */

			/*
			 * IEEE 1003.1-2008 Section 2.8.4
			 * conforming implementations should provide a range of at least 32
			 * priorities
			 *
			 * Note: we relax this requirement
			 */
			zassert_true(pmax > pmin, "pmax (%d) <= pmin (%d)", pmax, pmin,
				     "%s min/max inconsistency: pmin: %d pmax: %d",
				     policy_names[policy], pmin, pmax);

			/*
			 * Getting into the weeds a bit (i.e. whitebox testing), Zephyr
			 * cooperative threads use [-CONFIG_NUM_COOP_PRIORITIES,-1] and
			 * preemptive threads use [0, CONFIG_NUM_PREEMPT_PRIORITIES - 1],
			 * where the more negative thread has the higher priority. Since we
			 * cannot map those directly (a return value of -1 indicates error),
			 * we simply map those to the positive space.
			 */
			zassert_equal(pmin, 0, "unexpected pmin for %s", policy_names[policy]);
			zassert_equal(pmax, nprio[policy] - 1, "unexpected pmax for %s",
				      policy_names[policy]); /* test happy paths */
		}

		/* create threads with min and max priority levels for each policy */
		ARRAY_FOR_EACH(prios, i) {
			param.sched_priority = (i == 0) ? pmin : pmax;

			if (!policy_enabled[policy]) {
				zassert_not_ok(
					pthread_attr_setschedpolicy(&attr, policies[policy]));
				zassert_not_ok(
					pthread_attr_setschedparam(&attr, &param),
					"pthread_attr_setschedparam() failed for %s (%d) of %s",
					prios[i], param.sched_priority, policy_names[policy]);
				continue;
			}

			/* set policy */
			zassert_ok(pthread_attr_setschedpolicy(&attr, policies[policy]),
				   "pthread_attr_setschedpolicy() failed for %s (%d) of %s",
				   prios[i], param.sched_priority, policy_names[policy]);

			/* set priority */
			zassert_ok(pthread_attr_setschedparam(&attr, &param),
				   "pthread_attr_setschedparam() failed for %s (%d) of %s",
				   prios[i], param.sched_priority, policy_names[policy]);

			can_create_thread(&attr);
		}
	}
}

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

ZTEST_SUITE(xopen_realtime_threads, NULL, NULL, before, after, NULL);
