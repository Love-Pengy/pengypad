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
- KEY 1
  - OUTPUT: 5V  
  - READER: 29
- KEY 2
  - OUTPUT: 5V
  - READER: 28
- KEY 3
  - OUTPUT: 5V
  - READER: 27
- KEY 4
  - OUTPUT: 5V
  - READER: 26
- KEY 5
  - OUTPUT: 5V
  - READER: 15
- ENCODER: VOL UP/VOL DOWN
  - [PIN OUT](./references)
  - looking at it from the two prong side 
    - encoder
        - 8 is left 
        - 9 is middle
        - 10 is right
    - button 
        - 11 is left 
        - 12 is right
