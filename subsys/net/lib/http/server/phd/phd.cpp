#include <zephyr/kernel.h>
#include <zephyr/log/logging.h>
#include <zephyr/net/http/method.h>
#include <zephyr/net/http/parser.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/http/status.h>

LOG_MODULE_REGISTER(phd, CONFIG_NET_HTTP_SERVER_LOG_LEVEL);

static int ph_init(const struct device *unused)
{
	ARG_UNUSED(unused);

	LOG_INF("Hello, PhD world!");
}

SYS_INIT(ph_init, POST_KERNEL, CONFIG_HTTP_SERVER_INIT_PRIORITY);
