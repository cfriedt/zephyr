/*
 * Copyright 2022 Young Mei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>

#include <zephyr/ztest.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TFDTransport.h>
#include <thrift/transport/TSSLServerSocket.h>
#include <thrift/transport/TSSLSocket.h>
#include <thrift/transport/TServerSocket.h>

#include "context.hpp"
#include "server.hpp"
#include "thrift/server/TFDServer.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

ctx context;

static const char cert_pem[] = {
#include "qemu_cert.pem.inc"
	'\0'
};
static const char key_pem[] = {
#include "qemu_key.pem.inc"
	'\0'
};

static void *thrift_test_setup(void)
{
	if (IS_ENABLED(CONFIG_THRIFT_SSL_SOCKET)) {
		TSSLSocketFactory socketFactory;
		socketFactory.loadCertificateFromBuffer((const char *)&cert_pem[0]);
		socketFactory.loadPrivateKeyFromBuffer((const char *)&key_pem[0]);
		socketFactory.loadTrustedCertificatesFromBuffer((const char *)&cert_pem[0]);
	}

	return NULL;
}

std::unique_ptr<ThriftTestClient> setup_client()
{
	std::shared_ptr<TTransport> transport;
	std::shared_ptr<TProtocol> protocol;
	std::shared_ptr<TTransport> trans(new TFDTransport(context.fds[ctx::CLIENT]));

	if (IS_ENABLED(CONFIG_THRIFT_SSL_SOCKET)) {
		const int port = 4242;
		std::shared_ptr<TSSLSocketFactory> socketFactory =
			std::make_shared<TSSLSocketFactory>();
		socketFactory->authenticate(true);
		trans = socketFactory->createSocket(CONFIG_NET_CONFIG_MY_IPV4_ADDR, port);
	} else {
		trans = std::make_shared<TFDTransport>(context.fds[ctx::CLIENT]);
	}

	transport = std::make_shared<TBufferedTransport>(trans);

	if (IS_ENABLED(CONFIG_THRIFT_COMPACT_PROTOCOL)) {
		protocol = std::make_shared<TCompactProtocol>(transport);
	} else {
		protocol = std::make_shared<TBinaryProtocol>(transport);
	}
	transport->open();
	return std::unique_ptr<ThriftTestClient>(new ThriftTestClient(protocol));
}

static std::unique_ptr<TServer> setup_server()
{
	std::shared_ptr<TestHandler> handler(new TestHandler());
	std::shared_ptr<TProcessor> processor(new ThriftTestProcessor(handler));
	std::shared_ptr<TServerTransport> serverTransport;
	std::shared_ptr<TProtocolFactory> protocolFactory;
	std::shared_ptr<TTransportFactory> transportFactory;

	if (IS_ENABLED(CONFIG_THRIFT_SSL_SOCKET)) {
		const int port = 4242;
		std::shared_ptr<TSSLSocketFactory> socketFactory(new TSSLSocketFactory());
		socketFactory->server(true);
		serverTransport =
			std::make_shared<TSSLServerSocket>("0.0.0.0", port, socketFactory);
	} else {
		serverTransport = std::make_shared<TFDServer>(context.fds[ctx::SERVER]);
	}

	transportFactory = std::make_shared<TBufferedTransportFactory>();

	if (IS_ENABLED(CONFIG_THRIFT_COMPACT_PROTOCOL)) {
		protocolFactory = std::make_shared<TCompactProtocolFactory>();
	} else {
		protocolFactory = std::make_shared<TBinaryProtocolFactory>();
	}
	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	return std::unique_ptr<TServer>(
		new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory));
}

static void thrift_test_before(void *data)
{
	ARG_UNUSED(data);
	int rv;

	// create the communication channel
	printf("creating socketpair\n");
	rv = socketpair(AF_UNIX, SOCK_STREAM, 0, &context.fds.front());
	zassert_equal(0, rv, "socketpair failed: %d\n", rv);

	// set up server
	printf("setting up server..\n");
	context.server = setup_server();
	printf("finished setting up server\n");
}

static void thrift_test_after(void *data)
{
	ARG_UNUSED(data);

	for (auto &fd : context.fds) {
		printf("closing fd %d\n", fd);
		close(fd);
		fd = -1;
	}

	// try {
	// printf("resetting client..\n");
	// context.client.reset();
	// printf("resetting server..\n");
	// context.server.reset();
	// } catch (std::exception &e) {
	// 	printf("failed: %s\n", e.what());
	// }

	// k_msleep(CONFIG_NET_TCP_TIME_WAIT_DELAY);
}

ZTEST_SUITE(thrift, NULL, thrift_test_setup, thrift_test_before, thrift_test_after, NULL);
