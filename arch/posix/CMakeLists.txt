# SPDX-License-Identifier: Apache-2.0

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_HOST_SYSTEM_NAME}.${CMAKE_HOST_SYSTEM_PROCESSOR}.cmake)
  # @Intent: Set necessary compiler & linker options for this specific host architecture & OS
  include(${CMAKE_HOST_SYSTEM_NAME}.${CMAKE_HOST_SYSTEM_PROCESSOR}.cmake)
else()
  if (CONFIG_64BIT)
    # some gcc versions fail to build without -fPIC
    zephyr_compile_options(-m64 -fPIC)
    zephyr_link_libraries(-m64)
  else ()
    zephyr_compile_options(-m32)
    zephyr_link_libraries(-m32)
  endif ()
endif()

if (CONFIG_GPROF)
  zephyr_compile_options($<TARGET_PROPERTY:compiler,gprof>)
  zephyr_link_libraries($<TARGET_PROPERTY:linker,gprof>)
endif()
if (CONFIG_ASAN)
  zephyr_compile_options($<TARGET_PROPERTY:compiler,sanitize_address>)
  zephyr_link_libraries($<TARGET_PROPERTY:linker,sanitize_address>)
endif ()

if (CONFIG_UBSAN)
  zephyr_compile_options($<TARGET_PROPERTY:compiler,sanitize_undefined>)
  zephyr_link_libraries($<TARGET_PROPERTY:linker,sanitize_undefined>)
endif ()

zephyr_include_directories(${BOARD_DIR})

include(ExternalProject)

if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
# https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html
set(submake "$(MAKE)")
else() # Obviously no MAKEFLAGS. Let's hope a "make" can be found somewhere.
set(submake "make")
endif()

zephyr_get_include_directories_for_lang_as_string(       C includes)
zephyr_get_compile_definitions_for_lang_as_string(       C definitions)
zephyr_get_compile_options_for_lang_as_string(           C options)

set(external_project_cppflags "${includes} ${definitions} ${options} -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${ZEPHYR_BASE}/kernel/include")
set(external_project_cflags "-D_POSIX_C_SOURCE=200809 -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED")
set(external_project_prefix ${ZEPHYR_BINARY_DIR}/misc/posix_core)

ExternalProject_Add(
  posix_core_project
  PREFIX     ${external_project_prefix}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core
  BINARY_DIR ${external_project_prefix}/bin
  CONFIGURE_COMMAND ""
  BUILD_COMMAND
  ${submake} -C ${CMAKE_CURRENT_SOURCE_DIR}/core
  PREFIX=${external_project_prefix}
  SRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}/core
  CC=${CMAKE_C_COMPILER}
  AR=${CMAKE_AR}
  CFLAGS=
  CPPFLAGS=${external_project_cppflags}
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS ${external_project_prefix}/lib/libposix_core.a
  )

add_dependencies(
  posix_core_project
  ${KOBJ_TYPES_H_TARGET}
  ${SYSCALL_LIST_H_TARGET}
  )

# Create a wrapper CMake library that our app can link with
add_library(posix_core_lib STATIC IMPORTED GLOBAL)
add_dependencies(
  posix_core_lib
  posix_core_project
  )
set_target_properties(posix_core_lib PROPERTIES IMPORTED_LOCATION ${external_project_prefix}/lib/libposix_core.a)

zephyr_link_libraries(posix_core_lib)

zephyr_ld_options(
  -ldl
)
