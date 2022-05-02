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

#ifdef CONFIG_ZTEST
#define STATIC
#else
#define STATIC static
#endif

STATIC int z_strtod_exp(const char *in, int base, int *exp);
STATIC int z_strtod_integral(const char *in, int base, long long *integral);
STATIC int z_strtod_fractional(const char *in, int base, unsigned long long *num,
			       unsigned long long *den);

static double pow2(int x)
{
	ARG_UNUSED(x);

	return 0.0;
}

static double pow10(int x)
{
	int i;
	int sign;
	double p10 = 1.0;

	sign = (x >= 0) ? +1 : -1;
	x *= sign;

	if (sign == -1) {
		for (i = 0; i < x; ++i) {
			p10 /= 10;
		}
	} else {
		for (i = 0; i < x; ++i) {
			p10 *= 10;
		}
	}

	return p10;
}

double strtod(const char *nptr, char **endptr)
{
	int n;
	double value;
	int sign = +1;
	int base = 10;
	long long integral;
	int exponential = 0;
	const char *s = nptr;
	unsigned long long fractional_numerator;
	unsigned long long fractional_denominator;

	for (; isspace(*s); ++s) {
	}

	if (*s == '+' || *s == '-') {
		sign = (*s == '+') ? +1 : -1;
		++s;
	}

	if (0 == strncasecmp(s, "0x", 2)) {
		base = 16;
		s += 2;
	}

	n = z_strtod_integral(s, base, &integral);
	if (n < 0) {
		errno = EINVAL;
		return 0.0;
	}

	s += n;

	n = z_strtod_fractional(s, base, &fractional_numerator, &fractional_denominator);
	if (n < 0) {
		errno = EINVAL;
		return 0.0;
	}

	s += n;

	n = z_strtod_exp(s, base, &exponential);
	if (n < 0) {
		errno = EINVAL;
		return 0.0;
	}

	s += n;

	value = (double)integral + (double)fractional_numerator / (double)fractional_denominator;

	if (base == 10) {
		value *= pow10(exponential);
	} else if (base == 16) {
		value *= pow2(exponential);
	}

	value *= sign;

	if (endptr != NULL) {
		*endptr = (char *)s;
	}

	return value;
}

float strtof(const char *nptr, char **endptr)
{
	return (float)strtod(nptr, endptr);
}

/**
 * @brief parse decimal or binary exponent
 * 
 * Generally, exponents for floating point values are of thqe form `[eEpP][+\-]?[0-9]+`.
 * 
 * The only difference between decimal and binary exponents are that the
 * latter are prefixed with 'p' or 'P' instead of 'e' or 'E'.
 * 
 * @param in input character string
 * @param exp output exponent
 * @param base numeric base (either 10 or 16)
 * @return on success, number of characters consumed from @p in
 * @return -EINVAL if @p in, @p exp, or @p base are invalid.
 *
 * @see <a href="http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf">ISO/IEC 9899</a>, sections 6.4.4.2, 7.22.1.3.
 */
int z_strtod_exp(const char *in, int base, int *exp)
{
	int sign = +1;
	const char *s = in;
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

	return endptr - in;
}

int z_strtod_integral(const char *in, int base, long long *integral)
{
	const char *s = in;
	char *endptr = (char *)in;

	if (s == NULL || integral == NULL || !(base == 10 || base == 16)) {
		return -EINVAL;
	}

	*integral = strtoll(in, &endptr, base);

	return endptr - in;
}

int z_strtod_fractional(const char *in, int base, unsigned long long *num, unsigned long long *den)
{
	const char *s = in;
	char *endptr = (char *)in;

	if (s == NULL || num == NULL || den == NULL || !(base == 10 || base == 16)) {
		return -EINVAL;
	}

	if (*s != '.') {
		*num = 0;
		*den = 1;
		return 0;
	}

	++s;

	*num = strtoull(s, &endptr, base);
	*den = 1;

	if (base == 10) {
		for (; s < endptr; ++s) {
			*den *= 10;
		}
	} else {
		for (; s < endptr; ++s) {
			*den *= 16;
		}
	}

	return endptr - in;
}
