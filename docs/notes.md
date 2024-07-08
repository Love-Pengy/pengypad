# General Notes And Todo

<!--toc:start-->
- [General Notes And Todo](#general-notes-and-todo)
  - [Overall Idea](#overall-idea)
  - [Expected Path](#expected-path)
  - [Notes](#notes)
  - [Descriptors](#descriptors)
    - [Desired Desciptor Vals](#desired-desciptor-vals)
      - [Device Descriptor](#device-descriptor)
      - [Configuration Descriptor](#configuration-descriptor)
      - [Interface Descriptor](#interface-descriptor)
      - [Enpoint Descriptor](#enpoint-descriptor)
      - [String Descriptor](#string-descriptor)
        - [Language Descriptor](#language-descriptor)
        - [String Descriptor](#string-descriptor)
    - [Communication Notes](#communication-notes)
  - [**USB ENUMERATION (STARTUP)**
-](#usb-enumeration-startup)
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

- vendor id is the sequence next to ID in lsusb and the PID si the human sting
  - lsusb -v -d \<VID\>

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

### Desired Desciptor Vals

#### Device Descriptor

blength: sizeof(HIDDescriptor);  
bdescriptorType: 0x01 for the device descriptor
bcdUSB: 0x0110

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
bmAttributes: return to this (inerrupt transfer, and must also do control transfer)
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

### Communication Notes

(page 572 in MCU book)

- NOTE: in and out are from the perspective of the host
  - ex. in is when the device is sending data to the host
- there are three types of packets.
  - token packet
    - sent by host
      - |SYNC (8 bits)|PID (8 bits)|ADDR (7 bits)|ENDP (4 bits)|CRC5 (5 bits)|EOP (3 bits)|
  - data packet
    - sent by device
      - |SYNC (8 bits)|PID (8 bits)|DATA (0-1023 bits)|CRC16 (16 bits)|EOP (3 bits)|
  - acknowledge packet
    - send by host for in or by devicce for out
      - |SYNC (8 bits)|PID (8 bits)|EOP (3 bits)|
  - SYNC: this ensures that the clock is synced with both host nad device. SYNC byte is always 0b00000001
  - PID: this is the type of packet being sent. The lower four bits of the PID byte ar ethe inverse of the upper four. The least signifivant bit is sent out first.
    - ex. 0b10000111 is actually 0b0001
  - ADDR: Host assigned address. Device will use an address of 0 during the initial communication
  - ENDP: Uses four bits to identify 16 endpoints within a USB device
  - Data Field: this is the data portion. THe field size just depends on the transfer speed and type of that comm
  - CRC: This is the section used for payload corruption
    - This calculation is done liek the following: the data bits to be protected are treated as a binary number. After that divide the binary number by another predefined binary number. Select the remainder of the division as the crc code
  - EOP: EOP consists of SECO for two time units of a bit and j state for one time unit

# EXAMPLE OF SIMBLE KEEB DEVICE

```
Bus 001 Device 056: ID 03f0:2f4a HP, Inc HP Business Slim Keyboard
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0 
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0         8
  idVendor           0x03f0 HP, Inc
  idProduct          0x2f4a 
  bcdDevice            0.11
  iManufacturer           1 Chicony
  iProduct                2 HP Business Slim Keyboard
  iSerial                 0 
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x003b
    bNumInterfaces          2
    bConfigurationValue     1
    iConfiguration          0 
    bmAttributes         0xa0
      (Bus Powered)
      Remote Wakeup
    MaxPower              100mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      1 Boot Interface Subclass
      bInterfaceProtocol      1 Keyboard
      iInterface              0 
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.10
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength      65
         Report Descriptors: 
           ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0008  1x 8 bytes
        bInterval              10
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 
      bInterfaceProtocol      0 
      iInterface              0 
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.10
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength     103
         Report Descriptors: 
           ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x82  EP 2 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0008  1x 8 bytes
        bInterval              10
```
