/*
 * Copyright (c) 2025, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/posix/posix_features.h>

#include <errno.h>
#include <sys/mman.h>

#include <zephyr/toolchain.h>

int posix_mem_offset(const void *ZRESTRICT addr, size_t len,
    off_t *ZRESTRICT off, size_t *ZRESTRICT contig_len,
    int *ZRESTRICT fildes)
{
    ARG_UNUSED(addr);
    ARG_UNUSED(len);
    ARG_UNUSED(off);
    ARG_UNUSED(contig_len);
    ARG_UNUSED(fildes);

    return ENOSYS;
}

int posix_typed_mem_get_info(int fildes, struct posix_typed_mem_info *info)
{
    ARG_UNUSED(fildes);
    ARG_UNUSED(info);

    return ENOSYS;
}
