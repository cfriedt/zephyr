/*
 * Copyright (c) 2023, Emna Rekik
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/net/http/table.h>
#include <zephyr/net/http/server.h>
#include <zephyr/ztest.h>

#define _get_fun(s, k, v)                                                                          \
	((v) == NULL ? http_hpack_table_contains(s, k) : http_hpack_table_get(s, k, v))

struct table_fixture {
};

static void test_get_common(void **value)
{
	struct http_client_ctx server = {};
	int ret;

	/* 00 */
	zassert_equal(-EINVAL, _get_fun(NULL, HTTP_HPACK_INVALID, value));
	/* 01 */
	zassert_equal(-EINVAL, _get_fun(NULL, HTTP_HPACK_AUTHORITY, value));
	/* 10 */
	zassert_equal(-EINVAL, _get_fun(&server, HTTP_HPACK_INVALID, value));

	for (int i = HTTP_HPACK_AUTHORITY; i <= HTTP_HPACK_WWW_AUTHENTICATE; i++) {
		if (value != NULL) {
			*value = NULL;
		}
		ret = _get_fun(&server, i, value);
		zassert_ok(ret, "%s() failed %d",
			   value == NULL ? "http_table_contains" : "http_table_get", ret);
		if (value != NULL) {
			zassert_not_null(*value);
		}
	}
}

ZTEST(table, test_contains)
{
	test_get_common(NULL);
}

ZTEST(table, test_get)
{
	void *value;

	test_get_common(&value);
}

ZTEST(table, test_remove)
{
}

ZTEST(table, test_add)
{
}

void test_header_parsing_path(void)
{
	struct http_client_ctx ctx;
	uint8_t payload[] = {0x84};

	ctx.current_frame.payload = payload;
	ctx.current_frame.length = sizeof(payload);

	char *result = header_parsing(&ctx, HTTP_HPACK_PATH);

	zassert_true(strcmp(result, "/") == 0, "Expected /");
}

void test_header_parsing_method(void)
{
	struct http_client_ctx ctx;
	uint8_t payload[] = {0x82};

	ctx.current_frame.payload = payload;
	ctx.current_frame.length = sizeof(payload);

	char *result = header_parsing(&ctx, HTTP_HPACK_METHOD);

	zassert_true(strcmp(result, "GET") == 0, "Expected GET");
}

ZTEST(table, test_header_parser)
{
	test_header_parsing_method();
	test_header_parsing_path();
}

static void after(void *arg)
{
}

static void before(void *arg)
{
}

ZTEST_SUITE(table, NULL, NULL, before, after, NULL);
