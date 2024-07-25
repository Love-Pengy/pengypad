/**
 *Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "../encoder/encoder.h"

#define INPINSIZE 5
static uint8_t inPinArray[] = {29, 28, 27, 26, 15};

void initKeyboard(void) {
    for (int i = 0; i < INPINSIZE; i++) {
        gpio_init(inPinArray[i]);
        gpio_set_dir(inPinArray[i], GPIO_IN);
        gpio_pull_down(inPinArray[i]);
    }
}

// this assumes that initKeyboard was run first
uint32_t scanKeys(void) {
    // only one key should be active at a time atm so just take the first one
    // starting from 29
    uint8_t encoderVal = pollEncoder();
    if(encoderVal){
        return(encoderVal + 5);
    }
    /*encoderVal > 0 ? return(encoderVal + 5) : encoderVal; */
    for (int i = 0; i < INPINSIZE; i++) {
        if (gpio_get(inPinArray[i])) {
            return (i + 1);
        }
    }
    return (0);
}

/*int main() {*/
/*  // green = key 1*/
/*  // yellow = key 2*/
/*  // white = key 3*/
/*  // blue = key 4*/
/*  // red = key 5*/
/*  stdio_init_all();*/
/*  initKeyboard();*/
/*  while (true) {*/
/*    printf("%d\n", scanKeys());*/
/*  }*/
/*}*/
