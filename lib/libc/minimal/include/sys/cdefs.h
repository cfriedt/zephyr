/*
 * Copyright (c) 2022 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef	ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SYS_CDEFS_H_
#define	ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SYS_CDEFS_H_

#define _SYS_CDEFS_H 1

#ifdef	__cplusplus
# define __BEGIN_DECLS	extern "C" {
# define __END_DECLS	}
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#define __THROW
#define __THROWNL

#define __wur
#define __attr_access(x)
#define __warnattr(x)
#define __nonnull(x)

#endif /* ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SYS_CDEFS_H_ */
