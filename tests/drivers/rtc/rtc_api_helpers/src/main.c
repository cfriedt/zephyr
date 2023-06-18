/*
 * Copyright 2022 Bjarki Arge Andreasen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

BUILD_ASSERT(DT_HAS_CHOSEN(zephyr_rtc), "zephyr,rtc chosen node is missing in Devicetree");

ZTEST_SUITE(rtc_api_helpers, NULL, NULL, NULL, NULL, NULL);
