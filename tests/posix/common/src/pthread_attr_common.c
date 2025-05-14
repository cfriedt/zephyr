/*
 * Copyright (c) 2024, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

static bool detached_thread_has_finished;

static void *thread_entry(void *arg)
{
	bool joinable = (bool)POINTER_TO_UINT(arg);

	if (!joinable) {
		detached_thread_has_finished = true;
	}

	return NULL;
}

void create_thread_common_entry(const pthread_attr_t *attrp, bool expect_success,
				       bool joinable, void *(*entry)(void *arg), void *arg)
{
	pthread_t th;

	if (!joinable) {
		detached_thread_has_finished = false;
	}

	if (expect_success) {
		zexpect_ok(pthread_create(&th, attrp, entry, arg));
	} else {
		zexpect_not_ok(pthread_create(&th, attrp, entry, arg));
		return;
	}

	if (joinable) {
		zexpect_ok(pthread_join(th, NULL), "failed to join joinable thread");
		return;
	}

	/* should not be able to join detached thread */
	zexpect_not_ok(pthread_join(th, NULL));

	for (size_t i = 0; i < 10; ++i) {
		k_msleep(2 * CONFIG_PTHREAD_RECYCLER_DELAY_MS);
		if (detached_thread_has_finished) {
			break;
		}
	}

	zexpect_true(detached_thread_has_finished, "detached thread did not seem to finish");
}

static void create_thread_common(const pthread_attr_t *attrp, bool expect_success, bool joinable)
{
	create_thread_common_entry(attrp, expect_success, joinable, thread_entry,
				   UINT_TO_POINTER(joinable));
}

void can_create_thread(const pthread_attr_t *attrp)
{
	create_thread_common(attrp, true, true);
}

void cannot_create_thread(const pthread_attr_t *attrp)
{
	create_thread_common(attrp, false, true);
}
