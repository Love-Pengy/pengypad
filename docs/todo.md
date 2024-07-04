# General Notes And Todo

<!--toc:start-->
- [General Notes And Todo](#general-notes-and-todo)
  - [Overall Idea](#overall-idea)
  - [Expected Path](#expected-path)
  - [Notes](#notes)
  - [Descriptors](#descriptors)
    - [DESIRED VALS](#desired-vals)
      - [DEVICE DESCRIPTOR](#device-descriptor)
      - [Configuration Descriptor](#configuration-descriptor)
      - [Interface Descriptor](#interface-descriptor)
      - [Enpoint Descriptor](#enpoint-descriptor)
      - [String Descriptor](#string-descriptor)
        - [Language Descriptor](#language-descriptor)
        - [String Descriptor](#string-descriptor)
<!--toc:end-->

## Overall Idea

- Create a macropad from scratch using rp2040 chip and fully custom firmware

## Expected Path

- learn how the usb/hid spec works
  - Need to knows  
    - How computer knows that its an hid device
    - How to tell the computer what type of device it is and what its name is (name tbd)
    - Find out what the protocal for sending information to the computer is
    - Given that information what sequence do we need to send for the following keys
      - Skip
      - Prev
      - Mute/unmute
      - Pause/play my pomo timer
      - Suspending computer
      - Encoder would be vol up vol down
- Build out the pad itself hardware wise
- Implement debouncing

## Notes

- device descriptor
  - packet of information that describes the device
- endpoint descriptor
  - descriptor describing the type and capabilities
- hid descriptor
  - information about usb devices
- interface descriptor
  - describes the device as an HID device
- string descriptor
  - a table of text used by one or more descriptors

## Descriptors

- the computer knows what type if device it is and the name is from the destriptor

- spec for all descriptors: <https://www.beyondlogic.org/usbnutshell/usb5.shtml#ConfigurationDescriptors>

```C
// see Device Class Definition for Human Interface Devices (HID) Version 1.11 || page 22
struct HIDDescriptor{
  uint8_t  bLength            ; ///< Size of this descriptor in bytes. 1 byte
  uint8_t  bDescriptorType    ; ///< DEVICE Descriptor Type.
  uint16_t bcdUSB             ; ///< BUSB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H).

  uint8_t  bDeviceClass       ; ///< Class code (assigned by the USB-IF). 2 bytes
  uint8_t  bDeviceSubClass    ; ///< Subclass code (assigned by the USB-IF).
  uint8_t  bDeviceProtocol    ; ///< Protocol code (assigned by the USB-IF).
  uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid). For HS devices is fixed to 64.

  uint16_t idVendor           ; ///< Vendor ID (assigned by the USB-IF).
  uint16_t idProduct          ; ///< Product ID (assigned by the manufacturer).
  uint16_t bcdDevice          ; ///< Device release number in binary-coded decimal.
  uint8_t  iManufacturer      ; ///< Index of string descriptor describing manufacturer.
  uint8_t  iProduct           ; ///< Index of string descriptor describing product.
  uint8_t  iSerialNumber      ; ///< Index of string descriptor describing the device's serial number.

  uint8_t  bNumConfigurations ; ///< Number of possible configurations.-- 
    /* OPTIONALS
    DescriptorType //Name specifying typhe of optional descriptor 
    DescriptorLength //Total size of optional descriptor
    */
    
}    
```

### Desired Vals

#### Device Descriptor

blength: sizeof(HIDDescriptor);  
bdescriptorType: 0x01 for the device descriptor
bcdUSB: 0x110H

bDeviceClass: 0x0
bDeviceSubClass: 0x0
bDeviceProtocol: 0x0
bMaxPacketSize0: come back to this

idVendor: 0xBee3  
idProduct: come back to this
bcdDevice: 0x001H// looking for this to be usb 1.1
iManufacturer: DEPENDS ON STRING DESC
iProduct  DEPENDS ON STRING DESC
iSerialNumber  DEPENDS ON STRING DESC

bNumConfigurations 0x01

#### Configuration Descriptor

bLength: sizeof(whatever you name this struct)
bDescriptorType: 0x02
wTotalLength:
bNumInterfaces: 0x01
bConfigurationValue: 0x0
iConfiguration: 0

bmAttributes: D5
bMaxPower: 50 (double check this please)

#### Interface Descriptor

bLength: sizeof(whatever you name this struct)
bDescriptorType: 0x04
bInterfaceNumber: 0x0
bAlternateSetting: ?
bNumEndpoints: (should be the same value described above)
bInterfaceClass: 0x03h
bInterfaceSubClass: return to this
bInterfaceProtocol: return to this
iInterface: (should come from the string descriptor)

#### Enpoint Descriptor

bLength: sizeof(whatever you call the struct)
bDescriptorType: 0x05
bEndpointAddress: return to this
bmAttributes: return to this
wMaxPacketSize: return to this (depends on bmAttributes)
bInterval: not quite sure whats a suitable value for this

#### String Descriptor

**NOTE: STRING DESCRIPTORS ARE UNICODE NOT ASCII**

##### Language Descriptor

come back to all of this cause I need to think this through~
bLength: sizeof(whatever you call the struct)
bDescriptorType: 0x03
wLandID[]: hold the lang id (english is 0x0409)

##### String Descriptor

bLength: sizeof(whatever you call this descriptor)
bDescriptorType: 0x03
bString[]: the strings for the other descriptors

- VID (vendor id) and PID (product id) can not be the same

- us country code is 33~
