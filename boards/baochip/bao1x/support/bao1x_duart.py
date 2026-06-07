# Copyright (c) 2026 Baochip contributors
# SPDX-License-Identifier: Apache-2.0
#
# Minimal DUART model for Renode (TXD @ 0x00, SR @ 0x08).

if request.isInit:
    pass
elif request.isRead:
    request.value = 0 if request.offset == 0x8 else 0
elif request.isWrite:
    if request.offset == 0x0:
        import System
        System.Console.Write(chr(request.value & 0xFF))
