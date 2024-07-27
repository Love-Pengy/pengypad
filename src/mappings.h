#pragma once
#include "pico/stdlib.h"
#include "tusb.h"
#define HID_KEY_PREVIOUS_SONG 0xea
#define HID_KEY_NEXT_SONG 0xeb
#define HID_KEY_PLAY_PAUSE 0xe8

const uint8_t keyMappings[9][6] = {
    // invalid
    {0, 0, 0, 0, 0, 0},
    // key 1 (next)
    {HID_KEY_NEXT_SONG, 0, 0, 0, 0, 0},
    // key 2 (prev)
    {HID_KEY_PREVIOUS_SONG, 0, 0, 0, 0, 0},
    // key3 (play pause)
    {HID_KEY_PLAY_PAUSE, 0, 0, 0, 0, 0},
    // key 4 (toggle pomo timer)
    {HID_KEY_ALT_RIGHT, HID_KEY_SHIFT_RIGHT, HID_KEY_P, 0, 0, 0},
    // key 5 (suspend menu)
    {HID_KEY_ALT_RIGHT, HID_KEY_SHIFT_RIGHT, HID_KEY_X, 0, 0, 0},
    // encoder CW (vol up)
    {HID_KEY_VOLUME_UP, 0, 0, 0, 0, 0},
    // encoder CCW (vol down)
    {HID_KEY_VOLUME_DOWN, 0, 0, 0, 0, 0},
    // encoder button (mute)
    {HID_KEY_MUTE, 0, 0, 0, 0, 0},
};
