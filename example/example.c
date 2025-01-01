#include <stdbool.h>
#include <unistd.h>

#include "LSI2C.h"

LetsScreenI2C lcd;

int main() {

  // setupLcd sets everything up. It takes these parameters: Pointer to
  // LetsScreenI2C struct, I²C adress, enable backlight?, show cursor?, blink?,
  // number of lines, number of characters per line, should 5x10 font be used
  // instead of 5x8 font?, Pinmapping

  // pin nr.     | 7  | 6  | 5  | 4   | 3   | 2   | 1   | 0
  // meaning     | EN | BL | RS | R/W | DB7 | DB6 | DB5 | DB4
  // EXAMPLE POSITIONS:
  //             | 2  | 3  | 0  | 1   | 7   | 6   | 5   | 4
  // NOTE: Pinmapping takes pin positions from right to left!

  setupLcd(&lcd, "/dev/i2c-1", 0x27, false, false, false, 4, 20, true,
           (int[]){4, 5, 6, 7, 1, 0, 3, 2});

  sleep(1);

  // Write some Text to the Display
  // NOTE: Only ASCII is fully supported in LSI2C. Check HD44780U datasheet to
  // see all supported characters. If you need them, just add them in
  // `libLSI2C.c`. Feel free to PR if you do so! :)

  writeToLcd(&lcd, "Hello! ¥ \\ äöü █");

  sleep(1);

  // You can reconfigure the state of backlight, cursor and blink
  reconfigureLcd(&lcd, true, true, true);

  sleep(1);

  // Automatic line breaking demonstration
  char text[] = "Testing....";
  for (int i = 0; i < 125; i++) {
    writeToLcd(&lcd, (char[]){text[i % 11], 0});
    usleep(100000);
  }

  sleep(1);

  // You can use the '\n' character to line break too!
  clearLcd(&lcd);
  writeToLcd(&lcd, "Hello!\n"
                   "Loading...\n"
                   "[██████   ]");

  sleep(1);

  // Use moveLcdCursor to move to move the cursor (duh)
  moveLcdCursor(&lcd, 5, 0);
  writeToLcd(&lcd, " World");

  sleep(1);

  // Call closeLcd when you're done
  closeLcd(&lcd);
}
