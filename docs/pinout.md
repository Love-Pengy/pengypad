# PIN OUT

<!--toc:start-->
- [PIN OUT](#pin-out)
<!--toc:end-->

- I2C
  - pin 0
    - SDA
  - pin 1
    - SCL

- READER = the pin that checks the value
- OUTPUT = is just the 5v pin
  - should we just leave it at high or set it back to 0 every time
STARTING FROM UNDER THE ENCODER
- KEY 1
  - OUTPUT: 5V  
  - READER: 14
- KEY 2
  - OUTPUT: 5V
  - READER: 10
- KEY 3
  - OUTPUT: 5V
  - READER: 7
- KEY 4
  - OUTPUT: 5V
  - READER: 4
THIS ONE IS THE ONE RIGHT UNDER THE MCU
- KEY 5
  - OUTPUT: 5V
  - READER: 2
- ENCODER: VOL UP/VOL DOWN
  - [PIN OUT](./references)
  - looking at it from the two prong side 
    - encoder
        - 27 is left 
        - 26 is middle
        - 15 is right
    - button 
        - 29 is left 
        - 28 is right
