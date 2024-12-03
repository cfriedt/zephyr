/*
 * Copyright (c) 2018-2023 Intel Corporation
 * Copyright (c) Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SYS_SCHED_H_
#define _SYS_SCHED_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Other mandatory scheduling policy. Must be numerically distinct. May
 * execute identically to SCHED_RR or SCHED_FIFO. For Zephyr this is a
 * pseudonym for SCHED_RR.
 */
#define SCHED_OTHER 0

/* Cooperative scheduling policy */
#define SCHED_FIFO 1

/* Priority based preemptive scheduling policy */
#define SCHED_RR 2

#if defined(CONFIG_MINIMAL_LIBC) || defined(CONFIG_PICOLIBC) ||                                    \
	defined(CONFIG_ARMCLANG_STD_LIBC) || defined(CONFIG_ARCMWDT_LIBC)
struct sched_param {
	int sched_priority;
#if defined(_POSIX_SPORADIC_SERVER) || defined(_POSIX_THREAD_SPORADIC_SERVER)
	/* reserved */
#endif
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SCHED_H_ */
