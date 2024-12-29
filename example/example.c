#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "../src/libLSI2C.h"

LetsScreenI2C lcd;

int main() {

  // pin nr.     | 7  | 6  | 5  | 4   | 3   | 2   | 1   | 0
  // meaning     | EN | BL | RS | R/W | DB7 | DB6 | DB5 | DB4
  // pin mapping | 2  | 3  | 0  | 1   | 7   | 6   | 5   | 4

  printf("calling setupLcd\n");
  setupLcd(&lcd, "/dev/i2c-1", 0x27, false, false, false, 4, true,
           (int[]){4, 5, 6, 7, 1, 0, 3, 2});

  sleep(1);

  printf("calling wirteText\n");
  writeToLcd(&lcd, "Hello! ¥ \\ äöü █");

  sleep(1);

  printf("calling reconfigureLcd\n");
  reconfigureLcd(&lcd, true, true, true);

  sleep(1);

  printf("calling closeLcd\n");
  closeLcd(&lcd);
}
