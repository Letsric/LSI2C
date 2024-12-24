#include "libLSI2C.h"

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * This function takes a binary number and reorders the bits.
 * The array positions[] should contain the new positions of the bits (right
 * to left)
 *
 * Example:
 * input: 0b10101010,
 * positions: {0, 4, 1, 5, 2, 6, 3, 7},
 * returns: 0b11110000
 *
 * In this case, the zero at the far right doesn't get moved (from pos. 0 to
 * pos. 0). The next bit (the 2nd from right) gets moved to position 4 from
 * right. The next bit (the 3rd from right) gets moved to position 1 from
 * right. and so on...
 */
int convertBits(int input, int positions[]) {
  int len = floor(log2(input)) + 1;
  int output = 0;

  for (int i = 0; i < len; i++) {
    int bit = (input & (1 << i)) >> i;
    int shift = positions[i];
    output |= bit << shift;
  }

  // return (input & 2);
  return output;
}

int make_cmd(LetsScreenLCD *lcd, int cmd, bool enable) {
  if (lcd->backlight)
    cmd |= 0b01000000;
  if (enable)
    cmd |= 0b10000000;
  return convertBits(cmd, lcd->pinmapping);
}

#define send_cmd(cmd)                                                          \
  errno = i2c_smbus_write_byte(lcd->file, make_cmd(lcd, cmd, true));           \
  if (errno < 0)                                                               \
    return errno;                                                              \
  errno = i2c_smbus_write_byte(lcd->file, make_cmd(lcd, cmd, false));          \
  if (errno < 0)                                                               \
    return errno;

int LetsScreenI2CInit(LetsScreenLCD *lcd, char *devicefile, int adress,
                      bool backlight, bool blink, bool cursor, int lines,
                      bool font5x10, int pinmapping[]) {
  lcd->address = adress;
  lcd->backlight = backlight;
  lcd->blink = blink;
  lcd->cursor = cursor;
  lcd->pinmapping = pinmapping;
  lcd->file = open(devicefile, O_RDWR);
  if (lcd->file < 0) {
    printf("Error while opening device file\n");
    return -1;
  }

  if (ioctl(lcd->file, I2C_SLAVE, lcd->address) < 0) {
    printf("Error while initialising I2C\n");
    return -1;
  }

  int errno = 0;

  // Initialization for 4-bit interface (see page 46 on HD44780U datasheet)
  send_cmd(0b11); // Function set for init
  usleep(4150);
  send_cmd(0b11); // Function set for init
  usleep(150);
  send_cmd(0b11); // Function set for init
  usleep(50);
  send_cmd(0b10);
  usleep(50); // now display is in 4-bit-mode

  // Function set
  send_cmd(0b10);
  usleep(50);
  int byte = 0;
  if (lines > 1)
    byte |= 0b1000;
  if (font5x10)
    byte |= 0b100;
  send_cmd(0b1100); // Set lines and font
  usleep(50);

  // Display on/off/cursor/blink control
  send_cmd(0);
  usleep(50);
  byte = 0b1100; // Display is always on
  if (blink)
    byte |= 0b1;
  if (cursor)
    byte |= 0b10;
  send_cmd(byte);
  usleep(50);

  // clear display
  send_cmd(0);
  usleep(50);
  send_cmd(0b1);
  usleep(20000);

  // set entry mode
  send_cmd(0);
  usleep(50);
  send_cmd(0b110);
  usleep(50);

  return 0;
}

int closeLcd(LetsScreenLCD *lcd) {
  if (close(lcd->file) < 0) {
    printf("Error closing device file\n");
    return -1;
  }
  return 0;
}
