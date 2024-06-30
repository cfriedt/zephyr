/*
 * Copyright (c) 2023, Meta
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIB_LIBC_COMMON_INCLUDE_SYS_SIGNAL_H_
#define LIB_LIBC_COMMON_INCLUDE_SYS_SIGNAL_H_

/* exclude external libc sys/signal.h */
#define _SYS_SIGNAL_H_

#ifdef _POSIX_C_SOURCE
#include <sys/features.h>
#include <zephyr/posix/sys/_pthreadtypes.h>
#endif

#include <sys/_sigset.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIGHUP    1  /**< Hangup */
#define SIGINT    2  /**< Interrupt */
#define SIGQUIT   3  /**< Quit */
#define SIGILL    4  /**< Illegal instruction */
#define SIGTRAP   5  /**< Trace/breakpoint trap */
#define SIGABRT   6  /**< Aborted */
#define SIGBUS    7  /**< Bus error */
#define SIGFPE    8  /**< Arithmetic exception */
#define SIGKILL   9  /**< Killed */
#define SIGUSR1   10 /**< User-defined signal 1 */
#define SIGSEGV   11 /**< Invalid memory reference */
#define SIGUSR2   12 /**< User-defined signal 2 */
#define SIGPIPE   13 /**< Broken pipe */
#define SIGALRM   14 /**< Alarm clock */
#define SIGTERM   15 /**< Terminated */
/* 16 not used */
#define SIGCHLD   17 /**< Child status changed */
#define SIGCONT   18 /**< Continued */
#define SIGSTOP   19 /**< Stop executing */
#define SIGTSTP   20 /**< Stopped */
#define SIGTTIN   21 /**< Stopped (read) */
#define SIGTTOU   22 /**< Stopped (write) */
#define SIGURG    23 /**< Urgent I/O condition */
#define SIGXCPU   24 /**< CPU time limit exceeded */
#define SIGXFSZ   25 /**< File size limit exceeded */
#define SIGVTALRM 26 /**< Virtual timer expired */
#define SIGPROF   27 /**< Profiling timer expired */
/* 28 not used */
#define SIGPOLL   29 /**< Pollable event occurred */
/* 30 not used */
#define SIGSYS    31 /**< Bad system call */

#define SIGRTMIN 32
#define SIGRTMAX (SIGRTMIN + RTSIG_MAX)
#define _NSIG    (SIGRTMAX + 1)

#if defined(_POSIX_REALTIME_SIGNALS) || (_POSIX_C_SOURCE >= 199309L) || defined(__DOXYGEN__)

#define SIGEV_NONE   1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

#define SI_USER    1
#define SI_QUEUE   2
#define SI_TIMER   3
#define SI_ASYNCIO 4
#define SI_MESGQ   5

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
#endif

struct sigaction {
	void (*sa_handler)(int signno);
#if defined(_POSIX_REALTIME_SIGNALS) || (_POSIX_C_SOURCE >= 199309L) || defined(__DOXYGEN__)
	void (*sa_sigaction)(int signo, siginfo_t *info, void *context);
#endif
	sigset_t sa_mask;
	int sa_flags;
};

#if defined(_POSIX_C_SOURCE)

#define SIG_BLOCK   0
#define SIG_SETMASK 1
#define SIG_UNBLOCK 2

int kill(pid_t pid, int sig);
#if (_POSIX_C_SOURCE >= 199506L) || defined(__DOXYGEN__)
int pthread_sigmask(int how, const sigset_t *ZRESTRICT set, sigset_t *ZRESTRICT oset);
#endif
int sigaction(int sig, const struct sigaction *ZRESTRICT act, struct sigaction *ZRESTRICT oact);
char *strsignal(int signum);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigismember(const sigset_t *set, int signo);
int sigpending(sigset_t *set);
int sigprocmask(int how, const sigset_t *ZRESTRICT set, sigset_t *ZRESTRICT oset);
int sigsuspend(const sigset_t *sigmask);
int sigwait(const sigset_t *ZRESTRICT set, int *ZRESTRICT signo);

#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_POSIX_SIGNAL_H_ */
