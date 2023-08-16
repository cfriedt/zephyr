/*
 * Copyright (c) 2023, Emna Rekik
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>

#if defined(__ZEPHYR__) || defined(CONFIG_POSIX_API)

#include <zephyr/kernel.h>

#elif defined(__linux__)

#include <netinet/in.h>

#endif

static uint16_t test_http_service_port = htons(CONFIG_TEST_HTTP_SERVICE_PORT);
HTTP_SERVICE_DEFINE(test_http_service, CONFIG_NET_CONFIG_MY_IPV4_ADDR, &test_http_service_port, 1,
		    10, NULL);

static const uint8_t index_html_gz[] = {
#include "index.html.gz.inc"
};
HTTP_RESOURCE_DEFINE(index_html_gz_resource, test_http_service, "/", index_html_gz);

int main(void)
{
	return http_server_start();
}
