/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <zephyr/sys/fdtable.h>

static uint8_t my_object;

static ssize_t pipe_rw(void *obj, void *buf, size_t size, bool is_write)
{
	ARG_UNUSED(size);

	uint8_t *x = obj;

	if (is_write) {
		*x = *(uint8_t *)buf;
		return 1;
	}

	printf("this pipe is custom\n");
	*(uint8_t *)buf = *x;
	return 1;
}

static ssize_t pipe_read(void *obj, void *buf, size_t size)
{
	return pipe_rw(obj, buf, size, false);
}

static ssize_t pipe_write(void *obj, const void *buf, size_t size)
{
	return pipe_rw(obj, (void *)buf, size, true);
}

static const struct fd_op_vtable pipe_vtable = {
	.read = pipe_read,
	.write = pipe_write,
};

int pipe(int fd[2])
{
	fd[0] = z_alloc_fd(&my_object, &pipe_vtable);
	fd[1] = z_alloc_fd(&my_object, &pipe_vtable);

	return 0;
}
