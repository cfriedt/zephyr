/*
 * Copyright (c) 2026 Baochip contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include <kernel_arch_interface.h>

/* test hooks in arch/riscv/core/mmu.c */
extern int riscv_mmu_nb_free_tables(void);
extern int riscv_mmu_tables_total_usage(void);

#define TEST_VIRT_ADDR 0x456560000ULL
#define TEST_PHYS_ADDR 0x123230000ULL

static int initial_nb_free_tables;
static int initial_tables_usage;

static void *riscv_mmu_test_init(void)
{
	initial_nb_free_tables = riscv_mmu_nb_free_tables();
	initial_tables_usage = riscv_mmu_tables_total_usage();

	TC_PRINT("  Total page tables:           %d\n", CONFIG_MAX_RISCV_PT_TABLES);
	TC_PRINT("  Initial free tables:         %d\n", initial_nb_free_tables);
	TC_PRINT("  Initial total table usage:   %d\n", initial_tables_usage);

	zassert_true(initial_nb_free_tables > 1,
		     "initial_nb_free_tables = %d", initial_nb_free_tables);
	zassert_true(initial_tables_usage >= 1,
		     "initial_tables_usage = %d", initial_tables_usage);

	return NULL;
}

static int mem_map_test(uintptr_t virt_addr, uintptr_t phys_addr, size_t size)
{
	uintptr_t looked_up;
	int ret;

	arch_mem_map((void *)virt_addr, phys_addr, size, K_MEM_PERM_RW);

	ret = arch_page_phys_get((void *)virt_addr, &looked_up);
	zassert_equal(ret, 0, "arch_page_phys_get failed: %d", ret);
	zassert_equal(looked_up, phys_addr, "phys mismatch");

	int mapped_nb_free_tables = riscv_mmu_nb_free_tables();
	int mapped_tables_usage = riscv_mmu_tables_total_usage();

	zassert_true(mapped_nb_free_tables < initial_nb_free_tables,
		     "%d vs %d", mapped_nb_free_tables, initial_nb_free_tables);
	zassert_true(mapped_tables_usage > initial_tables_usage,
		     "%d vs %d", mapped_tables_usage, initial_tables_usage);

	arch_mem_unmap((void *)virt_addr, size);

	ret = arch_page_phys_get((void *)virt_addr, &looked_up);
	zassert_equal(ret, -EFAULT, "unmap did not clear mapping");

	return mapped_nb_free_tables - riscv_mmu_nb_free_tables();
}

ZTEST(riscv_mmu, test_riscv_mmu_single_page)
{
	int tables_used = mem_map_test(TEST_VIRT_ADDR, TEST_PHYS_ADDR, CONFIG_MMU_PAGE_SIZE);

	zassert_true(tables_used >= 1, "used %d tables", tables_used);
}

ZTEST_SUITE(riscv_mmu, NULL, riscv_mmu_test_init, NULL, NULL, NULL);
