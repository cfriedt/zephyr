#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <zephyr/llext/llext.h>
#include <zephyr/llext/buf_loader.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(hello_linux);

static const uint8_t hello[] = {
#include "hello.inc"
};

int main(void)
{
	int ret;
	struct llext *ext;
	struct llext_load_param param = LLEXT_LOAD_PARAM_DEFAULT;
	struct llext_buf_loader loader = LLEXT_BUF_LOADER(hello, ARRAY_SIZE(hello));

	LOG_INF("sizeof(hello): %zu", sizeof(hello));

	ret = llext_load(&loader.loader, "ext", &ext, &param);
	if (ret < 0) {
		LOG_ERR("%s() failed: %d", "llext_load", ret);
		return ret;
	}

	k_sleep(K_MSEC(100));

	typedef void (*voidfun_t)(void);

	// dirty hack in terms of entry point, obtained with (load_addr + (&_start - &syscall))
	// riscv64-linux-gnu-objdump -D build/hello.elf | less
	voidfun_t start_fn = (voidfun_t)((intptr_t)0x80010580 + (0x10106 - 0x100e8));
	//voidfun_t main_fn = llext_find_sym(&ext->exp_tab, "_start");

	if (start_fn == NULL) {
		LOG_ERR("%s() failed: %d", "llext_find_sym", -1);
		return -1;
	}

	start_fn();

	return llext_unload(&ext);
}
