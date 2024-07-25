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

#define START 0x0      // 0b0000
#define CW_START 0x1   // 0b0001
#define CW_NEXT 0x2    // 0b0010
#define CW_FINAL 0x3   // 0b0011
#define CCW_START 0x4  // 0b0100
#define CCW_NEXT 0x5   // 0b0101
#define CCW_FINAL 0x6  // 0b0110
#define SEND_CW 0x10
#define SEND_CCW 0x20

// the value of the current array will be where you go (first index val),
//  given the index of the array (where you're at) select the next spot you go
// 00 01 11 10 00
// 00 10 11 01 00
// WE ARE SENDING AT HIGH
static const unsigned char template[7][4] = {
    // START 0x0 (expecting 0x01 or 0x10
    {START, CW_START, CCW_START, START},
    // CW_START 0x1 (expecting 0x00)
    {CW_NEXT, CW_START, START, START},
    // CW_NEXT 0x2 (expecting 0x10)
    {CW_NEXT, CW_START, CW_FINAL, START},
    // CW_FINAL 0x3 (expecting 0x11)
    {CW_NEXT, START, CW_FINAL, SEND_CW},
    // CCW_START 0x4 (expecting 0x00)
    {CCW_NEXT, START, CCW_START, START},
    // CCW_NEXT 0x5 (expecting 0x01)
    {CCW_NEXT, CCW_FINAL, CCW_START, START},
    // CCW_FINAL 0x6 (expecting 0x11)
    {CCW_NEXT, CCW_FINAL, START, SEND_CCW},
};

void initEncoder(void) {
    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);
    gpio_init(9);
    gpio_set_dir(9, GPIO_IN);
    // MAKE SURE THIS IS ON PULL UP SILLY
    gpio_pull_up(9);
    gpio_init(10);
    gpio_set_dir(10, GPIO_OUT);
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_put(11, 1);
    gpio_init(12);
    gpio_set_dir(12, GPIO_IN);
    gpio_pull_down(12);
}

int main() {
    static unsigned char state = 0;
    static unsigned char pinVal = 0;
    bool resetButton = false;

    stdio_init_all();
    initEncoder();
    while (true) {
        pinVal &= 0x00;
        gpio_put(8, 1);
        sleep_us(50);
        pinVal |= gpio_get(9);
        gpio_put(8, 0);

        gpio_put(10, 1);
        sleep_us(50);
        pinVal |= (gpio_get(9) << 1);
        gpio_put(10, 0);
        state = template[state & 0xf][pinVal];
        if (state == SEND_CW) {
            if (!resetButton) {
                printf("SEND CW\n");
            }
        }
        else if (state == SEND_CCW) {
            if (!resetButton) {
                printf("SEND CCW\n");
            }
        }
        else {
            // check the button
            if (gpio_get(12)) {
                if (!resetButton) {
                    printf("PUSHED\n");
                    resetButton = true;
                }
            }
            else {
                resetButton = false;
            }
        }
    }
}
