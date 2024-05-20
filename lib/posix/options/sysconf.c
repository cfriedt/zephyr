/*
 * Copyright (c) 2024, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/posix/sys/sysconf.h>
#include <zephyr/posix/unistd.h>

#ifdef CONFIG_POSIX_SYSCONF_IMPL_FULL

#define z_sysconf(x) (long)CONCAT(__z_posix_sysconf, x)

long sysconf(int x)
{
	switch (x) {
	case SC_ADVISORY_INFO:
		z_sysconf(SC_ADVISORY_INFO);
	case SC_ASYNCHRONOUS_IO:
		z_sysconf(SC_ASYNCHRONOUS_IO);
	case SC_BARRIERS:
		z_sysconf(SC_BARRIERS);
	case SC_CLOCK_SELECTION:
		z_sysconf(SC_CLOCK_SELECTION);
	case SC_CPUTIME:
		z_sysconf(SC_CPUTIME);
	case SC_FSYNC:
		z_sysconf(SC_FSYNC);
	case SC_IPV6:
		z_sysconf(SC_IPV6);
	case SC_JOB_CONTROL:
		z_sysconf(SC_JOB_CONTROL);
	case SC_MAPPED_FILES:
		z_sysconf(SC_MAPPED_FILES);
	case SC_MEMLOCK:
		z_sysconf(SC_MEMLOCK);
	case SC_MEMLOCK_RANGE:
		z_sysconf(SC_MEMLOCK_RANGE);
	case SC_MEMORY_PROTECTION:
		z_sysconf(SC_MEMORY_PROTECTION);
	case SC_MESSAGE_PASSING:
		z_sysconf(SC_MESSAGE_PASSING);
	case SC_MONOTONIC_CLOCK:
		z_sysconf(SC_MONOTONIC_CLOCK);
	case SC_PRIORITIZED_IO:
		z_sysconf(SC_PRIORITIZED_IO);
	case SC_PRIORITY_SCHEDULING:
		z_sysconf(SC_PRIORITY_SCHEDULING);
	case SC_RAW_SOCKETS:
		z_sysconf(SC_RAW_SOCKETS);
	case SC_RE_DUP_MAX:
		z_sysconf(SC_RE_DUP_MAX);
	case SC_READER_WRITER_LOCKS:
		z_sysconf(SC_READER_WRITER_LOCKS);
	case SC_REALTIME_SIGNALS:
		z_sysconf(SC_REALTIME_SIGNALS);
	case SC_REGEXP:
		z_sysconf(SC_REGEXP);
	case SC_SAVED_IDS:
		z_sysconf(SC_SAVED_IDS);
	case SC_SEMAPHORES:
		z_sysconf(SC_SEMAPHORES);
	case SC_SHARED_MEMORY_OBJECTS:
		z_sysconf(SC_SHARED_MEMORY_OBJECTS);
	case SC_SHELL:
		z_sysconf(SC_SHELL);
	case SC_SPAWN:
		z_sysconf(SC_SPAWN);
	case SC_SPIN_LOCKS:
		z_sysconf(SC_SPIN_LOCKS);
	case SC_SPORADIC_SERVER:
		z_sysconf(SC_SPORADIC_SERVER);
	case SC_SS_REPL_MAX:
		z_sysconf(SC_SS_REPL_MAX);
	case SC_SYNCHRONIZED_IO:
		z_sysconf(SC_SYNCHRONIZED_IO);
	case SC_THREAD_ATTR_STACKADDR:
		z_sysconf(SC_THREAD_ATTR_STACKADDR);
	case SC_THREAD_ATTR_STACKSIZE:
		z_sysconf(SC_THREAD_ATTR_STACKSIZE);
	case SC_THREAD_CPUTIME:
		z_sysconf(SC_THREAD_CPUTIME);
	case SC_THREAD_PRIO_INHERIT:
		z_sysconf(SC_THREAD_PRIO_INHERIT);
	case SC_THREAD_PRIO_PROTECT:
		z_sysconf(SC_THREAD_PRIO_PROTECT);
	case SC_THREAD_PRIORITY_SCHEDULING:
		z_sysconf(SC_THREAD_PRIORITY_SCHEDULING);
	case SC_THREAD_PROCESS_SHARED:
		z_sysconf(SC_THREAD_PROCESS_SHARED);
	case SC_THREAD_ROBUST_PRIO_INHERIT:
		z_sysconf(SC_THREAD_ROBUST_PRIO_INHERIT);
	case SC_THREAD_ROBUST_PRIO_PROTECT:
		z_sysconf(SC_THREAD_ROBUST_PRIO_PROTECT);
	case SC_THREAD_SAFE_FUNCTIONS:
		z_sysconf(SC_THREAD_SAFE_FUNCTIONS);
	case SC_THREAD_SPORADIC_SERVER:
		z_sysconf(SC_THREAD_SPORADIC_SERVER);
	case SC_THREADS:
		z_sysconf(SC_THREADS);
	case SC_TIMEOUTS:
		z_sysconf(SC_TIMEOUTS);
	case SC_TIMERS:
		z_sysconf(SC_TIMERS);
	case SC_TRACE:
		z_sysconf(SC_TRACE);
	case SC_TRACE_EVENT_FILTER:
		z_sysconf(SC_TRACE_EVENT_FILTER);
	case SC_TRACE_EVENT_NAME_MAX:
		z_sysconf(SC_TRACE_EVENT_NAME_MAX);
	case SC_TRACE_INHERIT:
		z_sysconf(SC_TRACE_INHERIT);
	case SC_TRACE_LOG:
		z_sysconf(SC_TRACE_LOG);
	case SC_TRACE_NAME_MAX:
		z_sysconf(SC_TRACE_NAME_MAX);
	case SC_TRACE_SYS_MAX:
		z_sysconf(SC_TRACE_SYS_MAX);
	case SC_TRACE_USER_EVENT_MAX:
		z_sysconf(SC_TRACE_USER_EVENT_MAX);
	case SC_TYPED_MEMORY_OBJECTS:
		z_sysconf(SC_TYPED_MEMORY_OBJECTS);
	case SC_VERSION:
		z_sysconf(SC_VERSION);
	case SC_V6_ILP32_OFF32:
		z_sysconf(SC_V6_ILP32_OFF32);
	case SC_V6_ILP32_OFFBIG:
		z_sysconf(SC_V6_ILP32_OFFBIG);
	case SC_V6_LP64_OFF64:
		z_sysconf(SC_V6_LP64_OFF64);
	case SC_V6_LPBIG_OFFBIG:
		z_sysconf(SC_V6_LPBIG_OFFBIG);
	case SC_V7_ILP32_OFF32:
		z_sysconf(SC_V7_ILP32_OFF32);
	case SC_V7_ILP32_OFFBIG:
		z_sysconf(SC_V7_ILP32_OFFBIG);
	case SC_V7_LP64_OFF64:
		z_sysconf(SC_V7_LP64_OFF64);
	case SC_V7_LPBIG_OFFBIG:
		z_sysconf(SC_V7_LPBIG_OFFBIG);
	case SC_BC_BASE_MAX:
		z_sysconf(SC_BC_BASE_MAX);
	case SC_BC_DIM_MAX:
		z_sysconf(SC_BC_DIM_MAX);
	case SC_BC_SCALE_MAX:
		z_sysconf(SC_BC_SCALE_MAX);
	case SC_BC_STRING_MAX:
		z_sysconf(SC_BC_STRING_MAX);
	case SC_2_C_BIND:
		z_sysconf(SC_2_C_BIND);
	case SC_2_C_DEV:
		z_sysconf(SC_2_C_DEV);
	case SC_2_CHAR_TERM:
		z_sysconf(SC_2_CHAR_TERM);
	case SC_COLL_WEIGHTS_MAX:
		z_sysconf(SC_COLL_WEIGHTS_MAX);
	case SC_DELAYTIMER_MAX:
		z_sysconf(SC_DELAYTIMER_MAX);
	case SC_EXPR_NEST_MAX:
		z_sysconf(SC_EXPR_NEST_MAX);
	case SC_2_FORT_DEV:
		z_sysconf(SC_2_FORT_DEV);
	case SC_2_FORT_RUN:
		z_sysconf(SC_2_FORT_RUN);
	case SC_LINE_MAX:
		z_sysconf(SC_LINE_MAX);
	case SC_2_LOCALEDEF:
		z_sysconf(SC_2_LOCALEDEF);
	case SC_2_PBS:
		z_sysconf(SC_2_PBS);
	case SC_2_PBS_ACCOUNTING:
		z_sysconf(SC_2_PBS_ACCOUNTING);
	case SC_2_PBS_CHECKPOINT:
		z_sysconf(SC_2_PBS_CHECKPOINT);
	case SC_2_PBS_LOCATE:
		z_sysconf(SC_2_PBS_LOCATE);
	case SC_2_PBS_MESSAGE:
		z_sysconf(SC_2_PBS_MESSAGE);
	case SC_2_PBS_TRACK:
		z_sysconf(SC_2_PBS_TRACK);
	case SC_2_SW_DEV:
		z_sysconf(SC_2_SW_DEV);
	case SC_2_UPE:
		z_sysconf(SC_2_UPE);
	case SC_2_VERSION:
		z_sysconf(SC_2_VERSION);
	case SC_XOPEN_CRYPT:
		z_sysconf(SC_XOPEN_CRYPT);
	case SC_XOPEN_ENH_I18N:
		z_sysconf(SC_XOPEN_ENH_I18N);
	case SC_XOPEN_REALTIME:
		z_sysconf(SC_XOPEN_REALTIME);
	case SC_XOPEN_REALTIME_THREADS:
		z_sysconf(SC_XOPEN_REALTIME_THREADS);
	case SC_XOPEN_SHM:
		z_sysconf(SC_XOPEN_SHM);
	case SC_XOPEN_STREAMS:
		z_sysconf(SC_XOPEN_STREAMS);
	case SC_XOPEN_UNIX:
		z_sysconf(SC_XOPEN_UNIX);
	case SC_XOPEN_UUCP:
		z_sysconf(SC_XOPEN_UUCP);
	case SC_XOPEN_VERSION:
		z_sysconf(SC_XOPEN_VERSION);
	case SC_CLK_TCK:
		z_sysconf(SC_CLK_TCK);
	case SC_GETGR_R_SIZE_MAX:
		z_sysconf(SC_GETGR_R_SIZE_MAX);
	case SC_GETPW_R_SIZE_MAX:
		z_sysconf(SC_GETPW_R_SIZE_MAX);
	case SC_AIO_LISTIO_MAX:
		z_sysconf(SC_AIO_LISTIO_MAX);
	case SC_AIO_MAX:
		z_sysconf(SC_AIO_MAX);
	case SC_AIO_PRIO_DELTA_MAX:
		z_sysconf(SC_AIO_PRIO_DELTA_MAX);
	case SC_ARG_MAX:
		z_sysconf(SC_ARG_MAX);
	case SC_ATEXIT_MAX:
		z_sysconf(SC_ATEXIT_MAX);
	case SC_CHILD_MAX:
		z_sysconf(SC_CHILD_MAX);
	case SC_HOST_NAME_MAX:
		z_sysconf(SC_HOST_NAME_MAX);
	case SC_IOV_MAX:
		z_sysconf(SC_IOV_MAX);
	case SC_LOGIN_NAME_MAX:
		z_sysconf(SC_LOGIN_NAME_MAX);
	case SC_NGROUPS_MAX:
		z_sysconf(SC_NGROUPS_MAX);
	case SC_MQ_OPEN_MAX:
		z_sysconf(SC_MQ_OPEN_MAX);
	case SC_MQ_PRIO_MAX:
		z_sysconf(SC_MQ_PRIO_MAX);
	case SC_OPEN_MAX:
		z_sysconf(SC_OPEN_MAX);
	case SC_PAGE_SIZE:
		z_sysconf(SC_PAGE_SIZE);
	case SC_PAGESIZE:
		z_sysconf(SC_PAGESIZE);
	case SC_THREAD_DESTRUCTOR_ITERATIONS:
		z_sysconf(SC_THREAD_DESTRUCTOR_ITERATIONS);
	case SC_THREAD_KEYS_MAX:
		z_sysconf(SC_THREAD_KEYS_MAX);
	case SC_THREAD_STACK_MIN:
		z_sysconf(SC_THREAD_STACK_MIN);
	case SC_THREAD_THREADS_MAX:
		z_sysconf(SC_THREAD_THREADS_MAX);
	case SC_RTSIG_MAX:
		z_sysconf(SC_RTSIG_MAX);
	case SC_SEM_NSEMS_MAX:
		z_sysconf(SC_SEM_NSEMS_MAX);
	case SC_SEM_VALUE_MAX:
		z_sysconf(SC_SEM_VALUE_MAX);
	case SC_SIGQUEUE_MAX:
		z_sysconf(SC_SIGQUEUE_MAX);
	case SC_STREAM_MAX:
		z_sysconf(SC_STREAM_MAX);
	case SC_SYMLOOP_MAX:
		z_sysconf(SC_SYMLOOP_MAX);
	case SC_TIMER_MAX:
		z_sysconf(SC_TIMER_MAX);
	case SC_TTY_NAME_MAX:
		z_sysconf(SC_TTY_NAME_MAX);
	case SC_TZNAME_MAX:
		z_sysconf(SC_TZNAME_MAX);
	default:
		errno = EINVAL;
		return -1;
	}
}

#endif /* CONFIG_POSIX_SYSCONF_IMPL_FULL */
