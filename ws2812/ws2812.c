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

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void pattern_fade(uint len, uint t) {
    (void)len;
    (void)t;
    int max = 50;  // let's not draw too much current!
    t %= max;

    uint32_t val;
    for (int i = 0; i < len; ++i) {
        val = (t * 0x10101) > (0x10101 * 10) ? (0x10101 * 10) : (t * 0x10101);
        if(!val){
            PAUSE_INDICATOR = true;
        }
        put_pixel(val);
        if (++t >= max) t = 0;
    }
}

void pattern_snakes(uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(urgb_u32(0, 0, 0xff));
        else
            put_pixel(0);
    }
}

void pattern_random(uint len, uint t) {
    if (t % 8) return;
    for (int i = 0; i < len; ++i) put_pixel(rand());
}

void pattern_sparkle(uint len, uint t) {
    if (t % 8) return;
    for (int i = 0; i < len; ++i) put_pixel(rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(uint len, uint t) {
    int max = 100;  // let's not draw too much current!
    t %= max;
    for (int i = 0; i < len; ++i) {
        put_pixel(t * 0x10101);
        if (++t >= max) t = 0;
    }
}

typedef void (*pattern)(uint len, uint t);
const struct {
    pattern pat;
    const char* name;
} pattern_table[] = {
    //{pattern_snakes,  "Snakes!"},
    //{pattern_random,  "Random data"},
    //{pattern_sparkle, "Sparkles"},
    //{pattern_greys, "Greys"},
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

    int t = 0;
    while (1) {
        int pat = rand() % count_of(pattern_table);
        /*int dir = (rand() >> 30) & 1 ? 1 : -1;*/
        int dir = -1;
        // puts(pattern_table[pat].name);
        // puts(dir == 1 ? "(forward)" : "(backward)");
        for (int i = 0; i < 1000; ++i) {
            if(PAUSE_INDICATOR){
                PAUSE_INDICATOR = false;
                sleep_ms(500);
            }
            pattern_table[pat].pat(NUM_PIXELS, t);
            sleep_ms(10);
            if (!(i % 5)) {
                t += dir;
            }
        }
    }
}
