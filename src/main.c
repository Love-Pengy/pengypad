#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "encoder/encoder.h"
#include "keyScan/keyScan.h"
#include "mappings.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "ws2812/ws2812.h"

void led_blinking_task(void);
void hid_task(void);

int main(void) {
    stdio_init_all();
    multicore_launch_core1(ws2812Runner);
    multicore_fifo_pop_blocking();
    // have no clue what the flag value does (123)
    multicore_fifo_push_blocking(123);

    board_init();
    tusb_init();
    initKeyboard();
    initEncoder();
    while (1) {
        // causes ws2812Runner to break
        tud_task();
        // led_blinking_task();
        hid_task();
    }
}

// Invoked when device is mounted
void tud_mount_cb(void) {
    ws2812ChangeStatus(0);
    ws2812ChangeMode(1);
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    ws2812ChangeStatus(1);
    ws2812ChangeMode(0);
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from
// bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    ws2812ChangeStatus(2);
    ws2812ChangeMode(0);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    ws2812ChangeStatus(0);
    ws2812ChangeMode(1);
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t keyPressed) {
    // skip if hid is not ready yet
    if (!tud_hid_ready()) {
        return;
    }

    // use to avoid send multiple consecutive zero report for keyboard
    static bool has_keyboard_key = false;

    if (keyPressed && (keyPressed > 0 && keyPressed < 10)){
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keyMappings[keyPressed]);
        has_keyboard_key = true;
    }
    else {
        // send empty key report if previously has key pressed
        if (has_keyboard_key)
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
    }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse
// etc
// ..) tud_hid_report_complete_cb() is used to send the next report after
// previous one is complete
void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) {
        return;  // not enough time
    }
    start_ms += interval_ms;

    // for testing purposes we are assuming that it is always pressed
    uint32_t const btn = scanKeys();

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
    else {
        // Send the 1st of report chain, the rest will be sent by
        send_hid_report(REPORT_ID_KEYBOARD, btn);
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(
    uint8_t instance, uint8_t const* report, uint16_t len) {
    (void)instance;
    (void)len;

    uint8_t next_report_id = report[0] + 1;

    if (next_report_id < REPORT_ID_COUNT) {
        send_hid_report(next_report_id, scanKeys());
    }
}
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(
    uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
    uint8_t* buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(
    uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
    uint8_t const* buffer, uint16_t bufsize) {
    (void)instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            // bufsize should be (at least) 1
            if (bufsize < 1) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
                // Capslock On: disable blink, turn led on
            }
            else {
                // Caplocks Off: back to normal blink
                ws2812ChangeStatus(0);
            }
        }
    }
}
