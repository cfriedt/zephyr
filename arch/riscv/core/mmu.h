/*
 * Copyright (c) 2026 Baochip contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ARCH_RISCV_INCLUDE_MMU_H_
#define ARCH_RISCV_INCLUDE_MMU_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef CONFIG_RISCV_MMU

void z_riscv_mmu_init(void);

#ifdef CONFIG_ZTEST
int riscv_mmu_nb_free_tables(void);
int riscv_mmu_tables_total_usage(void);
#endif /* CONFIG_ZTEST */

#endif /* CONFIG_RISCV_MMU */

#endif /* ARCH_RISCV_INCLUDE_MMU_H_ */
