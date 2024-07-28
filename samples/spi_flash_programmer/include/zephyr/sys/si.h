#pragma once

#include <stdint.h>

static inline const char *const si_prefix(uint32_t val)
{
	if (val < 1000) {
		return "";
	}

	if (val < 1000 * 1000) {
		return "k";
	}

	if (val < 1000 * 1000 * 1000) {
		return "M";
	}

	return "G";
}

static inline const char *const si_base2_prefix(uint32_t val)
{
	if (val < 1024) {
		return "";
	}

	if (val < 1024 * 1024) {
		return "ki";
	}

	if (val < 1024 * 1024 * 1024) {
		return "Mi";
	}

	return "Gi";
}

static inline int si_base2_shift(uint32_t val)
{
	if (val < 1024) {
		return 0;
	}

	if (val < 1024 * 1024) {
		return 10;
	}

	if (val < 1024 * 1024 * 1024) {
		return 20;
	}

	return 30;
}

static inline int si_div(uint32_t val)
{
	if (val < 1000) {
		return 1;
	}

	if (val < 1000 * 1000) {
		return 1000;
	}

	if (val < 1000 * 1000 * 1000) {
		return 1000 * 1000;
	}

	return 1000 * 1000 * 1000;
}
