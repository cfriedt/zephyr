#ifndef ZEPHYR_INCLUDE_ARCH_POSIX_POSIX_ARCH_IF_H_
#define ZEPHYR_INCLUDE_ARCH_POSIX_POSIX_ARCH_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>

struct timespec;

void posix_arch_lock_cpu(void);
void posix_arch_lock_threads(void);
void posix_arch_broadcast_cpu(void);
void posix_arch_broadcast_threads(void);
void *posix_arch_calloc(size_t nmemb, size_t size);
int posix_arch_clock_gettime_mono(struct timespec *tv);
int posix_arch_clock_gettime_real(struct timespec *tv);
void posix_arch_exit(int val);
void posix_arch_fflush_stdout(void);
int posix_arch_fprintf_stdout(const char *format, ...);
void posix_arch_free(void *ptr);
int posix_arch_isatty_stdout(void);
int posix_arch_isatty_stderr(void);
void *posix_arch_malloc(size_t size);
int posix_arch_nanosleep(struct timespec *req, struct timespec *rem);
int posix_arch_pthread_cancel(uint64_t thread);
void posix_arch_pthread_create(uint64_t *thread, void *(*entry)(void *arg), void *arg);
void posix_arch_pthread_detach_self(void);
void posix_arch_pthread_exit(void *value);
int posix_arch_putchar(int ch);
long posix_arch_random(void);
void *posix_arch_realloc(void *ptr, size_t size);
void *posix_arch_reallocarray(void *ptr, size_t nmemb, size_t size);
void posix_arch_setvbuf_stderr(void);
void posix_arch_setvbuf_stdout(void);
void posix_arch_set_sig_handler(void (*handler)(int sig));
unsigned int posix_arch_sleep(unsigned int seconds);
void posix_arch_srandom(long seed);
/* this should eventually just be added to the minimal libc */
double posix_arch_strtod(const char *nptr, char **endptr);
void posix_arch_unlock_cpu(void);
void posix_arch_unlock_threads(void);
void posix_arch_wait_cpu(void);
void posix_arch_wait_threads(void);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_ARCH_POSIX_POSIX_ARCH_IF_H_ */
