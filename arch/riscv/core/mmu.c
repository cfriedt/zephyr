/*
 * Copyright (c) 2026 Baochip contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * RISC-V Sv32/Sv39 MMU support for Zephyr (S-mode, satp-based paging).
 */

#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/kernel/mm.h>
#include <zephyr/sys/util.h>
#include <zephyr/arch/riscv/csr.h>
#include <kernel_arch_interface.h>
#include <mmu.h>

BUILD_ASSERT(IS_ENABLED(CONFIG_RISCV_S_MODE),
	     "RISC-V MMU requires CONFIG_RISCV_S_MODE");
BUILD_ASSERT(CONFIG_MMU_PAGE_SIZE == 0x1000,
	     "RISC-V MMU currently supports 4 KiB pages only");

#if defined(CONFIG_64BIT)
typedef uint64_t riscv_pte_t;
#define RISCV_PTE_PPN_SHIFT 10
#define RISCV_PTE_CNT       512
#define RISCV_PTE_TOP_SHIFT 25
#else
typedef uint32_t riscv_pte_t;
#define RISCV_PTE_PPN_SHIFT 10
#define RISCV_PTE_CNT       (IS_ENABLED(CONFIG_RISCV_MMU_SV32) ? 1024 : 512)
#define RISCV_PTE_TOP_SHIFT (IS_ENABLED(CONFIG_RISCV_MMU_SV32) ? 22 : 25)
#endif

#define RISCV_PT_LEVELS (IS_ENABLED(CONFIG_RISCV_MMU_SV39) ? 3 : 2)
#define RISCV_PT_INDEX_BITS (IS_ENABLED(CONFIG_RISCV_MMU_SV32) ? 10 : 9)
#define RISCV_PT_INDEX_MASK ((1U << RISCV_PT_INDEX_BITS) - 1U)

#define RISCV_SATP_MODE (IS_ENABLED(CONFIG_RISCV_MMU_SV39) ? SATP_MODE_SV39 : SATP_MODE_SV32)

struct riscv_pt_page {
	riscv_pte_t entries[RISCV_PTE_CNT];
};

static struct riscv_pt_page pt_pool[CONFIG_MAX_RISCV_PT_TABLES]
	__aligned(CONFIG_MMU_PAGE_SIZE);
static uint8_t pt_usage[CONFIG_MAX_RISCV_PT_TABLES];
static riscv_pte_t *root_table;

static inline void riscv_fence_vma(void)
{
	__asm__ volatile("sfence.vma" ::: "memory");
}

static inline uintptr_t riscv_pt_index(uintptr_t va, int level)
{
	const int shift = 12 + (RISCV_PT_INDEX_BITS * level);

	return (va >> shift) & RISCV_PT_INDEX_MASK;
}

static inline uintptr_t riscv_pte_ppn(riscv_pte_t pte)
{
	return (uintptr_t)(pte >> RISCV_PTE_PPN_SHIFT);
}

static inline uintptr_t riscv_pte_pa(riscv_pte_t pte)
{
	return riscv_pte_ppn(pte) << 12;
}

static inline riscv_pte_t riscv_pa_pte(uintptr_t pa, riscv_pte_t flags)
{
	return (riscv_pte_t)((pa >> 12) << RISCV_PTE_PPN_SHIFT) | flags;
}

static inline bool riscv_va_valid(uintptr_t va)
{
#if defined(CONFIG_64BIT)
	const uintptr_t max_va = BIT(38);

	return va < max_va;
#else
	if (!IS_ENABLED(CONFIG_RISCV_MMU_SV39)) {
		return true;
	}

	const uintptr_t sign_bit = BIT(31);

	return ((va & sign_bit) == 0) || ((va & ~BIT_MASK(32 - 9)) == ~BIT_MASK(32 - 9));
#endif
}

static struct riscv_pt_page *pt_alloc(void)
{
	for (int i = 0; i < CONFIG_MAX_RISCV_PT_TABLES; i++) {
		if (pt_usage[i] == 0U) {
			memset(&pt_pool[i], 0, sizeof(pt_pool[i]));
			pt_usage[i] = 1U;
			return &pt_pool[i];
		}
	}

	return NULL;
}

#ifdef CONFIG_ZTEST
int riscv_mmu_nb_free_tables(void)
{
	int free_cnt = 0;

	for (int i = 0; i < CONFIG_MAX_RISCV_PT_TABLES; i++) {
		if (pt_usage[i] == 0U) {
			free_cnt++;
		}
	}

	return free_cnt;
}

int riscv_mmu_tables_total_usage(void)
{
	int used = 0;

	for (int i = 0; i < CONFIG_MAX_RISCV_PT_TABLES; i++) {
		used += pt_usage[i];
	}

	return used;
}
#endif /* CONFIG_ZTEST */

static riscv_pte_t *pt_walk(riscv_pte_t *table, uintptr_t va, bool alloc)
{
	for (int level = RISCV_PT_LEVELS - 1; level > 0; level--) {
		const uintptr_t idx = riscv_pt_index(va, level);
		riscv_pte_t *pte = &table[idx];

		if ((*pte & PTE_V) != 0U) {
			if (!PTE_TABLE(*pte)) {
				return NULL;
			}

			table = (riscv_pte_t *)(riscv_pte_pa(*pte));
			continue;
		}

		if (!alloc) {
			return NULL;
		}

		struct riscv_pt_page *new_table = pt_alloc();

		if (new_table == NULL) {
			return NULL;
		}

		*pte = riscv_pa_pte((uintptr_t)new_table, PTE_V);
		table = new_table->entries;
	}

	return &table[riscv_pt_index(va, 0)];
}

static riscv_pte_t riscv_flags_to_pte(uint32_t flags)
{
	riscv_pte_t pte = PTE_V | PTE_A | PTE_D;

	if ((flags & K_MEM_PERM_RW) == K_MEM_PERM_RW) {
		pte |= PTE_R | PTE_W;
	} else {
		pte |= PTE_R;
	}

	if ((flags & K_MEM_PERM_EXEC) == K_MEM_PERM_EXEC) {
		pte |= PTE_X;
	}

	if ((flags & K_MEM_PERM_USER) == K_MEM_PERM_USER) {
		pte |= PTE_U;
	}

	return pte;
}

static int map_range(uintptr_t va, uintptr_t pa, size_t size, uint32_t flags)
{
	const riscv_pte_t pte_flags = riscv_flags_to_pte(flags);

	__ASSERT((va & (CONFIG_MMU_PAGE_SIZE - 1)) == 0U, "va not aligned");
	__ASSERT((pa & (CONFIG_MMU_PAGE_SIZE - 1)) == 0U, "pa not aligned");
	__ASSERT((size & (CONFIG_MMU_PAGE_SIZE - 1)) == 0U, "size not aligned");

	for (size_t offset = 0; offset < size; offset += CONFIG_MMU_PAGE_SIZE) {
		const uintptr_t page_va = va + offset;
		const uintptr_t page_pa = pa + offset;
		riscv_pte_t *pte;

		if (!riscv_va_valid(page_va)) {
			return -EINVAL;
		}

		pte = pt_walk(root_table, page_va, true);
		if (pte == NULL) {
			return -ENOMEM;
		}

		*pte = riscv_pa_pte(page_pa, pte_flags);
	}

	return 0;
}

static void unmap_range(uintptr_t va, size_t size)
{
	__ASSERT((va & (CONFIG_MMU_PAGE_SIZE - 1)) == 0U, "va not aligned");
	__ASSERT((size & (CONFIG_MMU_PAGE_SIZE - 1)) == 0U, "size not aligned");

	for (size_t offset = 0; offset < size; offset += CONFIG_MMU_PAGE_SIZE) {
		const uintptr_t page_va = va + offset;
		riscv_pte_t *pte = pt_walk(root_table, page_va, false);

		if (pte != NULL) {
			*pte = 0;
		}
	}
}

static uintptr_t make_satp(riscv_pte_t *root)
{
	uintptr_t satp;
	const uintptr_t ppn = (uintptr_t)root >> 12;

#if defined(CONFIG_64BIT)
	satp = ((uintptr_t)RISCV_SATP_MODE << 60) | ppn;
#else
	satp = ((uintptr_t)RISCV_SATP_MODE << 31) | ppn;
#endif

	return satp;
}

void z_riscv_mmu_init(void)
{
	struct riscv_pt_page *root = pt_alloc();
	uintptr_t vm_base = CONFIG_KERNEL_VM_BASE;
	size_t vm_size = CONFIG_KERNEL_VM_SIZE;
	int ret;

	__ASSERT(root != NULL, "failed to allocate root page table");
	root_table = root->entries;

	/* Identity-map the kernel virtual memory window. */
	ret = map_range(vm_base, vm_base, vm_size, K_MEM_PERM_RW | K_MEM_PERM_EXEC);
	__ASSERT(ret == 0, "failed to map kernel VM window (%d)", ret);

	/* Identity-map low physical memory for MMIO (UART, INTC, etc.). */
	if (CONFIG_RISCV_MMU_BOOT_IO_SIZE > 0U) {
		ret = map_range(0, 0, CONFIG_RISCV_MMU_BOOT_IO_SIZE,
				K_MEM_PERM_RW | K_MEM_PERM_EXEC);
		__ASSERT(ret == 0, "failed to map boot IO window (%d)", ret);
	}

	riscv_fence_vma();
	csr_write(CSR_SATP, make_satp(root_table));
	riscv_fence_vma();
}

void arch_mem_map(void *virt, uintptr_t phys, size_t size, uint32_t flags)
{
	const int ret = map_range((uintptr_t)virt, phys, size, flags);

	if (ret != 0) {
		k_panic();
	}

	riscv_fence_vma();
}

void arch_mem_unmap(void *addr, size_t size)
{
	unmap_range((uintptr_t)addr, size);
	riscv_fence_vma();
}

int arch_page_phys_get(void *virt, uintptr_t *phys)
{
	const uintptr_t va = (uintptr_t)virt;
	riscv_pte_t *pte = pt_walk(root_table, va, false);

	if (pte == NULL || (*pte & PTE_V) == 0U || PTE_TABLE(*pte)) {
		return -EFAULT;
	}

	if (phys != NULL) {
		*phys = riscv_pte_pa(*pte) | (va & (CONFIG_MMU_PAGE_SIZE - 1));
	}

	return 0;
}
