/*
 * Copyright (c) 2024, Tenstorrent, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

// #define STACK_SIZE (4096 + CONFIG_TEST_EXTRA_STACK_SIZE)
#define STACK_SIZE 4096

static const int min_signal_number = 1;
static int handler1_magic;

static void handler1(int sig)
{
	handler1_magic = 0xabcd1234;
}

ZTEST(signals, test_k_sigaction)
{
	int i;
	struct k_signal_action sa;
	struct k_signal_action old_sa;

	/* degenerate cases */
	{
		zassert_not_ok(k_sigaction(-1, NULL, NULL));
		zassert_not_ok(k_sigaction(-1, NULL, &old_sa));
		zassert_not_ok(k_sigaction(-1, &sa, NULL));
		zassert_not_ok(k_sigaction(-1, &sa, &old_sa));
	}

	/* valid cases */
	zassert_ok(k_sigaction(2, NULL, NULL));
	zassert_ok(k_sigaction(2, NULL, &old_sa));

	sa = (struct k_signal_action){
		.sa_handler = handler1,
	};

	/* verify we read back the same handler after setting it */
	zassert_ok(k_sigaction(2, &sa, NULL));
	zassert_ok(k_sigaction(2, NULL, &old_sa));
	zassert_equal(old_sa.sa_handler, handler1);
	/* clear signal handler */
	zassert_ok(k_sigaction(2, K_SIGACTION_REMOVE, NULL));

	/* fix up the maximum number of handlers */
	for (i = 0; i < CONFIG_SIGNAL_HANDLERS_MAX; i++) {
		zassert_ok(k_sigaction(min_signal_number + i, &sa, NULL));
		old_sa = (struct k_signal_action){0};
		zassert_ok(k_sigaction(min_signal_number + i, NULL, &old_sa));
		zassert_equal(old_sa.sa_handler, handler1);
	}

	/* should be no more signal handlers to allocate */
	zassert_equal(k_sigaction(min_signal_number + i, &sa, NULL), -ENOMEM);

	/* clear signal handlers */
	for (i = 0; i < CONFIG_SIGNAL_HANDLERS_MAX; i++) {
		zassert_ok(k_sigaction(min_signal_number + i, K_SIGACTION_REMOVE, NULL));
	}
}

static K_THREAD_STACK_DEFINE(poll_sigaltstack, 512);

ZTEST(signals, test_k_sigaltstack)
{
	struct k_signal_stack ss;
	struct k_signal_stack old_ss;

	/* degenerate cases */
	{
		ss = (struct k_signal_stack){
			.ss_sp = K_THREAD_STACK_BUFFER(poll_sigaltstack),
			/* ss_size cannot be less than the minimum stack size */
			.ss_size = 0,
		};
		zassert_equal(k_sigaltstack(&ss, NULL), -ENOMEM);

		ss = (struct k_signal_stack){
			.ss_sp = K_THREAD_STACK_BUFFER(poll_sigaltstack),
			.ss_size = K_THREAD_STACK_SIZEOF(poll_sigaltstack),
			/* flags must be 0 */
			.ss_flags = K_SIGALTSTACK_ONSTACK,
		};
		zassert_equal(k_sigaltstack(&ss, NULL), -EINVAL);
	}

	zassert_ok(k_sigaltstack(NULL, NULL));
	ss = (struct k_signal_stack){
		.ss_sp = K_THREAD_STACK_BUFFER(poll_sigaltstack),
		.ss_size = K_THREAD_STACK_SIZEOF(poll_sigaltstack),
		.ss_flags = 0,
	};
	zassert_ok(k_sigaltstack(&ss, NULL));
	zassert_ok(k_sigaltstack(NULL, &old_ss));
	zassert_mem_equal(&ss, &old_ss, sizeof(ss));
	zassert_ok(k_sigaltstack(&ss, &old_ss));

	/* clear the sigalstack */
	zassert_ok(k_sigaltstack(K_SIGALTSTACK_REMOVE, NULL));
	zassert_ok(k_sigaltstack(NULL, &old_ss));
	/* verify it was cleared */
	ss = (struct k_signal_stack){0};
	zassert_mem_equal(&ss, &old_ss, sizeof(ss));
}

ZTEST(signals, test_k_sigmask)
{
	/* see tests/posix/common/src/signal.c */
	ztest_test_pass();
}

static bool infinite_loop_thread_started;
static bool infinite_loop_thread_done;
static volatile bool infinite_loop_thread_signaled;

static void infinite_loop_sighandler(int sig)
{
	infinite_loop_thread_signaled = true;

	printk("In signal handler\n");
}

static void infinite_loop_thread_fn(void *arg1, void *arg2, void *arg3)
{
	static const struct k_signal_action sa = {
		.sa_handler = infinite_loop_sighandler,
	};

	printk("Installing signal handler\n");
	zassert_ok(k_sigaction(1, &sa, NULL));

	infinite_loop_thread_started = true;
	printk("Infinite loop thread has started\n");

	while (!infinite_loop_thread_signaled) {
		printk("In infinite loop\n");
		k_msleep(1000);
	}

	infinite_loop_thread_done = true;
}
static struct k_thread infinite_loop_thread;
static K_THREAD_STACK_DEFINE(infinite_loop_thread_stack, STACK_SIZE);

ZTEST(signals, test_k_thread_kill)
{
	printk("testing degenerate cases\n");
	/* degenerate cases */
	{
		zassert_equal(k_thread_kill(NULL, -1), -EINVAL);
		zassert_equal(k_thread_kill(NULL, 0), -EINVAL);
		zassert_equal(k_thread_kill(k_current_get(), K_NSIG), -EINVAL);
	}

	printk("killing current thread with signal 0\n");

	/*
	 * Ensure that sending signal 0 does not trigger an action.
	 * Incidentally, this is one way to check if a thread is alive.
	 */
	zassert_ok(k_thread_kill(k_current_get(), 0));

	printk("creating infinite loop thread\n");
	/* start infinite loop thread */
	k_thread_create(&infinite_loop_thread, infinite_loop_thread_stack,
			K_THREAD_STACK_SIZEOF(infinite_loop_thread_stack), infinite_loop_thread_fn,
			NULL, NULL, NULL, K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_NO_WAIT);

	k_msleep(300);

	printk("ensuring infinite loop thread started\n");

	zassert_true(infinite_loop_thread_started);

	printk("Installing signal handler\n");

	/* ensure that sending signal 0 does not terminate the thread */
	printk("Killing infinite loop thread with signal 0\n");
	zassert_ok(k_thread_kill(&infinite_loop_thread, 0));
	k_msleep(200);

	zassert_false(infinite_loop_thread_done);
	zassert_false(infinite_loop_thread_signaled);

	printk("Killing infinite loop thread with signal 1\n");
	zassert_ok(k_thread_kill(&infinite_loop_thread, 1));
	/* until we have a hook in sched.c */

	while (!infinite_loop_thread_signaled) {
		printk("Waiting for thread to join\n");
		k_msleep(1000);
	}

	zassert_ok(k_thread_join(&infinite_loop_thread, K_MSEC(300)));
	zassert_true(infinite_loop_thread_signaled);
}

ZTEST(signals, test_k_sigpending)
{
	struct k_sigset set;

	/* degenerate cases */
	{
		zassert_equal(k_sigpending(NULL), -EFAULT);
	}

	zassert_ok(k_sigpending(&set));
}

ZTEST(signals, test_k_sigsuspend)
{
	ztest_test_fail();
}

ZTEST(signals, test_k_sigtimedwait)
{
	ztest_test_fail();
}

ZTEST_SUITE(signals, NULL, NULL, NULL, NULL, NULL);
