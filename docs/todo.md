# TODO

<!--toc:start-->
- [TODO](#todo)
<!--toc:end-->

- ~~understand usb descriptors~~
- ~~figure out how to get the on board led working~~
- ~~make the fade go fade back up rather than the abrupt pulse~~
- ~~make flash mode for ws2812~~
  - im a little silly and forgot I can just print :3
- ~~make colors for different states~~
  - ~~maybe blue for mounted~~
  - ~~red for unmounted~~
  - ~~yellow for suspended~~
- ~~learn threads so you can do one thread for led one thread for actual keeb stuff~~
  - ~~couldn't get the threads library to import properly (the posix ones) so just refactoring ws2812 instead~~ I lied
  - ~~use the multicore pico lib to throw the keyboard functionality on one core and then put the led status on the other~~
  - ~~move everything from ws2812 into main.c for the keyboard so that we can implement the main portion of ws2812 as a thread function~~ didn't need to do this 
- ~~import ws2812 into the actual tusb code~~
- ~~get usb device to show up~~

- change usb device name to something cool
