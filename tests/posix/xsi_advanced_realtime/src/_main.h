/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_TESTS_POSIX_XSI_ADVANCED_REALTIME_SRC__MAIN_H_
#define ZEPHYR_TESTS_POSIX_XSI_ADVANCED_REALTIME_SRC__MAIN_H_

#include <zephyr/posix/posix_features.h>
#include <zephyr/posix/posix_types.h>

#define FATFS_MNTP "/RAM:"
#define TEST_FILE  FATFS_MNTP "/testfile.txt"

extern int fd;

#endif /* ZEPHYR_TESTS_POSIX_XSI_ADVANCED_REALTIME_SRC__MAIN_H_ */
