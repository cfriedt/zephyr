#define DT_DRV_COMPAT zephyr_spi_flash_programmer

#include <stdint.h>

#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/flash/jesd216.h>
#include <zephyr/drivers/flash/spi_nor.h>
#include <zephyr/drivers/sfp.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/flash/jesd216.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/si.h>

/* TODO: create a Kconfig */
#define SFP_SFDP_SIZE 5

/* clang-format off */
const char *const sfp_vio_s[] = {
    [SFP_VIO_1p8V] = "1.8 V",
    [SFP_VIO_3p3V] = "3.3 V",
};
/* clang-format on */

/* clang-format off */
const char *const sfp_setting_s[] = {
    [SFP_SETTING_CACHE] = "cache",
    [SFP_SETTING_CPHA] = "cpha",
    [SFP_SETTING_CPOL] = "cpol",
    [SFP_SETTING_CSPOL] = "cspol",
    [SFP_SETTING_FREQ] = "freq",
    [SFP_SETTING_MAX_FREQ] = "max_freq",
    [SFP_SETTING_MIN_FREQ] = "min_freq",
    [SFP_SETTING_SFDP] = "sfdp",
    [SFP_SETTING_VIO] = "vio",
};
/* clang-format on */

/* clang-format off */
const char *const sfp_cmd_s[] = {
	[SFP_CMD_BURN] = "burn",
	[SFP_CMD_CACHE] = "cache",
	[SFP_CMD_GET] = "get",
	[SFP_CMD_HELP] = "help",
	[SFP_CMD_IDENTIFY] = "identify",
	[SFP_CMD_RESET] = "reset",
	[SFP_CMD_SET] = "set",
};
/* clang-format on */

struct sfp_impl_config {
	struct sfp_config_common common;
	const struct device *spi_dev;
	const struct device *spi_nor_dev;
	bool vio[_SFP_VIO_NUM];
};

struct sfp_impl_data {
	/* TODO: actually lock this mutex sometimes */
	struct k_mutex lock;
	struct sfp_setting settings[_SFP_SETTING_NUM];
};

static inline void sfp_impl_set_bool(const struct device *dev, enum sfp_setting_e setting, bool val)
{
	struct sfp_impl_data *data = dev->data;

	data->settings[setting].value.v_bool = val;
	data->settings[setting].is_set = true;
}

static inline void sfp_impl_set_u32(const struct device *dev, enum sfp_setting_e setting,
				    uint32_t val)
{
	struct sfp_impl_data *data = dev->data;

	data->settings[setting].value.v_u32 = val;
	data->settings[setting].is_set = true;
}

static inline void sfp_impl_set_str(const struct device *dev, enum sfp_setting_e setting,
				    const char *val)
{
	struct sfp_impl_data *data = dev->data;

	strncpy(data->settings[setting].value.v_str, val,
		sizeof(data->settings[setting].value.v_str));
	data->settings[setting].is_set = true;
}

static inline bool sfp_impl_get_bool(const struct device *dev, enum sfp_setting_e setting)
{
	struct sfp_impl_data *data = dev->data;

	return data->settings[setting].value.v_bool;
}

static inline uint32_t sfp_impl_get_u32(const struct device *dev, enum sfp_setting_e setting)
{
	struct sfp_impl_data *data = dev->data;

	return data->settings[setting].value.v_u32;
}

static uint32_t sfp_parse_jesd216_size(const struct device *dev,
				       const struct jesd216_sfdp_header *hp)
{
	uint32_t ret = 0;

	for (const struct jesd216_param_header *it = hp->phdr,
					       *end = it + MIN(SFP_SFDP_SIZE, 1 + hp->nph);
	     it < end; ++it) {
		uint16_t id = jesd216_param_id(it);
		uint32_t addr = jesd216_param_addr(it);

		/* FIXME: VLAs are bad. This should be a max-size type array */
		uint32_t dw[it->len_dw];

		if (flash_sfdp_read(dev, addr, dw, sizeof(dw)) < 0) {
			break;
		}

		if (id != JESD216_SFDP_PARAM_ID_BFP) {
			continue;
		}

		ret = (uint32_t)jesd216_bfp_density((struct jesd216_bfp *)dw);
		break;
	}

	return ret;
}

static int sfp_impl_identify(const struct device *dev)
{
	int rc;
	struct jedec_dt_spec jedec_dt;
	uint8_t jesd216_buf[JESD216_SFDP_SIZE(SFP_SFDP_SIZE)];
	const struct jesd216_sfdp_header *const sfdp_hdr =
		(const struct jesd216_sfdp_header *)jesd216_buf;
	const struct sfp_impl_config *config = dev->config;
	struct spi_dt_spec _spi_dt = {
		.bus = config->spi_dev,
		.config =
			{
				.frequency = sfp_sck_min(dev),
				.operation = SPI_WORD_SET(8) | SPI_LOCK_ON,
			},
	};
	struct spi_dt_spec *const spi_dt = &_spi_dt;

	for (int v = 0; v < _SFP_VIO_NUM; ++v) {
		if (!config->vio[v]) {
			continue;
		}

		/* TODO: switch to 'v' I/O voltage here */

		for (int cpha = (int)true; cpha >= (int)false; --cpha) {
			for (int cpol = (int)true; cpol >= (int)false; --cpol) {
				for (int cspol = (int)true; cspol >= (int)false; --cspol) {
					if ((bool)cpha) {
						spi_dt->config.operation |= SPI_MODE_CPHA;
					} else {
						spi_dt->config.operation &= ~SPI_MODE_CPHA;
					}

					if ((bool)cpol) {
						spi_dt->config.operation |= SPI_MODE_CPOL;
					} else {
						spi_dt->config.operation &= ~SPI_MODE_CPOL;
					}

					/*
					 * This one is backwards - also if controller does not
					 * support inverting CS, then it's necessary to use a GPIO,
					 * so that property should be set in the associated
					 * spi_cs_control.
					 */
					if ((bool)cspol) {
						spi_dt->config.operation &= ~SPI_CS_ACTIVE_HIGH;
					} else {
						spi_dt->config.operation |= SPI_CS_ACTIVE_HIGH;
					}

					rc = spi_nor_set_spi_dt_spec(config->spi_nor_dev, spi_dt);
					if (rc < 0) {
						printk("%s() failed: %d", "spi_nor_set_probe_data",
						       rc);
						return rc;
					}

					rc = flash_read_jedec_id(config->spi_nor_dev, jedec_dt.id);
					if (rc < 0) {
						continue;
					}

					rc = flash_sfdp_read(config->spi_nor_dev, 0, jesd216_buf,
							     sizeof(jesd216_buf));
					if ((rc < 0) ||
					    (JESD216_SFDP_MAGIC != jesd216_sfdp_magic(sfdp_hdr))) {
						continue;
					}

					jedec_dt.size = sfp_parse_jesd216_size(config->spi_nor_dev,
									       sfdp_hdr);
					rc = spi_nor_set_jedec_dt_spec(config->spi_nor_dev,
								       &jedec_dt);
					if (rc < 0) {
						return rc;
					}

					/* discovered a JESD216-compatible device */

					/* save parameters for subsequent use */
					sfp_impl_set_bool(dev, SFP_SETTING_VIO, v);
					sfp_impl_set_u32(dev, SFP_SETTING_SIZE, jedec_dt.size >> 3);
					sfp_impl_set_bool(dev, SFP_SETTING_CPHA, cpha);
					sfp_impl_set_bool(dev, SFP_SETTING_CPOL, cpol);
					sfp_impl_set_bool(dev, SFP_SETTING_CSPOL, cspol);
					sfp_impl_set_u32(dev, SFP_SETTING_FREQ, sfp_sck_min(dev));

					goto found_device;
				}
			}
		}
	}

	return -ENODEV;

found_device:
	/*
	 * For practicality reasons, cannot use stlib.h bsearch() since an array of all frequencies
	 * between e.g. 1 and 32M is too large for memory-constrained devices.
	 */

	{
		int i, step, N;

		N = sfp_sck_max(dev) - sfp_sck_min(dev) + 1;
		for (step = 1; step < N; step <<= 1) {
			/* nothing here */
		}
		for (i = 0; step; step >>= 1) {
			if (i + step < N) {
				spi_dt->config.frequency = sfp_sck_min(dev) + i + step;

				for (int j = 0; j < CONFIG_SFP_IDENTIFY_REPS; ++j) {
					uint8_t tmp_id[3];
					rc = flash_read_jedec_id(config->spi_nor_dev, tmp_id);
					if ((rc < 0) || (memcmp(jedec_dt.id, tmp_id,
								sizeof(jedec_dt.id))) != 0) {
						goto next_step;
					}
				}

				i += step;
				/* save parameters for subsequent use */
				sfp_impl_set_u32(dev, SFP_SETTING_FREQ, spi_dt->config.frequency);
			}
		next_step:
			continue;
		}
	}

	printk("Found [%02x, %02x, %02x], %u %sbit, vio: %s, cpol:%u, cpha:%u, cspol:%u, sck: "
	       "%u %sHz\n",
	       jedec_dt.id[0], jedec_dt.id[1], jedec_dt.id[2],
	       DIV_ROUND_UP(sfp_impl_get_u32(dev, SFP_SETTING_SIZE) << 3,
			    1 << si_base2_shift(sfp_impl_get_u32(dev, SFP_SETTING_SIZE) << 3)),
	       /* looks weird printing Mibit instead of Mbit here */
	       si_prefix(sfp_impl_get_u32(dev, SFP_SETTING_SIZE)),
	       sfp_vio_s[sfp_impl_get_u32(dev, SFP_SETTING_VIO)],
	       sfp_impl_get_bool(dev, SFP_SETTING_CPOL), sfp_impl_get_bool(dev, SFP_SETTING_CPHA),
	       sfp_impl_get_bool(dev, SFP_SETTING_CSPOL),
	       DIV_ROUND_UP(sfp_impl_get_u32(dev, SFP_SETTING_FREQ),
			    si_div(sfp_impl_get_u32(dev, SFP_SETTING_FREQ))),
	       si_prefix(sfp_impl_get_u32(dev, SFP_SETTING_FREQ)));

	return 0;
}

static const struct sfp_driver_api sfp_impl_driver = {
	// .get = sfp_impl_get,
	// .set = sfp_impl_set,
	// .unset = sfp_impl_unset,
	.identify = sfp_impl_identify,
};

static int sfp_impl_init(const struct device *dev)
{
	struct sfp_impl_data *data = dev->data;

	return k_mutex_init(&data->lock);
}

#define DEFINE_SFP(_n)                                                                             \
                                                                                                   \
	BUILD_ASSERT(DT_INST_PROP(_n, min_transceiver_sck_frequency) > 0);                         \
	BUILD_ASSERT(DT_INST_PROP(_n, max_transceiver_sck_frequency) >=                            \
		     DT_INST_PROP(_n, min_transceiver_sck_frequency));                             \
	BUILD_ASSERT(DT_INST_PROP(_n, max_transceiver_sck_frequency) < INT32_MAX);                 \
                                                                                                   \
	static const struct sfp_impl_config sfp_impl_config_##_n = {                               \
		.common =                                                                          \
			{                                                                          \
				.freq_min = DT_INST_PROP(_n, min_transceiver_sck_frequency),       \
				.freq_max = DT_INST_PROP(_n, max_transceiver_sck_frequency),       \
			},                                                                         \
		.spi_dev = DEVICE_DT_GET(DT_INST_PHANDLE(_n, spi_dev)),                            \
		.spi_nor_dev = DEVICE_DT_GET(DT_INST_PHANDLE(_n, spi_nor_dev)),                    \
		.vio =                                                                             \
			{                                                                          \
				[SFP_VIO_1p8V] = DT_INST_PROP(_n, vio_1p8_volts),                  \
				[SFP_VIO_3p3V] = DT_INST_PROP(_n, vio_3p3_volts),                  \
			},                                                                         \
	};                                                                                         \
                                                                                                   \
	static struct sfp_impl_data sfp_impl_data_##_n;                                            \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(_n, sfp_impl_init, NULL, &sfp_impl_data_##_n, &sfp_impl_config_##_n, \
			      POST_KERNEL, 99, &sfp_impl_driver);

DT_INST_FOREACH_STATUS_OKAY(DEFINE_SFP)
