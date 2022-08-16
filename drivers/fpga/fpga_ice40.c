/*
 * Copyright (c) 2022 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT lattice_ice40_fpga

#include <stdbool.h>
#include <stdio.h>

#include <zephyr/device.h>
#include <zephyr/drivers/fpga.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/posix/time.h>
#include <zephyr/sys/crc.h>
#include <zephyr/zephyr.h>

#ifndef BITS_PER_NIBBLE
#define BITS_PER_NIBBLE 4
#endif

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#endif

#ifndef NIBBLES_PER_BYTE
#define NIBBLES_PER_BYTE (BITS_PER_BYTE / BITS_PER_NIBBLE)
#endif

#define FPGA_ICE40_DELAY_CLOCKS 8

#define FPGA_ICE40_SPI_FREQ_MIN 1000000
#define FPGA_ICE40_SPI_FREQ_MAX 25000000

#define FPGA_ICE40_CRESET_DELAY_NS_MIN 200
#define FPGA_ICE40_CONFIG_DELAY_US_MIN 300
#define FPGA_ICE40_LEADING_CLOCKS_MIN  8
#define FPGA_ICE40_TRAILING_CLOCKS_MIN 149

LOG_MODULE_REGISTER(fpga_ice40);

struct fpga_ice40_data {
	uint32_t crc;
	/* simply use crc32 as info */
	char info[2 * sizeof(uint32_t) + 1];
	bool on;
	bool loaded;
	struct k_spinlock lock;
};

struct fpga_ice40_config {
	struct spi_dt_spec bus;
	struct gpio_dt_spec cdone;
	struct gpio_dt_spec creset;
	uint16_t config_delay_us;
	uint8_t creset_delay_ns;
	uint8_t leading_clocks;
	uint8_t trailing_clocks;
};

static void fpga_ice40_crc_to_str(uint32_t crc, char *s)
{
	char ch;
	uint8_t i;
	uint8_t nibble;
	const char *table = "0123456789abcdef";

	for (i = 0; i < sizeof(crc) * NIBBLES_PER_BYTE; ++i, crc >>= BITS_PER_NIBBLE) {
		nibble = crc & GENMASK(BITS_PER_NIBBLE, 0);
		ch = table[nibble];
		s[sizeof(crc) * NIBBLES_PER_BYTE - i - 1] = ch;
	}

	s[sizeof(crc) * NIBBLES_PER_BYTE] = '\0';
}

static enum FPGA_status fpga_ice40_get_status(const struct device *dev)
{
	enum FPGA_status st;
	k_spinlock_key_t key;
	struct fpga_ice40_data *data = dev->data;

	key = k_spin_lock(&data->lock);
	/* TODO: make 'on' stateless: i.e. direction == out && CRESET_B == 1 */
	if (data->loaded && data->on) {
		st = FPGA_STATUS_ACTIVE;
	} else {
		st = FPGA_STATUS_INACTIVE;
	}

	k_spin_unlock(&data->lock, key);

	return st;
}

/*
 * See iCE40 Family Handbook, Appendix A. SPI Slave Configuration Procedure, pp 15-21
 *
 * https://www.latticesemi.com/~/media/LatticeSemi/Documents/Handbooks/iCE40FamilyHandbook.pdf
 *
 * This is a bit tricky.
 *
 * We want to use the SPI hardware and driver to deliver perfectly-timed clocks and manage
 * the chip-select, mainly to avoid calibrated delay loops and bit-banging. However, SPI_CS
 * must be pulled high to deliver 8 leading clocks and 149 trailing clocks. Normally, SPI_CS is
 * pulled low by the driver, which poses a bit of a problem.
 *
 * Ideally, we would be able to change the active-polarity of the chip-select between buffers
 * but the Zephyr SPI driver model does not support that use case. Also, spi_dt_spec's are
 * usually in ROM which means it cannot be modified in-place.
 *
 * To achieve our goals, we
 * 1. clone the struct spi_dt_spec and flip active-polarity of the spi_cs_control gpio
 * 2. set SPI_CS high and send leading clocks
 * 3. set SPI_CS low and send the bitstream
 * 4. set SPI_CS high and send trailing clocks
 */
int fpga_ice40_load(const struct device *dev, uint32_t *image_ptr, uint32_t img_size)
{
	int ret;
	uint32_t crc;
	uint32_t delay_us1;
	uint32_t delay_us2;
	k_spinlock_key_t key;
	struct spi_buf tx_buf[3];
	struct spi_dt_spec tmp_bus;
	struct spi_cs_control tmp_cs;
	struct spi_buf_set tx_bufs = {
		.count = 1,
	};
	uint8_t zeros[UINT8_MAX + 1] = {0};
	struct fpga_ice40_data *data = dev->data;
	const struct fpga_ice40_config *config = dev->config;

	delay_us1 = ceiling_fraction(config->creset_delay_ns, NSEC_PER_USEC);
	delay_us2 = ceiling_fraction(config->creset_delay_ns, NSEC_PER_USEC);

	zeros[0] = 0xaa;
	tx_buf[0].buf = zeros;
	// tx_buf[0].len = ceiling_fraction(config->leading_clocks, BITS_PER_BYTE);
	tx_buf[0].len = 4;
	tx_buf[1].buf = image_ptr;
	// tx_buf[1].len = img_size;
	tx_buf[1].len = 4;
	tx_buf[2].buf = zeros;
	// tx_buf[2].len = ceiling_fraction(config->trailing_clocks, BITS_PER_BYTE);
	tx_buf[2].len = 4;

	/* cloned spi_dt_spec */
	tmp_bus = config->bus;
	tmp_cs = *config->bus.config.cs;
	tmp_bus.config.cs = &tmp_cs;

	if ((tmp_cs.gpio.dt_flags & GPIO_ACTIVE_LOW) == 0) {
		tmp_cs.gpio.dt_flags |= GPIO_ACTIVE_LOW;
	} else {
		tmp_cs.gpio.dt_flags &= ~GPIO_ACTIVE_LOW;
	}

	crc = crc32_c(0, (uint8_t *)image_ptr, img_size, true, true);
	if (data->loaded && crc == data->crc) {
		LOG_WRN("already loaded with image CRC32c: 0x%08x", data->crc);
	}

retry:
	key = k_spin_lock(&data->lock);

	LOG_DBG("Initializing GPIO");
	if (/*(ret = gpio_pin_configure_dt(&config->cdone, GPIO_INPUT)) || */
	    (ret = gpio_pin_configure_dt(&config->bus.config.cs->gpio, GPIO_OUTPUT_HIGH)) ||
	    (ret = gpio_pin_configure_dt(&config->creset, GPIO_OUTPUT_HIGH))) {
		LOG_ERR("Failed to initialize GPIO: %d", ret);
		goto unlock;
	}

	data->crc = 0;
	data->loaded = false;
	fpga_ice40_crc_to_str(0, data->info);

repeat:
	LOG_DBG("Initializing GPIO");
	if (/*(ret = gpio_pin_configure_dt(&config->cdone, GPIO_INPUT)) || */
	    (ret = gpio_pin_configure_dt(&config->bus.config.cs->gpio, GPIO_OUTPUT_HIGH)) ||
	    (ret = gpio_pin_configure_dt(&config->creset, GPIO_OUTPUT_HIGH))) {
		LOG_ERR("Failed to initialize GPIO: %d", ret);
		goto unlock;
	}

	LOG_DBG("Set CRESET low");
	LOG_DBG("Set SPI_CS low");
	/* Note: CRESET should be pulled low first */
	if ((ret = gpio_pin_configure_dt(&config->creset, GPIO_OUTPUT_LOW)) ||
	    (ret = gpio_pin_configure_dt(&config->bus.config.cs->gpio, GPIO_OUTPUT_LOW))) {
		LOG_ERR("failed to set CRESET or SPI_CS low: %d", ret);
		goto unlock;
	}

	goto repeat;

#if 0
	/* Wait a minimum of 200ns */
	LOG_DBG("Delay %u ns (%u us)", config->creset_delay_ns, delay_us1);
	k_busy_wait(delay_us1);
#endif

	LOG_DBG("Set CRESET high");
	ret = gpio_pin_set_dt(&config->creset, GPIO_OUTPUT_HIGH);
	if (ret < 0) {
		LOG_ERR("failed to set CRESET high: %d", ret);
		goto unlock;
	}

	LOG_DBG("Delay %u us", config->config_delay_us);
	k_busy_wait(config->config_delay_us);

	LOG_DBG("Set SPI_CS high");
	LOG_DBG("Send %u clocks", config->leading_clocks);
	tx_bufs.buffers = &tx_buf[0];
	ret = spi_write_dt(&tmp_bus, &tx_bufs);
	if (ret < 0) {
		LOG_ERR("Failed to send %u clocks: %d", config->leading_clocks, ret);
		goto unlock;
	}

	LOG_DBG("Set SPI_CS low");
	LOG_DBG("Send bin file");
	tx_bufs.buffers = &tx_buf[1];
	ret = spi_write_dt(&config->bus, &tx_bufs);
	if (ret < 0) {
		LOG_ERR("Failed to send bin file: %d", ret);
		goto unlock;
	}

	LOG_DBG("Set SPI_CS high");
	LOG_DBG("Send %u clocks", config->trailing_clocks);
	tx_bufs.buffers = &tx_buf[2];
	ret = spi_write_dt(&tmp_bus, &tx_bufs);
	if (ret < 0) {
		LOG_ERR("Failed to send %u clocks: %d", config->trailing_clocks, ret);
		goto unlock;
	}

	LOG_DBG("checking CDONE");
	ret = gpio_pin_get_dt(&config->cdone);
	if (ret < 0) {
		LOG_ERR("failed to read CDONE: %d", ret);
		goto unlock;
	} else if (ret != 1) {
		ret = -EIO;
		LOG_ERR("CDONE did not go high");
		goto unlock;
	}

	ret = 0;
	data->loaded = true;
	fpga_ice40_crc_to_str(crc, data->info);
	LOG_DBG("Loaded image with CRC32c 0x%08x", crc);

unlock:
	k_spin_unlock(&data->lock, key);

	goto retry;

	return ret;
}

static int fpga_ice40_on_off(const struct device *dev, bool on)
{
	int ret;
	k_spinlock_key_t key;
	struct fpga_ice40_data *data = dev->data;
	const struct fpga_ice40_config *config = dev->config;

	key = k_spin_lock(&data->lock);

	ret = gpio_pin_configure_dt(&config->creset, on ? GPIO_OUTPUT_HIGH : GPIO_OUTPUT_LOW);
	if (ret < 0) {
		goto unlock;
	}

	data->on = on;
	ret = 0;

unlock:
	k_spin_unlock(&data->lock, key);

	return ret;
}

static int fpga_ice40_on(const struct device *dev)
{
	return fpga_ice40_on_off(dev, true);
}

static int fpga_ice40_off(const struct device *dev)
{
	return fpga_ice40_on_off(dev, false);
}

static int fpga_ice40_reset(const struct device *dev)
{
	return fpga_ice40_off(dev) || fpga_ice40_on(dev);
}

static const char *fpga_ice40_get_info(const struct device *dev)
{
	struct fpga_ice40_data *data = dev->data;

	return data->info;
}

static const struct fpga_driver_api fpga_ice40_api = {
	.get_status = fpga_ice40_get_status,
	.reset = fpga_ice40_reset,
	.load = fpga_ice40_load,
	.on = fpga_ice40_on,
	.off = fpga_ice40_off,
	.get_info = fpga_ice40_get_info,
};

static int fpga_ice40_init(const struct device *dev)
{
	int ret;
	const struct fpga_ice40_config *config = dev->config;

	ret = gpio_pin_configure_dt(&config->creset, GPIO_OUTPUT_HIGH);
	if (ret < 0) {
		LOG_ERR("failed to configure CRESET: %d", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&config->cdone, GPIO_INPUT);
	{
		LOG_ERR("Failed to initialize CDONE: %d", ret);
		return ret;
	}

	return 0;
}

#define FPGA_ICE40_BUS_FREQ(inst) DT_INST_PROP(inst, spi_max_frequency)

#define FPGA_ICE40_BUS_PERIOD_NS(inst) (NSEC_PER_SEC / FPGA_ICE40_BUS_FREQ(inst))

#define FPGA_ICE40_CONFIG_DELAY_US(inst)                                                           \
	DT_INST_PROP_OR(inst, config_delay_us, FPGA_ICE40_CONFIG_DELAY_US_MIN)

#define FPGA_ICE40_CRESET_DELAY_NS(inst)                                                           \
	DT_INST_PROP_OR(inst, creset_delay_ns, FPGA_ICE40_CRESET_DELAY_NS_MIN)

#define FPGA_ICE40_LEADING_CLOCKS(inst)                                                            \
	DT_INST_PROP_OR(inst, leading_clocks, FPGA_ICE40_LEADING_CLOCKS_MIN)

#define FPGA_ICE40_TRAILING_CLOCKS(inst)                                                           \
	DT_INST_PROP_OR(inst, trailing_clocks, FPGA_ICE40_TRAILING_CLOCKS_MIN)

#define FPGA_ICE40_DEFINE(inst)                                                                    \
	BUILD_ASSERT(FPGA_ICE40_BUS_FREQ(inst) >= FPGA_ICE40_SPI_FREQ_MIN);                        \
	BUILD_ASSERT(FPGA_ICE40_BUS_FREQ(inst) <= FPGA_ICE40_SPI_FREQ_MAX);                        \
	BUILD_ASSERT(FPGA_ICE40_CONFIG_DELAY_US(inst) >= FPGA_ICE40_CONFIG_DELAY_US_MIN);          \
	BUILD_ASSERT(FPGA_ICE40_CONFIG_DELAY_US(inst) <= UINT16_MAX);                              \
	BUILD_ASSERT(FPGA_ICE40_CRESET_DELAY_NS(inst) >= FPGA_ICE40_CRESET_DELAY_NS_MIN);          \
	BUILD_ASSERT(FPGA_ICE40_CRESET_DELAY_NS(inst) <= UINT8_MAX);                               \
	BUILD_ASSERT(FPGA_ICE40_LEADING_CLOCKS(inst) >= FPGA_ICE40_LEADING_CLOCKS_MIN);            \
	BUILD_ASSERT(FPGA_ICE40_LEADING_CLOCKS(inst) <= UINT8_MAX);                                \
	BUILD_ASSERT(FPGA_ICE40_TRAILING_CLOCKS(inst) >= FPGA_ICE40_TRAILING_CLOCKS_MIN);          \
	BUILD_ASSERT(FPGA_ICE40_TRAILING_CLOCKS(inst) <= UINT8_MAX);                               \
                                                                                                   \
	static struct fpga_ice40_data fpga_ice40_data_##inst;                                      \
                                                                                                   \
	static const struct fpga_ice40_config fpga_ice40_config_##inst = {                         \
		.bus = SPI_DT_SPEC_INST_GET(                                                       \
			inst, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA,  \
			0),                                                                        \
		.cdone = GPIO_DT_SPEC_INST_GET(inst, cdone_gpios),                                 \
		.creset = GPIO_DT_SPEC_INST_GET(inst, creset_gpios),                               \
		.config_delay_us = FPGA_ICE40_CONFIG_DELAY_US(inst),                               \
		.creset_delay_ns = FPGA_ICE40_CRESET_DELAY_NS(inst),                               \
		.leading_clocks = FPGA_ICE40_LEADING_CLOCKS(inst),                                 \
		.trailing_clocks = FPGA_ICE40_TRAILING_CLOCKS(inst),                               \
	};                                                                                         \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(inst, fpga_ice40_init, NULL, &fpga_ice40_data_##inst,                \
			      &fpga_ice40_config_##inst, POST_KERNEL, 0, &fpga_ice40_api);

DT_INST_FOREACH_STATUS_OKAY(FPGA_ICE40_DEFINE)
