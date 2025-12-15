/*
 * Copyright (c) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <threads.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

void call_once(once_flag *flag, void (*func)(void))
{
	if (atomic_cas(flag, 0, 1)) {
		func();
	}
}
