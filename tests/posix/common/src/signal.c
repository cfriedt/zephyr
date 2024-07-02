/*
 * Copyright (c) 2023 Meta
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

/* picolibc (newlib?) only define SIGRTMIN and SIGRTMAX for rtems */
#ifndef SIGRTMIN
#define SIGRTMIN 32
#endif

#ifndef SIGRTMAX
#define SIGRTMAX (SIGRTMIN + RTSIG_MAX)
#endif

/* picolibc (newlib?) declare macros for sigemptyset(), sigaddset(), sigdelset(), sigismember() even
 * though __POSIX_VISIBLE is defined */
#undef sigaddset
#undef sigdelset
#undef sigemptyset
#undef sigfillset
#undef sigismember

ZTEST(signal, test_sigemptyset)
{
	sigset_t set;

	memset(&set, 0xff, sizeof(set));
	zassert_ok(sigemptyset(&set));

	for (int i = 1; i <= SIGRTMAX; ++i) {
		zassert_false(sigismember(&set, i), "signal %d is part of empty set", i);
	}
}

ZTEST(signal, test_sigfillset)
{
	sigset_t set;

	memset(&set, 0x00, sizeof(set));
	zassert_ok(sigfillset(&set));

	for (int i = 1; i <= SIGRTMAX; ++i) {
		zassert_true(sigismember(&set, i), "signal %d is not part of filled set", i);
	}
}

static void sigaddset_common(sigset_t *set, int signo)
{
	zassert_ok(sigemptyset(set));
	zassert_ok(sigaddset(set, signo), "failed to add signal %d", signo);
	for (int i = 1; i <= SIGRTMAX; ++i) {
		if (i == signo) {
			zassert_true(sigismember(set, i), "signal %d should be part of set", i);
		} else {
			zassert_false(sigismember(set, i), "signal %d should not be part of set",
				      i);
		}
	}
}

ZTEST(signal, test_sigaddset)
{
	sigset_t set;

	zassert_ok(sigemptyset(&set));

	printk("__POSIX_VISIBLE = %d\n", (int)__POSIX_VISIBLE);

	{
		/* Degenerate cases */
		zassert_equal(sigaddset(&set, -1), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");

		zassert_equal(sigaddset(&set, 0), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");

		zassert_equal(sigaddset(&set, SIGRTMAX + 1), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");
	}

	sigaddset_common(&set, SIGHUP);
	sigaddset_common(&set, SIGSYS);
	/* >=32, will be in the second sig set for 32bit */
	sigaddset_common(&set, SIGRTMIN);
	sigaddset_common(&set, SIGRTMAX);
}

static void sigdelset_common(sigset_t *set, int signo)
{
	zassert_ok(sigfillset(set));
	zassert_ok(sigdelset(set, signo));
	for (int i = 1; i <= SIGRTMAX; ++i) {
		if (i == signo) {
			zassert_false(sigismember(set, i), "signal %d should not be part of set",
				      i);
		} else {
			zassert_true(sigismember(set, i), "signal %d should be part of set", i);
		}
	}
}

ZTEST(signal, test_sigdelset)
{
	sigset_t set;

	zassert_ok(sigemptyset(&set));

	{
		/* Degenerate cases */
		zassert_equal(sigdelset(&set, -1), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");

		zassert_equal(sigdelset(&set, 0), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");

		zassert_equal(sigdelset(&set, SIGRTMAX + 1), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");
	}

	sigdelset_common(&set, SIGHUP);
	sigdelset_common(&set, SIGSYS);
	/* >=32, will be in the second sig set for 32bit */
	sigdelset_common(&set, SIGRTMIN);
	sigdelset_common(&set, SIGRTMAX);
}

ZTEST(signal, test_sigismember)
{
	sigset_t set;

	zassert_ok(sigemptyset(&set));

	{
		/* Degenerate cases */
		zassert_equal(sigismember(&set, -1), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");

		zassert_equal(sigismember(&set, 0), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");

		zassert_equal(sigismember(&set, SIGRTMAX + 1), -1, "rc should be -1");
		zassert_equal(errno, EINVAL, "errno should be %s", "EINVAL");
	}

	for (int i = 0; i < RTSIG_MAX; ++i) {
		zassert_equal(0, sigismember(&set, (SIGRTMIN + i)),
			      "signal %d should not be part of set", (SIGRTMIN + i));
	}

	zassert_ok(sigfillset(&set));

	for (int i = 0; i < RTSIG_MAX; ++i) {
		zassert_equal(1, sigismember(&set, (SIGRTMIN + i)),
			      "signal %d should be part of set", (SIGRTMIN + i));
	}

	zassert_ok(sigdelset(&set, SIGKILL));
	zassert_equal(sigismember(&set, SIGKILL), 0, "%s not expected to be member", "SIGKILL");
}

ZTEST(signal, test_signal_strsignal)
{
	/* Using -INT_MAX here because compiler resolves INT_MIN to (-2147483647 - 1) */
	char buf[sizeof("RT signal -" STRINGIFY(INT_MAX))] = {0};

	zassert_mem_equal(strsignal(-1), "Invalid signal", sizeof("Invalid signal"));
	zassert_mem_equal(strsignal(0), "Invalid signal", sizeof("Invalid signal"));
	zassert_mem_equal(strsignal(SIGRTMAX + 1), "Invalid signal", sizeof("Invalid signal"));

	zassert_mem_equal(strsignal(30), "Signal 30", sizeof("Signal 30"));
	snprintf(buf, sizeof(buf), "RT signal %d", SIGRTMIN - SIGRTMIN);
	zassert_mem_equal(strsignal(SIGRTMIN), buf, strlen(buf));
	snprintf(buf, sizeof(buf), "RT signal %d", SIGRTMAX - SIGRTMIN);
	zassert_mem_equal(strsignal(SIGRTMAX), buf, strlen(buf));

#ifdef CONFIG_POSIX_SIGNAL_STRING_DESC
	zassert_mem_equal(strsignal(SIGHUP), "Hangup", sizeof("Hangup"));
	zassert_mem_equal(strsignal(SIGSYS), "Bad system call", sizeof("Bad system call"));
#else
	zassert_mem_equal(strsignal(SIGHUP), "Signal 1", sizeof("Signal 1"));
	zassert_mem_equal(strsignal(SIGSYS), "Signal 31", sizeof("Signal 31"));
#endif
}

typedef int (*sigmask_fn)(int how, const sigset_t *set, sigset_t *oset);
static void *test_sigmask_entry(void *arg)
{
/* for clarity */
#define SIG_GETMASK SIG_SETMASK

	enum {
		NEW,
		OLD,
	};
	static sigset_t set[2];
	const int invalid_how = 0x9a2ba9e;
	sigmask_fn sigmask = arg;

	sigemptyset(&set[0]);
	sigemptyset(&set[1]);

	/* invalid how results in EINVAL */
	zassert_equal(sigmask(invalid_how, NULL, NULL), EINVAL);
	zassert_equal(sigmask(invalid_how, &set[NEW], &set[OLD]), EINVAL);

	/* verify setting / getting masks */
	zassert_ok(sigemptyset(&set[NEW]));
	zassert_ok(sigmask(SIG_SETMASK, &set[NEW], NULL));
	zassert_ok(sigfillset(&set[OLD]));
	zassert_ok(sigmask(SIG_GETMASK, NULL, &set[OLD]));
	zassert_mem_equal(&set[OLD], &set[NEW], sizeof(set[OLD]));

	zassert_ok(sigfillset(&set[NEW]));
	zassert_ok(sigmask(SIG_SETMASK, &set[NEW], NULL));
	zassert_ok(sigemptyset(&set[OLD]));
	zassert_ok(sigmask(SIG_GETMASK, NULL, &set[OLD]));
	zassert_mem_equal(&set[OLD], &set[NEW], sizeof(set[OLD]));

	/* start with an empty mask */
	zassert_ok(sigemptyset(&set[NEW]));
	zassert_ok(sigmask(SIG_SETMASK, &set[NEW], NULL));

	/* verify SIG_BLOCK: expect (SIGURG | SIGUSR2 | SIGHUP) */
	zassert_ok(sigemptyset(&set[NEW]));
	zassert_ok(sigaddset(&set[NEW], SIGURG));
	zassert_ok(sigmask(SIG_BLOCK, &set[NEW], NULL));

	zassert_ok(sigemptyset(&set[NEW]));
	zassert_ok(sigaddset(&set[NEW], SIGUSR2));
	zassert_ok(sigaddset(&set[NEW], SIGHUP));
	zassert_ok(sigmask(SIG_BLOCK, &set[NEW], NULL));

	zassert_ok(sigemptyset(&set[OLD]));
	zassert_ok(sigaddset(&set[OLD], SIGURG));
	zassert_ok(sigaddset(&set[OLD], SIGUSR2));
	zassert_ok(sigaddset(&set[OLD], SIGHUP));

	zassert_ok(sigmask(SIG_GETMASK, NULL, &set[NEW]));
	zassert_mem_equal(&set[NEW], &set[OLD], sizeof(set[NEW]));

	/* start with full mask */
	zassert_ok(sigfillset(&set[NEW]));
	zassert_ok(sigmask(SIG_SETMASK, &set[NEW], NULL));

	/* verify SIG_UNBLOCK: expect ~(SIGURG | SIGUSR2 | SIGHUP) */
	zassert_ok(sigemptyset(&set[NEW]));
	zassert_ok(sigaddset(&set[NEW], SIGURG));
	zassert_ok(sigmask(SIG_UNBLOCK, &set[NEW], NULL));

	zassert_ok(sigemptyset(&set[NEW]));
	zassert_ok(sigaddset(&set[NEW], SIGUSR2));
	zassert_ok(sigaddset(&set[NEW], SIGHUP));
	zassert_ok(sigmask(SIG_UNBLOCK, &set[NEW], NULL));

	zassert_ok(sigfillset(&set[OLD]));
	zassert_ok(sigdelset(&set[OLD], SIGURG));
	zassert_ok(sigdelset(&set[OLD], SIGUSR2));
	zassert_ok(sigdelset(&set[OLD], SIGHUP));

	zassert_ok(sigmask(SIG_GETMASK, NULL, &set[NEW]));
	zassert_mem_equal(&set[NEW], &set[OLD], sizeof(set[NEW]));

	return NULL;
}

ZTEST(signal, test_pthread_sigmask)
{
	pthread_t th;

	zassert_ok(pthread_create(&th, NULL, test_sigmask_entry, pthread_sigmask));
	zassert_ok(pthread_join(th, NULL));
}

ZTEST(signal, test_sigprocmask)
{
	if (IS_ENABLED(CONFIG_MULTITHREADING)) {
		if (!IS_ENABLED(CONFIG_ASSERT)) {
			zassert_not_ok(sigprocmask(SIG_SETMASK, NULL, NULL));
			zassert_equal(errno, ENOSYS);
		}
	} else {
		pthread_t th;

		zassert_ok(pthread_create(&th, NULL, test_sigmask_entry, sigprocmask));
		zassert_ok(pthread_join(th, NULL));
	}
}

static void before(void *arg)
{
	ARG_UNUSED(arg);

	if (!IS_ENABLED(CONFIG_DYNAMIC_THREAD)) {
		/* skip redundant testing if there is no thread pool / heap allocation */
		ztest_test_skip();
	}
}

ZTEST_SUITE(signal, NULL, NULL, before, NULL, NULL);
