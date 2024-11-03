zephyr_library_named(rpmi)

zephyr_library_sources_ifdef(CONFIG_DT_HAS_RPMI_TRANSPORT
  rpmi_transport.c
)

zephyr_library_sources_ifdef(CONFIG_DT_HAS_RPMI_TRANSPORT_SOCKET
  rpmi_transport_shmem.c
)

zephyr_library_sources_ifdef(CONFIG_DT_HAS_RPMI_TRANSPORT_SOCKET
  rpmi_transport_socket.c
)

if (CONFIG_RISCV_RPMI)
  zephyr_library_include_directories(rpm PRIVATE .)
endif()
