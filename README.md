# Pengypad

A macropad made from "scratch" using [Tiny USB](https://github.com/hathach/tinyusb) and the pico sdk in C. 

## Things Used 
+ Microcontroller: RP2040 Zero  
+ Case: [Print Files](/printFiles/) 
+ Switches: Durock Shrimps 
+ Keycaps: 

## Build Instructions 
+ Prerequisites:    
    + must have pico sdk setup and ready 

## Configuration  
+ To find out what keycodes you need please refer to the hid [keycodes](https://github.com/hathach/tinyusb/blob/cfbdc44a8d099240ad5ef208bd639487c2f28153/src/class/hid/hid.h#L370)
+ Then change what the keys/encoder are mapped by changing the arrays in [mappings](src/mappings.h) 






