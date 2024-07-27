cmake_minimum_required(VERSION 3.13)


add_executable(pengypad ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.c ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.h ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.pio ${CMAKE_CURRENT_LIST_DIR}/ws2812/generated/ws2812.py ${CMAKE_CURRENT_LIST_DIR}/keyScan/keyScan.c ${CMAKE_CURRENT_LIST_DIR}/keyScan/keyScan.h ${CMAKE_CURRENT_LIST_DIR}/encoder/encoder.c ${CMAKE_CURRENT_LIST_DIR}/encoder/encoder.h ${CMAKE_CURRENT_LIST_DIR}/mappings.h)
pico_generate_pio_header(pengypad
    ${CMAKE_CURRENT_LIST_DIR}/ws2812/ws2812.pio
)

target_sources(pengypad PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(pengypad PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/ws2812
        ${CMAKE_CURRENT_LIST_DIR}/keyScan
        ${CMAKE_CURRENT_LIST_DIR}/encoder
        
        )

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(pengypad PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board hardware_pio hardware_dma pico_multicore)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(pengypad PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(pengypad)

# add url via pico_set_program_url
example_auto_set_url(pengypad)

