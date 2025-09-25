#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright 2025, Friedt Professional Engineering Services, Inc

# Generate Linux syscall stubs for KCL
# Usage: python3 genlinuxsystem_calls.py [(-a|--arch) <arch>]

import argparse
import json
import logging
import requests
import sys

from pathlib import Path
from system_calls import syscalls


logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)


def parse_args():
    parser = argparse.ArgumentParser(description="Generate Linux syscall stubs for KCL")
    parser.add_argument("-a", "--arch", type=str, required=True,
                        help="Target architecture (compatible with Zephyr's CONFIG_ARCH)")
    args = parser.parse_args()

    arch = args.arch
    sc = syscalls()
    if arch not in sc.archs():
        raise ValueError(f"Error: Unsupported architecture '{arch}'. Supported architectures are: {sc.archs()}")

    return args


def populate_syscall_object(number, name):

    url = f"https://pubs.opengroup.org/onlinepubs/9699919799/functions/{name}.html"

    # print(f"Checking {url}... ", end="")
    if requests.get(url, timeout=5).status_code != 200:
        # print("not found")
        return None

    logger.debug(f"{name.ljust(24)} {str(number).ljust(8)} {url}")
    return {
        "number": number,
        "name": name,
        "url": url
    }


def get_syscall_table(arch: str) -> dict:

    table_path = Path(f"~/linux_syscalls_{arch}.json").expanduser()

    by_name = {}
    by_number = {}

    if table_path.exists():
        with open(table_path, "r") as json_file:
            entries = json.load(json_file)
            for entry in entries:
                by_name[entry["name"]] = entry
                by_number[entry["number"]] = entry
        return by_name, by_number

    sc = syscalls()
    sc.load_arch_table(arch)
    syscall_list = sc.names()

    for name in syscall_list:
        try:
            number = sc.get(name, arch)
        except Exception as e:
            continue
        obj = populate_syscall_object(number, name)
        if obj is None:
            continue
        by_name[name] = obj
        by_number[number] = obj

    with open(table_path, "w") as json_file:
        entries = list(by_number.values())
        json.dump(entries, json_file)

    return by_name, by_number


def main():
    args = parse_args()
    by_name, by_number = get_syscall_table(args.arch)

    for key in sorted(by_number.keys()):
        entry = by_number[key]
        print(f"#define __NR_{entry['name']} {entry['number']} ")

    print(f"typedef long (*syscall_t)();")

    for key in sorted(by_number.keys()):
        entry = by_number[key]
        print(f"syscall_t {entry['name']};")

    print("const syscall_t syscalls[] = {")
    for key in sorted(by_number.keys()):
        entry = by_number[key]
        print(f"[__NR_{entry['name']}] = {entry['name']},")
    print("};")

if __name__ == "__main__":
    sys.exit(main())
