/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_POSIX_SYS_FEATURES_H_
#define ZEPHYR_INCLUDE_POSIX_SYS_FEATURES_H_

#include <autoconf.h>              /* CONFIG_* */
#include <zephyr/sys/util_macro.h> /* COND_CODE_1() */

/* this allows any additional specification */
#include_next <sys/features.h>

#ifndef __GNUC_PREREQ__
/* some libc's need these macros defined */
#define __GNUC_PREREQ(_maj, _min)   (1)
#define __GNUC_PREREQ__(_maj, _min) (1)
#endif

/* Version test macros */
#define _POSIX_VERSION  200809L
#define _POSIX2_VERSION 200809L
#define _XOPEN_VERSION  700

#define _POSIX_CHOWN_RESTRICTED (0)
#define _POSIX_NO_TRUNC         (0)
#define _POSIX_VDISABLE         (0)

#ifdef CONFIG_POSIX_ADVISORY_INFO
#define _POSIX_ADVISORY_INFO _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_ASYNCHRONOUS_IO
#define _POSIX_ASYNCHRONOUS_IO _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_BARRIERS
#define _POSIX_BARRIERS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_CLOCK_SELECTION
#define _POSIX_CLOCK_SELECTION _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_CPUTIME
#define _POSIX_CPUTIME _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_FSYNC
#define _POSIX_FSYNC _POSIX_VERSION
#endif

#ifdef CONFIG_NET_IPV6
#define _POSIX_IPV6 _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_JOB_CONTROL
#define _POSIX_JOB_CONTROL _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_MAPPED_FILES
#define _POSIX_MAPPED_FILES _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_MEMLOCK
#define _POSIX_MEMLOCK _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_MEMLOCK_RANGE
#define _POSIX_MEMLOCK_RANGE _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_MEMORY_PROTECTION
#define _POSIX_MEMORY_PROTECTION _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_MESSAGE_PASSING
#define _POSIX_MESSAGE_PASSING _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_MONOTONIC_CLOCK
#define _POSIX_MONOTONIC_CLOCK _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_PRIORITIZED_IO
#define _POSIX_PRIORITIZED_IO _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_PRIORITY_SCHEDULING
#define _POSIX_PRIORITY_SCHEDULING _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_RAW_SOCKETS
#define _POSIX_RAW_SOCKETS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_READER_WRITER_LOCKS
#define _POSIX_READER_WRITER_LOCKS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_REALTIME_SIGNALS
#define _POSIX_REALTIME_SIGNALS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_REGEXP
#define _POSIX_REGEXP _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SAVED_IDS
#define _POSIX_SAVED_IDS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SEMAPHORES
#define _POSIX_SEMAPHORES _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SHARED_MEMORY_OBJECTS
#define _POSIX_SHARED_MEMORY_OBJECTS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SHELL
#define _POSIX_SHELL _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SPAWN
#define _POSIX_SPAWN _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SPIN_LOCKS
#define _POSIX_SPIN_LOCKS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SPORADIC_SERVER
#define _POSIX_SPORADIC_SERVER _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_SYNCHRONIZED_IO
#define _POSIX_SYNCHRONIZED_IO _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_ATTR_STACKADDR
#define _POSIX_THREAD_ATTR_STACKADDR _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_ATTR_STACKSIZE
#define _POSIX_THREAD_ATTR_STACKSIZE _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_CPUTIME
#define _POSIX_THREAD_CPUTIME _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_PRIO_INHERIT
#define _POSIX_THREAD_PRIO_INHERIT _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_PRIO_PROTECT
#define _POSIX_THREAD_PRIO_PROTECT _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_PRIORITY_SCHEDULING
#define _POSIX_THREAD_PRIORITY_SCHEDULING _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_PROCESS_SHARED
#define _POSIX_THREAD_PROCESS_SHARED _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_ROBUST_PRIO_INHERIT
#define _POSIX_THREAD_ROBUST_PRIO_INHERIT _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_ROBUST_PRIO_PROTECT
#define _POSIX_THREAD_ROBUST_PRIO_PROTECT _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_SAFE_FUNCTIONS
#define _POSIX_THREAD_SAFE_FUNCTIONS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREAD_SPORADIC_SERVER
#define _POSIX_THREAD_SPORADIC_SERVER _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_THREADS_BASE
#define _POSIX_THREADS _POSIX_VERSION
#ifdef CONFIG_POSIX_THREADS_EXT
#define _UNIX98_THREAD_MUTEX_ATTRIBUTES 1
#endif
#endif

#ifdef CONFIG_POSIX_TIMEOUTS
#define _POSIX_TIMEOUTS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_TIMERS
#define _POSIX_TIMERS _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_TRACE
#define _POSIX_TRACE _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_TRACE_EVENT_FILTER
#define _POSIX_TRACE_EVENT_FILTER _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_TRACE_EVENT_FILTER
#define _POSIX_TRACE_INHERIT _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_TRACE_LOG
#define _POSIX_TRACE_LOG _POSIX_VERSION
#endif

#ifdef CONFIG_POSIX_TYPED_MEMORY_OBJECTS
#define _POSIX_TYPED_MEMORY_OBJECTS _POSIX_VERSION
#endif

/* POSIX v6 options */
#if defined(CONFIG_64BIT)
#define _POSIX_V6_LP64_OFF64
/* #define _POSIX_V6_LPBIG_OFFBIG */
#else
#define _POSIX_V6_ILP32_OFF32
/* #define _POSIX_V6_ILP32_OFFBIG */
#endif

/* POSIX v7 options */
#if defined(CONFIG_64BIT)
#define _POSIX_V7_LP64_OFF64
/* #define _POSIX_V7_LPBIG_OFFBIG */
#else
#define _POSIX_V7_ILP32_OFF32
/* #define _POSIX_V7_ILP32_OFFBIG */
#endif

#define _POSIX2_C_BIND _POSIX2_VERSION
#define _POSIX2_C_DEV  _POSIX2_VERSION
/*
 * POSIX2 options
#define _POSIX2_CHAR_TERM		  (-1L)
#define _POSIX2_DELAYTIMER_MAX	  (-1L)
#define _POSIX2_FORT_DEV		  (-1L)
#define _POSIX2_FORT_RUN		  (-1L)
#define _POSIX2_LOCALEDEF		  (-1L)
#define _POSIX2_PBS			  (-1L)
#define _POSIX2_PBS_ACCOUNTING		  (-1L)
#define _POSIX2_PBS_CHECKPOINT		  (-1L)
#define _POSIX2_PBS_LOCATE		  (-1L)
#define _POSIX2_PBS_MESSAGE		  (-1L)
#define _POSIX2_PBS_TRACK		  (-1L)
#define _POSIX2_SW_DEV			  (-1L)
#define _POSIX2_UPE			  (-1L)
*/

/*
 * X/Open System Interfaces
 */

#ifdef CONFIG_XOPEN_CRYPT
#define _XOPEN_CRYPT _XOPEN_VERSION
#endif

#ifdef CONFIG_XOPEN_REALTIME
#define _XOPEN_REALTIME _POSIX_VERSION
#endif

#ifdef CONFIG_XOPEN_REALTIME_THREADS
#define _XOPEN_REALTIME_THREADS _XOPEN_REALTIME
#endif

#ifdef CONFIG_XOPEN_SHM
#define _XOPEN_SHM _POSIX_VERSION
#endif

#ifdef CONFIG_XOPEN_STREAMS
#define _XOPEN_STREAMS _POSIX_VERSION
#endif

/*
 * Unsupported X/Open System Interfaces
#define _XOPEN_UNIX
#define _XOPEN_UUCP
#define _XOPEN_ENH_I18N
*/

/* Maximum values */
#define _POSIX_CLOCKRES_MIN (20000000L)

/* Minimum values */
#define _POSIX_AIO_LISTIO_MAX               (2)
#define _POSIX_AIO_MAX                      (1)
#define _POSIX_ARG_MAX                      (4096)
#define _POSIX_CHILD_MAX                    (25)
#define _POSIX_DELAYTIMER_MAX               (32)
#define _POSIX_HOST_NAME_MAX                (255)
#define _POSIX_LINK_MAX                     (8)
#define _POSIX_LOGIN_NAME_MAX               (9)
#define _POSIX_MAX_CANON                    (255)
#define _POSIX_MAX_INPUT                    (255)
#define _POSIX_MQ_OPEN_MAX                  COND_CODE_1(CONFIG_POSIX_MQUEUE, CONFIG_MSG_COUNT_MAX)
#define _POSIX_MQ_PRIO_MAX                  (32)
#define _POSIX_NAME_MAX                     (14)
#define _POSIX_NGROUPS_MAX                  (8)
#define _POSIX_OPEN_MAX                     CONFIG_POSIX_OPEN_MAX
#define _POSIX_PATH_MAX                     (256)
#define _POSIX_PIPE_BUF                     (512)
#define _POSIX_RE_DUP_MAX                   (255)
#define _POSIX_RTSIG_MAX                    CONFIG_POSIX_RTSIG_MAX
#define _POSIX_SEM_NSEMS_MAX                COND_CODE_1(CONFIG_POSIX_SEMAPHORES, (CONFIG_POSIX_SEM_NSEMS_MAX), (0))
#define _POSIX_SEM_VALUE_MAX                COND_CODE_1(CONFIG_POSIX_SEMAPHORES, (CONFIG_POSIX_SEM_VALUE_MAX), (0))
#define _POSIX_SIGQUEUE_MAX                 (32)
#define _POSIX_SSIZE_MAX                    (32767)
#define _POSIX_SS_REPL_MAX                  (4)
#define _POSIX_STREAM_MAX                   (8)
#define _POSIX_SYMLINK_MAX                  (255)
#define _POSIX_SYMLOOP_MAX                  (8)
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS (4)
#define _POSIX_THREAD_THREADS_MAX                                                                  \
	COND_CODE_1(CONFIG_POSIX_THREADS_BASE, (CONFIG_POSIX_THREAD_THREADS_MAX), (0))
#define _POSIX_TIMER_MAX            COND_CODE_1(CONFIG_POSIX_TIMERS, (CONFIG_POSIX_TIMER_MAX), (0))
#define _POSIX_TRACE_EVENT_NAME_MAX (30)
#define _POSIX_TRACE_NAME_MAX       (8)
#define _POSIX_TRACE_SYS_MAX        (8)
#define _POSIX_TRACE_USER_EVENT_MAX (32)
#define _POSIX_TTY_NAME_MAX         (9)
#define _POSIX_TZNAME_MAX           (6)
#define _POSIX2_BC_BASE_MAX         (99)
#define _POSIX2_BC_DIM_MAX          (2048)
#define _POSIX2_BC_SCALE_MAX        (99)
#define _POSIX2_BC_STRING_MAX       (1000)
#define _POSIX2_CHARCLASS_NAME_MAX  (14)
#define _POSIX2_COLL_WEIGHTS_MAX    (2)
#define _POSIX2_EXPR_NEST_MAX       (32)
#define _POSIX2_LINE_MAX            (2048)
#define _XOPEN_IOV_MAX              (16)
#define _XOPEN_NAME_MAX             (255)
#define _XOPEN_PATH_MAX             (1024)

/* Other invariant values */
#define NL_LANGMAX (14)
#define NL_MSGMAX  (32767)
#define NL_SETMAX  (255)
#define NL_TEXTMAX (_POSIX2_LINE_MAX)
#define NZERO      (20)

/* Runtime invariant values */
#define AIO_LISTIO_MAX     _POSIX_AIO_LISTIO_MAX
#define AIO_MAX            _POSIX_AIO_MAX
#define AIO_PRIO_DELTA_MAX (0)
#define DELAYTIMER_MAX     _POSIX_DELAYTIMER_MAX
#define HOST_NAME_MAX      COND_CODE_1(CONFIG_NETWORKING, (NET_HOSTNAME_MAX_LEN), (_POSIX_HOST_NAME_MAX))
#define LOGIN_NAME_MAX     _POSIX_LOGIN_NAME_MAX
#define MQ_OPEN_MAX        _POSIX_MQ_OPEN_MAX
#define MQ_PRIO_MAX        _POSIX_MQ_PRIO_MAX

#ifndef ATEXIT_MAX
#define ATEXIT_MAX 8
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE BIT(CONFIG_POSIX_PAGE_SIZE_BITS)
#endif

#ifndef PAGESIZE
#define PAGESIZE PAGE_SIZE
#endif

#define PTHREAD_DESTRUCTOR_ITERATIONS _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#define _POSIX_THREAD_KEYS_MAX                                                                     \
	COND_CODE_1(CONFIG_POSIX_THREADS_BASE, (CONFIG_POSIX_THREAD_KEYS_MAX), (0))
#define PTHREAD_THREADS_MAX                                                                        \
	COND_CODE_1(CONFIG_POSIX_THREADS_BASE, (CONFIG_PTHREAD_THREADS_MAX), (0))
#define SEM_NSEMS_MAX COND_CODE_1(CONFIG_POSIX_SEMAPHORES, (CONFIG_SEM_NSEMS_MAX), (0))
#define SEM_VALUE_MAX COND_CODE_1(CONFIG_POSIX_SEMAPHORES, (CONFIG_SEM_VALUE_MAX), (0))
#define SIGQUEUE_MAX  _POSIX_SIGQUEUE_MAX
#define STREAM_MAX    _POSIX_STREAM_MAX
#define SYMLOOP_MAX   _POSIX_SYMLOOP_MAX
#define TIMER_MAX     Z_SC_VAL_IFDEF(CONFIG_TIMER, CONFIG_MAX_TIMER_COUNT)
#define TTY_NAME_MAX  _POSIX_TTY_NAME_MAX
#define TZNAME_MAX    _POSIX_TZNAME_MAX

/* Pathname variable values */
#define FILESIZEBITS             (32)
#define POSIX_ALLOC_SIZE_MIN     (256)
#define POSIX_REC_INCR_XFER_SIZE (1024)
#define POSIX_REC_MAX_XFER_SIZE  (32767)
#define POSIX_REC_MIN_XFER_SIZE  (1)
#define POSIX_REC_XFER_ALIGN     (4)
#define SYMLINK_MAX              _POSIX_SYMLINK_MAX

/* These are for compatibility / practicality */
#undef __POSIX_VISIBLE
#define __POSIX_VISIBLE _POSIX_VERSION

#define _UTSNAME_NODENAME_LENGTH                                                                   \
	COND_CODE_1(CONFIG_POSIX_SINGLE_PROCESS, (CONFIG_POSIX_UNAME_VERSION_LEN), (0))
#define _UTSNAME_VERSION_LENGTH                                                                    \
	COND_CODE_1(CONFIG_POSIX_SINGLE_PROCESS, (CONFIG_POSIX_UNAME_VERSION_LEN), (0))
#if 0
#define _UTSNAME_SYSNAME_LENGTH _UTSNAME_LENGTH
#define _UTSNAME_RELEASE_LENGTH _UTSNAME_LENGTH
#define _UTSNAME_MACHINE_LENGTH _UTSNAME_LENGTH
#endif

#endif /* ZEPHYR_INCLUDE_POSIX_SYS_FEATURES_H_ */
