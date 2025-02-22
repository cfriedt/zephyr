/*
 * Copyright (c) 2025, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

/* this allows us to avoid a warning promoted to error for comparing memptr to NULL */
static inline int __posix_memalign_internal(void **memptr, size_t alignment, size_t size)
{
    if ((memptr == NULL) || (alignment == 0) || !IS_POWER_OF_TWO(alignment)) {
        return EINVAL;
    }

    if (size == 0) {
        *memptr = NULL;
        return 0;
    }

    *memptr = aligned_alloc(alignment, size);
    if (*memptr == NULL) {
        return ENOMEM;
    }

    return 0;
}

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    return __posix_memalign_internal(memptr, alignment, size);
}