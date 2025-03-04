/*
 * Copyright 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_SPAWN_H_
#define ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_SPAWN_H_

#include <zephyr/posix/posix_features.h>
#include <zephyr/posix/posix_types.h>
#include <zephyr/posix/sched.h>

#if !defined(_MODE_T_DECLARED) && !defined(__mode_t_defined)
typedef unsigned long mode_t;
#define _MODE_T_DECLARED
#define __mode_t_defined
#endif

#if !defined(_PID_T_DECLARED) && !defined(__pid_t_defined)
typedef long pid_t;
#define _PID_T_DECLARED
#define __pid_t_defined
#endif

#if !defined(_SIGSET_T_DECLARED) && !defined(__sigset_t_defined)
typedef struct {
	unsigned long sig[DIV_ROUND_UP((32 + RTSIG_MAX + 1), BITS_PER_LONG)];
} sigset_t;
#define _SIGSET_T_DECLARED
#define __sigset_t_defined
#endif

typedef struct {
	short posix_attr_flags;
	pid_t posix_attr_pgroup;
	sigset_t posix_attr_sigmask;
	sigset_t posix_attr_sigdefault;
	int posix_attr_schedpolicy;
	struct sched_param posix_attr_schedparam;
} posix_spawnattr_t;

typedef struct {

} posix_spawn_file_actions_t;

#endif /* ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS_SPAWN_H_ */
