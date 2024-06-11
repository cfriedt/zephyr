# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(custom_option_group)

set(app_sources src/main.c)

if(CONFIG_TC_PROVIDES_POSIX_PIPE)
  list(APPEND app_sources src/custom_pipe.c)
endif()

target_sources(app PRIVATE ${app_sources})

target_compile_options(app PRIVATE -D_POSIX_C_SOURCE=200809L)
