/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT zephyr_log_uart

#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_core.h>
#include <zephyr/logging/log_output.h>
#include <zephyr/logging/log_output_dict.h>
#include <zephyr/logging/log_backend_std.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
LOG_MODULE_REGISTER(log_uart);

struct lbu_cb_ctx {
	const struct log_output *output;
	struct device *device;
};

struct lbu_data {
	struct k_sem sem;
	uint32_t log_format_current;
	volatile bool in_panic;
	bool use_async;
};

/* Fixed size to avoid auto-added trailing '\0'.
 * Used if CONFIG_LOG_BACKEND_UART_OUTPUT_DICTIONARY_HEX.
 */
static const char LOG_HEX_SEP[10] = "##ZLOGV1##";

static void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
	const struct lbu_cb_ctx *ctx = user_data;
	struct lbu_data *data = ctx->device->data;

	switch (evt->type) {
	case UART_TX_DONE:
		k_sem_give(&data->sem);
		break;
	default:
		break;
	}
}

static void dict_char_out_hex(const struct device *uart_dev, uint8_t *data, size_t length)
{
	for (size_t i = 0; i < length; i++) {
		char c;
		uint8_t x;

		/* upper 8-bit */
		x = data[i] >> 4;
		(void)hex2char(x, &c);
		uart_poll_out(uart_dev, c);

		/* lower 8-bit */
		x = data[i] & 0x0FU;
		(void)hex2char(x, &c);
		uart_poll_out(uart_dev, c);
	}
}

static int char_out(uint8_t *data, size_t length, void *unused, const struct lbu_cb_ctx *ctx)
{
	int err;
	struct lbu_data *lb_data = ctx->device->data;
	const struct device *uart_dev = ctx->device->config;

	ARG_UNUSED(unused);

	if (pm_device_runtime_is_enabled(uart_dev) && !k_is_in_isr()) {
		if (pm_device_runtime_get(uart_dev) < 0) {
			/* Enabling the UART instance has failed but this
			 * function MUST return the number of bytes consumed.
			 */
			return length;
		}
	}

	if (IS_ENABLED(CONFIG_LOG_BACKEND_UART_OUTPUT_DICTIONARY_HEX)) {
		dict_char_out_hex(uart_dev, data, length);
		goto cleanup;
	}

	if (!IS_ENABLED(CONFIG_LOG_BACKEND_UART_ASYNC) || lb_data->in_panic ||
	    !lb_data->use_async) {
		for (size_t i = 0; i < length; i++) {
			uart_poll_out(uart_dev, data[i]);
		}
		goto cleanup;
	}

	err = uart_tx(uart_dev, data, length, SYS_FOREVER_US);
	__ASSERT_NO_MSG(err == 0);

	err = k_sem_take(&lb_data->sem, K_FOREVER);
	__ASSERT_NO_MSG(err == 0);

	(void)err;
cleanup:
	if (pm_device_runtime_is_enabled(uart_dev) && !k_is_in_isr()) {
		/* As errors cannot be returned, ignore the return value */
		(void)pm_device_runtime_put(uart_dev);
	}

	return length;
}

static void process(const struct log_backend *const backend, union log_msg_generic *msg)
{
	const struct lbu_cb_ctx *ctx = backend->cb->ctx;
	struct lbu_data *data = ctx->device->data;

	uint32_t flags = log_backend_std_get_flags();

	log_format_func_t log_output_func = log_format_func_t_get(data->log_format_current);

	log_output_func(ctx->output, &msg->log, flags);
}

static int format_set(const struct log_backend *const backend, uint32_t log_type)
{
	const struct lbu_cb_ctx *ctx = backend->cb->ctx;
	struct lbu_data *data = ctx->device->data;

	data->log_format_current = log_type;
	return 0;
}

static void log_backend_uart_init(struct log_backend const *const backend)
{
	const struct lbu_cb_ctx *ctx = backend->cb->ctx;
	const struct device *uart_dev = ctx->device->config;
	struct lbu_data *data = ctx->device->data;

	__ASSERT_NO_MSG(device_is_ready(uart_dev));

	if (IS_ENABLED(CONFIG_LOG_BACKEND_UART_OUTPUT_DICTIONARY_HEX)) {
		/* Print a separator so the output can be fed into
		 * log parser directly. This is useful when capturing
		 * from UART directly where there might be other output
		 * (e.g. bootloader).
		 */
		for (int i = 0; i < sizeof(LOG_HEX_SEP); i++) {
			uart_poll_out(uart_dev, LOG_HEX_SEP[i]);
		}

		return;
	}

	if (IS_ENABLED(CONFIG_LOG_BACKEND_UART_ASYNC)) {
		int err = uart_callback_set(uart_dev, uart_callback, NULL);

		if (err == 0) {
			data->use_async = true;
			k_sem_init(&data->sem, 0, 1);
		} else {
			LOG_WRN("Failed to initialize asynchronous mode (err:%d). "
				"Fallback to polling.",
				err);
		}
	}
}

static void panic(struct log_backend const *const backend)
{
	const struct lbu_cb_ctx *ctx = backend->cb->ctx;
	struct lbu_data *data = ctx->device->data;
	__unused const struct device *uart_dev = ctx->device->config;

	/* Ensure that the UART device is in active mode */
#if defined(CONFIG_PM_DEVICE_RUNTIME)
	if (pm_device_runtime_is_enabled(uart_dev)) {
		if (k_is_in_isr()) {
			/* pm_device_runtime_get cannot be used from ISRs */
			pm_device_action_run(uart_dev, PM_DEVICE_ACTION_RESUME);
		} else {
			pm_device_runtime_get(uart_dev);
		}
	}
#elif defined(CONFIG_PM_DEVICE)
	enum pm_device_state pm_state;
	int rc;

	rc = pm_device_state_get(uart_dev, &pm_state);
	if ((rc == 0) && (pm_state == PM_DEVICE_STATE_SUSPENDED)) {
		pm_device_action_run(uart_dev, PM_DEVICE_ACTION_RESUME);
	}
#endif /* CONFIG_PM_DEVICE */

	data->in_panic = true;
	log_backend_std_panic(ctx->output);
}

static void dropped(const struct log_backend *const backend, uint32_t cnt)
{
	const struct lbu_cb_ctx *ctx = backend->cb->ctx;

	if (IS_ENABLED(CONFIG_LOG_BACKEND_UART_OUTPUT_DICTIONARY)) {
		log_dict_output_dropped_process(ctx->output, cnt);
	} else {
		log_backend_std_dropped(ctx->output, cnt);
	}
}

const struct log_backend_api log_backend_uart_api = {
	.process = process,
	.panic = panic,
	.init = log_backend_uart_init,
	.dropped = IS_ENABLED(CONFIG_LOG_MODE_IMMEDIATE) ? NULL : dropped,
	.format_set = format_set,
};

#define LBU_BUFFER_SIZE(_n) (CONFIG_LOG_BACKEND_UART_BUFFER_SIZE / sizeof(uintptr_t))
#define LBU_PARENT(_n)      DEVICE_DT_GET(DT_PARENT(DT_DRV_INST(_n)))

#define LBU_DEFINE(_n)                                                                             \
	/* forward declare this wrapper. It needs a handle on lbu_cb_ctx internally */             \
	static int char_out_##_n(uint8_t *data, size_t length, void *unused);                      \
                                                                                                   \
	static uintptr_t lbu_buffer_##_n[LBU_BUFFER_SIZE(_n)];                                     \
	LOG_OUTPUT_DEFINE(lbu_output_##_n, char_out_##_n, (uint8_t *)lbu_buffer_##_n,              \
			  sizeof(lbu_buffer_##_n));                                                \
                                                                                                   \
	static const struct lbu_cb_ctx lbu_cb_ctx_##_n = {                                         \
		.output = &lbu_output_##_n,                                                        \
		.device = (void *)&DEVICE_DT_NAME_GET(DT_DRV_INST(_n)),                            \
	};                                                                                         \
                                                                                                   \
	/* wrapper around the generic char_out() that implicitly knows the lbu_cb_ctx */           \
	static int char_out_##_n(uint8_t *data, size_t length, void *unused)                       \
	{                                                                                          \
		return char_out(data, length, unused, &lbu_cb_ctx_##_n);                           \
	}                                                                                          \
                                                                                                   \
	static struct lbu_data lbu_data_##_n = {                                                   \
		.log_format_current = CONFIG_LOG_BACKEND_UART_OUTPUT_DEFAULT,                      \
	};                                                                                         \
                                                                                                   \
	LOG_BACKEND_DEFINE(lbu_backend_##_n, log_backend_uart_api,                                 \
			   IS_ENABLED(CONFIG_LOG_BACKEND_UART_AUTOSTART),                          \
			   (void *)&lbu_cb_ctx_##_n);                                              \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(_n, NULL, NULL, &lbu_data_##_n, LBU_PARENT(_n), POST_KERNEL, 0, NULL);

DT_INST_FOREACH_STATUS_OKAY(LBU_DEFINE)
