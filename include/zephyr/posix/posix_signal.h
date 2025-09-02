/*
 * Copyright (c) 2025 The Zephyr Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_POSIX_POSIX_SIGNAL_H_
#define ZEPHYR_INCLUDE_POSIX_POSIX_SIGNAL_H_

#if defined(_POSIX_C_SOURCE) || defined(__DOXYGEN__)

#include <zephyr/toolchain.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SIG_DFL must be defined by the libc signal.h */
/* SIG_ERR must be defined by the libc signal.h */

#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
#define SIG_HOLD ((void *)-2)
#endif

/* SIG_IGN must be defined by the libc signal.h */

#if defined(_POSIX_THREADS) || defined(__DOXYGEN__)

#if !defined(_PTHREAD_T_DECLARED) && !defined(__pthread_t_defined)
typedef unsigned int pthread_t;
#define _PTHREAD_T_DECLARED
#define __pthread_t_defined
#endif

#endif /* defined(_POSIX_THREADS) || defined(__DOXYGEN__) */

/* size_t must be defined by the libc stddef.h */
#include <stddef.h>

#if !defined(_UID_T_DECLARED) && !defined(__uid_t_defined)
typedef int uid_t;
#define _UID_T_DECLARED
#define __uid_t_defined
#endif

#if !defined(_TIMESPEC_DECLARED) && !defined(__timespec_defined)
struct timespec {
	time_t tv_sec;
	long tv_nsec;
};
#define _TIMESPEC_DECLARED
#define __timespec_defined
#endif

/* sig_atomic_t must be defined by the libc signal.h */

/* sigset_t */
#define SIGRTMIN 32
#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)
BUILD_ASSERT(CONFIG_POSIX_RTSIG_MAX >= 0);
#define SIGRTMAX (SIGRTMIN + CONFIG_POSIX_RTSIG_MAX)
#else
#define SIGRTMAX SIGRTMIN
#endif

typedef struct {
	unsigned long sig[DIV_ROUND_UP(SIGRTMAX + 1, BITS_PER_LONG)];
} sigset_t;

#if !defined(_PID_T_DECLARED) && !defined(__pid_t_defined)
typedef long pid_t;
#define _PID_T_DECLARED
#define __pid_t_defined
#endif

#if defined(_POSIX_THREADS) || defined(__DOXYGEN__)

#if !defined(_PTHREAD_ATTR_T_DECLARED) && !defined(__pthread_attr_t_defined)
typedef struct {
	void *stack;
	unsigned int details[2];
} pthread_attr_t;
#define _PTHREAD_ATTR_T_DECLARED
#define __pthread_attr_t_defined
#endif

#endif

#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)

union sigval; /* forward declaration (to preserve spec order) */

#if !defined(_SIGEVENT_DECLARED) && !defined(__sigevent_defined)
typedef struct {
#if defined(_POSIX_THREADS) || defined(__DOXYGEN__)
	pthread_attr_t *sigev_thread_attr;
#endif
	union sigval sigev_value;
	int sigev_notify;
	int sigev_signo;
} sigevent_t;
#define _SIGEVENT_DECLARED
#define __sigevent_defined
#endif

#define SIGEV_NONE   1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

#if !defined(_SIGVAL_DECLARED) && !defined(__sigval_defined)
union sigval {
	int sival_int;
	void *sival_ptr;
};
#define _SIGVAL_DECLARED
#define __sigval_defined
#endif

#endif /* defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__) */

/* SIGRTMIN and SIGRTMAX defined above */

#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)

#if !defined(_SIGACTION_DECLARED) && !defined(__sigaction_defined)
struct sigaction {
	union {
		void (*sa_handler)(int sig);
		void (*sa_sigaction)(int sig, siginfo_t *info, void *context);
	};
	sigset_t sa_mask;
	int sa_flags;
};
#define _SIGACTION_DECLARED
#define __sigaction_defined
#endif

#define SIG_BLOCK   1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 0

#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
#define SA_NOCLDSTOP 0x00000001
#define SA_ONSTACK   0x00000002
#endif
#define SA_RESETHAND 0x00000004
#define SA_RESTART   0x00000008
#define SA_SIGINFO   0x00000010
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
#define SA_NOCLDWAIT 0x00000020
#endif
#define SA_NODEFER  0x00000040
#define SS_ONSTACK  0x00000001
#define SS_DISABLE  0x00000002
#define MINSIGSTKSZ 4096
#define SIGSTKSZ    4096

#if !defined(_MCONTEXT_T_DECLARED) && !defined(__mcontext_t_defined)
typedef struct {
	/* FIXME: there should be a much better Zephyr-specific structure that can be used here */
	unsigned long gregs[32];
	unsigned long flags;
} mcontext_t;
#define _MCONTEXT_T_DECLARED
#define __mcontext_defined
#endif

#if !defined(_UCONTEXT_T_DECLARED) && !defined(__ucontext_t_defined)
typedef struct {
	struct ucontext *uc_link;
	sigset_t uc_sigmask;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
} ucontext_t;
#define _UCONTEXT_T_DECLARED
#define __ucontext_defined
#endif

#if !defined(_STACK_T_DECLARED) && !defined(__stack_t_defined)
typedef struct {
	void *ss_sp;
	size_t ss_size;
	int ss_flags;
} stack_t;
#define _STACK_T_DECLARED
#define __stack_t_defined
#endif

#if !defined(_SIGINFO_T_DECLARED) && !defined(__siginfo_t_defined)
typedef struct {
	void *si_addr;
#if defined(_XOPEN_STREAMS) || defined(__DOXYGEN__)
	long si_band;
#endif
	union sigval si_value;
	pid_t si_pid;
	uid_t si_uid;
	int si_signo;
	int si_code;
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
	int si_errno;
#endif
	int si_status;
} siginfo_t;
#define _SIGINFO_T_DECLARED
#define __siginfo_t_defined
#endif

#endif /* defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__) */

typedef void (*sighandler_t)(int sig);

int kill(pid_t pid, int sig);
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
int killpg(pid_t pgrp, int sig);
#endif
void psiginfo(const siginfo_t *info, const char *message);
void psignal(int sig, const char *message);
#if defined(_POSIX_THREADS) || defined(__DOXYGEN__)
int pthread_kill(pthread_t thread, int sig);
int pthread_sigmask(int how, const sigset_t *ZRESTRICT set,
       sigset_t *ZRESTRICT oset);
#endif
/* raise() must be defined by the libc signal.h */
#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)
TOOLCHAIN_DISABLE_WARNING(TOOLCHAIN_WARNING_SHADOW);
int sigaction(int, const struct sigaction *ZRESTRICT, struct sigaction *ZRESTRICT);
TOOLCHAIN_ENABLE_WARNING(TOOLCHAIN_WARNING_SHADOW);
#endif
int sigaddset(sigset_t *set, int sig);
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
int sigaltstack(const stack_t *ZRESTRICT ss, stack_t *ZRESTRICT oss);
#endif
int sigdelset(sigset_t *set, int sig);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
int sighold(int sig);
int sigignore(int sig);
int siginterrupt(int sig, int flag);
#endif
int sigismember(const sigset_t *set, int sig);
/* signal() must be defined by the libc signal.h */
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
int sigpause(int sig);
#endif
int sigpending(sigset_t *set);
int sigprocmask(int how, const sigset_t *ZRESTRICT set, sigset_t *ZRESTRICT oset);
#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)
int sigqueue(pid_t pid, int sig, union sigval value);
#endif
#if defined(_XOPEN_SOURCE) || defined(__DOXYGEN__)
int sigrelse(int sig);
TOOLCHAIN_DISABLE_WARNING(TOOLCHAIN_WARNING_SHADOW);
sighandler_t sigset(int sig, sighandler_t disp);
TOOLCHAIN_ENABLE_WARNING(TOOLCHAIN_WARNING_SHADOW);
#endif
int sigsuspend(const sigset_t *set);
#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)
int sigtimedwait(const sigset_t *ZRESTRICT set, siginfo_t *ZRESTRICT info, const struct timespec *ZRESTRICT timeout);
#endif
int sigwait(const sigset_t *ZRESTRICT set, int *ZRESTRICT sig);
#if defined(_POSIX_REALTIME_SIGNALS) || defined(__DOXYGEN__)
int sigwaitinfo(const sigset_t *ZRESTRICT set, siginfo_t *ZRESTRICT info);
#endif

#ifdef __cplusplus
}
#endif

#endif /* defined(_POSIX_C_SOURCE) || defined(__DOXYGEN__) */

#endif /* ZEPHYR_INCLUDE_POSIX_POSIX_SIGNAL_H_ */
