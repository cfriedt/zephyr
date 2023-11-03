/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hello);

int main(void)
{
	for (;;) {
		LOG_INF("Hello World! %s", CONFIG_BOARD);
		k_msleep(1000);
	}

	return 0;
}
