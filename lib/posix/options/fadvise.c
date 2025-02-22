/*
 * Copyright (c) 2025, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <fcntl.h>

int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	ARG_UNUSED(fd);
	ARG_UNUSED(offset);
	ARG_UNUSED(len);
	ARG_UNUSED(advice);

	return ENOSYS;
}
