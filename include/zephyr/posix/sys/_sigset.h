/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS__SIGSET_H_
#define ZEPHYR_INCLUDE_ZEPHYR_POSIX_SYS__SIGSET_H_

#include <sys/features.h>
#include <zephyr/sys/util.h>

#define SIGRTMIN 32
#define SIGRTMAX (SIGRTMIN + RTSIG_MAX)

#if !defined(_SIGSET_T_DECLARED)
struct sigset_s {
	unsigned long sig[MAX(DIV_ROUND_UP((SIGRTMIN + RTSIG_MAX), BITS_PER_LONG), 1)];
};
typedef struct sigset_s __sigset_t;
typedef __sigset_t sigset_t;
#define _SIGSET_T_DECLARED
#endif

#endif
