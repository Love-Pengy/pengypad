cmake_minimum_required(VERSION 3.13)


add_executable(src ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.c ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.h ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.pio ${CMAKE_CURRENT_LIST_DIR}/ws2812/generated/ws2812.py)
pico_generate_pio_header(src
    ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.pio
)

target_sources(src PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(src PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/ws2812
        )

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(src PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board hardware_pio hardware_dma pico_multicore)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(src PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(src)

# add url via pico_set_program_url
example_auto_set_url(src)

