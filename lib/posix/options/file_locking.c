/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#undef getc_unlocked
#undef getchar_unlocked
#undef putc_unlocked
#undef putchar_unlocked

extern void zvfs_flockfile(FILE *file);
extern int zvfs_ftrylockfile(FILE *file);
extern void zvfs_funlockfile(FILE *file);
extern int zvfs_getc_unlocked(FILE *stream);
extern int zvfs_putc_unlocked(int c, FILE *stream);

void flockfile(FILE *file)
{
    zvfs_flockfile(file);
}

int ftrylockfile(FILE *file)
{
    return zvfs_ftrylockfile(file);
}

void funlockfile(FILE *file)
{
    zvfs_funlockfile(file);
}

int getc_unlocked(FILE *stream)
{
    return zvfs_getc_unlocked(stream);
}

int getchar_unlocked(void)
{
    return zvfs_getc_unlocked(stdin);
}

int putc_unlocked(int c, FILE *stream)
{
    return zvfs_putc_unlocked(c, stream);
}

int putchar_unlocked(int c)
{
    return zvfs_putc_unlocked(c, stdout);
}
