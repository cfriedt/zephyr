# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zvfs_fd_permission)

# shortcut: the PR head lacks these poll aliases; remove when fdtable.h defines them.
zephyr_compile_definitions(
  ZVFS_POLLRDNORM=64
  ZVFS_POLLWRNORM=4
  ZVFS_POLLRDBAND=128
  ZVFS_POLLWRBAND=256
)

target_sources(app PRIVATE src/main.c)
