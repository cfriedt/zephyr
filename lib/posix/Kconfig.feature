# Copyright (c) 2023, Meta
#
# SPDX-License-Identifier: Apache-2.0

config NEED_POSIX_C_SOURCE
	bool
	# Note: Picolibc is only defaulted here temporarily so that tests/lib/c_lib/common passes
	default y if PICOLIBC
	# Note: Newlib is only defaulted here temporarily so that tests/lib/c_lib/common passes
	default y if NEWLIB_LIBC
	help
	  Platforms can select this at the SoC level if they require the POSIX API to be visible
	  during compilation (i.e. require _POSIX_C_SOURCE=200809). This option should only be used
	  in a very limited number of circumstances as it then exposes the entire application to the
	  POSIX API which may not be required or intended.

	  Normally, for the application, _POSIX_C_SOURCE=200809 is automatically declared when
	  required by a POSIX Kconfig option.

	  This option is not user-selectable.

config NEED_XOPEN_SOURCE
	bool
	help
	  Platforms can select this at the SoC level if they require the X/Open API to be visible
	  during compilation (i.e. require _XOPEN_SOURCE=700). This option should only be used in a
	  very limited number of circumstances as it then exposes the entire application to the POSIX
	  API which may not be required or intended.

	  Normally, for the application, _XOPEN_SOURCE=700 is automatically declared whene required by
	  a POSIX Kconfig option.

	  This option is not user-selectable.

config NEED_XOPEN_SOURCE_EXTENDED
	bool
	depends on NEED_XOPEN_SOURCE
	help
	  Platforms can select this in Kconfig.soc if they require the X/Open Extended API to be
	  visible during compilation (i.e. require _XOPEN_SOURCE_EXTENDED). This option should only be
	  used in a very limited number of circumstances as it then exposes the entire application to
	  the POSIX API which may not be required or intended.

	  Normally, for the application, _XOPEN_SOURCE_EXTENDED is automatically declared whenever
	  required by a POSIX Kconfig option.

	  This option is not user-selectable.
