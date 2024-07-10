#!/bin/sh
export PICO_SDK_PATH="~/Applications/pico/pico-sdk/"
rm -rf build
mkdir build 
cd build 
cmake ..
make -j8
