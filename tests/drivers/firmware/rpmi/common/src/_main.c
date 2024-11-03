/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

#include <zephyr/rpmi/rpmi.h>

ZTEST(rpmi_common, test_nada)
{
    zassert_equal(true, true);
}

ZTEST_SUITE(rpmi_common, NULL, NULL, NULL, NULL, NULL);
