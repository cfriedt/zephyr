/*
 * Copyright (c) 2022 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include <ztest.h>

extern double z_strtod_pow2(int x);
extern double z_strtod_pow10(int x);
extern int z_strtod_exp(const char *in, int base, int *exp);
extern int z_strtod_integral(const char *in, int base, long long *integral);
extern int z_strtod_fractional(const char *in, int base, unsigned long long *num,
			       unsigned long long *den);

ZTEST(test_c_lib, test_z_strod_pow2)
{
}

ZTEST(test_c_lib, test_z_strod_pow10)
{
	/*
	 * Output of pow(10, x) from glibc.
	 * 
	 * 10.0^-1000: 0.00000000000000000e+00
	 * 10.0^-308: 9.99999999999999909e-309
	 * 10.0^-100: 1.00000000000000002e-100
	 * 10.0^-10: 1.00000000000000004e-10
	 * 10.0^-2: 1.00000000000000002e-02
	 * 10.0^-1: 1.00000000000000006e-01
	 * 10.0^0: 1.00000000000000000e+00
	 * 10.0^1: 1.00000000000000000e+01
	 * 10.0^2: 1.00000000000000000e+02
	 * 10.0^10: 1.00000000000000000e+10
	 * 10.0^100: 1.00000000000000002e+100
	 * 10.0^308: 1.00000000000000001e+308
	 * 10.0^1000: inf
	 */
	zassert_equal(0, z_strtod_pow10(-1000), "");
	zassert_within(1e-308, z_strtod_pow10(-308), DBL_EPSILON, "");
	zassert_within(1e-100, z_strtod_pow10(-100), DBL_EPSILON, "");
	zassert_within(1e-10, z_strtod_pow10(-10), DBL_EPSILON, "");
	zassert_within(1e-2, z_strtod_pow10(-2), DBL_EPSILON, "");
	zassert_within(1e-1, z_strtod_pow10(-1), DBL_EPSILON, "");
	zassert_within(1, z_strtod_pow10(0), DBL_EPSILON, "");
	zassert_within(1e1, z_strtod_pow10(1), DBL_EPSILON, "");
	zassert_within(1e2, z_strtod_pow10(2), DBL_EPSILON, "");
	zassert_within(1e10, z_strtod_pow10(10), DBL_EPSILON, "");
	zassert_within(1e100, z_strtod_pow10(100), DBL_EPSILON, "");
	zassert_within(1e308, z_strtod_pow10(308), DBL_EPSILON, "");
	zassert_equal(INFINITY, z_strtod_pow10(1000), "");
}

ZTEST(test_c_lib, test_z_strod_integral)
{
	long long integral;

	integral = 73;
	zassert_equal(0, z_strtod_integral(".0006789e-0123", 10, &integral), "");
	zassert_equal(0, integral, "");

	integral = 73;
	zassert_equal(5, z_strtod_integral("12345.0006789e-0123", 10, &integral), "");
	zassert_equal(12345, integral, "");

	integral = 73;
	zassert_equal(5, z_strtod_integral("12345.e-0123", 10, &integral), "");
	zassert_equal(12345, integral, "");

	integral = 73;
	zassert_equal(4, z_strtod_integral("ffff.0006789p-0123", 16, &integral), "");
	zassert_equal(0xffff, integral, "");
}

ZTEST(test_c_lib, test_z_strod_fractional)
{
	unsigned long long numerator;
	unsigned long long denominator;

	numerator = 73;
	denominator = 42;
	zassert_equal(8, z_strtod_fractional(".0006789e-0123", 10, &numerator, &denominator), "");
	zassert_equal(6789, numerator, "");
	zassert_equal(10000000, denominator, "");

	numerator = 73;
	denominator = 42;
	zassert_equal(5, z_strtod_fractional(".00ffp-0123", 16, &numerator, &denominator), "");
	zassert_equal(0xff, numerator, "");
	zassert_equal(65536, denominator, "");

	numerator = 73;
	denominator = 42;
	zassert_equal(0, z_strtod_fractional("e-0123", 16, &numerator, &denominator), "");
	zassert_equal(0, numerator, "");
	zassert_equal(1, denominator, "");
}

/*
 * Test Plan:
 * - derived from
 *   - http://www.open-std.org/jtc1/sc22/wg14/
 * - "dimensionality" indicated by [N]
 * - Happy Path tests accumulate multiplicatively
 * - Unhappy Path / Corner Case tests accumulate additively
 */

	/** @brief test decimal exponent parsing */
	ZTEST(test_c_lib, test_z_strtod_exp)
	{
		int dexp;

		/*
     * Decimal Exponent Happy Path :)
     * - e or E [1]
     * - nothing / + / - [3]
     * - decimal digits [1]
     * ==========================
     * 3 tests
     */
		dexp = 73;
		zassert_equal(3, z_strtod_exp("e42", 10, &dexp), "");
		zassert_equal(42, dexp, "");
		dexp = 73;
		zassert_equal(4, z_strtod_exp("E+42", 10, &dexp), "");
		zassert_equal(42, dexp, "");
		dexp = 73;
		zassert_equal(4, z_strtod_exp("e-42", 10, &dexp), "");
		zassert_equal(-42, dexp, "");

		/*
     * Decimal Exponent Unhappy Path :(
     * - not e or E [1]
     * - not + / - [1]
     * - not decimal digits [1]
     * ==========================
     * 3 tests (+ null checks)
     */
		dexp = 73;
		zassert_equal(0, z_strtod_exp("x+42", 10, &dexp), "");
		zassert_equal(0, dexp, "");
		dexp = 73;
		zassert_equal(0, z_strtod_exp("e*42", 10, &dexp), "");
		zassert_equal(0, dexp, "");
		dexp = 73;
		zassert_equal(0, z_strtod_exp("Ea", 10, &dexp), "");
		zassert_equal(0, dexp, "");

		/*
     * Decimal Exponent Corner Cases
     * - NULL arguments [2]
     * - empty string [1]
     * - e0, e+0 [2]
     * - e001 [1]
     * - integer overflow (+ve) [3]
     * - integer overflow (-ve) [3]
     * - integer overflow (>>>>0) [1]
     * - binary exponent [1]
     * - incomplete string [2]
     * ==========================
     * 16 tests
     */
		zassert_equal(-EINVAL, z_strtod_exp(NULL, 10, &dexp), "");
		zassert_equal(-EINVAL, z_strtod_exp("e+73", 10, NULL), "");

		dexp = 73;
		zassert_equal(0, z_strtod_exp("", 10, &dexp), "");
		zassert_equal(0, dexp, "");

		dexp = 73;
		zassert_equal(2, z_strtod_exp("e0", 10, &dexp), "");
		zassert_equal(0, dexp, "");
		dexp = 73;
		zassert_equal(3, z_strtod_exp("e+0", 10, &dexp), "");
		zassert_equal(0, dexp, "");

		dexp = 73;
		zassert_equal(5, z_strtod_exp("e-001", 10, &dexp), "");
		zassert_equal(-1, dexp, "");

		dexp = 73;
		zassert_equal(11, z_strtod_exp("e2147483647", 10, &dexp), "");
		zassert_equal(INT_MAX, dexp, "");
		dexp = 73;
		zassert_equal(11, z_strtod_exp("e2147483648", 10, &dexp), "");
		zassert_equal(INT_MAX, dexp, "");
		dexp = 73;
		zassert_equal(16, z_strtod_exp("e999999999999999", 10, &dexp), "");
		zassert_equal(INT_MAX, dexp, "");

		dexp = 73;
		zassert_equal(12, z_strtod_exp("e-2147483648", 10, &dexp), "");
		zassert_equal(INT_MIN, dexp, "");
		dexp = 73;
		zassert_equal(12, z_strtod_exp("e-2147483649", 10, &dexp), "");
		zassert_equal(INT_MIN, dexp, "");
		dexp = 73;
		zassert_equal(16, z_strtod_exp("e-99999999999999", 10, &dexp), "");
		zassert_equal(INT_MIN, dexp, "");

		dexp = 73;
		zassert_equal(
			81,
			z_strtod_exp(
				"e18446744073709551615184467440737095516151844674407370955161518446744073709551615",
				10, &dexp),
			"");
		zassert_equal(INT_MAX, dexp, "");

		/* decimal and binary exponents differ only by the prefix */
		dexp = 73;
		zassert_equal(3, z_strtod_exp("p42", 16, &dexp), "");
		zassert_equal(42, dexp, "");

		/* incomplete string */
		dexp = 73;
		zassert_equal(0, z_strtod_exp("e", 10, &dexp), "");
		zassert_equal(0, dexp, "");

		dexp = 73;
		zassert_equal(0, z_strtod_exp("e+", 10, &dexp), "");
		zassert_equal(0, dexp, "");
}

/** @brief test binary exponent parsing */
ZTEST(test_c_lib, test_strtod_binary_exponent)
{
	/* no need to duplicate all of test_strtod_decimal_exponent() */
}

ZTEST(test_c_lib, test_strtod_decimal)
{
	//char *endptr;

	/*
    * Decimal Happy Path :)
    * - nospace / whitespace [2]
    * - nosign / + / - [3]
    * - decimal digits without / with '.' [2]
    * - noexp / decimal exponent [2]
    * ==========================
    * >= 24 tests
    */

	/* nospace nosign nodot noexp */
	zassert_equal(1., strtod("1", NULL), "");
	/* nospace nosign nodot exp */
	zassert_equal(1., strtod("1e0", NULL), "");
	/* nospace nosign dot   noexp */
	zassert_equal(1., strtod("1.", NULL), "");
	/* nospace nosign dot   exp */
	zassert_equal(0.1, strtod("1.e-1", NULL), "");
	/* nospace +      nodot noexp */
	zassert_equal(1., strtod("+1", NULL), "");
	/* nospace +      nodot exp */
	zassert_equal(1., strtod("+1e0", NULL), "");
	/* nospace +      dot   noexp */
	zassert_equal(1., strtod("+1.", NULL), "");
	/* nospace +      dot   exp */
	zassert_equal(1., strtod("+1.e0", NULL), "");
	/* nospace -      nodot noexp */
	zassert_equal(-1., strtod("-1", NULL), "");
	/* nospace -      nodot exp */
	zassert_equal(-1., strtod("-1e0", NULL), "");
	/* nospace -      dot   noexp */
	zassert_equal(-1., strtod("-1.", NULL), "");
	/* nospace -      dot   exp */
	zassert_equal(-1., strtod("-1.e0", NULL), "");
	/* space   nosign nodot noexp */
	zassert_equal(1., strtod("\t\n\v\f\r 1", NULL), "");
	/* space   nosign nodot exp */
	zassert_equal(1., strtod("\t\n\v\f\r 1e0", NULL), "");
	/* space   nosign dot   noexp */
	zassert_equal(1., strtod("\t\n\v\f\r 1.", NULL), "");
	/* space   nosign dot   exp */
	zassert_equal(1., strtod("\t\n\v\f\r 1.e0", NULL), "");
	/* space   +      nodot noexp */
	zassert_equal(1., strtod("\t\n\v\f\r +1", NULL), "");
	/* space   +      nodot exp */
	zassert_equal(1., strtod("\t\n\v\f\r +1e0", NULL), "");
	/* space   +      dot   noexp */
	zassert_equal(1., strtod("\t\n\v\f\r +1.", NULL), "");
	/* space   +      dot   exp */
	zassert_equal(1., strtod("\t\n\v\f\r +1.e0", NULL), "");
	/* space   -      nodot noexp */
	zassert_equal(-1., strtod("\t\n\v\f\r -1", NULL), "");
	/* space   -      nodot exp */
	zassert_equal(-1., strtod("\t\n\v\f\r -1e0", NULL), "");
	/* space   -      dot   noexp */
	zassert_equal(-1., strtod("\t\n\v\f\r -1.", NULL), "");
	/* space   -      dot   exp */
	zassert_equal(-1., strtod("\t\n\v\f\r -1.e0", NULL), "");

	/* 
    * Decimal Unhappy Path :(
    * - not + / -
    * - not decimal digit [1]
    * - not decimal exponent [1]
    * ==========================
    * 3 tests
    */

	/*
    * Decimal Corner Cases
    * - endptr check
    * - very large values
    * - . placement (with a few more significant digits)
    */
	zassert_within(-12345.0006789e-123, strtod("-12345.0006789e-0123", NULL), 1e-7, "");
}

ZTEST(test_c_lib, test_strtod_hexadecimal)
{
	/*
    * Hexadecimal Happy Path :)
    * - nothing / whitespace [2]
    * - nothing / + / - [3]
    * - "0x" or "0X" [1]
    * - hexadecimal digits with / without '.' [2]
    *   - probably 3 tests w.r.t. position of '.'
    * - nothing / binary exponent [2]
    * ==========================
    * >= 24 tests
    */

	/* 
    * Hexadecimal Unhappy Path :(
    * - not whitespace [1]
    * - not + / - [1]
    * - not "0x" or "0X" [1]
    * - not hexadecimal digits [1]
    * - not binary exponent [1]
    * - at least one of radix or binary exponent must be present [1]
    * ==========================
    * 6 tests
    */
}

ZTEST(test_c_lib, test_strtod_inf)
{
	/*
    * INFINITY Happy Path :)
    * - nospace / whitespace [2]
    * - nosign / + / - [3]
    * - inf / INFINITY [2]
    * ==========================
    * 12 tests
    */
	/* nospace nosign inf */
	zassert_equal(INFINITY, strtod("inf", NULL), "");
	/* nospace nosign INFINITY */
	zassert_equal(INFINITY, strtod("INFINITY", NULL), "");
	/* nospace +      inf */
	zassert_equal(INFINITY, strtod("+inf", NULL), "");
	/* nospace +      INFINITY */
	zassert_equal(INFINITY, strtod("+INFINITY", NULL), "");
	/* nospace -      inf */
	zassert_equal(-INFINITY, strtod("-inf", NULL), "");
	/* nospace -      INFINITY */
	zassert_equal(-INFINITY, strtod("-INFINITY", NULL), "");
	/* space   nosign inf */
	zassert_equal(INFINITY, strtod("\t\n\v\f\r inf", NULL), "");
	/* space   nosign INFINITY */
	zassert_equal(INFINITY, strtod("\t\n\v\f\r INFINITY", NULL), "");
	/* space   +      inf */
	zassert_equal(INFINITY, strtod("\t\n\v\f\r +inf", NULL), "");
	/* space   +      INFINITY */
	zassert_equal(INFINITY, strtod("\t\n\v\f\r +INFINITY", NULL), "");
	/* space   -      inf */
	zassert_equal(-INFINITY, strtod("\t\n\v\f\r -inf", NULL), "");
	/* space   -      INFINITY */
	zassert_equal(-INFINITY, strtod("\t\n\v\f\r -INFINITY", NULL), "");

	/*
    * INFINITY Unappy Path :(
    * - not whitespace [1]
    * - not + / - [1]
    * - not inf / INFINITY [1]
    * ==========================
    * 3 tests
    */
}

ZTEST(test_c_lib, test_strtod_nan)
{
	/*
    * NaN Happy Path :)
    * - nothing / whitespace [2]
    * - nothing / + / - [3]
    * - NaN [1]
    * - nothing / (string) [2]
    * ==========================
    * 12 tests
    */

	/*
    * NaN Unappy Path :(
    * - not whitespace [1]
    * - not + / - [1]
    * - not NaN [1]
    * - not nothing / (string) [1]
    * ==========================
    * 4 tests
    */
}
