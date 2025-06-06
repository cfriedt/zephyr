/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_TIME_H_
#define ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_TIME_H_

#include <stddef.h>
#include <stdint.h>
#include <zephyr/toolchain.h>
#include <sys/_types.h>
#include <sys/_timespec.h>

#include <zephyr/sys/clock.h>

/* Minimal time.h to fulfill the requirements of certain libraries
 * like mbedTLS and to support time APIs.
 */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__clock_t_defined) && !defined(_CLOCK_T_DECLARED)
#define __clock_t_defined
#define _CLOCK_T_DECLARED
typedef _CLOCK_T_ clock_t;
#endif

/* size_t declared in stddef.h */

/* time_t defined in sys/types.h */

#if defined(_POSIX_C_SOURCE) && !defined(__clockid_t_defined) && !defined(_CLOCKID_T_DECLARED)
#define __clockid_t_defined
#define _CLOCKID_T_DECLARED
typedef _CLOCKID_T_ clock_t;
#endif

#if defined(_POSIX_C_SOURCE) && !defined(__timer_t_defined) && !defined(_TIMER_T_DECLARED)
#define __timer_t_defined
#define _TIMER_T_DECLARED
typedef unsigned long timer_t;
#endif

#if !defined(__locale_t_defined) && !defined(_LOCALE_T_DECLARED)
#define __locale_t_defined
#define _LOCALE_T_DECLARED
struct _local_struct {
	char x;
};
typedef struct _local_struct locale_t;
#endif

#if !defined(__pid_t_defined) && !defined(_PID_T_DECLARED)
#define __pid_t_defined
#define _PID_T_DECLARED
typedef unsigned long pid_t;
#endif

#if !defined(__pid_t_defined) && !defined(_PID_T_DECLARED)
#define __pid_t_defined
#define _PID_T_DECLARED
typedef unsigned long pid_t;
#endif

struct sigevent;

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

/* struct timespec declared in sys/_timespec.h */

struct itimerspec {
	struct timespec it_interval; /* Timer interval */
	struct timespec it_value;    /* Timer expiration */
};

/* NULL declared in stddef.h */

/* Note: XSI-conformance is not necessarily the case */
#define CLOCKS_PER_SEC ((clock_t)1000000L)

#define CLOCK_REALTIME ((clockid_t)SYS_CLOCK_REALTIME)

#if _POSIX_CPUTIME >= 200112L
#define CLOCK_PROCESS_CPUTIME_ID ((clockid_t)2)
#endif

#if _POSIX_THREAD_CPUTIME >= 200112L
#define CLOCK_THREAD_CPUTIME_ID ((clockid_t)3)
#endif

#if _POSIX_MONOTONIC_CLOCK >= 200112L
#define CLOCK_MONOTONIC ((clockid_t)SYS_CLOCK_MONOTONIC)
#endif

#define TIMER_ABSTIME SYS_TIMER_ABSTIME

/* Note: XSI-conformance is not necessarily the case */
#define getdate_err ((int)0)

char *asctime(const struct tm *timeptr);

#if defined(_POSIX_C_LANG_SUPPORT_R) && _POSIX_C_LANG_SUPPORT_R < 202405L
char *asctime_r(const struct tm *ZRESTRICT tp, char *ZRESTRICT buf);
#endif

clock_t clock(void);

#if _POSIX_CPUTIME >= 200112L
int clock_getcpuclockid(pid_t pid, clockid_t *clock_id);
#endif

#if defined(_POSIX_C_SOURCE)
int clock_getres(clockid_t clock_id, struct timespec *ts);
int clock_gettime(clockid_t clock_id, struct timespec *ts);
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
		    struct timespec *rmtp);
int clock_settime(clockid_t clock_id, const struct timespec *tp);
#endif

char *ctime(const time_t *clock);

#if defined(_POSIX_C_LANG_SUPPORT_R)
char *ctime_r(const time_t *clock, char *buf);
#endif

double difftime(time_t, time_t);

#if defined(_XOPEN_SOURCE)
struct tm *getdate(const char *);
#endif

struct tm *gmtime(const time_t *timep);

#if defined(_POSIX_C_LANG_SUPPORT_R)
struct tm *gmtime_r(const time_t *ZRESTRICT timep, struct tm *ZRESTRICT result);
#endif

struct tm *localtime(const time_t *timer);

/* also available in C23 */
#if defined(_POSIX_C_LANG_SUPPORT_R)
struct tm *localtime_r(const time_t *ZRESTRICT timer, struct tm *ZRESTRICT result);
#endif

time_t mktime(struct tm *);

#if defined(_POSIX_C_SOURCE)
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
#endif

size_t strftime(char *ZRESTRICT, size_t, const char *ZRESTRICT, const struct tm *ZRESTRICT);
size_t strftime_l(char *ZRESTRICT, size_t, const char *ZRESTRICT, const struct tm *ZRESTRICT,
		  locale_t);
#if defined(_XOPEN_SOURCE)
char *strptime(const char *ZRESTRICT, const char *ZRESTRICT, struct tm *ZRESTRICT);
#endif

time_t time(time_t *);

#if defined(_POSIX_TIMERS)
int timer_create(clockid_t, struct sigevent *ZRESTRICT, timer_t *ZRESTRICT);
int timer_delete(timer_t);
int timer_getoverrun(timer_t);
int timer_gettime(timer_t, struct itimerspec *);
int timer_settime(timer_t, int, const struct itimerspec *ZRESTRICT, struct itimerspec *ZRESTRICT);
#endif

void tzset(void);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_STDIO_H_ */
