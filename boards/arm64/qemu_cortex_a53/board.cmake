# Copyright (c) 2019 Carlo Caione <ccaione@baylibre.com>
# SPDX-License-Identifier: Apache-2.0

set(SUPPORTED_EMU_PLATFORMS qemu)
set(QEMU_ARCH aarch64)

set(QEMU_CPU_TYPE_${ARCH} cortex-a53)

if(CONFIG_ARMV8_A_NS)
set(QEMU_MACH virt,gic-version=3)
else()
set(QEMU_MACH virt,secure=on,gic-version=3)
endif()

set(QEMU_FLAGS_${ARCH}
  -cpu ${QEMU_CPU_TYPE_${ARCH}}
  -nographic
  -machine ${QEMU_MACH}
  )

  if(CONFIG_PCIE)
  # PCI test device
  set(QEMU_FLAGS_${ARCH}
  ${QEMU_FLAGS_${ARCH}}
  -device pci-testdev,bus=pcie.0		# 00:01:0
  -device edu,bus=pcie.0,dma_mask=0x1FFFFFFFFFF # 00:02:0
  )
  set(QEMU_FLAGS_${ARCH}
  ${QEMU_FLAGS_${ARCH}}
   # 2 bridges on same DEV id with edu on each of them
  -device ioh3420,id=root_port1,chassis=1,slot=1,multifunction=on,addr=3.0	# 00:03:0
        -device edu,bus=root_port1,dma_mask=0x1FFFFFFFFFF			# 01:00:0

  # on 00:03:0	-> dev 1
        -device ioh3420,id=root_port11,chassis=1,slot=3,bus=root_port1		# 01:01:0
                -device edu,bus=root_port11,dma_mask=0x1FFFFFFFFFF		# 02:00:0
  )
endif()

if(CONFIG_XIP)
  # This should be equivalent to
  #   ... -drive if=pflash,file=build/zephyr/zephyr.bin,format=raw
  # without having to pad the binary file to the FLASH size
  set(QEMU_KERNEL_OPTION
  -bios ${PROJECT_BINARY_DIR}/${CONFIG_KERNEL_BIN_NAME}.bin
  )
endif()

board_set_debugger_ifnset(qemu)
