#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/flash/jesd216.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI Flash Programmer (SFP) API
 * @defgroup sfp_api SPI Flash Programmer API
 * @{
 */

/** @brief SPI flash programmer I/O voltage enumerations. */
enum sfp_vio_e {
	SFP_VIO_1p8V, /**< 1.8V I/O voltage */
	SFP_VIO_3p3V, /**< 3.3V I/O voltage */
	_SFP_VIO_NUM,
};

/** @brief SPI flash programmer setting enumerations. */
enum sfp_setting_e {
	SFP_SETTING_CACHE,    /**< cache an image name for the 'burn' command */
	SFP_SETTING_CPHA,     /**< SPI clock phase setting (0 := 1st clock, 1 := 2nd clock) */
	SFP_SETTING_CPOL,     /**< SPI clock idle state (0 := logic low, 1 := logic high) */
	SFP_SETTING_CSPOL,    /**< SPI chip select idle state (0 := logic low, 1 := logic high) */
	SFP_SETTING_FREQ,     /**< SPI clock (SCK) frequency, in Hz */
	SFP_SETTING_MAX_FREQ, /**< Minimum SCK frequency supported by the flash device, in Hz */
	SFP_SETTING_MIN_FREQ, /**< Maximum SCK frequency supported by the flash device, in Hz */
	SFP_SETTING_SIZE,     /**< Size of the flash device, in bits */
	SFP_SETTING_SECT_SIZE, /**< Erase sector size of the flash device, in bytes */
	SFP_SETTING_VIO,      /**< I/O voltage supported by the programmer (see @ref sfp_vio_e) */
	_SFP_SETTING_NUM,
};

#define SFP_SETTING_TYPE "sbbbuuuuuu"

/* clang-format off */
/** @brief Bitmap of ready-only settings for the SPI flash programmer. */
#define SFP_SETTING_RO  (0 \
    | (false * BIT(SFP_SETTING_CACHE)) \
    | (false * BIT(SFP_SETTING_CPOL)) \
    | (false * BIT(SFP_SETTING_CPHA)) \
    | (false * BIT(SFP_SETTING_CSPOL)) \
    | (false * BIT(SFP_SETTING_FREQ)) \
    | (false * BIT(SFP_SETTING_MAX_FREQ)) \
    | (false * BIT(SFP_SETTING_MIN_FREQ)) \
    | (true * BIT(SFP_SETTING_SIZE)) \
    | (true * BIT(SFP_SETTING_SECT_SIZE)) \
    | (false * BIT(SFP_SETTING_VIO)) \
    | 0)
/* clang-format on */

/** @brief SPI flash programmer command enumerations. */
enum sfp_cmd_e {
	SFP_CMD_BURN,     /**< Burn a SPI flash device with a given image */
	SFP_CMD_CACHE,    /**< Cache an image name to simplify calls to 'burn' */
	SFP_CMD_GET,      /**< Get a SPI flash programmer setting */
	SFP_CMD_HELP,     /**< Print help */
	SFP_CMD_IDENTIFY, /**< Identify a JESD216 compatible SPI flash device */
	SFP_CMD_RESET,    /**< Reset the system, all settings, and tristate all I/O lines */
	SFP_CMD_SET,      /**< Set a SPI flash programmer setting */
	_SFP_CMD_NUM,
};

/** @brief SPI flash programmer command strings */
extern const char *const sfp_cmd_s[_SFP_CMD_NUM];
/** @brief SPI flash programmer setting strings */
extern const char *const sfp_setting_s[_SFP_SETTING_NUM];

/**
 * @brief Determin if a SPI flash programmer setting is read-only.
 *
 * @param setting The SPI flash programmer setting to check
 * @return true if the setting is valid and is read-only, otherwise false.
 */
static inline bool sfp_is_setting_ro(enum sfp_setting_e setting)
{
	return (setting >= 0) && (setting < _SFP_SETTING_NUM) &&
	       (SFP_SETTING_RO & BIT(setting)) != 0;
}

union sfp_setting_u {
	uint32_t v_u32;
	char v_str[16];
	bool v_bool;
};

struct sfp_setting {
	enum sfp_setting_e type : LOG2CEIL(_SFP_SETTING_NUM);
	union sfp_setting_u value;
	bool is_set;
};

/*
 * All implementations must have this object as the first item in the driver config struct
 * It is aliased with the driver's config struct in the functions below.
 */
struct sfp_config_common {
	uint32_t freq_min;
	uint32_t freq_max;
};

struct sfp_data_common {
	struct k_mutex lock;
	struct sfp_setting settings[_SFP_SETTING_NUM];
};

static inline uint32_t sfp_sck_min(const struct device *dev)
{
	const struct sfp_config_common *cfg = dev->config;

	return cfg->freq_min;
}

static inline uint32_t sfp_sck_max(const struct device *dev)
{
	const struct sfp_config_common *cfg = dev->config;

	return cfg->freq_max;
}

typedef int (*sfp_api_get_t)(const struct device *dev, struct sfp_setting *setting, size_t n);
typedef int (*sfp_api_set_t)(const struct device *dev, const struct sfp_setting *setting, size_t n);
typedef int (*sfp_api_unset_t)(const struct device *dev, enum sfp_setting_e *setting, size_t n);
typedef int (*sfp_api_identify_t)(const struct device *dev);
/*
 * TODO: populate and use these helpful output parameters with sfp_identify()
 *
 * struct spi_dt_spec *spec
 * struct jesd216_param_header *phdr
 * struct jesd216_sfdp_header *sfdp, struct jesd216_bfp *bfp
 */

__subsystem struct sfp_driver_api {
	sfp_api_get_t get;
	sfp_api_identify_t identify;
	sfp_api_set_t set;
	sfp_api_unset_t unset;
};

static inline int sfp_get(const struct device *dev, struct sfp_setting *setting, size_t n)
{
	const struct sfp_driver_api *api = dev->api;

	return api->get(dev, setting, n);
}

static inline int sfp_set(const struct device *dev, const struct sfp_setting *setting, size_t n)
{
	const struct sfp_driver_api *api = dev->api;

	return api->set(dev, setting, n);
}

static inline int sfp_unset(const struct device *dev, enum sfp_setting_e *setting, size_t n)
{
	const struct sfp_driver_api *api = dev->api;

	return api->unset(dev, setting, n);
}

/**
 * @brief Identify a serial flash peripheral.
 *
 * This function attempts to identify a SPI flash device connected to a given SPI flash
 * programmer, @a dev.
 *
 * The algorithm has two phases:
 *
 * Phase I:
 * - uses *only* the minimum SPI clock frequency
 * - attempts to read the Serial Flash Discoverable Parameters (SFDP)
 * - permutes other SPI parameters such as sck polarity, phase, cs polarity
 * - starts at 1.8 V for I/O, and then tries gradually higher voltages
 * - only attempts to use an I/O voltage setting if it is supported by the device
 * - on success, populates SPI parameters used and proceeds to Phase II
 *
 * Phase II:
 * - find the highest SPI clock frequency supported by the device
 * - uses the previously discovered SPI parameters (sck polarity, phase, cs polarity,
 *   I/O voltage)
 * - performs a binary search between sck_freq_min and sck_freq_max to find
 *   the highest sck frequency supported by the device that works reliably.
 *
 * @param dev The SPI flash programmer device pointer.
 *
 * @retval 0 on success.
 * @retval -EINVAL when an invalid parameter has been provided.
 * @retval -EIO if an I/O error has occurred.
 * @retval -ENODEV when no device has been detected.
 */
static inline int sfp_identify(const struct device *dev)
{
	const struct sfp_driver_api *api = dev->api;

	return api->identify(dev);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// #include <zephyr/syscalls/sfp.h>
