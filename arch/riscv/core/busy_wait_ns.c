/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>

#define NS_PER_CYCLE (NSEC_PER_SEC / sys_clock_hw_cycles_per_sec())

void arch_busy_wait_ns(uint32_t nsec_to_wait)
{
	uint64_t cycle_count;

	__asm__ volatile("rdcycle %0" : "=r"(cycle_count));
	uint64_t cycles = DIV_ROUND_UP(nsec_to_wait, NS_PER_CYCLE) + cycle_count;
	
	while (cycle_count < cycles) {
		__asm__ volatile("rdcycle %0" : "=r"(cycle_count));
	}
}
