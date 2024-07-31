# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(flash_jesd216)

set(app_sources "")

list(APPEND app_sources
  src/main.c
  src/sfp.c
)

if(CONFIG_SFP_SHELL)
list(APPEND app_sources
  src/sfp_shell.c
)
endif()

target_sources(app PRIVATE ${app_sources})
target_include_directories(app PRIVATE include)
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/..)

zephyr_syscall_header(
  ${ZEPHYR_BASE}/samples/spi_flash_programmer/include/zephyr/drivers/sfp.h
)
