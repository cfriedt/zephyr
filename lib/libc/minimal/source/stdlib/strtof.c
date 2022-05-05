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

int z_parse_inf(const char *nptr);
int z_parse_nan(const char *nptr);
int z_parse_exp(const char *nptr, int base, int *exp);

float strtof(const char *nptr, char **endptr)
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

int z_parse_exp(const char *nptr, int base, int *exp)
{
	int sign = +1;
	const char *s = nptr;
	char *endptr = NULL;
	unsigned long long mexp;

	if (s == NULL || exp == NULL || !(base == 10 || base == 16)) {
		return -EINVAL;
	}

	if (base == 10) {
		if (!(*s == 'e' || *s == 'E')) {
			*exp = 0;
			return 0;
		}

		++s;
	}

	if (base == 16) {
		if (!(*s == 'p' || *s == 'P')) {
			*exp = 0;
			return 0;
		}

		++s;
	}

	if (*s == '+' || *s == '-') {
		sign = (*s == '+') ? +1 : -1;
		++s;
	}

	if (!isdigit(*s)) {
		/* must be decimal */
		*exp = 0;
		return 0;
	}

	/* endptr valid even for values larger than UULONG_MAX */
	mexp = strtoull(s, &endptr, 10);
	if (mexp == ULLONG_MAX) {
		*exp = (sign == +1) ? INT_MAX : INT_MIN;
	} else if (sign == +1 && mexp >= INT_MAX) {
		*exp = INT_MAX;
	} else if (sign == -1 && mexp >= (unsigned long long)INT_MAX + 1) {
		*exp = INT_MIN;
	} else {
		*exp = sign * (int)mexp;
	}

	return endptr - nptr;
}
