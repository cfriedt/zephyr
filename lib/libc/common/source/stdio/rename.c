/*
 * Copyright (c) 2018 Intel Corporation
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/fs/fs.h>

int rename(const char *old, const char *new)
{
	int rc;

	if (!IS_ENABLED(CONFIG_FILE_SYSTEM)) {
		errno = ENOTSUP;
		return -1;
	}

	rc = fs_rename(old, new);
	if (rc < 0) {
		errno = -rc;
		return -1;
	}

	return 0;
}
