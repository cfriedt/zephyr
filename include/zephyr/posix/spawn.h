/*
 * Copyright 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYR_POSIX_SPAWN_H_
#define ZEPHYR_INCLUDE_ZEPHYR_POSIX_SPAWN_H_

#if _POSIX_SPAWN >= 200112L

#include <zephyr/toolchain.h>
#include <zephyr/posix/sys/spawn.h>
#include <zephyr/sys/util.h>

#define POSIX_SPAWN_RESETIDS  BIT(0)
#define POSIX_SPAWN_SETPGROUP BIT(1)
#if _POSIX_PRIORITY_SCHEDULING >= 200112L
#define POSIX_SPAWN_SETSCHEDPARAM BIT(2)
#define POSIX_SPAWN_SETSCHEDULER  BIT(3)
#endif
#define POSIX_SPAWN_SETSIGDEF  BIT(4)
#define POSIX_SPAWN_SETSIGMASK BIT(5)

struct sched_param;

int posix_spawn(pid_t *ZRESTRICT pid, const char *ZRESTRICT path,
		const posix_spawn_file_actions_t *file_actions,
		const posix_spawnattr_t *ZRESTRICT attrp, char *const argv[ZRESTRICT],
		char *const envp[ZRESTRICT]);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions, int fildes);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions, int fildes,
				     int newfildes);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *ZRESTRICT file_actions, int fildes,
				     const char *ZRESTRICT path, int oflag, mode_t mode);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions);
int posix_spawnattr_destroy(posix_spawnattr_t *attr);
int posix_spawnattr_getflags(const posix_spawnattr_t *ZRESTRICT attr, short *ZRESTRICT flags);
int posix_spawnattr_getpgroup(const posix_spawnattr_t *ZRESTRICT attr, pid_t *ZRESTRICT pgroup);

#if _POSIX_PRIORITY_SCHEDULING >= 200112L
int posix_spawnattr_getschedparam(const posix_spawnattr_t *ZRESTRICT attr,
				  struct sched_param *ZRESTRICT schedparam);
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *ZRESTRICT attr,
				   int *ZRESTRICT schedpolicy);
#endif

int posix_spawnattr_getsigdefault(const posix_spawnattr_t *ZRESTRICT attr,
				  sigset_t *ZRESTRICT sigdefault);
int posix_spawnattr_getsigmask(const posix_spawnattr_t *ZRESTRICT attr,
			       sigset_t *ZRESTRICT sigmask);
int posix_spawnattr_init(posix_spawnattr_t *attr);
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags);
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup);

#if _POSIX_PRIORITY_SCHEDULING >= 200112L
int posix_spawnattr_setschedparam(posix_spawnattr_t *ZRESTRICT,
				  const struct sched_param *ZRESTRICT);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy);
#endif

int posix_spawnattr_setsigdefault(posix_spawnattr_t *ZRESTRICT attr,
				  const sigset_t *ZRESTRICT sigdefault);
int posix_spawnattr_setsigmask(posix_spawnattr_t *ZRESTRICT attr,
			       const sigset_t *ZRESTRICT sigmask);
int posix_spawnp(pid_t *ZRESTRICT pid, const char *ZRESTRICT file,
		 const posix_spawn_file_actions_t *file_actions,
		 const posix_spawnattr_t *ZRESTRICT attrp, char *const argv[ZRESTRICT],
		 char *const envp[ZRESTRICT]);

#endif

#endif /* ZEPHYR_INCLUDE_ZEPHYR_POSIX_SPAWN_H_ */
