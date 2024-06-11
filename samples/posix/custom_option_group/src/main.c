/*
 * Copyright (c) 2024, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int ret;
	int fd[2];
	uint8_t buf[1];

	ret = pipe(fd);
	if (ret < 0) {
		perror("pipe");
		goto out;
	}

	ret = write(fd[0], "\x42", 1);
	if (ret != 1) {
		perror("write");
		goto out;
	}
	printf("write: %x\n", 0x42);

	ret = read(fd[1], buf, 1);
	if (ret != 1) {
		perror("read");
		goto out;
	}
	printf("read: %x\n", buf[0]);

out:
	close(fd[0]);
	close(fd[1]);

	return (ret == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
