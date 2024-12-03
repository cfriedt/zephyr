/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYS__SIGSET_H_
#define _SYS__SIGSET_H_

#include <zephyr/posix/sys/features.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned long sig[DIV_ROUND_UP((RTSIG_MAX + 1), BITS_PER_LONG)];
} __sigset_t;

#ifdef __cplusplus
}
#endif

#endif /* _SYS__SIGSET_H_ */
