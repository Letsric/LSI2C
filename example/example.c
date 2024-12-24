#include "../src/libLSI2C.h"

LetsScreenLCD lcd;

int main() {

  // pin nr.     | 7  | 6  | 5  | 4   | 3   | 2   | 1   | 0
  // meaning     | EN | BL | RS | R/W | DB7 | DB6 | DB5 | DB4
  // pin mapping | 2  | 3  | 0  | 1   | 7   | 6   | 5   | 4

  LetsScreenI2CInit(&lcd, "/dev/i2c-1", 0x27, false, false, false, 4, true,
                    (int[]){4, 5, 6, 7, 1, 0, 3, 2});
  closeLcd(&lcd);
}
