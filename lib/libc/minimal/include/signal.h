/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_POSIX_SIGNAL_H_
#define ZEPHYR_INCLUDE_POSIX_SIGNAL_H_

#include <stddef.h>
#include <sys/types.h>
#include <sys/_timespec.h>
#include <sys/signal.h>

#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_DFL ((void *)0)
#define SIG_IGN ((void *)1)
#define SIG_ERR ((void *)-1)

#if defined(_POSIX_THREADS) && !defined(__pthread_t_defined) && !defined(_PTHREAD_T_DECLARED)
#define __pthread_t_defined
#define _PTHREAD_T_DECLARED
typedef unsigned int pthread_t;
#endif

/* size_t defined in stddef.h */

#if defined(_POSIX_C_SOURCE) && !defined(__uid_t_defined) && !defined(_UID_T_DECLARED)
#define __uid_t_defined
#define _UID_T_DECLARED
typedef int uid_t;
#endif

/* timespec defined in sys/_timespec.h */

typedef int	sig_atomic_t;

/* sigset_t defined in sys/_sigset.h */

#if defined(_POSIX_C_SOURCE) && !defined(__pid_t_defined) && !defined(_PID_T_DECLARED)
#define __pid_t_defined
#define _PID_T_DECLARED
typedef unsigned long pid_t; 
#endif

union sigval {
	void *sival_ptr;
	int sival_int;
};

struct sigevent {
	void (*sigev_notify_function)(union sigval val);
	pthread_attr_t *sigev_notify_attributes;
	union sigval sigev_value;
	int sigev_notify;
	int sigev_signo;
};

typedef struct {
	int si_signo;
	int si_code;
	union sigval si_value;
} siginfo_t;

struct sigaction {
	void (*sa_handler)(int signno);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_sigaction)(int signo, siginfo_t *info, void *context);
};

typedef void (*sighandler_t)(int signo);

unsigned int alarm(unsigned int seconds);
int kill(pid_t pid, int sig);
int pause(void);
int raise(int signo);
TOOLCHAIN_DISABLE_WARNING(TOOLCHAIN_WARNING_SHADOW);
int sigaction(int sig, const struct sigaction *ZRESTRICT act, struct sigaction *ZRESTRICT oact);
TOOLCHAIN_ENABLE_WARNING(TOOLCHAIN_WARNING_SHADOW);
int sigpending(sigset_t *set);
int sigsuspend(const sigset_t *sigmask);
int sigwait(const sigset_t *ZRESTRICT set, int *ZRESTRICT signo);
char *strsignal(int signum);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
int sigismember(const sigset_t *set, int signo);
sighandler_t signal(int signo, sighandler_t handler);
int sigprocmask(int how, const sigset_t *ZRESTRICT set, sigset_t *ZRESTRICT oset);

int pthread_sigmask(int how, const sigset_t *ZRESTRICT set, sigset_t *ZRESTRICT oset);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_POSIX_SIGNAL_H_ */
