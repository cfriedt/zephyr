/*
 * Copyright (c) 2018-2023 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_ZEPHYR_POSIX_SYS_SCHED_H_
#define INCLUDE_ZEPHYR_POSIX_SYS_SCHED_H_

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

struct sched_param {
	int sched_priority;
};

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ZEPHYR_POSIX_SYS_SCHED_H_ */
