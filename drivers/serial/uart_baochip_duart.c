/*
 * Copyright (c) 2026 Baochip contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT baochip_duart

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define DUART_REG_TXD 0x00
#define DUART_REG_CR  0x04
#define DUART_REG_SR  0x08

struct uart_baochip_duart_config {
	mm_reg_t base;
};

static inline volatile uint32_t *duart_reg(const struct uart_baochip_duart_config *cfg,
					     uint32_t reg)
{
	return (volatile uint32_t *)(cfg->base + reg);
}

static inline uint32_t duart_read(const struct uart_baochip_duart_config *cfg, uint32_t reg)
{
	return *duart_reg(cfg, reg);
}

static inline void duart_write(const struct uart_baochip_duart_config *cfg, uint32_t reg,
			       uint32_t val)
{
	*duart_reg(cfg, reg) = val;
}

static void uart_baochip_duart_poll_out(const struct device *dev, unsigned char c)
{
	const struct uart_baochip_duart_config *cfg = dev->config;

	while (duart_read(cfg, DUART_REG_SR) != 0U) {
	}

	duart_write(cfg, DUART_REG_TXD, c);
}

static int uart_baochip_duart_poll_in(const struct device *dev, unsigned char *c)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(c);

	return -1;
}

static int uart_baochip_duart_configure(const struct device *dev,
					const struct uart_config *cfg)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(cfg);

	return -ENOSYS;
}

static int uart_baochip_duart_config_get(const struct device *dev, struct uart_config *cfg)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(cfg);

	return -ENOSYS;
}

static DEVICE_API(uart, uart_baochip_duart_driver_api) = {
	.poll_in = uart_baochip_duart_poll_in,
	.poll_out = uart_baochip_duart_poll_out,
	.configure = uart_baochip_duart_configure,
	.config_get = uart_baochip_duart_config_get,
};

static int uart_baochip_duart_init(const struct device *dev)
{
	const struct uart_baochip_duart_config *cfg = dev->config;

	duart_write(cfg, DUART_REG_CR, 1U);

	return 0;
}

#define UART_BAOCHIP_DUART_INIT(n)						\
	static const struct uart_baochip_duart_config uart_baochip_duart_cfg_##n = { \
		.base = DT_INST_REG_ADDR(n),					\
	};									\
										\
	DEVICE_DT_INST_DEFINE(n, uart_baochip_duart_init, NULL, NULL,		\
			      &uart_baochip_duart_cfg_##n, PRE_KERNEL_1,	\
			      CONFIG_SERIAL_INIT_PRIORITY,			\
			      &uart_baochip_duart_driver_api);

DT_INST_FOREACH_STATUS_OKAY(UART_BAOCHIP_DUART_INIT)
