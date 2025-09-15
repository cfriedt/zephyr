/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT riscv_machine_timer

#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>

//#define USE_MTIME 1
#define MTIME_REG    DT_INST_REG_ADDR_BY_NAME(0, mtime)
#define NS_PER_CYCLE (NSEC_PER_SEC / sys_clock_hw_cycles_per_sec())

static inline uint64_t mtime(void)
{
	uint64_t cycle_count;

#ifdef USE_MTIME
#ifdef CONFIG_64BIT
	cycle_count = *(volatile uint64_t *)MTIME_REG;
#else
	volatile uint32_t *r = (uint32_t *)MTIME_REG;
	uint32_t lo, hi;

	/* Likewise, must guard against rollover when reading */
	do {
		hi = r[1];
		lo = r[0];
	} while (r[1] != hi);

	cycle_count = (((uint64_t)hi) << 32) | lo;
#endif
#else
	__asm__ volatile("rdcycle %0" : "=r"(cycle_count));
#endif

	return cycle_count;
}

void arch_busy_wait_ns(uint32_t nsec_to_wait)
{
	uint64_t now = mtime();
	uint64_t then = now + k_ns_to_cyc_ceil64(nsec_to_wait);

	for ( ; now < then; now = mtime()) {
		/* busy loop */
	}

	/*
	 * k_cycle_get_64()
	 * mtime() << CONFIG_RISCV_MACHINE_TIMER_SYSTEM_CLOCK_DIVIDER;
	 */
}
