/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/util.h>

BUILD_ASSERT(CONFIG_I2C_TIMEOUT_DEFAULT_MS > 0, "CONFIG_I2C_TIMEOUT_DEFAULT_MS must be greater than 0");

static const uint8_t buf[] = {0x00, 0x01, 0x02, 0x03};
static const struct device *const dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

ZTEST(i2c_timeout, test_i2c_set_timeout)
{
    uint32_t end;
    uint32_t begin;
    uint32_t actual;
    uint32_t expect;
    static const uint32_t timeout_ms[] = {
	    0, // K_NO_WAIT
	    1,
	    CONFIG_I2C_TIMEOUT_DEFAULT_MS / 2,
	    CONFIG_I2C_TIMEOUT_DEFAULT_MS * 2,
    };

    ARRAY_FOR_EACH(timeout_ms, i) {
	    int ret;
	    uint32_t to_ms = timeout_ms[i];

	    ret = i2c_set_timeout(dev, K_MSEC(to_ms));
	    if (ret == -ENOSYS) {
		    ztest_test_skip();
	    }
	    zexpect_ok(ret, "Failed to set timeout: %d", ret);

	    begin = k_uptime_get_32();
	    zexpect_equal(i2c_write(dev, buf, sizeof(buf), 1), -ETIMEDOUT);
	    end = k_uptime_get_32();

	    actual = end - begin;
	    expect = to_ms + CONFIG_TEST_I2C_TIMEOUT_SYSCALL_OVERHEAD_MS;
	    TC_PRINT("to_ms: %u, expect <= %u, actual: %u\n", to_ms, expect, actual);

	    zexpect_true(actual <= expect, "too slow (%u instead of %u). to_ms: %u", actual, expect,
			 to_ms);

	    expect = k_ticks_to_ms_floor32(k_ms_to_ticks_floor32(to_ms));
	    TC_PRINT("to_ms: %u, expect >= %u, actual: %u\n", to_ms, expect, actual);

	    zexpect_true(actual >= expect, "too fast (%u instead of %u). to_ms: %u", actual, expect,
			 to_ms);
    }
}

static void before(void *arg)
{
    if (!IS_ENABLED(CONFIG_I2C_TIMEOUT)) {
        ztest_test_skip();
    }

    zassert_ok(i2c_set_timeout(dev, K_MSEC(CONFIG_I2C_TIMEOUT_DEFAULT_MS)));
}

ZTEST_SUITE(i2c_timeout, NULL, NULL, before, NULL, NULL);
