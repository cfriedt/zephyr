/*
 * Copyright 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#include <zephyr/ztest.h>
#include <zephyr/sys_clock.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/sys/timespec_util.h>

/* Normally NSEC_PER_SEC is defined as UL, which breaks static declarations below */
#undef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000LL

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
		.invalid_ts =                                                                      \
			{                                                                          \
				.tv_sec = (sec),                                                   \
				.tv_nsec = (nsec),                                                 \
			},                                                                         \
		.valid_ts =                                                                        \
			{                                                                          \
				.tv_sec = (sec),                                                   \
				.tv_nsec = (nsec),                                                 \
			},                                                                         \
		.expect_valid = true,                                                              \
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
		.valid_ts =                                                                        \
			{                                                                          \
				.tv_sec = (valid_sec),                                             \
				.tv_nsec = (valid_nsec),                                           \
			},                                                                         \
		.invalid_ts =                                                                      \
			{                                                                          \
				.tv_sec = (invalid_sec),                                           \
				.tv_nsec = (invalid_nsec),                                         \
			},                                                                         \
		.expect_valid = false,                                                             \
	}

/*
 * Overflow tests for invalid timespecs cannot be normalized as their tv_sec values would overflow.
 */
#define DECL_OVFLW_TS_TEST(invalid_sec, invalid_nsec)                                              \
	{                                                                                          \
		.invalid_ts =                                                                      \
			{                                                                          \
				.tv_sec = (invalid_sec),                                           \
				.tv_nsec = (invalid_nsec),                                         \
			},                                                                         \
		.expect_valid = false, .expect_ovflw = true,                                       \
	}

/*
 * Easily verifiable tests for both valid and invalid timespecs.
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
	DECL_VALID_TS_TEST(INT64_MIN, 0),
	DECL_VALID_TS_TEST(INT64_MIN, NSEC_PER_SEC - 1),
	DECL_VALID_TS_TEST(INT64_MAX, 0),
	DECL_VALID_TS_TEST(INT64_MAX, NSEC_PER_SEC - 1),

	/* Correctable, invalid cases */
	DECL_INVALID_TS_TEST(0, -1, -1, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(0, NSEC_PER_SEC, 1, 0),
	DECL_INVALID_TS_TEST(1, -1, 0, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(1, NSEC_PER_SEC, 2, 0),

	DECL_INVALID_TS_TEST(-1, -1, -2, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(0, NSEC_PER_SEC, 1, 0),
	DECL_INVALID_TS_TEST(1, -1, 0, NSEC_PER_SEC - 1),
	DECL_INVALID_TS_TEST(1, NSEC_PER_SEC, 2, 0),

	DECL_INVALID_TS_TEST(INT64_MIN, NSEC_PER_SEC, INT64_MIN + 1, 0),
	DECL_INVALID_TS_TEST(INT64_MAX, -1, INT64_MAX - 1, NSEC_PER_SEC - 1),

	/* Incorrectable, invalid cases */
	DECL_OVFLW_TS_TEST(INT64_MIN, -1),
	DECL_OVFLW_TS_TEST(INT64_MIN, -NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT64_MIN, -NSEC_PER_SEC - 1),
	DECL_OVFLW_TS_TEST(INT64_MIN, -2 * NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT64_MIN, -2 * NSEC_PER_SEC - 1),
	DECL_OVFLW_TS_TEST(INT64_MIN + 1, -1 * NSEC_PER_SEC - 1),
	DECL_OVFLW_TS_TEST(INT64_MIN + 2, -2 * NSEC_PER_SEC - 1),

	DECL_OVFLW_TS_TEST(INT64_MAX, NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT64_MAX, NSEC_PER_SEC + 1),
	DECL_OVFLW_TS_TEST(INT64_MAX, 2 * NSEC_PER_SEC),
	DECL_OVFLW_TS_TEST(INT64_MAX, 2 * NSEC_PER_SEC + 1),
	DECL_OVFLW_TS_TEST(INT64_MAX - 1, 3 * NSEC_PER_SEC - 1),
	DECL_OVFLW_TS_TEST(INT64_MAX - 2, 4 * NSEC_PER_SEC - 1),
};
BUILD_ASSERT(sizeof(((struct timespec *)0)->tv_sec) == sizeof(int64_t), "tv_sec must be 64 bits");

ZTEST(timespec, test_timespac_is_valid)
{
	ARRAY_FOR_EACH(ts_tests, i) {
		const struct ts_test_spec *const tspec = &ts_tests[i];
		const struct timespec *const ts = &tspec->invalid_ts;
		bool valid = timespec_is_valid(&tspec->invalid_ts);

		if (tspec->expect_valid) {
			zexpect_equal(valid, true,
				      "timespec_is_valid({%" PRId64
				      ", %ld}) = false, expected true",
				      ts->tv_sec, ts->tv_nsec);
		} else {
			zexpect_equal(valid, false,
				      "timespec_is_valid({%" PRId64
				      ", %ld}) = true, expected false",
				      ts->tv_sec, ts->tv_nsec);
		}
	}
}

ZTEST(timespec, test_timespec_normalize)
{
	ARRAY_FOR_EACH(ts_tests, i) {
		struct timespec norm;
		const struct ts_test_spec *const tspec = &ts_tests[i];
		const struct timespec *const valid = &tspec->valid_ts;
		const struct timespec *const invalid = &tspec->invalid_ts;

		zexpect_true(timespec_is_valid(valid),
			     "Valid timespec {%" PRId64 ", %ld} is not valid", valid->tv_sec,
			     valid->tv_nsec);

		if (tspec->expect_ovflw) {
			/* by definition, invalid, and normalize should overflow */
			zexpect_false(timespec_is_valid(invalid),
				      "Invalid timespec {%" PRId64 ", %ld} is not invalid",
				      (int64_t)invalid->tv_sec, (int64_t)invalid->tv_nsec);

			norm.tv_sec = invalid->tv_sec;
			norm.tv_nsec = invalid->tv_nsec;
			zexpect_true(timespec_normalize(&norm),
				     "timespec_normalize({%" PRId64 ", %ld}) did not overflow",
				     (int64_t)tspec->invalid_ts.tv_sec,
				     (int64_t)tspec->invalid_ts.tv_nsec);

			continue;
		}

		/* either valid or correctable timespecs */
		if (tspec->expect_valid) {
			norm = *valid;
			zexpect_false(timespec_normalize(&norm),
				      "timespec_normalize({%" PRId64 ", %ld}) caused an overflow",
				      valid->tv_sec, valid->tv_nsec);
			zexpect_true((norm.tv_sec == valid->tv_sec) &&
					     (norm.tv_nsec == valid->tv_nsec),
				     "timespec_normalize({%" PRId64
				     ", %ld}) modified a valid timespec to "
				     "{%" PRId64 ", %ld}",
				     valid->tv_sec, valid->tv_nsec, norm.tv_sec, norm.tv_nsec);
		} else {
			zexpect_false(timespec_is_valid(invalid),
				      "Invalid timespec {%" PRId64 ", %ld} is not valid",
				      invalid->tv_sec, invalid->tv_nsec);

			norm = *invalid;
			zexpect_false(timespec_normalize(&norm),
				      "timespec_normalize({%" PRId64 ", %ld}) caused an overflow",
				      invalid->tv_sec, invalid->tv_nsec);
			zexpect_true((norm.tv_sec == valid->tv_sec) &&
					     (norm.tv_nsec == valid->tv_nsec),
				     "timespec_normalize({%" PRId64 ", %ld}) did not result in the "
				     "expected valid timespec {%" PRId64 ", %ld}",
				     norm.tv_sec, norm.tv_nsec, valid->tv_sec, valid->tv_nsec);
		}
	}
}

ZTEST(timespec, test_timespec_neg)
{
	struct test_ts_neg_spec {
		struct timespec ts;
		struct timespec neg;
		bool expect_ovflw;
	};

	struct timespec res;
	static const struct test_ts_neg_spec specs[] = {
		{{INT64_MIN, LONG_MIN}, {0, 0}, true},
		{{INT64_MIN, -NSEC_PER_SEC - 1}, {0, 0}, true},
		{{INT64_MIN, -NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MIN, -NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MIN, 0}, {0, 0}, true},
		{{INT64_MIN, NSEC_PER_SEC - 1}, {0, 0}, true},
		{{INT64_MIN, NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MIN, NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MAX}, {0, 0}, true},

		{{0, LONG_MIN}, {0, 0}, false},
		{{0, -NSEC_PER_SEC - 1}, {0, 0}, false},
		{{0, -NSEC_PER_SEC}, {0, 0}, false},
		{{0, -NSEC_PER_SEC + 1}, {0, 0}, false},
		{{0, 0}, {0, 0}, false},
		{{0, NSEC_PER_SEC - 1}, {0, 0}, false},
		{{0, NSEC_PER_SEC}, {0, 0}, false},
		{{0, NSEC_PER_SEC + 1}, {0, 0}, false},
		{{0, LONG_MAX}, {0, 0}, false},

		{{INT64_MAX, LONG_MIN}, {0, 0}, true},
		{{INT64_MAX, -NSEC_PER_SEC - 1}, {0, 0}, true},
		{{INT64_MAX, -NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MAX, -NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MAX, 0}, {0, 0}, false},
		{{INT64_MAX, NSEC_PER_SEC - 1}, {0, 0}, true},
		{{INT64_MAX, NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MAX, NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MAX, LONG_MAX}, {0, 0}, true},
	};

	ARRAY_FOR_EACH(ts_tests, i) {
		const struct test_ts_neg_spec *const tspec = &specs[i];
		const struct timespec *const ts = &tspec->ts;
		const struct timespec *const neg = &tspec->neg;

		res = *ts;
		if (tspec->expect_ovflw) {
			res = *ts;
			zexpect_true(timespec_neg(&res),
				     "timespec_neg({%" PRId64 ", %ld}) did not overflow",
				     ts->tv_sec, ts->tv_nsec);
			continue;
		}

		zexpect_false(timespec_neg(&res),
			      "timespec_neg({%" PRId64 ", %ld}) caused an overflow", ts->tv_sec,
			      ts->tv_nsec);
		zexpect_equal(res.tv_sec, neg->tv_sec,
			      "timespec_neg({%" PRId64 ", %ld}) did not result in the expected "
			      "negated timespec {%" PRId64 ", %ld}",
			      ts->tv_sec, ts->tv_nsec, neg->tv_sec, neg->tv_nsec);
	}
}

#if 0
ZTEST(timespec, test_timespec_add)
{
	struct test_ts_add_spec {
		struct timespec a;
		struct timespec b;
		struct timespec c;
		bool expect_ovflw;
	};

	struct timespec res;
	static const struct test_ts_add_spec ts_add_specs[] = {
		/* There are many ways to generate both negative and positive overflows using both
		   inputs */
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, LONG_MIN}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, -NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, -NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, -1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, 0}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, LONG_MAX}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {-1, LONG_MIN}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, -NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, -NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, -1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, 0}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, NSEC_PER_SEC}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, NSEC_PER_SEC + 1}, {0, 0}, true},
		{{INT64_MIN, LONG_MIN}, {INT64_MIN, LONG_MAX}, {0, 0}, true},
	};
}
#endif

ZTEST_SUITE(timespec, NULL, NULL, NULL, NULL, NULL);
