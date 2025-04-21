/*
 * Copyright 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <time.h>

#include <zephyr/ztest.h>
#include <zephyr/sys_clock.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/sys/timespec_util.h>

/*
 * test spec for simple timespec validation
 *
 * If a timespec is expected to be valid, then invalid_ts and valid_ts are equal.
 *
 * If a timespec is expected to be valid, then invalid_ts and valid_ts are equal.
 */
struct ts_test_spec {
	struct timespec invalid_ts;
	struct timespec valid_ts;
	bool expect_valid;
	bool expect_ovflw;
};

/*
 * Valid timespecs do not require any normalization, and will remain unchanged by the
 * process of normalization.
 */
#define DECL_VALID_TS_TEST(sec, nsec)                                                              \
	{                                                                                          \
		.invalid_ts = {(sec), (nsec)}, .valid_ts = {(sec), (nsec)}, .expect_valid = true,  \
	}

/*
 * Invalid timespecs can usually be converted to valid ones by either making the
 * tv_sec field more negative or more positive, as the case may be.
 *
 * There are two particular corner cases:
 * 1. making tv_sec more negative would overflow the tv_sec field in the negative direction
 * 1. making tv_sec more positive would overflow the tv_sec field in the positive direction
 */
#define DECL_INVALID_TS_TEST(invalid_sec, invalid_nsec, valid_sec, valid_nsec)                     \
	{                                                                                          \
		.valid_ts = {(valid_sec), (valid_nsec)},                                           \
		.invalid_ts = {(invalid_sec), (invalid_nsec)}, .expect_valid = false,              \
	}

/*
 * Overflow tests for invalid timespecs cannot be normalized as their tv_sec values would overflow.
 */
#define DECL_OVFLW_TS_TEST(invalid_sec, invalid_nsec)                                              \
	{                                                                                          \
		.invalid_ts = {(invalid_sec), (invalid_nsec)}, .expect_valid = false,              \
		.expect_ovflw = true,                                                              \
	}

/*
 * Easily verifiable tests for both valid and invalid timespecs.
 *
 * Note: ISO C does not specify minimum / maximum values for time_t so we only include tests
 * with preprocessor constant values in the int32_t range below.
 *
 * There are tests for the full range of tv_sec and tv_nsec further down in this file, but
 * they use compile-time checks.
 */
static const struct ts_test_spec ts_tests[] = {
	/* Valid cases */
	DECL_VALID_TS_TEST(0, 0),
	DECL_VALID_TS_TEST(0, 1),
	DECL_VALID_TS_TEST(1, 0),
	DECL_VALID_TS_TEST(1, 1),
	DECL_VALID_TS_TEST(1, NSEC_PER_SEC - 1),
	DECL_VALID_TS_TEST(-1, 0),
	DECL_VALID_TS_TEST(-1, 1),
	DECL_VALID_TS_TEST(-1, 0),
	DECL_VALID_TS_TEST(-1, 1),
	DECL_VALID_TS_TEST(-1, NSEC_PER_SEC - 1),
	DECL_VALID_TS_TEST(INT32_MIN, 0),
	DECL_VALID_TS_TEST(INT32_MIN, NSEC_PER_SEC - 1),
	DECL_VALID_TS_TEST(INT32_MAX, 0),
	DECL_VALID_TS_TEST(INT32_MAX, NSEC_PER_SEC - 1),

	/* Correctable, invalid cases */
	DECL_INVALID_TS_TEST(0, -1, -1, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(0, NSEC_PER_SEC, 1, 0),
	DECL_INVALID_TS_TEST(1, -1, 0, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(1, NSEC_PER_SEC, 2, 0),
	DECL_INVALID_TS_TEST(-1, -1, -2, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(0, NSEC_PER_SEC, 1, 0),
	DECL_INVALID_TS_TEST(1, -1, 0, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(1, NSEC_PER_SEC, 2, 0),

	DECL_INVALID_TS_TEST(INT32_MIN, NSEC_PER_SEC, INT32_MIN + 1, 0),
	DECL_INVALID_TS_TEST(INT32_MAX, -1, INT32_MAX - 1, NSEC_PER_SEC - 1),

	/* Incorrectable, invalid cases */
	DECL_OVFLW_TS_TEST(INT32_MIN, -1),
	DECL_OVFLW_TS_TEST(INT32_MIN + 1, -NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT32_MIN + 2, -2 * NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT32_MAX, NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT32_MAX, NSEC_PER_SEC + 1),
	DECL_OVFLW_TS_TEST(INT32_MAX - 1, 2 * NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT32_MAX - 1, 2 * NSEC_PER_SEC + 1),
};

ZTEST(timespec, test_timespac_is_valid)
{
	ARRAY_FOR_EACH(ts_tests, i) {
		const struct ts_test_spec *const tspec = &ts_tests[i];
		const struct timespec *const ts = &tspec->invalid_ts;
		bool valid = timespec_is_valid(&tspec->invalid_ts);

		if (tspec->expect_valid) {
			zexpect_equal(valid, true,
				      "timespec_is_valid({%ld, %ld}) = false, expected true",
				      ts->tv_sec, ts->tv_nsec);
		} else {
			zexpect_equal(valid, false,
				      "timespec_is_valid({%ld, %ld}) = true, expected false",
				      ts->tv_sec, ts->tv_nsec);
		}
	}
}

ZTEST(timespec, test_timespac_normalize)
{
	ARRAY_FOR_EACH(ts_tests, i) {
		struct timespec norm;
		const struct ts_test_spec *const tspec = &ts_tests[i];
		const struct timespec *const valid = &tspec->valid_ts;
		const struct timespec *const invalid = &tspec->invalid_ts;

		zassert_true(timespec_is_valid(valid), "Valid timespec {%ld, %ld} is not valid",
			     valid->tv_sec, valid->tv_nsec);

		if (tspec->expect_ovflw) {
			/* if we expect normalization should overflow, then they should be invalid
			 */
			zassert_false(timespec_is_valid(invalid),
				      "Invalid timespec {%ld, %ld} is not invalid", invalid->tv_sec,
				      invalid->tv_nsec);

			/* normalize should overflow */
			norm = *invalid;
			zassert_true(timespec_normalize(&norm),
				     "timespec_normalize({%ld, %ld}) did not overflow",
				     tspec->invalid_ts.tv_sec, tspec->invalid_ts.tv_nsec);

			continue;
		}

		/* either valid or correctable timespecs */
		if (tspec->expect_valid) {
			norm = *valid;
			zassert_false(timespec_normalize(&norm),
				      "timespec_normalize({%ld, %ld}) caused an overflow",
				      valid->tv_sec, valid->tv_nsec);
			zexpect_true((norm.tv_sec == valid->tv_sec) &&
					     (norm.tv_nsec == valid->tv_nsec),
				     "timespec_normalize({%ld, %ld}) modified a valid timespec to "
				     "{%ld, %ld}",
				     valid->tv_sec, valid->tv_nsec, norm.tv_sec, norm.tv_nsec);
		} else {
			zassert_false(timespec_is_valid(invalid),
				      "Invalid timespec {%ld, %ld} is not valid", invalid->tv_sec,
				      invalid->tv_nsec);

			norm = *invalid;
			zassert_false(timespec_normalize(&norm),
				      "timespec_normalize({%ld, %ld}) caused an overflow",
				      invalid->tv_sec, invalid->tv_nsec);
			zexpect_true((norm.tv_sec == valid->tv_sec) &&
					     (norm.tv_nsec == valid->tv_nsec),
				     "timespec_normalize({%ld, %ld}) did not result in the "
				     "expected valid timespec {%ld, %ld}",
				     norm.tv_sec, norm.tv_nsec, valid->tv_sec, valid->tv_nsec);
		}
	}
}

ZTEST_SUITE(timespec, NULL, NULL, NULL, NULL, NULL);
