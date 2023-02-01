/*
 * Copyright 2022 Young Mei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/sys/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

enum {
	test_max_sample_size = 65536
};

static unsigned char defl_buf[test_max_sample_size];
static unsigned char infl_buf[test_max_sample_size];

static const char sample[] =
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
	" eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
	" quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo conse quat.  "
	"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat"
	" nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui    "
	"officia deserunt mollit anim id est laborum.";

static z_stream *deflater;
static z_stream *inflater;

static void muzic_test_before(void *data)
{
	ARG_UNUSED(data);
	deflater = (z_stream *)malloc(sizeof(z_stream));
	zassert_not_equal(deflater, NULL, NULL);
	deflater->next_in = Z_NULL;
	deflater->avail_in = 0;
	deflater->total_in = 0;
	deflater->total_out = 0;
	deflater->zalloc = Z_NULL;
	deflater->zfree = Z_NULL;
	deflater->opaque = Z_NULL;
	int res = deflateInit(deflater, Z_DEFAULT_COMPRESSION);

	zassert_equal(res, Z_OK, "deflate initialization failed: %d", res);

	inflater = (z_stream *)malloc(sizeof(z_stream));
	zassert_not_equal(inflater, NULL, NULL);
	inflater->next_in = Z_NULL;
	inflater->avail_in = 0;
	inflater->total_in = 0;
	inflater->total_out = 0;
	inflater->zalloc = Z_NULL;
	inflater->zfree = Z_NULL;
	inflater->opaque = Z_NULL;
	res = inflateInit(inflater);
	zassert_equal(res, Z_OK, "inflate initialization failed: %d", res);

	/*
	 * Zero set them in case error occurred, but output from previous test
	 * causes a false negative
	 */
	memset(defl_buf, 0, sizeof(defl_buf));
	memset(infl_buf, 0, sizeof(infl_buf));
}

static void muzic_test_after(void *data)
{
	ARG_UNUSED(data);
	deflateEnd(deflater);
	free(deflater);
	deflater = NULL;
	inflateEnd(inflater);
	free(inflater);
	inflater = NULL;
}

ZTEST(muzic, test_deflate_finish)
{
	int res;

	deflater->next_in = (unsigned char *)sample;
	deflater->avail_in = sizeof(sample);
	deflater->next_out = defl_buf;
	deflater->avail_out = sizeof(defl_buf);

	res = deflate(deflater, Z_FINISH);
	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(deflater->next_in, (unsigned char *)sample + sizeof(sample), NULL);
	zassert_equal(deflater->avail_in, 0, NULL);
	zassert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf), NULL);
	zassert_equal(deflater->total_out, deflater->next_out - defl_buf, NULL);
	zassert_equal(deflater->total_in, sizeof(sample), NULL);

	inflater->next_in = defl_buf;
	inflater->avail_in = deflater->next_out - defl_buf;
	inflater->next_out = infl_buf;
	inflater->avail_out = sizeof(infl_buf);
	res = inflate(inflater, Z_SYNC_FLUSH);
	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(sample), NULL);
	zassert_equal(inflater->avail_out, sizeof(infl_buf) - sizeof(sample), NULL);
	zassert_equal(inflater->total_out, sizeof(sample), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0, NULL);
}

ZTEST(muzic, test_deflate_full_flush)
{
	int res;

	deflater->next_in = (unsigned char *)sample;
	deflater->avail_in = sizeof(sample);
	deflater->next_out = defl_buf;
	deflater->avail_out = sizeof(defl_buf);

	res = deflate(deflater, Z_FULL_FLUSH);
	zassert_equal(res, Z_OK, NULL);
	zassert_equal(deflater->next_in, (unsigned char *)sample + sizeof(sample), NULL);
	zassert_equal(deflater->avail_in, 0, NULL);
	zassert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf), NULL);
	zassert_equal(deflater->total_out, deflater->next_out - defl_buf, NULL);
	zassert_equal(deflater->total_in, sizeof(sample), NULL);

	inflater->next_in = defl_buf;
	inflater->avail_in = deflater->next_out - defl_buf;
	inflater->next_out = infl_buf;
	inflater->avail_out = sizeof(infl_buf);
	res = inflate(inflater, Z_SYNC_FLUSH);
	zassert_equal(res, Z_OK, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(sample), NULL);
	zassert_equal(inflater->avail_out, sizeof(infl_buf) - sizeof(sample), NULL);
	zassert_equal(inflater->total_out, sizeof(sample), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0, NULL);
}

ZTEST(muzic, test_deflate_full_flush_frag_in)
{
	int res;
	size_t len;
	const size_t step = sizeof(uint32_t);

	deflater->next_in = (unsigned char *)sample;
	deflater->next_out = defl_buf;
	deflater->avail_out = sizeof(defl_buf);

	len = sizeof(sample);
	while (len > step) {
		deflater->avail_in = step;
		int res = deflate(deflater, Z_FULL_FLUSH);

		zassert_equal(res, Z_OK, NULL);
		len -= step;
	}

	if (len != 0) {
		deflater->avail_in = len;
		int res = deflate(deflater, Z_FULL_FLUSH);

		zassert_equal(res, Z_OK, NULL);
	}
	zassert_equal(deflater->next_in, (unsigned char *)sample + sizeof(sample), NULL);
	zassert_equal(deflater->avail_in, 0, NULL);
	zassert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf), NULL);
	zassert_equal(deflater->total_out, deflater->next_out - defl_buf, NULL);
	zassert_equal(deflater->total_in, sizeof(sample), NULL);

	inflater->next_in = defl_buf;
	inflater->avail_in = deflater->next_out - defl_buf;
	inflater->next_out = infl_buf;
	inflater->avail_out = sizeof(infl_buf);
	res = inflate(inflater, Z_SYNC_FLUSH);
	zassert_equal(res, Z_OK, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(sample), NULL);
	zassert_equal(inflater->avail_out, sizeof(infl_buf) - sizeof(sample), NULL);
	zassert_equal(inflater->total_out, sizeof(sample), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0, NULL);
}

ZTEST(muzic, test_deflate_finish_frag_out)
{
	int res;

	deflater->next_in = (unsigned char *)sample;
	deflater->avail_in = sizeof(sample);
	deflater->next_out = defl_buf;
	deflater->avail_out = 4;

	res = deflate(deflater, Z_FINISH);
	zassert_equal(res, Z_BUF_ERROR, NULL);
}

ZTEST(muzic, test_deflate_full_flush_frag_out)
{
	int res;
	const size_t step = sizeof(uint32_t);
	unsigned char defl_buf[4096];

	deflater->next_in = (unsigned char *)sample;
	deflater->avail_in = sizeof(sample);
	deflater->next_out = defl_buf;
	deflater->avail_out = 0;

	while (deflater->avail_out == 0) {
		deflater->avail_out = step;
		int res = deflate(deflater, Z_FULL_FLUSH);

		zassert_equal(res, Z_OK, NULL);
	}

	zassert_equal(deflater->next_in, (unsigned char *)sample + sizeof(sample), NULL);
	zassert_equal(deflater->avail_in, 0, NULL);
	zassert_equal(deflater->total_out, deflater->next_out - defl_buf, NULL);
	zassert_equal(deflater->total_in, sizeof(sample), NULL);

	inflater->next_in = defl_buf;
	inflater->avail_in = deflater->next_out - defl_buf;
	inflater->next_out = infl_buf;
	inflater->avail_out = sizeof(infl_buf);
	res = inflate(inflater, Z_SYNC_FLUSH);
	zassert_equal(res, Z_OK, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(sample), NULL);
	zassert_equal(inflater->avail_out, sizeof(infl_buf) - sizeof(sample), NULL);
	zassert_equal(inflater->total_out, sizeof(sample), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0, NULL);
}

ZTEST(muzic, test_inflate_frag_in)
{
	int res;
	size_t rest;
	const size_t step = 4;

	deflater->next_in = (unsigned char *)sample;
	deflater->avail_in = sizeof(sample);
	deflater->next_out = defl_buf;
	deflater->avail_out = sizeof(defl_buf);

	res = deflate(deflater, Z_FINISH);
	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(deflater->next_in, (unsigned char *)sample + sizeof(sample), NULL);
	zassert_equal(deflater->avail_in, 0, NULL);
	zassert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf), NULL);
	zassert_equal(deflater->total_out, deflater->next_out - defl_buf, NULL);
	zassert_equal(deflater->total_in, sizeof(sample), NULL);

	inflater->next_in = defl_buf;
	inflater->next_out = infl_buf;
	inflater->avail_out = sizeof(infl_buf);

	res = Z_OK;
	rest = deflater->total_out;
	while (res == Z_OK) {
		inflater->avail_in = step < rest ? step : rest;
		res = inflate(inflater, Z_SYNC_FLUSH);
		rest -= step;
	}

	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(sample), NULL);
	zassert_equal(inflater->total_out, sizeof(sample), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0, NULL);
}

ZTEST(muzic, test_inflate_frag_out)
{
	int res;
	size_t rest;
	const size_t step = 4;

	deflater->next_in = (unsigned char *)sample;
	deflater->avail_in = sizeof(sample);
	deflater->next_out = defl_buf;
	deflater->avail_out = sizeof(defl_buf);

	res = deflate(deflater, Z_FINISH);
	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(deflater->next_in, (unsigned char *)sample + sizeof(sample), NULL);
	zassert_equal(deflater->avail_in, 0, NULL);
	zassert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf), NULL);
	zassert_equal(deflater->total_out, deflater->next_out - defl_buf, NULL);
	zassert_equal(deflater->total_in, sizeof(sample), NULL);

	inflater->next_in = defl_buf;
	inflater->avail_in = deflater->next_out - defl_buf;
	inflater->next_out = infl_buf;

	res = Z_OK;
	rest = sizeof(sample);
	while (res == Z_OK) {
		inflater->avail_out = step < rest ? step : rest;
		res = inflate(inflater, Z_SYNC_FLUSH);
		rest -= step;
	}

	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(sample), NULL);
	/* zassert_equal(inflater->avail_out, 1); , NULL*/
	zassert_equal(inflater->total_out, sizeof(sample), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0, NULL);
}

/* Output large enough to make the internal buffer of inflate rewind */
ZTEST(muzic, test_inflate_buf_rewind)
{
	int res;
	size_t len;
	static char largeInput[test_max_sample_size];

	for (len = test_max_sample_size; len > 0; --len) {
		largeInput[len] = '1' + (len & 0b111111);
	}

	deflater->next_in = (unsigned char *)largeInput;
	deflater->avail_in = sizeof(largeInput);
	deflater->next_out = defl_buf;
	deflater->avail_out = sizeof(defl_buf);

	res = deflate(deflater, Z_FINISH);

	inflater->next_in = defl_buf;
	inflater->avail_in = deflater->next_out - defl_buf;
	inflater->next_out = infl_buf;
	inflater->avail_out = sizeof(infl_buf);
	res = inflate(inflater, Z_SYNC_FLUSH);
	zassert_equal(res, Z_STREAM_END, NULL);
	zassert_equal(inflater->next_in, deflater->next_out, NULL);
	zassert_equal(inflater->avail_in, 0, NULL);
	zassert_equal(inflater->next_out, infl_buf + sizeof(largeInput), NULL);
	zassert_equal(inflater->avail_out, 0, NULL);
	zassert_equal(inflater->total_out, sizeof(largeInput), NULL);
	zassert_equal(inflater->total_in, deflater->total_out, NULL);

	zassert_equal(memcmp(infl_buf, largeInput, sizeof(largeInput)), 0, NULL);
}

ZTEST_SUITE(muzic, NULL, NULL, muzic_test_before, muzic_test_after, NULL);
