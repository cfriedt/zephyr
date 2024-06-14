/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/posix/signal.h>

int sigqueue(pid_t pid, int signo, union sigval value)
{
	return k_sigqueue(pid, signo, value.sival_int);
}

int sigtimedwait(const sigset_t *ZRESTRICT set, siginfo_t *ZRESTRICT info,
		 const struct timespec *ZRESTRICT timeout)
{
	k_timeout_t to;

	if (timeout == NULL) {
		to = K_FOREVER;
	} else if (timeout.tv_sec == 0 && timeout.tv_nsec == 0) {
		to = K_NO_WAIT;
	} else {
		to = K_MSEC(timeout->tv_sec * MSEC_PER_SEC + timeout->tv_nsec / NSEC_PER_MSEC);
	}

	return k_sigtimedwait(set, info, to);
}

int sigwaitinfo(const sigset_t *restrict set, siginfo_t *restrict info)
{
	return k_sigtimedwait(set, info, K_FOREVER);
}
