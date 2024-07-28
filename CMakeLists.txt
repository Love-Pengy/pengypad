cmake_minimum_required(VERSION 3.13)

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")
set(CMAKE_C_STANDARD 11)


include(pico_sdk_import.cmake)

project(pengypad C CXX ASM)

pico_sdk_init()

add_subdirectory(src pengypad)

