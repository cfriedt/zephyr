# Copyright (c) 2026 Baochip contributors
# SPDX-License-Identifier: Apache-2.0

set(SUPPORTED_EMU_PLATFORMS renode)
set(RENODE ${CMAKE_CURRENT_LIST_DIR}/support/renode.sh CACHE FILEPATH "Renode launcher")
set(RENODE_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/support/bao1x_renode.resc)
set(RENODE_UART sysbus.duart)

include(${ZEPHYR_BASE}/boards/common/renode.board.cmake)
