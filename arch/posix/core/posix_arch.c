/*
 * Copyright (c) 2022 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <dlfcn.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <gnu/lib-names.h>

#include <arch/posix/posix_arch_if.h>
#include <logging/log.h>

#include "posix_core.h"
#include "posix_arch_internal.h"
#include "soc.h"

LOG_MODULE_DECLARE(os, CONFIG_KERNEL_LOG_LEVEL);

struct posix_arch_interface {
    void *libc_handle;
    FILE *stdout;
    FILE *stderr;

	void *(*calloc)(size_t nmemb, size_t size);
    int (*clock_gettime)(clockid_t clk_id, struct timespec *tp);
	void (*exit)(int val);
    int (*fflush)(FILE *stream);
	void (*free)(void *ptr);
	int (*isatty)(int fd);
	void *(*malloc)(size_t size);
	int (*nanosleep)(const struct timespec *req, struct timespec *rem);
	int (*pthread_cancel)(pthread_t thread);
	int (*pthread_cond_broadcast)(pthread_cond_t *cond);
	int (*pthread_cond_wait)(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
	int (*pthread_create)(pthread_t *restrict thread, const pthread_attr_t *restrict attr,
			      void *(*start_routine)(void *), void *restrict arg);
	int (*pthread_detach)(pthread_t thread);
	int (*pthread_exit)(void *value);
	int (*pthread_mutex_lock)(pthread_mutex_t *mutex);
	int (*pthread_mutex_unlock)(pthread_mutex_t *mutex);
	pthread_t (*pthread_self)(void);
	int (*putchar)(int ch);
	long (*random)(void);
	void *(*realloc)(void *ptr, size_t size);
	void *(*reallocarray)(void *ptr, size_t nmemb, size_t size);
	int (*setvbuf)(FILE *restrict stream, char *restrict buf, int type, size_t size);
	int (*sigaction)(int signum, const struct sigaction *restrict act,
			 struct sigaction *restrict oldact);
	int (*sigemptyset)(sigset_t *set);
	unsigned int (*sleep)(unsigned int seconds);
	void (*srandom)(long seed);
	/* Technically, this could be done in the minimal libc at some point */
	double (*strtod)(const char *nptr, char **endptr);
	/* use vdprintf instead of vfprintf to avoid references to stdout, stderr, etc */
	int (*vdprintf)(int fd, const char *fmt, va_list ap);
};

static const struct posix_arch_interface *_if(void);

/* Conditional variable to know if the CPU is running or halted/idling */
static pthread_cond_t cond_cpu = PTHREAD_COND_INITIALIZER;
/* Mutex for the conditional variable posix_soc_cond_cpu */
static pthread_mutex_t mtx_cpu = PTHREAD_MUTEX_INITIALIZER;

/*
 * Conditional variable to block/awake all threads during swaps()
 * (we only need 1 mutex and 1 cond variable for all threads)
 */
static pthread_cond_t cond_threads = PTHREAD_COND_INITIALIZER;
/* Mutex for the conditional variable posix_core_cond_threads */
static pthread_mutex_t mtx_threads = PTHREAD_MUTEX_INITIALIZER;

void posix_arch_broadcast_cpu(void)
{
	PC_SAFE_CALL(_if()->pthread_cond_broadcast(&cond_cpu));
}

void posix_arch_broadcast_threads(void)
{
	PC_SAFE_CALL(_if()->pthread_cond_broadcast(&cond_threads));
}

void *posix_arch_calloc(size_t nmemb, size_t size)
{
	return _if()->calloc(nmemb, size);
}

int posix_arch_clock_gettime_mono(struct timespec *tv)
{
    return
#if defined(CLOCK_MONOTONIC_RAW)
	_if()->clock_gettime(CLOCK_MONOTONIC_RAW, tv);
#else
	_if()->clock_gettime(CLOCK_MONOTONIC, tv);
#endif
}

int posix_arch_clock_gettime_real(struct timespec *tv)
{
	return _if()->clock_gettime(CLOCK_REALTIME, tv);
}

void posix_arch_exit(int val) {
	_if()->exit(val);
}

void posix_arch_free(void *ptr)
{
	_if()->free(ptr);
}

int posix_arch_isatty_stderr(void)
{
	return _if()->isatty(STDERR_FILENO);
}

int posix_arch_isatty_stdout(void)
{
	return _if()->isatty(STDOUT_FILENO);
}

void posix_arch_lock_cpu(void)
{
	PC_SAFE_CALL(_if()->pthread_mutex_lock(&mtx_cpu));
}

void posix_arch_lock_threads(void)
{
	PC_SAFE_CALL(_if()->pthread_mutex_lock(&mtx_threads));
}

void *posix_arch_malloc(size_t size)
{
	return _if()->malloc(size);
}

int posix_arch_nanosleep(struct timespec *req, struct timespec *rem)
{
	return _if()->nanosleep(req, rem);
}

void posix_arch_fflush_stdout(void)
{
    _if()->fflush(stdout);
}

int posix_arch_fprintf_stdout(const char *format, ...)
{
	int r;
	va_list variable_args;

	va_start(variable_args, format);
	r = _if()->vdprintf(STDOUT_FILENO, format, variable_args);
	va_end(variable_args);

	return r;
}

int posix_arch_pthread_cancel(uint64_t thread)
{
	return _if()->pthread_cancel((pthread_t)thread);
}

void posix_arch_pthread_create(uint64_t *thread, void *(*entry)(void *arg), void *arg)
{
	pthread_t th;

	_if()->pthread_create(&th, NULL, entry, arg);
	*thread = (uint64_t)th;
}

void posix_arch_pthread_detach_self(void)
{
	_if()->pthread_detach(_if()->pthread_self());
}

void posix_arch_pthread_exit(void *value)
{
	_if()->pthread_exit(value);
}

int posix_arch_putchar(int ch)
{
	return _if()->putchar(ch);
}

long posix_arch_random(void)
{
	return _if()->random();
}

void *posix_arch_realloc(void *ptr, size_t size)
{
	return _if()->realloc(ptr, size);
}

void *posix_arch_reallocarray(void *ptr, size_t nmemb, size_t size)
{
	return _if()->reallocarray(ptr, nmemb, size);
}

void posix_arch_setvbuf_stdout(void)
{
	_if()->setvbuf(_if()->stdout, NULL, _IOLBF, 512);
}

void posix_arch_set_sig_handler(void (*handler)(int sig))
{
	struct sigaction act;

	act.sa_handler = handler;
	PC_SAFE_CALL(_if()->sigemptyset(&act.sa_mask));

	act.sa_flags = SA_RESETHAND;

	PC_SAFE_CALL(_if()->sigaction(SIGTERM, &act, NULL));
	PC_SAFE_CALL(_if()->sigaction(SIGINT, &act, NULL));
}

unsigned int posix_arch_sleep(unsigned int seconds)
{
    return _if()->sleep(seconds);
}

void posix_arch_srandom(long seed)
{
	_if()->srandom(seed);
}

double posix_arch_strtod(const char *nptr, char **endptr)
{
	return _if()->strtod(nptr, endptr);
}

void posix_arch_unlock_cpu(void)
{
	PC_SAFE_CALL(_if()->pthread_mutex_unlock(&mtx_cpu));
}

void posix_arch_unlock_threads(void)
{
	PC_SAFE_CALL(_if()->pthread_mutex_unlock(&mtx_threads));
}

void posix_arch_wait_cpu(void)
{
	PC_SAFE_CALL(_if()->pthread_cond_wait(&cond_cpu, &mtx_cpu));
}

void posix_arch_wait_threads(void)
{
	_if()->pthread_cond_wait(&cond_threads, &mtx_threads);
}

static struct posix_arch_interface posix_arch_if;

const struct posix_arch_interface *_if(void)
{
	return &posix_arch_if;
}

static void posix_arch_if_init(void)
{
#define _decl(x)                                                                                   \
	{                                                                                          \
		.ptr = (void **)((uint8_t *)&posix_arch_if + offsetof(struct posix_arch_interface, x)), \
        .str = #x,                 \
	}

	static const struct pstr {
		void **ptr;
		const char *str;
	} pstr[] = {
		_decl(calloc),
		_decl(clock_gettime),
		_decl(exit),
		_decl(fflush),
		_decl(free),
		_decl(isatty),
		_decl(malloc),
		_decl(nanosleep),
		_decl(pthread_cancel),
		_decl(pthread_cond_broadcast),
		_decl(pthread_cond_wait),
		_decl(pthread_create),
		_decl(pthread_detach),
		_decl(pthread_exit),
		_decl(pthread_mutex_lock),
		_decl(pthread_mutex_unlock),
		_decl(pthread_self),
		_decl(putchar),
		_decl(random),
		_decl(realloc),
		_decl(reallocarray),
		_decl(setvbuf),
		_decl(sigaction),
		_decl(sigemptyset),
		_decl(sleep),
		_decl(srandom),
		_decl(stdout),
		_decl(stderr),
		_decl(strtod),
		_decl(vdprintf),
	};

	posix_arch_if.libc_handle = dlopen(LIBC_SO, RTLD_NOW | RTLD_LOCAL);
	__ASSERT(posix_arch_if.libc_handle != NULL, "dlopen libc failed: %s", dlerror());

	for (size_t i = 0; i < ARRAY_SIZE(pstr); ++i) {
		*pstr[i].ptr = dlsym(posix_arch_if.libc_handle, pstr[i].str);
		__ASSERT(*pstr[i].ptr != NULL, "dlsym %s failed: %s", dlerror());
    }

#undef _decl
}

static void posix_arch_if_fini(void)
{
	dlclose(posix_arch_if.libc_handle);
}

NATIVE_TASK(posix_arch_if_init, PRE_BOOT_1, 0);
NATIVE_TASK(posix_arch_if_fini, ON_EXIT, 999);

void posix_print_error_and_exit(const char *format, ...)
{
	va_list variable_args;

	va_start(variable_args, format);
	_if()->vdprintf(STDERR_FILENO, format, variable_args);
	va_end(variable_args);
	_if()->exit(1);
}

void posix_print_warning(const char *format, ...)
{
	va_list variable_args;

	va_start(variable_args, format);
	_if()->vdprintf(STDERR_FILENO, format, variable_args);
	va_end(variable_args);
}

void posix_print_trace(const char *format, ...)
{
	va_list variable_args;

	va_start(variable_args, format);
	_if()->vdprintf(STDOUT_FILENO, format, variable_args);
	va_end(variable_args);
}
