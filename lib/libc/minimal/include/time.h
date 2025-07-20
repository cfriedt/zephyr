/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_TIME_H_
#define ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_TIME_H_

#include <stdint.h>
#include <zephyr/sys/clock.h>
#include <zephyr/toolchain.h>
#include <sys/_types.h>
#include <sys/_timespec.h>

#if defined(_POSIX_C_SOURCE)
#include <zephyr/posix/sys/features.h>
#endif

#if defined(_POSIX_TIMERS) || defined(__DOXYGEN__)

#define CLOCK_REALTIME ((clockid_t)SYS_CLOCK_REALTIME)
#if defined(_POSIX_CPUTIME) || defined(__DOXYGEN__)
#define CLOCK_PROCESS_CPUTIME_ID ((clockid_t)2)
#endif
#if defined(_POSIX_THREAD_CPUTIME) || defined(__DOXYGEN__)
#define CLOCK_THREAD_CPUTIME_ID ((clockid_t)3)
#endif
#if defined(_POSIX_MONOTONIC_CLOCK) || defined(__DOXYGEN__)
#define CLOCK_MONOTONIC ((clockid_t)SYS_CLOCK_MONOTONIC)
#define TIMER_ABSTIME   ((unsigned int)SYS_TIMER_ABSTIME)
#endif

struct itimerspec {
	struct timespec it_interval; /* Timer interval */
	struct timespec it_value;    /* Timer expiration */
};

#endif

/* Minimal time.h to fulfill the requirements of certain libraries
 * like mbedTLS and to support time APIs.
 */

#ifdef __cplusplus
extern "C" {
#endif

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

#if !defined(__time_t_defined)
#define __time_t_defined
typedef _TIME_T_ time_t;
#endif

#if !defined(__suseconds_t_defined)
#define __suseconds_t_defined
typedef _SUSECONDS_T_ suseconds_t;
#endif

/*
 * Conversion between civil time and UNIX time.  The companion
 * mktime() is not provided here since it
 * require access to time zone information.
 *
 * The localtime() & localtime_r() simply
 * wraps around the gmtime() & gmtime_r() functions, the
 * results are always expressed as UTC.
 */
char *asctime(const struct tm *timeptr);
char *ctime(const time_t *clock);
struct tm *gmtime(const time_t *timep);
struct tm *localtime(const time_t *timer);
time_t time(time_t *tloc);

#if defined(_POSIX_THREAD_SAFE_FUNCTIONS) || defined(__DOXYGEN__)
char *asctime_r(const struct tm *ZRESTRICT tp, char *ZRESTRICT buf);
char *ctime_r(const time_t *clock, char *buf);
#endif /* _POSIX_THREAD_SAFE_FUNCTIONS || __DOXYGEN__ */

#if defined(_POSIX_THREAD_SAFE_FUNCTIONS) || (__STDC_VERSION__ >= 202311L) || defined(__DOXYGEN__)
struct tm *gmtime_r(const time_t *ZRESTRICT timer, struct tm *ZRESTRICT result);
struct tm *localtime_r(const time_t *ZRESTRICT timer, struct tm *ZRESTRICT result);
#endif /* _POSIX_THREAD_SAFE_FUNCTIONS || (__STDC_VERSION__ >= 202311L) || __DOXYGEN__ */

#if defined(_POSIX_TIMERS) || defined(__DOXYGEN__)
#if defined(_POSIX_CPUTIME) || defined(__DOXYGEN__)
int clock_getcpuclockid(pid_t pid, clockid_t *clock_id);
#endif
int clock_getres(clockid_t clock_id, struct timespec *ts);
int clock_gettime(clockid_t clock_id, struct timespec *ts);
#if defined(_POSIX_CLOCK_SELECTION) || defined(__DOXYGEN__)
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
		    struct timespec *rmtp);
#endif
int clock_settime(clockid_t clock_id, const struct timespec *ts);
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
int timer_create(clockid_t clockId, struct sigevent *evp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_getoverrun(timer_t timerid);
int timer_gettime(timer_t timerid, struct itimerspec *its);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value,
		  struct itimerspec *ovalue);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_STDIO_H_ */
