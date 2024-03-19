/*
 * Copyright 2022 Young Mei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TESTS_LIB_THRIFT_THRIFTTEST_SRC_CONTEXT_HPP_
#define TESTS_LIB_THRIFT_THRIFTTEST_SRC_CONTEXT_HPP_

#include <pthread.h>

#include <memory>

#include <thrift/server/TSimpleServer.h>

#include "ThriftTest.h"

using namespace apache::thrift::server;
using namespace thrift::test;

struct ctx {
	enum {
		SERVER,
		CLIENT,
	};

	std::array<int, CLIENT + 1> fds;
	std::unique_ptr<ThriftTestClient> client;
	std::unique_ptr<TServer> server;
	std::thread server_thread;
};

std::unique_ptr<ThriftTestClient> setup_client();

extern ctx context;

#define PRE() \
	std::thread th([]{context.server->serve();}); \
	k_msleep(100); \
	context.client = setup_client()

#define POST() \
	context.server->stop(); \
	th.join()

#endif /* TESTS_LIB_THRIFT_THRIFTTEST_SRC_CONTEXT_HPP_ */
