#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright 2025, Friedt Professional Engineering Services, Inc

# Generate Linux syscall stubs for KCL
# Usage: python3 genlinuxsystem_calls.py [(-a|--arch) <arch>]

import argparse
import sys

from system_calls import syscalls

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


def main():
    args = parse_args()

    sc = syscalls()
    sc.load_arch_table(args.arch)
    syscall_list = sc.names()

    by_name = {name: sc.get(name) for name in syscall_list}
    by_number = {}

    for entry in by_name.items():
        by_number[]

    for k in syscall_list:
        print(f"Syscall: {k}, Number: {sc.get(k, args.arch)}")

    # with open("linux_system_calls.h", "w") as header_file, open("linux_system_calls.c", "w") as source_file:
    #     header_file.write("// Auto-generated Linux syscall stubs for KCL\n")
    #     header_file.write("#ifndef LINUX_SYSCALLS_H\n#define LINUX_SYSCALLS_H\n\n")
    #     header_file.write("#include <stdint.h>\n\n")

    #     source_file.write("// Auto-generated Linux syscall stubs for KCL\n")
    #     source_file.write("#include \"linux_system_calls.h\"\n")
    #     source_file.write("#include <unistd.h>\n#include <sys/syscall.h>\n#include <errno.h>\n\n")

    #     for syscall in syscall_list:
    #         func_name = f"sys_{syscall['name']}"
    #         ret_type = syscall.get('return_type', 'long')
    #         params = syscall.get('params', [])
    #         param_list = ", ".join([f"{p['type']} {p['name']}" for p in params])
    #         param_names = ", ".join([p['name'] for p in params])

    #         # Write function prototype to header file
    #         header_file.write(f"{ret_type} {func_name}({param_list});\n")

    #         # Write function definition to source file
    #         source_file.write(f"{ret_type} {func_name}({param_list}) {{\n")
    #         source_file.write(f"    return syscall(SYS_{syscall['name'].upper()}, {param_names});\n")
    #         source_file.write("}\n\n")

    #     header_file.write("\n#endif // LINUX_SYSCALLS_H\n")


if __name__ == "__main__":
    sys.exit(main())
