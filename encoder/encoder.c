/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"

// 8 A
// 9: C
// 10: B

void initEncoder(void) {
    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);
    gpio_init(9);
    gpio_set_dir(9, GPIO_IN);
    gpio_init(10);
    gpio_set_dir(10, GPIO_OUT);
}

int main() {
    stdio_init_all();
    initEncoder();
    static uint8_t state = 0;
    static uint8_t dir = 0;
    static uint8_t last = 0;
    static bool reset = false;

    while (true) {
        state &= 0x00;
        gpio_put(8, 1);
        sleep_us(50);
        if (gpio_get(9)) {
            state |= 0x01;
        }
        gpio_put(8, 0);
        gpio_put(10, 1);
        sleep_us(50);
        if (gpio_get(9)) {
            state |= 0x10;
        }
        gpio_put(10, 0);
        if (!dir) {
            switch (state) {
                case 0x01:
                    dir = 0x01;
                    last = 0x10;
                    break;
                case 0x10:
                    dir = 0x10;
                    last = 0x01;
                    break;
                default:
                    break;
            }
        }
        else {
            if (!reset) {
                if (state == last) {
                    reset = true;
                }
            }
            else {
                if (!state && reset) {
                    printf("SEND: %d\n", dir);
                    dir = 0x00;
                    reset = false;
                }
            }
        }
    }
}

