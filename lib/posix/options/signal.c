/*
 * Copyright (c) 2023 Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "posix/strsignal_table.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include <zephyr/posix/pthread.h>
#include <zephyr/sys/util.h>

#define SIGNO_WORD_IDX(_signo) (_signo / BITS_PER_LONG)
#define SIGNO_WORD_BIT(_signo) (_signo & BIT_MASK(LOG2(BITS_PER_LONG)))

#undef SIGRTMIN
#undef SIGRTMAX

#define SIGRTMIN 32
#define SIGRTMAX                                                                                   \
	(SIGRTMIN + COND_CODE_1(CONFIG_POSIX_REALTIME_SIGNALS, (CONFIG_POSIX_RTSIG_MAX), (0)))
#define _NSIG    (SIGRTMAX + 1)

BUILD_ASSERT(CONFIG_POSIX_RTSIG_MAX >= 0);

static inline bool signo_valid(int signo)
{
	return ((signo > 0) && (signo < _NSIG));
}

static inline bool signo_is_rt(int signo)
{
#if defined(_POSIX_REALTIME_SIGNALS)
	return ((signo >= SIGRTMIN) && (signo <= SIGRTMAX));
#endif

	return false;
}

#undef sigemptyset
int sigemptyset(sigset_t *set)
{
	*set = (sigset_t){0};
	return 0;
}

#undef sigfillset
int sigfillset(sigset_t *set)
{
	unsigned long *const arr = (unsigned long *)set;

	for (size_t i = 0; i < sizeof(*set) / sizeof(*arr); ++i) {
		arr[i] = -1;
	}

	return 0;
}

#undef sigaddset
int sigaddset(sigset_t *set, int signo)
{
	unsigned long *const arr = (unsigned long *)set;

	if (!signo_valid(signo)) {
		errno = EINVAL;
		return -1;
	}

	WRITE_BIT(arr[SIGNO_WORD_IDX(signo)], SIGNO_WORD_BIT(signo), 1);

	return 0;
}

#undef sigdelset
int sigdelset(sigset_t *set, int signo)
{
	unsigned long *const arr = (unsigned long *)set;

	if (!signo_valid(signo)) {
		errno = EINVAL;
		return -1;
	}

	WRITE_BIT(arr[SIGNO_WORD_IDX(signo)], SIGNO_WORD_BIT(signo), 0);

	return 0;
}

#undef sigismember
int sigismember(const sigset_t *set, int signo)
{
	unsigned long *const arr = (unsigned long *)set;

	if (!signo_valid(signo)) {
		errno = EINVAL;
		return -1;
	}

	return 1 & (arr[SIGNO_WORD_IDX(signo)] >> SIGNO_WORD_BIT(signo));
}

char *strsignal(int signum)
{
	/* Using -INT_MAX here because compiler resolves INT_MIN to (-2147483647 - 1) */
	static char buf[sizeof("RT signal -" STRINGIFY(INT_MAX))];

	if (!signo_valid(signum)) {
		errno = EINVAL;
		return "Invalid signal";
	}

	if (signo_is_rt(signum)) {
		snprintf(buf, sizeof(buf), "RT signal %d", signum - SIGRTMIN);
		return buf;
	}

	if (IS_ENABLED(CONFIG_POSIX_SIGNAL_STRING_DESC)) {
		if (strsignal_list[signum] != NULL) {
			return (char *)strsignal_list[signum];
		}
	}

	snprintf(buf, sizeof(buf), "Signal %d", signum);

	return buf;
}

int sigprocmask(int how, const sigset_t *ZRESTRICT set, sigset_t *ZRESTRICT oset)
{
	if (!IS_ENABLED(CONFIG_MULTITHREADING)) {
		return pthread_sigmask(how, set, oset);
	}

	/*
	 * Until Zephyr supports processes and specifically querying the number of active threads in
	 * a process For more information, see
	 * https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_sigmask.html
	 */
	__ASSERT(false, "In multi-threaded environments, please use pthread_sigmask() instead of "
			"%s()", __func__);

	errno = ENOSYS;
	return -1;
}

/*
 * The functions below are provided so that conformant POSIX applications and libraries can still
 * link.
 */

unsigned int alarm(unsigned int seconds)
{
	ARG_UNUSED(seconds);
	return 0;
}

int kill(pid_t pid, int sig)
{
	ARG_UNUSED(pid);
	ARG_UNUSED(sig);
	errno = ENOSYS;
	return -1;
}
#ifdef CONFIG_POSIX_SIGNALS_ALIAS_KILL
FUNC_ALIAS(kill, _kill, int);
#endif /* CONFIG_POSIX_SIGNALS_ALIAS_KILL */

int pause(void)
{
	errno = ENOSYS;
	return -1;
}

int sigaction(int sig, const struct sigaction *ZRESTRICT act, struct sigaction *ZRESTRICT oact)
{
	ARG_UNUSED(sig);
	ARG_UNUSED(act);
	ARG_UNUSED(oact);
	errno = ENOSYS;
	return -1;
}

int sigpending(sigset_t *set)
{
	ARG_UNUSED(set);
	errno = ENOSYS;
	return -1;
}

int sigsuspend(const sigset_t *sigmask)
{
	ARG_UNUSED(sigmask);
	errno = ENOSYS;
	return -1;
}

int sigwait(const sigset_t *ZRESTRICT set, int *ZRESTRICT sig)
{
	ARG_UNUSED(set);
	ARG_UNUSED(sig);
	errno = ENOSYS;
	return -1;
}
