# SPDX-License-Identifier: Apache-2.0

set(GEN_DIR ${ZEPHYR_BINARY_DIR}/include/generated)

if(CONFIG_POSIX_API)
  zephyr_include_directories(${ZEPHYR_BASE}/include/zephyr/posix)
endif()

if (NOT CONFIG_TC_PROVIDES_POSIX_SIGNALS)
  if(CONFIG_POSIX_SIGNALS)
    set(STRSIGNAL_TABLE_H ${GEN_DIR}/posix/strsignal_table.h)

    add_custom_command(
      OUTPUT ${STRSIGNAL_TABLE_H}
      COMMAND
      ${PYTHON_EXECUTABLE}
      ${ZEPHYR_BASE}/scripts/build/gen_strsignal_table.py
      -i ${ZEPHYR_BASE}/lib/libc/minimal/include/signal.h
      -o ${STRSIGNAL_TABLE_H}
      DEPENDS ${ZEPHYR_BASE}/lib/libc/minimal/include/signal.h
    )
  endif()
endif()

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_EVENTFD eventfd.c)

if (NOT CONFIG_TC_PROVIDES_POSIX_ASYNCHRONOUS_IO)
  zephyr_library_sources_ifdef(CONFIG_POSIX_ASYNCHRONOUS_IO aio.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_ASYNCHRONOUS_IO -D_POSIX_ASYNCHRONOUS_IO=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_BARRIERS)
  zephyr_library_sources_ifdef(CONFIG_POSIX_BARRIERS barrier.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_BARRIERS -D_POSIX_BARRIERS=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_CLOCK_SELECTION)
  zephyr_library_sources_ifdef(CONFIG_POSIX_CLOCK_SELECTION clock_selection.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_CLOCK_SELECTION -D_POSIX_CLOCK_SELECTION=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_C_LIB_EXT)
  zephyr_library_sources_ifdef(CONFIG_POSIX_C_LIB_EXT
    fnmatch.c
    getentropy.c
    getopt/getopt.c
    getopt/getopt_common.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_C_LIB_EXT -D_POSIX_C_LIB_EXT=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_DEVICE_IO)
  zephyr_library_sources_ifdef(CONFIG_POSIX_DEVICE_IO
    # perror should be moved to the common libc
    perror.c
    device_io.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_DEVICE_IO -D_POSIX_DEVICE_IO=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_FD_MGMT)
  zephyr_library_sources_ifdef(CONFIG_POSIX_FD_MGMT
    fd_mgmt.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_FD_MGMT -D_POSIX_FD_MGMT=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_FILE_SYSTEM)
  zephyr_library_sources_ifdef(CONFIG_POSIX_FILE_SYSTEM fs.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_FILE_SYSTEM -D_POSIX_FILE_SYSTEM=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_FILE_SYSTEM_R)
  zephyr_library_sources_ifdef(CONFIG_POSIX_FILE_SYSTEM_R file_system_r.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_FILE_SYSTEM_R -D_POSIX_FILE_SYSTEM_R=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_MEMORY_PROTECTION)
  zephyr_library_sources_ifdef(CONFIG_POSIX_MEMORY_PROTECTION mprotect.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_MEMORY_PROTECTION -D_POSIX_MEMORY_PROTECTION=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_MAPPED_FILES)
  zephyr_library_sources_ifdef(CONFIG_POSIX_MAPPED_FILES mmap.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_MAPPED_FILES -D_POSIX_MAPPED_FILES=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_MULTI_PROCESS)
  zephyr_library_sources_ifdef(CONFIG_POSIX_MULTI_PROCESS
    sleep.c
    multi_process.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_MULTI_PROCESS -D_POSIX_MULTI_PROCESS=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_NETWORKING)
  zephyr_library_sources_ifdef(CONFIG_POSIX_NETWORKING net.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_NETWORKING -D_POSIX_NETWORKING=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_SIGNALS)
  zephyr_library_sources_ifdef(CONFIG_POSIX_SIGNALS signal.c ${STRSIGNAL_TABLE_H})
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_SIGNALS -D_POSIX_SIGNALS=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_SINGLE_PROCESS)
  zephyr_library_sources_ifdef(CONFIG_POSIX_SINGLE_PROCESS
    confstr.c
    env.c
    env_common.c
    sysconf.c
    uname.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_SINGLE_PROCESS -D_POSIX_SINGLE_PROCESS=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_SPIN_LOCKS)
  zephyr_library_sources_ifdef(CONFIG_POSIX_SPIN_LOCKS spinlock.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_SPIN_LOCKS -D_POSIX_SPIN_LOCKS=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_TIMERS)
  zephyr_library_sources_ifdef(CONFIG_POSIX_TIMERS
    clock.c
    timer.c
    timespec_to_timeout.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_TIMERS -D_POSIX_TIMERS=200809L)
# this is technically not an option group, but an option. Temporary hack here. Should be enabled
# with CONFIG_XSI_ADVANCED_REALTIME.
zephyr_compile_options_ifdef(CONFIG_POSIX_MONOTONIC_CLOCK -D_POSIX_MONOTONIC_CLOCK=200809L)
# this is technically not an option group, but an option. Temporary hack here. Should be enabled
# with CONFIG_XSI_ADVANCED_REALTIME.
zephyr_compile_options_ifdef(CONFIG_POSIX_CPUTIME -D_POSIX_CPUTIME=200809L)


if (NOT CONFIG_TC_PROVIDES_POSIX_READER_WRITER_LOCKS)
  # Note: the Option is _POSIX_READER_WRITER_LOCKS, while the Option Group is POSIX_RW_LOCKS.
  # We have opted to use POSIX_READER_WRITER_LOCKS here to match the Option name.
  zephyr_library_sources_ifdef(CONFIG_POSIX_READER_WRITER_LOCKS rwlock.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_READER_WRITER_LOCKS -D_POSIX_READER_WRITER_LOCKS=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_SEMAPHORES)
  zephyr_library_sources_ifdef(CONFIG_POSIX_SEMAPHORES semaphore.c)
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_SEMAPHORES -D_POSIX_SEMAPHORES=200809L)

if (NOT CONFIG_TC_PROVIDES_POSIX_THREADS)
  # Note: the Option is _POSIX_THREADS, while the Option Group is POSIX_THREADS_BASE.
  # We have opted to use POSIX_THREADS here to match the Option name.
  zephyr_library_sources_ifdef(CONFIG_POSIX_THREADS
    cond.c
    grp.c
    key.c
    mutex.c
    pthread.c
    pwd.c
  )
endif()
zephyr_compile_options_ifdef(CONFIG_POSIX_THREADS -D_POSIX_THREADS=200809L)

if(NOT CONFIG_TC_PROVIDES_XSI_REALTIME)
  zephyr_library_sources_ifdef(CONFIG_POSIX_FSYNC fsync.c)
  zephyr_library_sources_ifdef(CONFIG_POSIX_MEMLOCK mlockall.c)
  zephyr_library_sources_ifdef(CONFIG_POSIX_MEMLOCK_RANGE mlock.c)
  zephyr_library_sources_ifdef(CONFIG_POSIX_MESSAGE_PASSING mqueue.c)
  zephyr_library_sources_ifdef(CONFIG_POSIX_PRIORITY_SCHEDULING sched.c)
  zephyr_library_sources_ifdef(CONFIG_POSIX_SHARED_MEMORY_OBJECTS shm.c)
endif()
if(CONFIG_XSI_REALTIME OR (CONFIG_POSIX_FSYNC AND CONFIG_POSIX_MEMLOCK AND CONFIG_POSIX_MEMLOCK_RANGE AND CONFIG_POSIX_MESSAGE_PASSING AND CONFIG_POSIX_PRIORITY_SCHEDULING AND CONFIG_POSIX_SHARED_MEMORY_OBJECTS))
  zephyr_compile_options(-D_XOPEN_SOURCE=700)
endif()

zephyr_library_sources_ifdef(CONFIG_XOPEN_STREAMS stropts.c)

if (NOT CONFIG_TC_PROVIDES_XSI_SINGLE_PROCESS)
  zephyr_library_sources_ifdef(CONFIG_XSI_SINGLE_PROCESS
    env_common.c
    xsi_single_process.c
  )
endif()

if (NOT CONFIG_TC_PROVIDES_XSI_SYSTEM_LOGGING)
  zephyr_library_sources_ifdef(CONFIG_XSI_SYSTEM_LOGGING syslog.c)
endif()
zephyr_compile_options_ifdef(CONFIG_XSI_SYSTEM_LOGGING -D_XSI_SYSTEM_LOGGING=700)

zephyr_library_sources_ifdef(CONFIG_GETOPT_LONG
  getopt/getopt_long.c
)
zephyr_include_directories_ifdef(CONFIG_POSIX_C_LIB_EXT
  getopt/
)

zephyr_library_include_directories(
  ${ZEPHYR_BASE}/kernel/include
  ${ARCH_DIR}/${ARCH}/include
)

zephyr_library_property(ALLOW_EMPTY TRUE)

zephyr_library_compile_options(-U_POSIX_C_SOURCE -D_POSIX_C_SOURCE=200809L)
