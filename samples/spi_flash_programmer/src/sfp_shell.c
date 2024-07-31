#include "sfp_shell.h"

#include <zephyr/devicetree.h>
#include <zephyr/drivers/sfp.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

static const struct device *const sfp_dev = DEVICE_DT_GET(DT_NODELABEL(sfp0));

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
	int rc;
	size_t n_settings;
	enum sfp_setting_e e;
	const char *const t = SFP_SETTING_TYPE;
	struct sfp_setting s[_SFP_SETTING_NUM];

	if ((argc == 1) && (argv[1][0] == '?')) {
		/* print the names of each setting */
		for (e = 0; e < _SFP_SETTING_NUM; ++e) {
			shell_print(sh, "%s: %s", sfp_setting_s[e], sfp_setting_desc[e]);
		}

		return 0;
	}

	if (argc > 1) {
		/* dump only the specified settings */
		n_settings = argc - 1;

		for (int i = 1; i < argc; ++i) {
			rc = sfp_str_to_setting(argv[i]);
			if (rc < 0) {
				return rc;
			}

			e = (enum sfp_setting_e)rc;
			s[i - 1].type = e;
		}
	} else {
		/* dump all settings */
		n_settings = ARRAY_SIZE(s);
		for (e = 0; e < ARRAY_SIZE(s); ++e) {
			s[e].type = e;
		}
	}

	rc = sfp_get(sfp_dev, s, n_settings);
	if (rc < 0) {
		shell_error(sh, "%s() failed: %d", "sfp_get", rc);
		return rc;
	}

	for (size_t i = 0; i < n_settings; ++i) {
		e = s[i].type;

		if (!s[i].is_set) {
			shell_print(sh, "%s: <unset>", sfp_setting_s[e]);
			continue;
		}

		switch (t[e]) {
		case 'b':
		case 'u':
			if (e == SFP_SETTING_VIO) {
				shell_print(sh, "%s: %s", sfp_setting_s[e],
					    sfp_vio_s[s[i].value.v_u32]);
			} else {
				shell_print(sh, "%s: %u", sfp_setting_s[e], s[i].value.v_u32);
			}
			break;
		case 's':
			shell_print(sh, "%s: %s", sfp_setting_s[e], s[i].value.v_str);
			break;
		}
	}

	return 0;
}
SFP_CMD_ADD(get, NULL, "get [?|key..]", sfp_cmd_get, 1, 11);

static int sfp_cmd_identify(const struct shell *sh, size_t argc, char **argv)
{
	int rc;

	rc = sfp_identify(sfp_dev);
	if (rc < 0) {
		shell_error(sh, "%s() failed: %d", "sfp_identify", rc);
		return rc;
	}

	return 0;
}
SFP_CMD_ADD(identify, NULL, "identify", sfp_cmd_identify, 0, 0);

static int sfp_cmd_set(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "%s()", __func__);

	return 0;
}
SFP_CMD_ADD(set, NULL, "set [key value ..]", sfp_cmd_set, 0, 20);

SHELL_SUBCMD_SET_CREATE(sfp_cmds, (sfp));
SHELL_CMD_ARG_REGISTER(sfp, &sfp_cmds, "SPI flash programmer commands", NULL, 2, 20);
