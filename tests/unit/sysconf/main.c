/*
 * Copyright (C) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <limits.h>
#include <stdio.h>
#include <sys/user.h>
#include <unistd.h>

// #include <zephyr/ztest.h>

// ZTEST(posix_sysconf, test_sysconf)
// {

int main()
{
	/*
	 * See https://man7.org/linux/man-pages/man3/sysconf.3p.html
	 */

	/* minimal set */
	printf("_SC_ARG_MAX: %d, _POSIX_ARG_MAX: %ld, sysconf(_SC_ARG_MAX): %ld\n", _SC_ARG_MAX,
	       (long)_POSIX_ARG_MAX, sysconf(_SC_ARG_MAX));
	printf("_SC_CHILD_MAX: %d, _POSIX_CHILD_MAX: %ld, sysconf(_SC_CHILD_MAX): %ld\n",
	       _SC_CHILD_MAX, (long)_POSIX_CHILD_MAX, sysconf(_SC_CHILD_MAX));
	printf("_SC_CLK_TCK: %d, N/A, sysconf(_SC_CLK_TCK): %ld\n", _SC_CLK_TCK,
	       sysconf(_SC_CLK_TCK));
	printf("_SC_COLL_WEIGHTS_MAX: %d, COLL_WEIGHTS_MAX: %ld, sysconf(_SC_COLL_WEIGHTS_MAX): "
	       "%ld\n",
	       _SC_COLL_WEIGHTS_MAX, (long)COLL_WEIGHTS_MAX, sysconf(_SC_COLL_WEIGHTS_MAX));
	printf("_SC_DELAYTIMER_MAX: %d, DELAYTIMER_MAX: %ld, sysconf(_SC_DELAYTIMER_MAX): %ld\n",
	       _SC_DELAYTIMER_MAX, (long)DELAYTIMER_MAX, sysconf(_SC_DELAYTIMER_MAX));
	printf("_SC_EXPR_NEST_MAX: %d, EXPR_NEST_MAX: %ld, sysconf(_SC_EXPR_NEST_MAX): %ld\n",
	       _SC_EXPR_NEST_MAX, (long)EXPR_NEST_MAX, sysconf(_SC_EXPR_NEST_MAX));
	printf("_SC_HOST_NAME_MAX: %d, _POSIX_HOST_NAME_MAX: %ld, sysconf(_SC_HOST_NAME_MAX): "
	       "%ld\n",
	       _SC_HOST_NAME_MAX, (long)_POSIX_HOST_NAME_MAX, sysconf(_SC_HOST_NAME_MAX));
	printf("_SC_LINE_MAX: %d, LINE_MAX: %ld, sysconf(_SC_LINE_MAX): %ld\n", _SC_LINE_MAX,
	       (long)LINE_MAX, sysconf(_SC_LINE_MAX));
	printf("_SC_LOGIN_NAME_MAX: %d, _POSIX_LOGIN_NAME_MAX: %ld, sysconf(_SC_LOGIN_NAME_MAX): "
	       "%ld\n",
	       _SC_LOGIN_NAME_MAX, (long)_POSIX_LOGIN_NAME_MAX, sysconf(_SC_LOGIN_NAME_MAX));
	printf("_SC_NGROUPS_MAX: %d, _POSIX_NGROUPS_MAX: %ld, sysconf(_SC_NGROUPS_MAX): %ld\n",
	       _SC_NGROUPS_MAX, (long)_POSIX_NGROUPS_MAX, sysconf(_SC_NGROUPS_MAX));
	printf("_SC_MQ_PRIO_MAX: %d, MQ_PRIO_MAX: %ld, sysconf(_SC_MQ_PRIO_MAX): %ld\n",
	       _SC_MQ_PRIO_MAX, (long)MQ_PRIO_MAX, sysconf(_SC_MQ_PRIO_MAX));
	printf("_SC_PAGE_SIZE: %d, PAGE_SIZE: %ld, sysconf(_SC_PAGE_SIZE): %ld\n", _SC_PAGE_SIZE,
	       (long)PAGE_SIZE, sysconf(_SC_PAGE_SIZE));
	printf("_SC_THREAD_DESTRUCTOR_ITERATIONS: %d, PTHREAD_DESTRUCTOR_ITERATIONS: %ld, "
	       "sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS): %ld\n",
	       _SC_THREAD_DESTRUCTOR_ITERATIONS, (long)PTHREAD_DESTRUCTOR_ITERATIONS,
	       sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS));
	printf("_SC_THREAD_KEYS_MAX: %d, PTHREAD_KEYS_MAX: %ld, sysconf(_SC_THREAD_KEYS_MAX): "
	       "%ld\n",
	       _SC_THREAD_KEYS_MAX, (long)PTHREAD_KEYS_MAX, sysconf(_SC_THREAD_KEYS_MAX));
	printf("_SC_THREAD_STACK_MIN: %d, PTHREAD_STACK_MIN: %ld, sysconf(_SC_THREAD_STACK_MIN): "
	       "%ld\n",
	       _SC_THREAD_STACK_MIN, (long)PTHREAD_STACK_MIN, sysconf(_SC_THREAD_STACK_MIN));
	printf("_SC_RE_DUP_MAX: %d, RE_DUP_MAX: %ld, sysconf(_SC_RE_DUP_MAX): %ld\n",
	       _SC_RE_DUP_MAX, (long)RE_DUP_MAX, sysconf(_SC_RE_DUP_MAX));
	printf("_SC_RTSIG_MAX: %d, RTSIG_MAX: %ld, sysconf(_SC_RTSIG_MAX): %ld\n", _SC_RTSIG_MAX,
	       (long)RTSIG_MAX, sysconf(_SC_RTSIG_MAX));
	printf("_SC_STREAM_MAX: %d, _POSIX_STREAM_MAX: %ld, sysconf(_SC_STREAM_MAX): %ld\n",
	       _SC_STREAM_MAX, (long)_POSIX_STREAM_MAX, sysconf(_SC_STREAM_MAX));
	printf("_SC_SYMLOOP_MAX: %d, _POSIX_SYMLOOP_MAX: %ld, sysconf(_SC_SYMLOOP_MAX): %ld\n",
	       _SC_SYMLOOP_MAX, (long)_POSIX_SYMLOOP_MAX, sysconf(_SC_SYMLOOP_MAX));
	printf("_SC_TIMER_MAX: %d, _POSIX_TIMER_MAX: %ld, sysconf(_SC_TIMER_MAX): %ld\n",
	       _SC_TIMER_MAX, (long)_POSIX_TIMER_MAX, sysconf(_SC_TIMER_MAX));
	printf("_SC_TTY_NAME_MAX: %d, TTY_NAME_MAX: %ld, sysconf(_SC_TTY_NAME_MAX): %ld\n",
	       _SC_TTY_NAME_MAX, (long)TTY_NAME_MAX, sysconf(_SC_TTY_NAME_MAX));
	printf("_SC_TZNAME_MAX: %d, _POSIX_TZNAME_MAX: %ld, sysconf(_SC_TZNAME_MAX): %ld\n",
	       _SC_TZNAME_MAX, (long)_POSIX_TZNAME_MAX, sysconf(_SC_TZNAME_MAX));
	printf("_SC_ADVISORY_INFO: %d, _POSIX_ADVISORY_INFO: %ld, sysconf(_SC_ADVISORY_INFO): "
	       "%ld\n",
	       _SC_ADVISORY_INFO, (long)_POSIX_ADVISORY_INFO, sysconf(_SC_ADVISORY_INFO));
	printf("_SC_BARRIERS: %d, _POSIX_BARRIERS: %ld, sysconf(_SC_BARRIERS): %ld\n", _SC_BARRIERS,
	       (long)_POSIX_BARRIERS, sysconf(_SC_BARRIERS));
	printf("_SC_ASYNCHRONOUS_IO: %d, _POSIX_ASYNCHRONOUS_IO: %ld, "
	       "sysconf(_SC_ASYNCHRONOUS_IO): %ld\n",
	       _SC_ASYNCHRONOUS_IO, (long)_POSIX_ASYNCHRONOUS_IO, sysconf(_SC_ASYNCHRONOUS_IO));
	printf("_SC_CLOCK_SELECTION: %d, _POSIX_CLOCK_SELECTION: %ld, "
	       "sysconf(_SC_CLOCK_SELECTION): %ld\n",
	       _SC_CLOCK_SELECTION, (long)_POSIX_CLOCK_SELECTION, sysconf(_SC_CLOCK_SELECTION));
	printf("_SC_CPUTIME: %d, _POSIX_CPUTIME: %ld, sysconf(_SC_CPUTIME): %ld\n", _SC_CPUTIME,
	       (long)_POSIX_CPUTIME, sysconf(_SC_CPUTIME));
	printf("_SC_FSYNC: %d, _POSIX_FSYNC: %ld, sysconf(_SC_FSYNC): %ld\n", _SC_FSYNC,
	       (long)_POSIX_FSYNC, sysconf(_SC_FSYNC));
	printf("_SC_IPV6: %d, _POSIX_IPV6: %ld, sysconf(_SC_IPV6): %ld\n", _SC_IPV6,
	       (long)_POSIX_IPV6, sysconf(_SC_IPV6));
	printf("_SC_JOB_CONTROL: %d, _POSIX_JOB_CONTROL: %ld, sysconf(_SC_JOB_CONTROL): %ld\n",
	       _SC_JOB_CONTROL, (long)_POSIX_JOB_CONTROL, sysconf(_SC_JOB_CONTROL));
	printf("_SC_MAPPED_FILES: %d, _POSIX_MAPPED_FILES: %ld, sysconf(_SC_MAPPED_FILES): %ld\n",
	       _SC_MAPPED_FILES, (long)_POSIX_MAPPED_FILES, sysconf(_SC_MAPPED_FILES));
	printf("_SC_MEMLOCK: %d, _POSIX_MEMLOCK: %ld, sysconf(_SC_MEMLOCK): %ld\n", _SC_MEMLOCK,
	       (long)_POSIX_MEMLOCK, sysconf(_SC_MEMLOCK));
	printf("_SC_MEMLOCK_RANGE: %d, _POSIX_MEMLOCK_RANGE: %ld, sysconf(_SC_MEMLOCK_RANGE): "
	       "%ld\n",
	       _SC_MEMLOCK_RANGE, (long)_POSIX_MEMLOCK_RANGE, sysconf(_SC_MEMLOCK_RANGE));
	printf("_SC_MEMORY_PROTECTION: %d, _POSIX_MEMORY_PROTECTION: %ld, "
	       "sysconf(_SC_MEMORY_PROTECTION): %ld\n",
	       _SC_MEMORY_PROTECTION, (long)_POSIX_MEMORY_PROTECTION,
	       sysconf(_SC_MEMORY_PROTECTION));
	printf("_SC_MESSAGE_PASSING: %d, _POSIX_MESSAGE_PASSING: %ld, "
	       "sysconf(_SC_MESSAGE_PASSING): %ld\n",
	       _SC_MESSAGE_PASSING, (long)_POSIX_MESSAGE_PASSING, sysconf(_SC_MESSAGE_PASSING));
	printf("_SC_MONOTONIC_CLOCK: %d, _POSIX_MONOTONIC_CLOCK: %ld, "
	       "sysconf(_SC_MONOTONIC_CLOCK): %ld\n",
	       _SC_MONOTONIC_CLOCK, (long)_POSIX_MONOTONIC_CLOCK, sysconf(_SC_MONOTONIC_CLOCK));
	printf("_SC_PRIORITIZED_IO: %d, _POSIX_PRIORITIZED_IO: %ld, sysconf(_SC_PRIORITIZED_IO): "
	       "%ld\n",
	       _SC_PRIORITIZED_IO, (long)_POSIX_PRIORITIZED_IO, sysconf(_SC_PRIORITIZED_IO));
	printf("_SC_PRIORITY_SCHEDULING: %d, _POSIX_PRIORITY_SCHEDULING: %ld, "
	       "sysconf(_SC_PRIORITY_SCHEDULING): %ld\n",
	       _SC_PRIORITY_SCHEDULING, (long)_POSIX_PRIORITY_SCHEDULING,
	       sysconf(_SC_PRIORITY_SCHEDULING));
	printf("_SC_RAW_SOCKETS: %d, _POSIX_RAW_SOCKETS: %ld, sysconf(_SC_RAW_SOCKETS): %ld\n",
	       _SC_RAW_SOCKETS, (long)_POSIX_RAW_SOCKETS, sysconf(_SC_RAW_SOCKETS));
	printf("_SC_READER_WRITER_LOCKS: %d, _POSIX_READER_WRITER_LOCKS: %ld, "
	       "sysconf(_SC_READER_WRITER_LOCKS): %ld\n",
	       _SC_READER_WRITER_LOCKS, (long)_POSIX_READER_WRITER_LOCKS,
	       sysconf(_SC_READER_WRITER_LOCKS));
	printf("_SC_REALTIME_SIGNALS: %d, _POSIX_REALTIME_SIGNALS: %ld, "
	       "sysconf(_SC_REALTIME_SIGNALS): %ld\n",
	       _SC_REALTIME_SIGNALS, (long)_POSIX_REALTIME_SIGNALS, sysconf(_SC_REALTIME_SIGNALS));
	printf("_SC_REGEXP: %d, _POSIX_REGEXP: %ld, sysconf(_SC_REGEXP): %ld\n", _SC_REGEXP,
	       (long)_POSIX_REGEXP, sysconf(_SC_REGEXP));
	printf("_SC_SAVED_IDS: %d, _POSIX_SAVED_IDS: %ld, sysconf(_SC_SAVED_IDS): %ld\n",
	       _SC_SAVED_IDS, (long)_POSIX_SAVED_IDS, sysconf(_SC_SAVED_IDS));
	printf("_SC_SEMAPHORES: %d, _POSIX_SEMAPHORES: %ld, sysconf(_SC_SEMAPHORES): %ld\n",
	       _SC_SEMAPHORES, (long)_POSIX_SEMAPHORES, sysconf(_SC_SEMAPHORES));
	printf("_SC_SHARED_MEMORY_OBJECTS: %d, _POSIX_SHARED_MEMORY_OBJECTS: %ld, "
	       "sysconf(_SC_SHARED_MEMORY_OBJECTS): %ld\n",
	       _SC_SHARED_MEMORY_OBJECTS, (long)_POSIX_SHARED_MEMORY_OBJECTS,
	       sysconf(_SC_SHARED_MEMORY_OBJECTS));
	printf("_SC_SHELL: %d, _POSIX_SHELL: %ld, sysconf(_SC_SHELL): %ld\n", _SC_SHELL,
	       (long)_POSIX_SHELL, sysconf(_SC_SHELL));
	printf("_SC_SPAWN: %d, _POSIX_SPAWN: %ld, sysconf(_SC_SPAWN): %ld\n", _SC_SPAWN,
	       (long)_POSIX_SPAWN, sysconf(_SC_SPAWN));
	printf("_SC_SPIN_LOCKS: %d, _POSIX_SPIN_LOCKS: %ld, sysconf(_SC_SPIN_LOCKS): %ld\n",
	       _SC_SPIN_LOCKS, (long)_POSIX_SPIN_LOCKS, sysconf(_SC_SPIN_LOCKS));
	printf("_SC_SPORADIC_SERVER: %d, _POSIX_SPORADIC_SERVER: %ld, "
	       "sysconf(_SC_SPORADIC_SERVER): %ld\n",
	       _SC_SPORADIC_SERVER, (long)_POSIX_SPORADIC_SERVER, sysconf(_SC_SPORADIC_SERVER));
	printf("_SC_SYNCHRONIZED_IO: %d, _POSIX_SYNCHRONIZED_IO: %ld, "
	       "sysconf(_SC_SYNCHRONIZED_IO): %ld\n",
	       _SC_SYNCHRONIZED_IO, (long)_POSIX_SYNCHRONIZED_IO, sysconf(_SC_SYNCHRONIZED_IO));
	printf("_SC_THREAD_ATTR_STACKADDR: %d, _POSIX_THREAD_ATTR_STACKADDR: %ld, "
	       "sysconf(_SC_THREAD_ATTR_STACKADDR): %ld\n",
	       _SC_THREAD_ATTR_STACKADDR, (long)_POSIX_THREAD_ATTR_STACKADDR,
	       sysconf(_SC_THREAD_ATTR_STACKADDR));
	printf("_SC_THREAD_ATTR_STACKSIZE: %d, _POSIX_THREAD_ATTR_STACKSIZE: %ld, "
	       "sysconf(_SC_THREAD_ATTR_STACKSIZE): %ld\n",
	       _SC_THREAD_ATTR_STACKSIZE, (long)_POSIX_THREAD_ATTR_STACKSIZE,
	       sysconf(_SC_THREAD_ATTR_STACKSIZE));
	printf("_SC_THREAD_CPUTIME: %d, _POSIX_THREAD_CPUTIME: %ld, sysconf(_SC_THREAD_CPUTIME): "
	       "%ld\n",
	       _SC_THREAD_CPUTIME, (long)_POSIX_THREAD_CPUTIME, sysconf(_SC_THREAD_CPUTIME));
	printf("_SC_THREAD_PRIO_INHERIT: %d, _POSIX_THREAD_PRIO_INHERIT: %ld, "
	       "sysconf(_SC_THREAD_PRIO_INHERIT): %ld\n",
	       _SC_THREAD_PRIO_INHERIT, (long)_POSIX_THREAD_PRIO_INHERIT,
	       sysconf(_SC_THREAD_PRIO_INHERIT));
	printf("_SC_THREAD_PRIO_PROTECT: %d, _POSIX_THREAD_PRIO_PROTECT: %ld, "
	       "sysconf(_SC_THREAD_PRIO_PROTECT): %ld\n",
	       _SC_THREAD_PRIO_PROTECT, (long)_POSIX_THREAD_PRIO_PROTECT,
	       sysconf(_SC_THREAD_PRIO_PROTECT));
	printf("_SC_THREAD_PRIORITY_SCHEDULING: %d, _POSIX_THREAD_PRIORITY_SCHEDULING: %ld, "
	       "sysconf(_SC_THREAD_PRIORITY_SCHEDULING): %ld\n",
	       _SC_THREAD_PRIORITY_SCHEDULING, (long)_POSIX_THREAD_PRIORITY_SCHEDULING,
	       sysconf(_SC_THREAD_PRIORITY_SCHEDULING));
	printf("_SC_THREAD_PROCESS_SHARED: %d, _POSIX_THREAD_PROCESS_SHARED: %ld, "
	       "sysconf(_SC_THREAD_PROCESS_SHARED): %ld\n",
	       _SC_THREAD_PROCESS_SHARED, (long)_POSIX_THREAD_PROCESS_SHARED,
	       sysconf(_SC_THREAD_PROCESS_SHARED));
	printf("_SC_THREAD_ROBUST_PRIO_INHERIT: %d, _POSIX_THREAD_ROBUST_PRIO_INHERIT: %ld, "
	       "sysconf(_SC_THREAD_ROBUST_PRIO_INHERIT): %ld\n",
	       _SC_THREAD_ROBUST_PRIO_INHERIT, (long)_POSIX_THREAD_ROBUST_PRIO_INHERIT,
	       sysconf(_SC_THREAD_ROBUST_PRIO_INHERIT));
	printf("_SC_THREAD_ROBUST_PRIO_PROTECT: %d, _POSIX_THREAD_ROBUST_PRIO_PROTECT: %ld, "
	       "sysconf(_SC_THREAD_ROBUST_PRIO_PROTECT): %ld\n",
	       _SC_THREAD_ROBUST_PRIO_PROTECT, (long)_POSIX_THREAD_ROBUST_PRIO_PROTECT,
	       sysconf(_SC_THREAD_ROBUST_PRIO_PROTECT));
	printf("_SC_THREAD_SAFE_FUNCTIONS: %d, _POSIX_THREAD_SAFE_FUNCTIONS: %ld, "
	       "sysconf(_SC_THREAD_SAFE_FUNCTIONS): %ld\n",
	       _SC_THREAD_SAFE_FUNCTIONS, (long)_POSIX_THREAD_SAFE_FUNCTIONS,
	       sysconf(_SC_THREAD_SAFE_FUNCTIONS));
	printf("_SC_THREAD_SPORADIC_SERVER: %d, _POSIX_THREAD_SPORADIC_SERVER: %ld, "
	       "sysconf(_SC_THREAD_SPORADIC_SERVER): %ld\n",
	       _SC_THREAD_SPORADIC_SERVER, (long)_POSIX_THREAD_SPORADIC_SERVER,
	       sysconf(_SC_THREAD_SPORADIC_SERVER));
	printf("_SC_THREADS: %d, _POSIX_THREADS: %ld, sysconf(_SC_THREADS): %ld\n", _SC_THREADS,
	       (long)_POSIX_THREADS, sysconf(_SC_THREADS));
	printf("_SC_TIMEOUTS: %d, _POSIX_TIMEOUTS: %ld, sysconf(_SC_TIMEOUTS): %ld\n", _SC_TIMEOUTS,
	       (long)_POSIX_TIMEOUTS, sysconf(_SC_TIMEOUTS));

	return 0;
}

// ZTEST_SUITE(posix_sysconf, NULL, NULL, NULL, NULL, NULL);
