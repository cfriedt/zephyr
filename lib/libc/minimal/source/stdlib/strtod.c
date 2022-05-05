/*
 * Copyright (c) 2022 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/util.h>

#include <zephyr.h>

extern int z_parse_inf(const char *nptr);
extern int z_parse_nan(const char *nptr);
extern int z_parse_exp(const char *nptr, int base, int *exp);

double strtod(const char *nptr, char **endptr)
{
	int n;
	int sign = +1;
	int base = 10;
	long double value;
	const char *s = nptr;

	for (; isspace(*s); ++s) {
	}

	if (*s == '+' || *s == '-') {
		sign = (*s == '+') ? +1 : -1;
		++s;
	}

	n = z_parse_inf(s);
	if (n < 0) {
		goto error;
	} else if (n > 0) {
		s += n;
		value = sign * INFINITY;
		goto write_endptr;
	}

	n = z_parse_nan(s);
	if (n < 0) {
		goto error;
	} else if (n > 0) {
		s += n;
		value = NAN;
		goto write_endptr;
	}

	if (0 == strncasecmp(s, "0x", 2)) {
		base = 16;
		s += 2;
	}

	n = z_parse_float(s, &value);
	if (n < 0) {
		goto error;
	}

	goto write_endptr;

error:
	s = nptr;
	value = 0.0;
	errno = EINVAL;

write_endptr:
	if (endptr != NULL) {
		*endptr = (char *)s;
	}

	return value;
}
