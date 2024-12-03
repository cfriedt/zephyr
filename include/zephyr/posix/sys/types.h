/*
 * Copyright (c) 2018 Intel Corporation
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#if defined(_POSIX_C_SOURCE)

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <zephyr/posix/sys/_pthreadtypes.h>
#include <zephyr/kernel.h>

#if !defined(_BLKCNT_T_DECLARED) && !defined(__blkcnt_t_defined)
typedef unsigned long blkcnt_t;
#define _BLKCNT_T_DECLARED
#define __blkcnt_t_defined
#endif

#if !defined(_BLKSIZE_T_DECLARED) && !defined(__blksize_t_defined)
typedef unsigned long blksize_t;
#define _BLKSIZE_T_DECLARED
#define __blksize_t_defined
#endif

#if !defined(_CLOCK_T_DECLARED) && !defined(__clock_t_defined)
typedef unsigned long clock_t;
#define _CLOCK_T_DECLARED
#define __clock_t_defined
#endif

#if !defined(_CLOCKID_T_DECLARED) && !defined(__clockid_t_defined)
typedef unsigned long clockid_t;
#define _CLOCKID_T_DECLARED
#define __clockid_t_defined
#endif

#if !defined(_DEV_T_DECLARED) && !defined(__dev_t_defined)
typedef int dev_t;
#define _DEV_T_DECLARED
#define __dev_t_defined
#endif

#if !defined(_FSBLKCNT_T_DECLARED) && !defined(__fsblkcnt_t_defined)
typedef unsigned long fsblkcnt_t;
#define _FSBLKCNT_T_DECLARED
#define __fsblkcnt_t_defined
#endif

#if !defined(_FSFILCNT_T_DECLARED) && !defined(__fsfilcnt_t_defined)
typedef unsigned long fsfilcnt_t;
#define _FSFILCNT_T_DECLARED
#define __fsfilcnt_t_defined
#endif

#if !defined(_GID_T_DECLARED) && !defined(__gid_t_defined)
typedef unsigned short gid_t;
#define _GID_T_DECLARED
#define __gid_t_defined
#endif

#if !defined(_ID_T_DECLARED) && !defined(__id_t_defined)
typedef uintptr_t id_t;
#define _ID_T_DECLARED
#define __id_t_defined
#endif

/* in_addr_t ? */
/* in_port_t ? */

#if !defined(_INO_T_DECLARED) && !defined(__ino_t_defined)
typedef int ino_t;
#define _INO_T_DECLARED
#define __ino_t_defined
#endif

#if !defined(_KEY_T_DECLARED) && !defined(__key_t_defined)
typedef unsigned short gid_t;
#define _KEY_T_DECLARED
#define __key_t_defined
#endif

#if !defined(_MODE_T_DECLARED) && !defined(__mode_t_defined)
typedef unsigned short mode_t;
#define _MODE_T_DECLARED
#define __mode_t_defined
#endif

#if !defined(_NLINK_T_DECLARED) && !defined(__nlink_t_defined)
typedef unsigned short nlink_t;
#define _NLINK_T_DECLARED
#define __nlink_t_defined
#endif

#if !defined(_OFF_T_DECLARED) && !defined(__off_t_defined)
typedef int64_t off_t;
#define _OFF_T_DECLARED
#define __off_t_defined
#endif

#if !defined(_PID_T_DECLARED) && !defined(__pid_t_defined)
typedef k_tid_t pid_t;
#define _PID_T_DECLARED
#define __pid_t_defined
#endif

/*
 * pthread_attr_t, pthread_barrier_t, pthread_barrierattr_t, pthread_cond_t, pthread_condattr_t,
 * pthread_key_t, pthread_mutex_t, pthread_mutexattr_t, pthread_once_t, pthread_rwlock_t,
 * pthread_rwlockattr_t, pthread_spinlock_t, pthread_t defined in <sys/_pthreadtypes.h>
 */

/* size_t defined in <stddef.h> */

#if !defined(_SSIZE_T_DECLARED) && !defined(__ssize_t_defined)
#ifdef __SIZE_TYPE__
#define unsigned signed /* parasoft-suppress MISRAC2012-RULE_20_4-a MISRAC2012-RULE_20_4-b */
typedef __SIZE_TYPE__ ssize_t;
#undef unsigned
#else
#ifdef CONFIG_64BIT
typedef long ssize_t;
#else
typedef int ssize_t;
#endif
#endif
#define _SSIZE_T_DECLARED
#define __ssize_t_defined
#endif

#if !defined(_SUSECONDS_T_DECLARED) && !defined(__suseconds_t_defined)
typedef long suseconds_t;
#define _SUSECONDS_T_DECLARED
#define __suseconds_t_defined
#endif

#if !defined(_TIME_T_DECLARED) && !defined(__time_t_defined)
typedef int64_t time_t;
#define _TIME_T_DECLARED
#define __time_t_defined
#endif

#if !defined(_TIMER_T_DECLARED) && !defined(__timer_t_defined)
typedef unsigned long timer_t;
#define _TIMER_T_DECLARED
#define __timer_t_defined
#endif

/* trace_attr_t, trace_event_id_t, trace_id_t not defined */

#if !defined(_UID_T_DECLARED) && !defined(__uid_t_defined)
typedef unsigned short uid_t;
#define _UID_T_DECLARED
#define __uid_t_defined
#endif

#if !defined(_USECONDS_T_DECLARED) && !defined(__useconds_t_defined)
typedef unsigned long useconds_t;
#define _USECONDS_T_DECLARED
#define __useconds_t_defined
#endif

#ifdef __cplusplus
}
#endif

#endif /* defined(_POSIX_C_SOURCE) */

#endif /* _SYS_TYPES_H */
