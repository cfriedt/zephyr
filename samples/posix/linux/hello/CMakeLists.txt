# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(linux_hello)

target_sources(app PRIVATE src/main.c)
target_include_directories(app PRIVATE ${ZEPHYR_BINARY_DIR}/include/generated)

# Note: assume the host is Ubuntu, a cross-toolchain can be installed with:
# sudo apt install -y crossbuild-essential-riscv64

if(CONFIG_RISCV)
  set(CROSS_COMPILE riscv64-linux-gnu-)
  set(CXXFLAGS
    # general optimization flags for size
    -nostdlib -nostartfiles -static -g -Oz
    # Use the musl libc (produces generally smaller binaries than glibc)
    -mmusl
    # machine specific flags
    -mcmodel=medany -mabi=lp64d -march=rv64gc_zicntr
    # remove
    -Wl,--build-id=none
    # needs to have no undefined symbols if we are to execute
    -Wl,--no-undefined
    # remove unused sections (can also done with strip later)
    -Wl,--gc-sections
  )
endif()

set(hello_c ${CMAKE_CURRENT_SOURCE_DIR}/src/hello.c)
set(hello_elf ${PROJECT_BINARY_DIR}/hello.elf)
set(hello_elf_stripped ${PROJECT_BINARY_DIR}/hello.elf.stripped)
set(hello_elf_gz ${PROJECT_BINARY_DIR}/hello.elf.gz)
set(hello_inc ${ZEPHYR_BINARY_DIR}/include/generated/hello.inc)

generate_inc_file_for_target(app ${hello_elf} ${hello_inc})

add_custom_target(
  build_hello_elf
  COMMENT "Cross-compiling ${hello_elf} from ${hello_c}"
  COMMAND ${CROSS_COMPILE}gcc ${CXXFLAGS} ${hello_c} -o ${hello_elf}
  DEPENDS ${hello_c}
  BYPRODUCTS ${hello_elf}
  SOURCES ${hello_c}
)

add_custom_target(
  build_hello_elf_stripped
  COMMENT "Stripping ${hello_elf} of symbols and saving to ${hello_elf_stripped}"
  COMMAND ${CROSS_COMPILE}strip --strip-unneeded ${hello_elf} -o ${hello_elf_stripped}
  DEPENDS ${hello_elf}
  BYPRODUCTS ${hello_elf_stripped}
  SOURCES ${hello_elf}
)

add_custom_target(
  build_hello_elf_gz
  COMMENT "Compressing ${hello_elf_stripped} to ${hello_elf_gz}"
  COMMAND cat ${hello_elf_stripped} | gzip -f -9 > ${hello_elf_gz}
  DEPENDS ${hello_elf_stripped}
  BYPRODUCTS ${hello_elf_gz}
  SOURCES ${hello_elf_stripped}
)
