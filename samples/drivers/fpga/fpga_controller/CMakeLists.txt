# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(fpga_controller)

target_sources_ifdef(CONFIG_BOARD_QUICKFEATHER app PRIVATE src/main.c)

# This will eventually be a shell-driven sample. Blocked on #52605 and #52764
target_sources_ifdef(CONFIG_BOARD_ICEV_WIRELESS app PRIVATE src/icev_main.c)
