/*
 * Copyright (c) 2025, Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/toolchain.h>
#include <zephyr/posix/posix_features.h>
#include <zephyr/posix/spawn.h>

int posix_spawn(pid_t *ZRESTRICT pid, const char *ZRESTRICT path,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *ZRESTRICT attrp, char *const argv[ZRESTRICT],
    char *const envp[ZRESTRICT])
{
    return ENOSYS;
}

int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions, int fildes)
{
    return ENOSYS;
}

int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions, int fildes,
                 int newfildes)
{
    return ENOSYS;
}

int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *ZRESTRICT file_actions, int fildes,
                 const char *ZRESTRICT path, int oflag, mode_t mode)
{
    return ENOSYS;
}

int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions)
{
    return ENOSYS;
}

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions)
{
    return ENOSYS;
}

int posix_spawnattr_destroy(posix_spawnattr_t *attr)
{
    return ENOSYS;
}

int posix_spawnattr_getflags(const posix_spawnattr_t *ZRESTRICT attr, short *ZRESTRICT flags)
{
    return ENOSYS;
}

int posix_spawnattr_getpgroup(const posix_spawnattr_t *ZRESTRICT attr, pid_t *ZRESTRICT pgroup)
{
    return ENOSYS;
}

#ifdef CONFIG_POSIX_PRIORITY_SCHEDULING
int posix_spawnattr_getschedparam(const posix_spawnattr_t *ZRESTRICT attr,
              struct sched_param *ZRESTRICT schedparam)
{
    return ENOSYS;
}

int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *ZRESTRICT attr,
               int *ZRESTRICT schedpolicy)
{
    return ENOSYS;
}
#endif

int posix_spawnattr_getsigdefault(const posix_spawnattr_t *ZRESTRICT attr,
              sigset_t *ZRESTRICT sigdefault)
{
    return ENOSYS;
}

int posix_spawnattr_getsigmask(const posix_spawnattr_t *ZRESTRICT attr,
               sigset_t *ZRESTRICT sigmask)
{
    return ENOSYS;
}

int posix_spawnattr_init(posix_spawnattr_t *attr)
{
    return ENOSYS;
}

int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags)
{
    return ENOSYS;
}

int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup)
{
    return ENOSYS;
}

#ifdef CONFIG_POSIX_PRIORITY_SCHEDULING
int posix_spawnattr_setschedparam(posix_spawnattr_t *ZRESTRICT,
              const struct sched_param *ZRESTRICT)
{
    return ENOSYS;
}

int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy)
{
    return ENOSYS;
}
#endif

int posix_spawnattr_setsigdefault(posix_spawnattr_t *ZRESTRICT attr,
              const sigset_t *ZRESTRICT sigdefault)
{
    return ENOSYS;
}

int posix_spawnattr_setsigmask(posix_spawnattr_t *ZRESTRICT attr,
               const sigset_t *ZRESTRICT sigmask)
{
    return ENOSYS;
}

int posix_spawnp(pid_t *ZRESTRICT pid, const char *ZRESTRICT file,
     const posix_spawn_file_actions_t *file_actions,
     const posix_spawnattr_t *ZRESTRICT attrp, char *const argv[ZRESTRICT],
     char *const envp[ZRESTRICT])
{
    return ENOSYS;
}
