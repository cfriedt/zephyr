#ifndef INCLUDE_ZEPHYR_POSIX_SYS__SIGSET_H_
#define INCLUDE_ZEPHYR_POSIX_SYS__SIGSET_H_

#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned long sig[MAX(DIV_ROUND_UP(6 + COND_CODE_1(CONFIG_POSIX_SIGNALS, (26), (0)) +
						   COND_CODE_1(CONFIG_POSIX_REALTIME_SIGNALS,
							       (CONFIG_POSIX_RTSIG_MAX), (0)),
					   BITS_PER_LONG),
			      1)];
} sigset_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ZEPHYR_POSIX_SYS__SIGSET_H_ */
