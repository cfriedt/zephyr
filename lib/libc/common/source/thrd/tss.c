/*
 * Copyright (c) 2023, Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <threads.h>

#include <zephyr/kernel.h>

static inline int sys_thread_key_create(intptr_t key, void (*destructor)(void *arg))
{
	return -ENOSYS;
}

static inline void *sys_thread_getspecific(intptr_t key)
{
	return NULL;
}

static inline int sys_thread_setspecific(intptr_t key, void *val)
{
	return -ENOSYS;
}

static inline int sys_thread_key_delete(intptr_t key)
{
	return -ENOSYS;
}

int tss_create(tss_t *key, tss_dtor_t destructor)
{
	switch (sys_thread_key_create(*key, destructor)) {
	case 0:
		return thrd_success;
	case -EAGAIN:
		return thrd_busy;
	case -ENOMEM:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

void *tss_get(tss_t key)
{
	return sys_thread_getspecific(key);
}

int tss_set(tss_t key, void *val)
{
	switch (sys_thread_setspecific(key, val)) {
	case 0:
		return thrd_success;
	case -ENOMEM:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

void tss_delete(tss_t key)
{
	(void)sys_thread_key_delete(key);
}
