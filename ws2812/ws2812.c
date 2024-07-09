/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"

#define IS_RGBW true
#define NUM_PIXELS 1
#define WS2812_PIN 16
static bool PAUSE_INDICATOR = false;
// start going up
static int dir = 1;

enum{
    ENFORCE = 0, 
    SIDEEYE, 
};

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void pattern_fade(uint len, uint t) {
    (void)len;
    (void)t;
    int max = 35;  
    t %= max;

    uint32_t val;
    int zeroEnforcer = SIDEEYE;
    for (int i = 0; i < len; ++i) {
        val = (t * 0x10101) > (0x10101 * 10) ? (0x10101 * 10) : (t * 0x10101);
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
        if(zeroEnforcer){
            put_pixel(val);
        }
        //if (++t >= max){ t = 0; printf("MAXX\n");}
    }
}

typedef void (*pattern)(uint len, uint t);
const struct {
    pattern pat;
    const char* name;
} pattern_table[] = {
    {pattern_fade, "Fade"},
};

int main() {
    // set_sys_clock_48();
    stdio_init_all();
    // printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 1;
    while (1) {
        int pat = 0;
        for (int i = 0; i < 1000; ++i) {
            pattern_table[pat].pat(NUM_PIXELS, t);
            sleep_ms(10);
            if (!(i % 5)) {
                t += dir;
            }
        }
        //sleep_ms(2000);
    }
}
