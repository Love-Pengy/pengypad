/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

#include <stdio.h>

#define INPINSIZE 5
static uint8_t inPinArray[] = {29, 28, 27, 26, 15};

void initKeyboard(void) {
  for (int i = 0; i < INPINSIZE; i++) {
    gpio_init(inPinArray[i]);
    gpio_set_dir(inPinArray[i], GPIO_IN);
    gpio_pull_down(inPinArray[i]);
  }
}

// this assumes that initKeyboard was run first
uint32_t scanKeys(void) {
  // only one key should be active at a time atm so just take the first one
  // starting from 29
  for (int i = 0; i < INPINSIZE; i++) {
    if (gpio_get(inPinArray[i])) {
      // printf("%d\n", i);
      return (i);
    }
  }
  // printf("NONE FOUND\n");
  return (-1);
}

int main() {
  // green = key 1
  // yellow = key 2
  // white = key 3
  // blue = key 4
  // red = key 5
  stdio_init_all();
  initKeyboard();
  while (true) {
    printf("%d\n", scanKeys());
  }
}
