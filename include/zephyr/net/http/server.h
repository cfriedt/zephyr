/*
 * Copyright (c) 2023, Emna Rekik
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_NET_HTTP_SERVER_H_
#define ZEPHYR_INCLUDE_NET_HTTP_SERVER_H_

#include <stdint.h>

#if !defined(__ZEPHYR__) || defined(CONFIG_POSIX_API)

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>

#else

#include <zephyr/posix/poll.h>

#endif

#if !defined(__ZEPHYR__)

#define CONFIG_NET_HTTP_MAX_CLIENTS        10
#define CONFIG_NET_HTTP_MAX_STREAMS        100
#define CONFIG_NET_HTTP_CLIENT_BUFFER_SIZE 256

#endif

#define CLIENT_BUFFER_SIZE CONFIG_NET_HTTP_CLIENT_BUFFER_SIZE
#define MAX_CLIENTS        CONFIG_NET_HTTP_MAX_CLIENTS
#define MAX_STREAMS        CONFIG_NET_HTTP_MAX_STREAMS

struct http_server_config {
	int port;
	sa_family_t address_family;
};

enum http_stream_state {
	HTTP_STREAM_IDLE,
	HTTP_STREAM_RESERVED_LOCAL,
	HTTP_STREAM_RESERVED_REMOTE,
	HTTP_STREAM_OPEN,
	HTTP_STREAM_HALF_CLOSED_LOCAL,
	HTTP_STREAM_HALF_CLOSED_REMOTE,
	HTTP_STREAM_CLOSED
};

enum http_server_state {
	HTTP_FRAME_HEADER_STATE,
	HTTP_PREFACE_STATE,
	HTTP_REQUEST_STATE,
	HTTP_FRAME_DATA_STATE,
	HTTP_FRAME_HEADERS_STATE,
	HTTP_FRAME_SETTINGS_STATE,
	HTTP_FRAME_PRIORITY_STATE,
	HTTP_FRAME_WINDOW_UPDATE_STATE,
	HTTP_FRAME_CONTINUATION_STATE,
	HTTP_FRAME_PING_STATE,
	HTTP_FRAME_RST_STREAM_STATE,
	HTTP_FRAME_GOAWAY_STATE,
	HTTP_DONE_STATE,
};

struct http_stream_ctx {
	int stream_id;
	enum http_stream_state stream_state;
};

struct http_frame {
	uint32_t length;
	uint32_t stream_identifier;
	uint8_t type;
	uint8_t flags;
	uint8_t *payload;
};

struct http_client_ctx {
	int client_fd;
	int offset;
	unsigned char buffer[CLIENT_BUFFER_SIZE];
	enum http_server_state server_state;
	struct http_frame current_frame;
	int bytes_parsed_in_current_frame;
	struct http_stream_ctx streams[MAX_STREAMS];
};

struct http_server_ctx {
	int server_fd;
	int event_fd;
	size_t num_clients;
	int infinite;
	struct http_server_config config;
	struct pollfd fds[MAX_CLIENTS + 2];
	struct http_client_ctx clients[MAX_CLIENTS];
};

/* Starts the HTTP2 server */
int http_server_start(void);

/* Stops the HTTP2 server */
int http_server_stop(void);

#endif
