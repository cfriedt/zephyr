#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/http/method.h>
#include <zephyr/net/http/parser.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/http/status.h>

#include "../../http_log.h"

static int ph_init(const struct device *unused)
{
	ARG_UNUSED(unused);

	LOG_INF("Hello, PhD world!");

	return 0;
}

SYS_INIT(ph_init, APPLICATION, CONFIG_HTTP_SERVER_INIT_PRIORITY);
