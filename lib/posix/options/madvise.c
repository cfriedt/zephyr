/*
 * Copyright (c) 2025, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <sys/mman.h>

int posix_madvise(void *addr, size_t len, int advice)
{
    ARG_UNUSED(addr);
    ARG_UNUSED(len);
    ARG_UNUSED(advice);

    return ENOSYS;
}
