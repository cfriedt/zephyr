#!/bin/sh
# Copyright (c) 2026 Baochip contributors
# SPDX-License-Identifier: Apache-2.0
#
# Launch Renode from the macOS application bundle via Mono.

RENODE_ROOT="${RENODE_ROOT:-/Applications/Renode.app/Contents/MacOS}"
MONO_FRAMEWORK="${MONO_FRAMEWORK:-/Library/Frameworks/Mono.framework/Versions/Current}"

export PATH="${MONO_FRAMEWORK}/bin:${PATH}"
export DYLD_FALLBACK_LIBRARY_PATH="${RENODE_ROOT}:${RENODE_ROOT}/bin:${MONO_FRAMEWORK}/lib:/lib:/usr/lib"

exec mono "${RENODE_ROOT}/bin/Renode.exe" "$@"
