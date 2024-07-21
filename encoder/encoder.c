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
    // 0x00 = both low
    // 0x01 = B high
    // 0x10 = A high
    // 0x11 = perform action
    static uint8_t state = 0;
    // 0 = none
    // 1 = A side
    // 2 = B side
    /*
     * check which one goes high first
     * send key for direction
     * wait for the opposite one to go low
     * once that happens reset
     * */
    static uint8_t dir = 0;
    // TODO: fix both being registered on each tick. The direction is right
    // already
    static bool a = false;
    static bool b = false;
    static bool needReset = false;

    while (true) {
        //printf("TEST: %d %d\n", needReset, state);
        if (!state) {
            a = false;
            b = false;
            gpio_put(8, 1);
            sleep_us(50);
            if (gpio_get(9)) {
                a = true;
                gpio_put(8, 0);
            }
            else {
                gpio_put(10, 1);
                sleep_us(50);
                if (gpio_get(9)) {
                    b = true;
                    gpio_put(10, 0);
                }
            }
            if (!needReset) {
                if (a ^ b) {
                    if (a) {
                        state |= 0x01;
                    }
                    else {
                        state |= 0x10;
                    }
                }
            }
            else {
                if (!a && !b) {
                    needReset = false;
                }
            }
        }
        else {
            switch (state) {
                case 0x01:
                    gpio_put(10, 1);
                    sleep_us(50);
                    if (gpio_get(9) && !needReset) {
                        printf("SEND 8->10 KEY\n");
                        needReset = true;
                        state = 0x00;
                    }
                    gpio_put(10, 0);
                    sleep_us(50);
                    break;
                case 0x10:
                    gpio_put(8, 1);
                    sleep_us(50);
                    if (gpio_get(9) && !needReset) {
                        printf("SEND 10->8 KEY\n");
                        needReset = true;
                        state = 0x00;
                    }
                    gpio_put(8, 0);
                    sleep_us(50);
                    break;
                default:
                    break;
            }
        }
    }
}
