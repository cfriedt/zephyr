#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0

# This script generates a list of header dependencies for POSIX compliance.
# It can be used to determine which order headers should be written from scratch
# for any POSIX-compliant implementation.

import json
import logging
import os
import re
import requests
import sys
import tempfile
import subprocess
import re
import os

from pathlib import Path

CPP_URL = 'https://en.cppreference.com/w/c/header'
POSIX_URL = 'https://pubs.opengroup.org/onlinepubs/9699919799/idx/head.html'
CPP_H_JSON = Path('/tmp/cpp_headers.json')
POSIX_H_JSON = Path('/tmp/posix_headers.json')
H_DEPS_JSON = Path('/tmp/header_deps.json')

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def is_cpp_header(header: str) -> bool:
    header_bn = header.rstrip('.h')
    response = requests.get(f'{CPP_URL}/{header_bn}')
    return response.status_code == 200

def extract_header_names() -> tuple[list[str], list[str]]:
    """
    Extracts header names from the HTML content of the POSIX header dependencies page.
    Returns a set of header names.
    """

    if CPP_H_JSON.exists() and POSIX_H_JSON.exists():
        logger.info(f'Using cached header dependencies {CPP_H_JSON} and {POSIX_H_JSON}')
        with open(CPP_H_JSON, 'r') as f:
            cpp_headers = json.load(f)
        with open(POSIX_H_JSON, 'r') as f:
            posix_headers = json.load(f)
        logger.debug(f'cpp_headers: {cpp_headers}')
        logger.debug(f'posix_headers: {posix_headers}')
        return (cpp_headers, posix_headers)

    cpp_headers = list([])
    posix_headers = list([])
    header_pat = r'.*&lt;([a-z_/]+\.h)&gt;.*'
    
    logger.info(f'Fetching POSIX header requirements from {POSIX_URL}')
    response = requests.get(POSIX_URL)
    if response.status_code != 200:
        logger.error(f'Failed to fetch POSIX header requirements. Status code: {response.status_code}')
        return os.EX_UNAVAILABLE
    
    for match in re.finditer(header_pat, response.text):
        header = match.group(1)
        logger.debug(f'categorizing header "{header}"')
        header_bn = header.rstrip('.h')
        response = requests.get(f'{CPP_URL}/{header_bn}')
        if response.status_code == 200:
            cpp_headers.append(header)
        else:
            posix_headers.append(header)

        with open(CPP_H_JSON, 'w') as f:
            json.dump(cpp_headers, f)
        with open(POSIX_H_JSON, 'w') as f:
            json.dump(posix_headers, f)
        
    logger.debug(f'cpp_headers: {cpp_headers}')
    logger.debug(f'posix_headers: {posix_headers}')

    return (cpp_headers, posix_headers)


def filter_deps(header: str, deps: list[str], stdc_headers: set[str]) -> list[str]:
    filtered_deps = []
    for dep in deps:
        if not dep.endswith('.h'):
            continue
        dep_bn = dep.removeprefix(
            '/usr/include/').removeprefix('x86_64-linux-gnu/')
        if dep_bn == header:
            continue
        if is_cpp_header(dep_bn):
            stdc_headers.add(dep_bn)
        if dep_bn in stdc_headers:
            filtered_deps.append(dep_bn)
            continue
        if dep_bn.startswith('sys/'):
            filtered_deps.append(dep_bn)
            continue
        if dep_bn.startswith('bits/') and not dep_bn.endswith('64.h'):
            filtered_deps.append(dep_bn)
            continue

    return filtered_deps


def extract_single_header_dependencies(header: str, stdc_headers: set[str]) -> list[str]:
    content = f'#include <{header}>\n'

    # Create the temp .c file
    with tempfile.NamedTemporaryFile(suffix=".c", delete=True, mode="w", encoding="utf-8") as tf:
        tf.write(content)
        tf.flush()
        tmp_c_path = tf.name

        # Build gcc -M command
        cmd = ['gcc', "-M", tmp_c_path]

        # Run gcc
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            logger.debug(f"gcc failed: {result.stderr.strip()}")
            return []

        output = result.stdout
        # Collapse line continuations (backslash-newline)
        single_line = re.sub(r'\\\n\s*', ' ', output)

        # Split at colon to get the right side (dependencies)
        parts = single_line.split(":", 1)
        if len(parts) != 2:
            logger.warn(f"Unexpected gcc output format: {output!r}")
            return []

        deps = parts[1].strip().split()

        return filter_deps(header, deps, stdc_headers)


def extract_header_dependencies(cpp_h: list[str], posix_h: list[str]) -> dict[str, list[str]]:

    def_not_stdc_headers = set(posix_h)
    stdc_headers = set(cpp_h)

    dependencies = {}
    if H_DEPS_JSON.exists():
        logger.info(f'Using cached header dependencies {H_DEPS_JSON}')
        with open(H_DEPS_JSON, 'r') as f:
            dependencies = json.load(f)
    else:
        logger.info(f'Building header dependency cache {H_DEPS_JSON}')
        for header in cpp_h + posix_h:
            dependencies[header] = extract_single_header_dependencies(
                header, stdc_headers)
        with open(H_DEPS_JSON, 'w') as f:
            json.dump(dependencies, f)

    nodes = set(cpp_h + posix_h)
    for k, v in dependencies.items():
        nodes.update(v)

    return nodes, dependencies

def main() -> int:
    cpp_h, posix_h = extract_header_names()
    h, h_deps = extract_header_dependencies(cpp_h, posix_h)

    print("Header Dependencies:")
    print(f'time.h: {h_deps["time.h"]}')
    print(f'signal.h: {h_deps["signal.h"]}')

    return os.EX_OK

if __name__ == '__main__':
    sys.exit(main())
