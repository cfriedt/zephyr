/*
 * Copyright (c) 2023, Emna Rekik
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_NET_HTTP_FRAME_H_
#define ZEPHYR_INCLUDE_NET_HTTP_FRAME_H_

enum http_frame_type {
	HTTP_DATA_FRAME = 0x00,
	HTTP_HEADERS_FRAME = 0x01,
	HTTP_PRIORITY_FRAME = 0x02,
	HTTP_RST_STREAM_FRAME = 0x03,
	HTTP_SETTINGS_FRAME = 0x04,
	HTTP_PUSH_PROMISE_FRAME = 0x05,
	HTTP_PING_FRAME = 0x06,
	HTTP_GOAWAY_FRAME = 0x07,
	HTTP_WINDOW_UPDATE_FRAME = 0x08,
	HTTP_CONTINUATION_FRAME = 0x09
};

#define HTTP_HPACK_METHOD 0
#define HTTP_HPACK_PATH   1

#define HTTP_HPACK_STATUS_2OO 0x88
#define HTTP_HPACK_STATUS_4O4 0x8D

#define HTTP_FLAG_SETTINGS_ACK 0x1
#define HTTP_FLAG_END_HEADERS  0x4
#define HTTP_FLAG_END_STREAM   0x1

#define HTTP_FRAME_HEADER_SIZE      9
#define HTTP_FRAME_LENGTH_OFFSET    0
#define HTTP_FRAME_TYPE_OFFSET      3
#define HTTP_FRAME_FLAGS_OFFSET     4
#define HTTP_FRAME_STREAM_ID_OFFSET 5

#endif
