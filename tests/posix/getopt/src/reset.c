/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>

#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include <zephyr/posix/unistd.h>
#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>
#include <getopt.h>

ZTEST(getopt_test_suite, test_reset)
{
	char *const args1[] = {"crc", "-t", "8", "0", "42"};
	char *const args2[] = {"crc", "-t", "8", "-r", "0", "42"};
	const int exp = 't';
	int act;

	optind = 1;
	act = getopt(ARRAY_SIZE(args1), args1, "t:a:s:");
	zassert_equal(exp, act, "exp: %d '%c' act: %d '%c'", exp, (char)exp, act,
		      (char)(isgraph(act) ? act : '?'));

	optind = 1;
	act = getopt(ARRAY_SIZE(args2), args2, "t:a:s:");
	zassert_equal(exp, act, "exp: %d '%c' act: %d '%c'", exp, (char)exp, act,
		      (char)(isgraph(act) ? act : '?'));

	optind = 1;
	act = getopt(ARRAY_SIZE(args1), args1, "t:a:s:");
	zassert_equal(exp, act, "exp: %d '%c' act: %d '%c'", exp, (char)exp, act,
		      (char)(isgraph(act) ? act : '?'));
}
