/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "_main.h"

#include <errno.h>
#include <fcntl.h>
#include <spawn.h>

#include <zephyr/ztest.h>

ZTEST(xsi_advanced_realtime, test_posix_spawn)
{
	/* int posix_spawn(pid_t *restrict pid, const char *restrict path,
       const posix_spawn_file_actions_t *file_actions,
       const posix_spawnattr_t *restrict attrp,
       char *const argv[restrict], char *const envp[restrict]); */

	pid_t pid = 0;
	const char *path = EXE_FILE;
	const posix_spawn_file_actions_t *file_actions = NULL;
	posix_spawnattr_t *attr = NULL;

	zexpect_ok(posix_spawn(NULL, NULL, NULL, NULL, NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawn_file_actions_addclose)
{
	zexpect_ok(posix_spawn_file_actions_addclose(NULL, 0));
}

ZTEST(xsi_advanced_realtime, test_posix_spawn_file_actions_adddup2)
{
	zexpect_ok(posix_spawn_file_actions_adddup2(NULL, 0, 1));
}

ZTEST(xsi_advanced_realtime, test_posix_spawn_file_actions_addopen)
{
	zexpect_ok(posix_spawn_file_actions_addopen(NULL, 0, "file", O_RDONLY, 0));
}

ZTEST(xsi_advanced_realtime, test_posix_spawn_file_actions_destroy)
{
	zexpect_ok(posix_spawn_file_actions_destroy(NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawn_file_actions_init)
{
	zexpect_ok(posix_spawn_file_actions_init(NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_destroy)
{
	zexpect_ok(posix_spawnattr_destroy(NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_getflags)
{
	zexpect_ok(posix_spawnattr_getflags(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_getpgroup)
{
	zexpect_ok(posix_spawnattr_getpgroup(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_getschedparam)
{
	zexpect_ok(posix_spawnattr_getschedparam(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_getschedpolicy)
{
	zexpect_ok(posix_spawnattr_getschedpolicy(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_getsigdefault)
{
	zexpect_ok(posix_spawnattr_getsigdefault(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_getsigmask)
{
	zexpect_ok(posix_spawnattr_getsigmask(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_init)
{
	zexpect_ok(posix_spawnattr_init(NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_setflags)
{
	zexpect_ok(posix_spawnattr_setflags(NULL, 0));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_setpgroup)
{
	zexpect_ok(posix_spawnattr_setpgroup(NULL, 0));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_setschedparam)
{
	zexpect_ok(posix_spawnattr_setschedparam(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_setschedpolicy)
{
	zexpect_ok(posix_spawnattr_setschedpolicy(NULL, 0));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_setsigdefault)
{
	zexpect_ok(posix_spawnattr_setsigdefault(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnattr_setsigmask)
{
	zexpect_ok(posix_spawnattr_setsigmask(NULL, NULL));
}

ZTEST(xsi_advanced_realtime, test_posix_spawnp)
{
	zexpect_ok(posix_spawnp(NULL, NULL, NULL, NULL, NULL, NULL));
}
