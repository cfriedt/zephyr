// #include <arc_defines.h>
#include "tt_boot_fs.h"

uint32_t get_next_fd_addr(uint32_t last_fd_addr)
{
	return (last_fd_addr + sizeof(tt_boot_fs_fd));
}

// Setups hardware abstraction layer (HAL) callbacks, initializes HEAD fd
int tt_boot_fs_mount(tt_boot_fs *tt_boot_fs, tt_boot_fs_read hal_read, tt_boot_fs_write hal_write,
		     tt_boot_fs_erase hal_erase)
{
	tt_boot_fs->hal_spi_read_f = hal_read;
	tt_boot_fs->hal_spi_write_f = hal_write;
	tt_boot_fs->hal_spi_erase_f = hal_erase;

	return TT_BOOT_FS_OK;
}

#ifndef TT_BOOT_FS_RDONLY

// Allocates new file descriptor on SPI device
// Writes associated data to correct address on SPI device
int tt_boot_fs_add_file(tt_boot_fs *tt_boot_fs, tt_boot_fs_fd fd, uint8_t *image_data_src,
			bool isFailoverEntry, bool isSecurityBinaryEntry)
{
	uint32_t curr_fd_addr;

	// Failover image has specific file descriptor location (BOOT_START + DESC_REGION_SIZE)
	if (isFailoverEntry) {
		curr_fd_addr = TT_BOOT_FS_FAILOVER_HEAD_ADDR;
	} else if (isSecurityBinaryEntry) {
		curr_fd_addr = TT_BOOT_FS_SECURITY_BINARY_FD_ADDR;
	} else {
		// Regular file descriptor
		tt_boot_fs_fd head = {0};
		curr_fd_addr = TT_BOOT_FS_FD_HEAD_ADDR;

		tt_boot_fs->hal_spi_read_f(TT_BOOT_FS_FD_HEAD_ADDR, sizeof(tt_boot_fs_fd),
					   (uint8_t *)&head);

		// Traverse until we find an invalid file descriptor entry in SPI device array
		while (head.flags.f.invalid == 0) {
			curr_fd_addr = get_next_fd_addr(curr_fd_addr);
			tt_boot_fs->hal_spi_read_f(curr_fd_addr, sizeof(tt_boot_fs_fd),
						   (uint8_t *)&head);
		}
	}

	tt_boot_fs->hal_spi_write_f(curr_fd_addr, sizeof(tt_boot_fs_fd), (uint8_t *)&fd);

	// Now copy total image size from image_data_src pointer into the SPI device address
	// specified Total image size = image_size + signature_size (security) + padding (future
	// work)
	uint32_t total_image_size = fd.flags.f.image_size + fd.security_flags.f.signature_size;
	tt_boot_fs->hal_spi_write_f(fd.spi_addr, total_image_size, image_data_src);

	return TT_BOOT_FS_OK;
}

#endif

#if 0
tt_checksum_res_t calculate_and_compare_checksum(uint8_t *data, uint32_t num_bytes, uint32_t expected, bool skip_checksum) {
  if (!skip_checksum) {
    WRITE_RESET_UNIT(SCRATCH_3, 0xc0000000);

    // Always read 1 fewer word, and handle the 4 possible alignment cases outside the loop
    const uint32_t num_dwords = (num_bytes / sizeof(uint32_t)) - 1;

    uint32_t calculated_checksum = 0;
    uint32_t *data_as_dwords = (uint32_t *)data;
    for (uint32_t i = 0; i < num_dwords; i++) {
      calculated_checksum += *data_as_dwords++;
    }

    switch (num_bytes % 4) {
      case 3: calculated_checksum += *data_as_dwords & 0x000000ff; break;
      case 2: calculated_checksum += *data_as_dwords & 0x0000ffff; break;
      case 1: calculated_checksum += *data_as_dwords & 0x00ffffff; break;
      case 0: calculated_checksum += *data_as_dwords & 0xffffffff; break;
    }

    RESET_UNIT_BOOTROM_MISC_STATUS_reg_u bootrom_status;
    bootrom_status.val = READ_RESET_UNIT(BOOTROM_MISC_STATUS);
    if (calculated_checksum != expected) {
      WRITE_RESET_UNIT(SCRATCH_5, calculated_checksum);
      WRITE_RESET_UNIT(SCRATCH_6, num_bytes);
      WRITE_RESET_UNIT(SCRATCH_7, expected);

      bootrom_status.f.tt_boot_fs_checksum_fail = 1;
      WRITE_RESET_UNIT(BOOTROM_MISC_STATUS, bootrom_status.val);

      return TT_BOOT_FS_CHK_FAIL;
    }

    bootrom_status.f.tt_boot_fs_checksum_fail = 0;
    WRITE_RESET_UNIT(BOOTROM_MISC_STATUS, bootrom_status.val);
  }

  return TT_BOOT_FS_CHK_OK;
}
#endif
