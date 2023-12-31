set(srcs "cpu_util_esp32h2.c"
         "rtc_clk_init.c"
         "rtc_clk.c"
         "rtc_init.c"
         "rtc_pm.c"
         "rtc_sleep.c"
         "rtc_time.c"
         "chip_info.c"
         )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "../async_memcpy_impl_gdma.c"
                     "esp_hmac.c"
                     "esp_crypto_lock.c"
                     "esp_ds.c"
                     "dport_access.c"
                     "sar_periph_ctrl.c")

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . private_include)
target_include_directories(${COMPONENT_LIB} PRIVATE ../hal)
