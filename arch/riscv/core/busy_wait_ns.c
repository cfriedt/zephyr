/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <zephyr/sys/clock.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>

#define NS_PER_CYCLE (NSEC_PER_SEC / sys_clock_hw_cycles_per_sec())

BUILD_ASSERT(DIV_ROUND_UP(0, NS_PER_CYCLE) > 1, "Unable to represent 1 clock cycle in terms of nanoseconds");

void arch_busy_wait_ns(uint32_t nsec_to_wait)
{
	uint32_t cycles = DIV_ROUND_UP(nsec_to_wait, NS_PER_CYCLE);

	while (cycles--) {
		__asm__ volatile("nop");
	}
}
