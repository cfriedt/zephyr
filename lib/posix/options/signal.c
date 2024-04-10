/*
 * Copyright (c) 2023 Meta
 * Copyright (c) 2024, Tenstorrent
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "posix/strsignal_table.h"

#include <errno.h>
#include <stdio.h>

#include <zephyr/posix/signal.h>

#define SIGNO_WORD_IDX(_signo) (signo / BITS_PER_LONG)
#define SIGNO_WORD_BIT(_signo) (signo & BIT_MASK(LOG2(BITS_PER_LONG)))

BUILD_ASSERT(CONFIG_POSIX_LIMITS_RTSIG_MAX >= 0);
BUILD_ASSERT(CONFIG_POSIX_RTSIG_MAX >= CONFIG_POSIX_LIMITS_RTSIG_MAX);

static inline bool signo_valid(int signo)
{
	return ((signo > 0) && (signo < _NSIG));
}

static inline bool signo_is_rt(int signo)
{
	return ((signo >= SIGRTMIN) && (signo <= SIGRTMAX));
}

int sigemptyset(sigset_t *set)
{
	*set = (sigset_t){0};
	return 0;
}

int sigfillset(sigset_t *set)
{
	for (int i = 0; i < ARRAY_SIZE(set->sig); i++) {
		set->sig[i] = -1;
	}

	return 0;
}

int sigaddset(sigset_t *set, int signo)
{
	if (!signo_valid(signo)) {
		errno = EINVAL;
		return -1;
	}

	WRITE_BIT(set->sig[SIGNO_WORD_IDX(signo)], SIGNO_WORD_BIT(signo), 1);

	return 0;
}

int sigdelset(sigset_t *set, int signo)
{
	if (!signo_valid(signo)) {
		errno = EINVAL;
		return -1;
	}

	WRITE_BIT(set->sig[SIGNO_WORD_IDX(signo)], SIGNO_WORD_BIT(signo), 0);

	return 0;
}

int sigismember(const sigset_t *set, int signo)
{
	if (!signo_valid(signo)) {
		errno = EINVAL;
		return -1;
	}

	return 1 & (set->sig[SIGNO_WORD_IDX(signo)] >> SIGNO_WORD_BIT(signo));
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

int sigwait(const sigset_t *ZRESTRICT set, int *ZRESTRICT sig)
{
	int ret;

	ret = sigwaitinfo(set, NULL);
	if (ret == -1) {
		return -1;
	}

	if (sig != NULL) {
		*sig = ret;
	}

	return 0;
}

int sigwaitinfo(const sigset_t *ZRESTRICT set, siginfo_t *ZRESTRICT info)
{
	return sigwaitinfo(set, info, NULL);
}

int sigtimedwait(const sigset_t *ZRESTRICT set, siginfo_t *ZRESTRICT info,
		 const struct timespec *ZRESTRICT timeout)
{
	int ret;
	k_timeout_t kto;
	sigset_t pending;

	if (set == NULL) {
		errno = EINVAL;
		return -1;
	}

	sigemptyset(&pending);
	if (memcmp(&pending, set, sizeof(*set)) == 0) {
		/* user has requested to wait on an empty set of signals */
		return 0;
	}

	ret = sigpending(&pending);
	if (ret < 0) {
		return -1;
	}

	ret = -1;
	ARRAY_FOR_EACH(set->sig, i)
	{
		size_t bits = set->sig[i] & pending.sig[i];

		if (bits == 0) {
			continue;
		}

		bits = (sizeof(bits) == sizeof(uint64_t)) ? u64_count_leading_zeros(bits)
							  : u32_count_leading_zeros(bits);

		if (info != NULL) {
			info->si_signo = i * BITS_PER_LONG + bits;
			/* TODO: capture the reason for the signal! */
			info->si_code = SI_USER;
			info->si_value.sival_int = 0;
		}

		ret = bits;
	}

	if (ret != -1) {
		return ret;
	}

	if (timeout == NULL) {
		kto = K_FOREVER;
	} else if (timeout->tv_sec == 0 && timeout->tv_nsec == 0) {
		/* equivalent of K_NO_WAIT - return immediately */
		errno = EAGAIN;
		return -1;
	} else {
		kto = timespec_to_timeout(timeout);
	}

	ret = k_sem_take(&posix_signal_sem, kto);
	if (ret < 0) {
		errno = -ret;
		ret = -1;
	}

	return ret;
}
