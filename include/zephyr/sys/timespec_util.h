/*
 * Copyright 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_SYS_TSUTIL_H_
#define ZEPHYR_INCLUDE_SYS_TSUTIL_H_

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <zephyr/sys_clock.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

BUILD_ASSERT(sizeof(((struct timespec *)0)->tv_sec) == sizeof(int64_t), "tv_sec must be 64-bit");

/**
 * @defgroup timespec_util_apis Timespec Utility APIs
 * @ingroup utilities
 * @defgroup timespec_util_repr_apis Timespec Representation APIs
 * @ingroup timespec_util_apis
 * @{
 */

/**
 * @brief Check if a timespec is valid.
 *
 * Check if a timespec is valid by ensuring that the `tv_nsec` field is in the range [0,
 * NSEC_PER_SEC-1]`.
 *
 * @note @p ts must not be `NULL`.
 *
 * @param ts the timespec to check
 *
 * @return `true` if the timespec is valid, otherwise `false`.
 */
static inline bool timespec_is_valid(const struct timespec *ts)
{
	__ASSERT_NO_MSG(ts != NULL);

	return (ts->tv_nsec >= 0) && (ts->tv_nsec < NSEC_PER_SEC);
}

#if (__STDC_VERSION__ >= 201112L) && HAS_BUILTIN(__builtin_add_overflow) || defined(__DOXYGEN__)
/**
 * @brief Normalize a timespec so that the `tv_nsec` field is in valid range.
 *
 * Normalize a timespec by adjusting the `tv_sec` and `tv_nsec` fields so that the `tv_nsec` field
 * is in the range `[0, NSEC_PER_SEC-1]`. This is achieved by converting nanoseconds to seconds and
 * accumulating seconds in either the positive direction when `tv_nsec` > `NSEC_PER_SEC`, or in the
 * negative direction when `tv_nsec` < 0.
 *
 * In pseudocode, normalization can be done as follows:
 * ```python
 * if ts.tv_nsec >= NSEC_PER_SEC:
 *     sec = ts.tv_nsec / NSEC_PER_SEC
 *     ts.tv_sec += sec
 *     ts.tv_nsec -= sec * NSEC_PER_SEC
 * elif ts.tv_nsec < 0:
 *      # div_round_up(abs(ts->tv_nsec), NSEC_PER_SEC)
 *	    sec = (NSEC_PER_SEC - ts.tv_nsec - 1) / NSEC_PER_SEC
 *	    ts.tv_sec -= sec;
 *	    ts.tv_nsec += sec * NSEC_PER_SEC;
 * ```
 *
 * @note There are two cases where the normalization can result in integer overflow. These can
 * be extrapolated to not simply overflowing the `tv_sec` field by one second, but also by any
 * realizable multiple of `NSEC_PER_SEC`.
 *
 * 1. When `tv_nsec` is negative and `tv_sec` is already most negative.
 * 2. When `tv_nsec` is greater-or-equal to `NSEC_PER_SEC` and `tv_sec` is already most positive.
 *
 * @note @p ts must be non-`NULL`.
 *
 * @param ts the timespec to be normalized
 *
 * @return `true` if the operation would result in integer overflow, otherwise `false`.
 */
static inline bool timespec_normalize(struct timespec *ts)
{
	__ASSERT_NO_MSG(ts != NULL);

	/*
	 * This implementation is branchless, which improves performance on systems with branch
	 * prediction.
	 */

	int sign = (ts->tv_nsec >= 0) - (ts->tv_nsec < 0);
	int64_t sec = (ts->tv_nsec >= NSEC_PER_SEC) * (ts->tv_nsec / NSEC_PER_SEC) +
		      (ts->tv_nsec < 0) * DIV_ROUND_UP(-ts->tv_nsec, NSEC_PER_SEC);

	return __builtin_add_overflow(ts->tv_sec, sign * sec, &ts->tv_sec) ||
	       __builtin_sub_overflow(ts->tv_nsec, (long)(sign * sec * NSEC_PER_SEC), &ts->tv_nsec);
}

/**
 * @brief Add one timespec to another
 *
 * This function sums the two timespecs pointed to by @p a and @p b and stores the result in the
 * timespce pointed to by @p a.
 *
 * @note @p a and @p b must be non-`NULL`.
 *
 * @param a the timespec which is added to
 * @param b the timespec to be added
 *
 * @return `true` if the operation would result in integer overflow, otherwise `false`.
 */
static inline bool timespec_add(struct timespec *a, const struct timespec *b)
{
	__ASSERT_NO_MSG(a != NULL);
	__ASSERT_NO_MSG(b != NULL);

	return __builtin_add_overflow(a->tv_sec, b->tv_sec, &a->tv_sec) ||
	       __builtin_add_overflow(a->tv_nsec, b->tv_nsec, &a->tv_nsec) || timespec_normalize(a);
}

/**
 * @brief Negate a timespec object
 *
 * Negate the timespec object pointed to be @p ts and store the result in the same
 * memory location.
 *
 * @param ts The timespec object to negate.
 *
 * @return `true` of the operation would overflow, otherwise `false`.
 */
static inline bool timespec_neg(struct timespec *ts)
{
	__ASSERT_NO_MSG(ts != NULL);

	return __builtin_sub_overflow(0LL, ts->tv_sec, &ts->tv_sec) ||
	       __builtin_sub_overflow(0L, ts->tv_nsec, &ts->tv_nsec) || timespec_normalize(ts);
}
#else

static inline bool timespec_normalize(struct timespec *ts)
{
	__ASSERT_NO_MSG(ts != NULL);

	/* This implementation is unoptimized mainly for readability */

	long sec = 0;

	if (ts->tv_nsec >= NSEC_PER_SEC) {
		sec = ts->tv_nsec / NSEC_PER_SEC;
	} else if (ts->tv_nsec < 0) {
		sec = DIV_ROUND_UP(-ts->tv_nsec, NSEC_PER_SEC);
	}

	if (sizeof((ts->tv_sec) == sizeof(int64_t))) {
		if ((ts->tv_nsec < 0) && (ts->tv_sec < 0) && (ts->tv_sec - INT64_MIN < sec)) {
			/*
			 * When `tv_nsec` is negative and `tv_sec` is already most negative,
			 * further subtraction would cause integer overflow.
			 */
			return true;
		}

		if ((ts->tv_nsec >= NSEC_PER_SEC) && (ts->tv_sec > 0) &&
		    (INT64_MAX - ts->tv_sec < sec)) {
			/*
			 * When `tv_nsec` is >= `NSEC_PER_SEC` and `tv_sec` is already most
			 * positive, further addition would cause integer overflow.
			 */
			return true;
		}
	}

	if (ts->tv_nsec >= NSEC_PER_SEC) {
		ts->tv_sec += sec;
		ts->tv_nsec -= sec * NSEC_PER_SEC;
	} else if (ts->tv_nsec < 0) {
		ts->tv_sec -= sec;
		ts->tv_nsec += sec * NSEC_PER_SEC;
	}

	return false;
}

static inline bool timespec_add(struct timespec *a, const struct timespec *b)
{
	__ASSERT_NO_MSG(a != NULL);
	__ASSERT_NO_MSG(b != NULL);

	if ((a->tv_sec < 0) && (b->tv_sec < 0) && (INT64_MIN - a->tv_sec < -b->tv_sec)) {
		/* negative integer overflow would occur */
		return true;
	}

	if ((a->tv_sec > 0) && (b->tv_sec > 0) && (INT64_MAX - a->tv_sec < b->tv_sec)) {
		/* positive integer overflow would occur */
		return true;
	}

	if ((a->tv_nsec < 0) && (b->tv_nsec < 0) && (LONG_MIN - a->tv_nsec < -b->tv_nsec)) {
		/* negative integer overflow would occur */
		return true;
	}

	if ((a->tv_nsec > 0) && (b->tv_nsec > 0) && (LONG_MAX - a->tv_nsec < b->tv_nsec)) {
		/* positive integer overflow would occur */
		return true;
	}

	a->tv_sec += b->tv_sec;
	a->tv_nsec += b->tv_nsec;

	return timespec_normalize(a);
}

static inline bool timespec_neg(struct timespec *ts)
{
	__ASSERT_NO_MSG(ts != NULL);

	if (ts->tv_sec == INT64_MIN) {
		/* -INT64_MIN > INT64_MAX, so +ve integer overflow would occur */
		return true;
	}

	if (ts->tv_nsec == LONG_MIN) {
		/* -LONG_MIN > LONG_MAX, so +ve integer overflow would occur */
		return true;
	}

	ts->tv_sec = -ts->tv_sec;
	ts->tv_nsec = -ts->tv_nsec;

	return timespec_normalize(ts);
}
#endif /* (__STDC_VERSION__ >= 201112L) && HAS_BUILTIN(__builtin_add_overflow) ||                  \
	  defined(__DOXYGEN__) */

/**
 * @brief Subtract one timespec from another
 *
 * This function subtracts the timespec pointed to by @p b from the timespec pointed to by @p b and
 * stores the result in the timespce pointed to by @p a.
 *
 * @note @p a and @p b must be non-`NULL`.
 *
 * @param a the timespec which is subtracted from
 * @param b the timespec to be subtracted
 *
 * @return `true` if the operation would result in integer overflow, otherwise `false`.
 */
static inline bool timespec_sub(struct timespec *a, const struct timespec *b)
{
	__ASSERT_NO_MSG(a != NULL);
	__ASSERT_NO_MSG(b != NULL);

	struct timespec neg = *b;

	return timespec_neg(&neg) || timespec_add(a, &neg);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_SYS_TSUTIL_H_ */
