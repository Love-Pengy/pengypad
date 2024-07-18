#include "usb_descriptors.h"

#include "pico/unique_id.h"
#include "tusb.h"

// TAKEN FROM EXAMPLE PLAY WITH LATER
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID \
    (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
     _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

#define USB_VID 0xCafe
#define USB_BCD 0x0200
//////////////////////////////////////

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const description_hid_report[] = {
    // this creates our entire report structure
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1)),
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_hid_descriptor_report_cb(uint8_t instance) {
    (void)instance;
    return description_hid_report;
}

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const device_descriptor = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,  // specifies that this is a device descriptor
    .bcdUSB = USB_BCD,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,

    // These are indexes of the string descriptor make sure they match up
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01

};

// Called when we get GET DEVICE DESCRIPTOR
// for this we just return the pointer to the descriptor
uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&device_descriptor;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

// assign interfaces with config indexes
enum { ITF_NUM_HID, ITF_NUM_TOTAL };

// this is the endpoint address. If I understand this right you start at this
// address and increatement for every config discriptor (I only will have one)
#define EPNUM_HID 0x81

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

// THIS SETUP IS BASICALLY THIS:

uint8_t const description_configuration[] = {
    // THIS IS CONFIGURATION DESCRIPTOR IN NOTES
    // Config number, interface count, string index, total length, attribute,
    // power in mA (NOT DIVIDED BY TWO FUNCTION HANDLES THIS)
    TUD_CONFIG_DESCRIPTOR(
        1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    // THIS IS INTERFACE DESCRIPTOR IN NOTES
    // Interface number, string index, protocol, report descriptor len, EP In
    // address, size & polling interval
    TUD_HID_DESCRIPTOR(
        ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(description_hid_report),
        EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5)};

// Called when we get GET CONFIGURATION DESCRIPTOR
// return a pointer as alaways
uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // this is a cool way of having an unused parameter!!!
    return (description_configuration);
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// apparently buff to hold flash ID within (I dont quite know why we need this
// yet)
//  this is set when requested
char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

// array of pointer to string descriptors
char const* string_desciptor_arr[] = {
    (const char[]){
        0x09, 0x04},  // index 0: supported language English (code is 0x0409)
    "Pengy Pro.",         // index 1: Manufacturer
    "PengyPad",  // index 2: product name
    serial,           // index 3: our seral id
};

static uint16_t description_string[32];

// called when we get a GET STRING DESCRIPTOR request
// return pointer to descriptor (who would have thunk it :3)
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;  // genuinly why didn't someone tell me this before this is
                   // awesome
    // var to keep track of size of return
    uint8_t char_count;
    // DO NOT FORGET THESE ARE UINT16 THEREFORE TWO BYTES OR FOUR HEX FIT INTO
    // ONE INDEX
    if (index == 0) {
        // we copy two bytes because we have four hex numbers
        memcpy(&description_string[1], string_desciptor_arr[0], 2);
        char_count = 1;
    }
    else {
        // handle second special case of serial id
        if (index == 3) {
            pico_get_unique_board_id_string(serial, sizeof(serial));
        }

        // check for invalid index. THis is only doable because its a [] array
        // not a malloced one
        if (index >
            sizeof(string_desciptor_arr) / sizeof(string_desciptor_arr[0])) {
            return (NULL);
        }

        const char* string = string_desciptor_arr[index];

        char_count = strlen(string);
        if (char_count > 31) {
            char_count = 31;
        }

        // They only take unicode strings so we have to convert it!
        for (uint8_t i = 0; i < char_count; i++) {
            // we are taking 8 bit chars and putting them into uint16 format so
            // every other uint16 we put one 8 bit char
            description_string[1 + i] = string[i];
        }
    }
    // This is where we handle the rest of the string descriptor
    // first byte is the length second byte second byte is the type id for
    // string desc
    description_string[0] = (0x03 << 8) | (2 * char_count + 2);

    return (description_string);
}
