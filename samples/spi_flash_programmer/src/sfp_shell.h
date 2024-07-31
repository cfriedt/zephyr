#pragma once

#include <zephyr/shell/shell.h>

#define SFP_CMD_ADD(_syntax, _subcmd, _help, _handler, _mand, _opt)                                \
	SHELL_SUBCMD_ADD((sfp), _syntax, _subcmd, _help, _handler, _mand, _opt);
