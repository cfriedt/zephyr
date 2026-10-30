# Copyright (c) 2026 Chris Friedt
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_XIP)
  # The firmware shares the SPI flash with the FPGA bitstream, which occupies
  # the first 6 MiB; the AE350 maps flash offset 0x600000 at 0x80000000.
  board_runner_args(openfpgaloader
    --board tangmega138k
    --external-flash
    --offset=0x600000
    --verify
  )

  include(${ZEPHYR_BASE}/boards/common/openfpgaloader.board.cmake)
  board_runner_args(openocd --no-load)
else()
  # Non-XIP images are loaded directly into RAM by OpenOCD.
  set(OPENOCD_USE_LOAD_IMAGE YES)
  board_runner_args(openocd --file-type=elf)
endif()

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
