/*
 * Copyright (c) 2024 Synopsys
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SIGNAL_H_
#define ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SIGNAL_H_

#if defined(_POSIX_C_SOURCE)
#include <sys/signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_DFL ((void (*)(int))0)    /**< Default signal handler */
#define SIG_IGN ((void (*)(int))1)    /**< Ignored signal handler */
#define SIG_ERR ((void (*)(int))(-1)) /**< Ignored signal handler */

#define SIGABRT 6  /**< Aborted */
#define SIGFPE  8  /**< Arithmetic exception */
#define SIGILL  4  /**< Illegal instruction */
#define SIGINT  2  /**< Interrupt */
#define SIGSEGV 11 /**< Invalid memory reference */
#define SIGTERM 15 /**< Terminated */

/** @brief integer type that can be accessed atomically */
typedef volatile long sig_atomic_t;

/** @brief non-standard shorthand for the standard signal handler function pointer type */
typedef void (*sighandler_t)(int sig);

/**
 * @brief Set the signal handler for the specified signal.
 *
 * @param sig the signal for which the handler is to be set
 * @param handler the new signal handler to install
 * @return the previous signal handler for the specified signal, or `SIG_ERR` on error
 */
sighandler_t signal(int sig, sighandler_t handler);

/**
 * @brief Raise a signal to the calling process.
 *
 * @param sig the signal to raise
 * @return 0 on success, non-zero on error
 */
int raise(int sig);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SIGNAL_H_ */
