zephyr_library_named(rpmi)

zephyr_library_sources_ifdef(CONFIG_RISCV_RPMI 
  rpmi_context.c 
  rpmi_hsm.c 
  rpmi_service_group_clock.c
  rpmi_service_group_cppc.c
  rpmi_service_group_hsm.c
  rpmi_service_group_sysreset.c 
  rpmi_service_group_syssusp.c 
  rpmi_shmem.c 
  rpmi_transport_shmem.c 
  rpmi_transport.c 
)

if (CONFIG_RISCV_RPMI)
  zephyr_library_include_directories(rpm PRIVATE .)
endif()

