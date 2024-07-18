/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"


//########################################
// ALL OF THESE ARE ASSUMING A SINGLE LED#
//########################################

enum {
    ENFORCE = 0,
    SIDEEYE,
};

// NOTE: VALUES ARE IN GRB
enum status {
    MOUNTED = 0,  // green (0x110000)
    UNMOUNTED,    // red (0x001100)
    SUSPENDED,    // blue (0x000011)
    DEBUG, //white (0x111111)
};

#define IS_RGBW true
#define NUM_PIXELS 1
#define WS2812_PIN 16

// start going up
static int dir = 1;

static enum status currStatus = UNMOUNTED;

static uint32_t currColor = 0x001100;

static int currMode = 0;

void ws2812ChangeStatus(int val) {
    switch (val) {
        case 0:
            currStatus = MOUNTED;
            currColor = 0x110000;
            break;
        case 1:
            currStatus = UNMOUNTED;
            currColor = 0x001100;
            break;
        case 2:
            currStatus = SUSPENDED;
            currColor = 0x000011;
            break; 
        default: 
            currStatus = DEBUG;
            currColor = 0x111111;
            break;
    }
}


static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void pattern_fade(uint len, uint t) {
    int max = 35;
    t %= max;

    uint32_t val;
    int zeroEnforcer = SIDEEYE;
    for (int i = 0; i < len; ++i) {
        val = (t * currColor) > (currColor * 10) ? (currColor * 10)
                                                 : (t * currColor);
        if (!val || (++t >= max)) {
            switch (dir) {
                case 1:
                    dir = -1;
                    break;
                case -1:
                    dir = 1;
                    zeroEnforcer = ENFORCE;
                    put_pixel(val);
                    sleep_ms(500);
                    break;
            }
        }
        if (zeroEnforcer) {
            put_pixel(val);
        }
    }
}

void pattern_solid (uint len, uint t){
    put_pixel(currColor * 3);
}


typedef void (*pattern)(uint len, uint t);
const struct {
    pattern pat;
    const char* name;
} pattern_table[] = {
    {pattern_fade, "Fade"},
    {pattern_solid, "Solid"}, 
};

// corresponds to the index of the mode
void ws2812ChangeMode(int val){
    int size = sizeof(pattern_table)/sizeof(pattern_table[0]);        
    if(val < size){
       currMode = val; 
    }
}

void ws2812Runner(void) {
    multicore_fifo_push_blocking(123);

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 1;
    while (1) {
        for (int i = 0; i < 1000; ++i) {
            pattern_table[currMode].pat(NUM_PIXELS, t);
            sleep_ms(10);
            if (!(i % 5)) {
                t += dir;
            }
        }
    }

    multicore_fifo_pop_blocking();
}


