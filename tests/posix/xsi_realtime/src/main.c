/*
 * Copyright (c) 2025 Marvin Ouma <pancakesdeath@protonmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

void test_mount(void *arg);
void test_unmount(void *arg);

ZTEST_SUITE(xsi_realtime, NULL, NULL, test_mount, test_unmount, NULL);
