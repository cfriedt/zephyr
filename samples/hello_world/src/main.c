/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);

	/*
	 * Register a callback for when the user button is pressed
	 * This can really be any kind of asynchronous event handler.
	 */
	int ret;
	static struct gpio_callback button_cb_data;
	static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		errno = -ret;
		perror("gpio_pin_configure_dt");
		return EXIT_FAILURE;
	}

	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		errno = -ret;
		perror("gpio_pin_interrupt_configure_dt");
		return EXIT_FAILURE;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	ret = gpio_add_callback(button.port, &button_cb_data);
	if (ret < 0) {
		errno = -ret;
		perror("gpio_add_callback");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
