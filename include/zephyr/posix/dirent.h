/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _DIRENT_H_
#define	_DIRENT_H_

#include <sys/dirent.h>

#if !defined(MAXNAMLEN) && __BSD_VISIBLE
#define MAXNAMLEN 1024
#endif

__BEGIN_DECLS
#if __MISC_VISIBLE || __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE >= 700
int	 alphasort(const struct dirent **, const struct dirent **);
int	 dirfd(DIR *);
#endif
#if __BSD_VISIBLE
int	 fdclosedir(DIR *);
#endif
DIR	*opendir(const char *);
DIR	*fdopendir(int);
struct dirent *
	 readdir(DIR *);
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 500
int	 readdir_r(DIR *__restrict, struct dirent *__restrict,
	    struct dirent **__restrict);
#endif
void	 rewinddir(DIR *);
#if __MISC_VISIBLE || __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE >= 700
int	 scandir(const char *, struct dirent ***,
	    int (*)(const struct dirent *), int (*)(const struct dirent **,
	    const struct dirent **));
#endif
#ifdef _COMPILING_NEWLIB
void	 _seekdir(DIR *, long);
#endif
#if __MISC_VISIBLE || __XSI_VISIBLE
#ifndef __INSIDE_CYGWIN__
void	 seekdir(DIR *, long);
long	 telldir(DIR *);
#endif
#endif
int closedir(DIR *);
__END_DECLS

#endif /*_DIRENT_H_*/
