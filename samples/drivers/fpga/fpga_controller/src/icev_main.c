#include <stdbool.h>

#include <zephyr/device.h>
#include <zephyr/drivers/fpga.h>
#include <zephyr/kernel.h>

/*
 * The user is responsible for synthesizing bitstream.bin from
 * source and converting it to the C header below using xxd.
 *
 * See
 * https://github.com/ICE-V-Wireless/ICE-V-Wireless/tree/main/Gateware
 * boards/riscv/icev_wireless/doc/index.rst
 *
 * This is temporary workaround for #52605 and #52764.
 */
#include "bitstream.h"

void main(void)
{
	int result;
	const struct device *dev;

	dev = DEVICE_DT_GET(DT_NODELABEL(fpga0));
	__ASSERT(dev != NULL, "device is NULL");

	__ASSERT(device_is_ready(fixture->fpga), "device is not ready");

	result = fpga_reset(dev);
	__ASSERT(result == 0, "failed to reset iCE40 (%d)", result);

	result = fpga_off(dev);
	__ASSERT(result == 0, "failed to turn iCE40 off (%d)", result);

	result = fpga_on(dev);
	__ASSERT(result == 0, "failed to turn iCE40 on (%d)", result);

	result = fpga_load(dev, (uint32_t *)bitstream, sizeof(bitstream));
	__ASSERT(result == 0, "failed to load iCE40 bitstream (%d)", result);
}