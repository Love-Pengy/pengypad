/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "encoder.h"
#include <stdio.h>

#include "pico/stdlib.h"

// 27 A
// 26: C
// 15: B

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
    gpio_init(27);
    gpio_set_dir(27, GPIO_OUT);
    gpio_init(26);
    gpio_set_dir(26, GPIO_IN);
    // MAKE SURE THIS IS ON PULL UP SILLY
    gpio_pull_down(26);
    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);
    gpio_init(29);
    gpio_set_dir(29, GPIO_OUT);
    gpio_put(29, 1);
    gpio_init(28);
    gpio_set_dir(28, GPIO_IN);
    gpio_pull_down(28);
}

// 0 send nothing
// 1 send cw
// 2 send ccw
// 3 send push button
static unsigned char state = 0;
static unsigned char pinVal = 0;
static bool resetButton = false;
static unsigned char previousState = 0;

unsigned char pollEncoder(void) {
    pinVal &= 0x00;
    gpio_put(27, 1);
    sleep_us(50);
    pinVal |= gpio_get(26);
    gpio_put(27, 0);

    gpio_put(15, 1);
    sleep_us(50);
    pinVal |= (gpio_get(26) << 1);
    gpio_put(15, 0);
    state = template[state & 0xf][pinVal];
    switch (state) {
        case SEND_CW:
            if (!resetButton) {
                return (1);
            }
            goto BUTTON;
        case SEND_CCW:
            if (!resetButton) {
                return (2);
            }
        BUTTON:
        default:
            if (gpio_get(12)) {
                if (!resetButton) {
                    return (3);
                    resetButton = true;
                }
                else {
                    resetButton = false;
                }
            }
    }
    return(0);
}



