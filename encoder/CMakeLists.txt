add_executable(encoder
        encoder.c
        )

# pull in common dependencies
target_link_libraries(encoder pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(encoder)

# add url via pico_set_program_url
example_auto_set_url(encoder)
pico_enable_stdio_usb(encoder 1)
pico_enable_stdio_uart(encoder 0)

