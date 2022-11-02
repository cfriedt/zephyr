/*
 * Copyright 2022 Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_PCIE_EDU_H_
#define ZEPHYR_DRIVERS_PCIE_EDU_H_

#include <zephyr/sys/util.h>

/* See https://github.com/qemu/qemu/blob/master/docs/specs/edu.txt */

#define PCIE_EDU_REG_ID 0x00

#define PCIE_EDU_REG_ID_VER_MAJ_MASK  BIT_MASK(8)
#define PCIE_EDU_REG_ID_VER_MAJ_SHIFT 24
#define PCIE_EDU_REG_ID_VER_MAJ(word)                                                              \
	((uint8_t)(((word) >> PCIE_EDU_REG_ID_VER_MAJ_SHIFT) & PCIE_EDU_REG_ID_VER_MAJ_MASK))

#define PCIE_EDU_REG_ID_VER_MIN_MASK  BIT_MASK(8)
#define PCIE_EDU_REG_ID_VER_MIN_SHIFT 16
#define PCIE_EDU_REG_ID_VER_MIN(word)                                                              \
	((uint8_t)(((word) >> PCIE_EDU_REG_ID_VER_MIN_SHIFT) & PCIE_EDU_REG_ID_VER_MIN_MASK))

#define PCIE_EDU_REG_ID_MAGIC_MASK  BIT_MASK(16)
#define PCIE_EDU_REG_ID_MAGIC_SHIFT 0
#define PCIE_EDU_REG_ID_MAGIC(word)                                                                \
	((uint16_t)(((word) >> PCIE_EDU_REG_ID_MAGIC_SHIFT) & PCIE_EDU_REG_ID_MAGIC_MASK))

#define PCIE_EDU_REG_LIVE		 0x04
#define PCIE_EDU_REG_FACT		 0x08
#define PCIE_EDU_REG_STATUS		 0x20
#define PCIE_EDU_REG_STATUS_FACT_MASK	 BIT_MASK(1)
#define PCIE_EDU_REG_STATUS_FACT_SHIFT	 0
#define PCIE_EDU_REG_STATUS_IRQ_MASK	 BIT_MASK(1)
#define PCIE_EDU_REG_STATUS_IRQ_SHIFT	 7
#define PCIE_EDU_REG_IRQ_STATUS		 0x24
#define PCIE_EDU_REG_IRQ_RAISE		 0x60
#define PCIE_EDU_REG_IRQ_ACK		 0x60
#define PCIE_EDU_REG_DMA_SRC_ADDR	 0x80
#define PCIE_EDU_REG_DMA_DST_ADDR	 0x88
#define PCIE_EDU_REG_DMA_CNT		 0x90
#define PCIE_EDU_REG_DMA_CMD		 0x98
#define PCIE_EDU_REG_DMA_CMD_START_MASK	 BIT_MASK(1)
#define PCIE_EDU_REG_DMA_CMD_START_SHIFT 0
#define PCIE_EDU_REG_DMA_CMD_DIR_MASK	 BIT_MASK(1)
#define PCIE_EDU_REG_DMA_CMD_DIR_SHIFT	 1
#define PCIE_EDU_REG_DMA_CMD_RAISE_MASK	 BIT_MASK(1)
#define PCIE_EDU_REG_DMA_CMD_RAISE_SHIFT 2

#endif
