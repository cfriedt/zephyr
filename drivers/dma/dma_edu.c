/*
 * Copyright 2022 Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT zephyr_dma_edu

#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/pcie/edu.h>
#include <zephyr/drivers/pcie/msi.h>
#include <zephyr/drivers/pcie/pcie.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>

LOG_MODULE_DECLARE(dma_edu, LOG_LEVEL_DBG);

#ifdef CONFIG_DMA_64BIT
typedef uint64_t dma_addr_t;
#else
typedef uint32_t dma_addr_t;
#endif

struct dma_edu_config {
	pcie_bdf_t pcie_bdf;
	pcie_id_t pcie_id;
	uint16_t edu_magic;
};

struct dma_edu_data {
	struct dma_context context;
	struct pcie_bar mbar;
	mm_reg_t base;
	msi_vector_t msi;
};

static int dma_edu_configure(const struct device *dev, uint32_t channel, struct dma_config *config)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return -ENOSYS;
}

static int dma_edu_reload(const struct device *dev, uint32_t channel, dma_addr_t src,
			  dma_addr_t dst, size_t size)
{
	LOG_DBG("%s(): channel: %u, src: %p, addr: %p, size: %zu", __func__, channel, (void *)src,
		(void *)dst, size);
	return -ENOSYS;
}

static int dma_edu_start(const struct device *dev, uint32_t channel)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return -ENOSYS;
}

static int dma_edu_stop(const struct device *dev, uint32_t channel)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return -ENOSYS;
}

static int dma_edu_suspend(const struct device *dev, uint32_t channel)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return -ENOSYS;
}

static int dma_edu_resume(const struct device *dev, uint32_t channel)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return -ENOSYS;
}

static int dma_edu_get_status(const struct device *dev, uint32_t channel, struct dma_status *status)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return -ENOSYS;
}

static bool dma_edu_chan_filter(const struct device *dev, int channel, void *filter_param)
{
	LOG_DBG("%s(): channel: %u", __func__, channel);
	return false;
}

__subsystem static const struct dma_driver_api dma_edu_api = {
	.config = dma_edu_configure,
	.reload = dma_edu_reload,
	.start = dma_edu_start,
	.stop = dma_edu_stop,
	.suspend = dma_edu_suspend,
	.resume = dma_edu_resume,
	.get_status = dma_edu_get_status,
	.chan_filter = dma_edu_chan_filter,
};

static void dma_edu_isr(const void *parameter)
{
	const struct device *dev = parameter;
	const struct dma_edu_config *config = dev->config;
	struct dma_edu_data *data = dev->data;
	uint32_t irq_status;

	irq_status = sys_read32(data->base + PCIE_EDU_REG_IRQ_STATUS);

	LOG_DBG("irq triggered with status %x", irq_status);

	sys_write32(irq_status, data->base + PCIE_EDU_REG_IRQ_STATUS);
}

static int dma_edu_init(const struct device *dev)
{
	uint32_t id;
	struct dma_edu_data *data = dev->data;
	const struct dma_edu_config *config = dev->config;

	if (!pcie_probe(config->pcie_bdf, config->pcie_id)) {
		LOG_ERR("Failed to probe [%02x:%02x.%x]", PCIE_BDF_TO_BUS(config->pcie_bdf),
			PCIE_BDF_TO_DEV(config->pcie_bdf), PCIE_BDF_TO_FUNC(config->pcie_bdf));
		return -ENODEV;
	}

	if (!pcie_probe_mbar(config->pcie_bdf, 0, &data->mbar)) {
		LOG_ERR("Failed to probe mbar 0 of [%02x:%02x.%x]",
			PCIE_BDF_TO_BUS(config->pcie_bdf), PCIE_BDF_TO_DEV(config->pcie_bdf),
			PCIE_BDF_TO_FUNC(config->pcie_bdf));
		return -ENODEV;
	}

	pcie_set_cmd(config->pcie_bdf, PCIE_CONF_CMDSTAT_MEM, true);

	device_map(&data->base, data->mbar.phys_addr, data->mbar.size, K_MEM_CACHE_NONE);

	id = sys_read32(data->base + PCIE_EDU_REG_ID);
	if (PCIE_EDU_REG_ID_MAGIC(id) != config->edu_magic) {
		LOG_ERR("failed to read 0x%x from ID register (%x)", config->edu_magic, id);
		return -ENODEV;
	}

	if (pcie_msi_vectors_allocate(config->pcie_bdf, 0, &data->msi, 1) != 1) {
		LOG_ERR("Failed to allocate msi for [%02x:%02x.%x]",
			PCIE_BDF_TO_BUS(config->pcie_bdf), PCIE_BDF_TO_DEV(config->pcie_bdf),
			PCIE_BDF_TO_FUNC(config->pcie_bdf));
		return -EIO;
	}

	if (!pcie_msi_vector_connect(config->pcie_bdf, &data->msi, dma_edu_isr, dev, 0)) {
		LOG_ERR("Failed to connect msi vector for [%02x:%02x.%x]",
			PCIE_BDF_TO_BUS(config->pcie_bdf), PCIE_BDF_TO_DEV(config->pcie_bdf),
			PCIE_BDF_TO_FUNC(config->pcie_bdf));
		return -EIO;
	}

	if (!pcie_msi_enable(config->pcie_bdf, &data->msi, 1, 0)) {
		LOG_ERR("Failed to enable msi for [%02x:%02x.%x]",
			PCIE_BDF_TO_BUS(config->pcie_bdf), PCIE_BDF_TO_DEV(config->pcie_bdf),
			PCIE_BDF_TO_FUNC(config->pcie_bdf));
		return -EIO;
	}

	LOG_INF("Mapped bar 0 of %X device at BDF [%02x:%02x.%x] from %p to %p", config->edu_magic,
		PCIE_BDF_TO_BUS(config->pcie_bdf), PCIE_BDF_TO_DEV(config->pcie_bdf),
		PCIE_BDF_TO_FUNC(config->pcie_bdf), (void *)data->mbar.phys_addr,
		(void *)data->base);

	return 0;
}

#define DMA_EDU_MAGIC(inst)    DT_INST_PROP_OR(inst, edu_magic, 0x00edu)
#define DMA_EDU_CHANNELS(inst) DT_INST_PROP_OR(inst, dma_channels, 1)
#define DMA_EDU_ATOMICS(inst)  ceiling_fraction(DMA_EDU_CHANNELS(inst), sizeof(atomic_t))

#define DEFINE_DMA_EDU(inst)                                                                       \
	static const struct dma_edu_config dma_edu_config_##inst = {                               \
		.pcie_bdf = DT_INST_REG_ADDR(inst),                                                \
		.pcie_id = DT_INST_REG_SIZE(inst),                                                 \
		.edu_magic = DMA_EDU_MAGIC(inst),                                                  \
	};                                                                                         \
	BUILD_ASSERT(DMA_EDU_MAGIC(inst) <= UINT16_MAX, "Invalid EDU magic");                      \
                                                                                                   \
	static atomic_t dma_edu_atomics_##inst[DMA_EDU_ATOMICS(inst)];                             \
	static struct dma_edu_data dma_edu_data_##inst = {                                         \
		.context =                                                                         \
			{                                                                          \
				.dma_channels = DMA_EDU_CHANNELS(inst),                            \
				.atomic = dma_edu_atomics_##inst,                                  \
			},                                                                         \
	};                                                                                         \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(inst, dma_edu_init, PM_DEVICE_DT_INST_GET(inst),                     \
			      &dma_edu_data_##inst, &dma_edu_config_##inst, POST_KERNEL,           \
			      CONFIG_DMA_INIT_PRIORITY, &dma_edu_api);

DT_INST_FOREACH_STATUS_OKAY(DEFINE_DMA_EDU)
