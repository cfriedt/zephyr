#include "sfp_shell.h"

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static int sfp_cmd_burn(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(burn, NULL, "burn [setting..]", sfp_cmd_burn, 0, 10);

static int sfp_cmd_cache(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(cache, NULL, "cache [setting]", sfp_cmd_cache, 0, 10);

static int sfp_cmd_get(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(get, NULL, "get [?|key..]", sfp_cmd_get, 0, 10);

static int sfp_cmd_identify(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(identify, NULL, "identify", sfp_cmd_identify, 0, 0);

static int sfp_cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(reset, NULL, "reset [warm|cold]", sfp_cmd_reset, 0, 1);

static int sfp_cmd_set(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(set, NULL, "set [key value ..]", sfp_cmd_set, 0, 20);

SHELL_SUBCMD_SET_CREATE(sfp_cmds, (sfp));
SHELL_CMD_ARG_REGISTER(sfp, &sfp_cmds, "SPI flash programmer commands", NULL, 2, 20);
