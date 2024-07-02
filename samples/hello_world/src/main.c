/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tt_boot_fs.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>

static tt_boot_fs boot_fs;
static const struct device *flash_dev = DEVICE_DT_GET(DT_NODELABEL(flash1));

static int z_tt_boot_fs_read(uint32_t addr, uint32_t size, uint8_t *dst)
{
	return flash_read(flash_dev, addr, dst, size);
}

static int z_tt_boot_fs_write(uint32_t addr, uint32_t size, uint8_t *src)
{
	return flash_write(flash_dev, addr, src, size);
}

static int z_tt_boot_fs_erase(uint32_t addr, uint32_t size)
{
	return flash_erase(flash_dev, addr, size);
}

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD_TARGET);

	if (!device_is_ready(flash_dev)) {
		printk("Flash device %s is not ready\n", flash_dev->name);
		return EXIT_FAILURE;
	}

	printk("reading flash %s\n", flash_dev->name);

	if (tt_boot_fs_mount(&boot_fs, z_tt_boot_fs_read, z_tt_boot_fs_write, z_tt_boot_fs_erase) ==
	    TT_BOOT_FS_ERR) {
		printk("tt_boot_fs_mount() failed\n");
	}

	int num_images = 0;
	for (uint32_t addr = TT_BOOT_FS_FD_HEAD_ADDR, blah = 1; blah == 1;
	     addr = get_next_fd_addr(addr), ++num_images) {
		tt_boot_fs_fd fd;

		int rc = flash_read(flash_dev, addr, (uint8_t *)&fd, sizeof(tt_boot_fs_fd));
		if (rc < 0) {
			printk("flash_read() failed: %d\n", rc);
			break;
		}

		if ((fd.fd_crc == 0xffffffff) || (fd.data_crc == 0xffffffff)) {
			break;
		}

		printk("flash image %d:\n\t"
		       "spi_addr: %x\n\t"
		       "copy_dest: %x\n\t"
		       "flags: %x\n\t"
		       "data_crc: %x\n\t"
		       "security_flags: %x\n\t"
		       "image_tag: %s\n\t"
		       "fd_crc: %x\n",
		       num_images, fd.spi_addr, fd.copy_dest, fd.flags.val, fd.data_crc,
		       fd.security_flags.val, fd.image_tag, fd.fd_crc);
	}

	printk("Found %d images in flash\n", num_images);

	return EXIT_SUCCESS;
}
