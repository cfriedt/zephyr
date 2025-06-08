#ifndef ZEPHYR_INCLUDE_SYS__SIGSET_H_
#define ZEPHYR_INCLUDE_SYS__SIGSET_H_

#include <sys/signal.h>

#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_POSIX_C_SOURCE) && !defined(__sigset_t_defined) && !defined(_SIGSET_T_DECLARED)
#define __sigset_t_defined
#define _SIGSET_T_DECLARED
typedef struct {
	unsigned long sig[DIV_ROUND_UP(_NSIG, BITS_PER_LONG)];
} sigset_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SYS__SIGSET_H_ */
