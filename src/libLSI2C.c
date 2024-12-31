#include "libLSI2C.h"

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <stdint.h>
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
  int output = 0;

  for (int i = 0; i < 8; i++) {
    int bit = (input & (1 << i)) >> i;
    int shift = positions[i];
    output |= bit << shift;
  }

  // return (input & 2);
  return output;
}

int make_cmd(LetsScreenI2C *lcd, int cmd, bool enable) {
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
  usleep(50); /* This isn't needed but makes me feel better :) */              \
  errno = i2c_smbus_write_byte(lcd->file, make_cmd(lcd, cmd, false));          \
  if (errno < 0)                                                               \
    return errno;

int setupLcd(LetsScreenI2C *lcd, char *devicefile, int adress, bool backlight,
             bool blink, bool cursor, int lines, int linelengh, bool font5x10,
             int pinmapping[]) {
  lcd->address = adress;
  lcd->lines = lines;
  lcd->linelen = linelengh;
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

  return errno;
}

int reconfigureLcd(LetsScreenI2C *lcd, bool backlight, bool blink,
                   bool cursor) {
  lcd->backlight = backlight;

  int errno = 0;
  send_cmd(0);
  usleep(50);
  int byte = 0b1100; // Display is always on
  if (blink)
    byte |= 0b1;
  if (cursor)
    byte |= 0b10;
  send_cmd(byte);
  usleep(50);

  return errno;
}

int clearLcd(LetsScreenI2C *lcd) {
  int errno = 0;
  send_cmd(0);
  usleep(50);
  send_cmd(0b1);
  usleep(20000);
  moveLcdCursor(lcd, 0, 0);
  return 0;
}

int moveLcdCursor(LetsScreenI2C *lcd, int x, int y) {
  // Line wrapping
  if (1 + x >= lcd->linelen) {
    y += (1 + x) / lcd->linelen;
    x = x % lcd->linelen;
  }
  if (1 + y >= lcd->lines)
    y = y % lcd->lines;

  lcd->cursorx = x;
  lcd->cursory = y;

  int errno = 0;
  // See HD44780 datasheet page 29
  // Adress is 0x0 to 0x4F for 1line display
  // Adress is 0x0 to 0x27, then 0x40 to 0x67 for 2line display
  // 4line display acts like 2line display, but lines are split:
  // 1st line is 1st half of first line, 2nd line is 1st half of second line,
  // 3rd line is 2nd hald to first line, 4th line is 2nd halt of second line
  if (lcd->lines == 1) { // We can ignore y
    send_cmd(0b1000 | (x >> 4));
    usleep(50);
    send_cmd(x & 0b1111);
    usleep(50);
  } else if (lcd->lines == 2) {
    int addr = (lcd->linelen * y) + x;
    // Bridge the gap from 0x27 to 0x40 (see comment above)
    if (addr > 39)
      addr += 24;
    send_cmd(0b1000 | (addr >> 4));
    usleep(50);
    send_cmd(addr & 0b1111);
    usleep(50);
  } else if (lcd->lines == 4) {
    // Translate 1 2 3 4 to 1 3 2 4 (see comment above)
    if (y == 2)
      y = 1;
    else if (y == 1)
      y = 2;
    int addr = (lcd->linelen * y) + x;
    // Bridge the gap from 0x27 to 0x40 (see comment above)
    if (addr > 39)
      addr += 24;
    send_cmd(0b1000 | (addr >> 4));
    usleep(50);
    send_cmd(addr & 0b1111);
    usleep(50);
  }
  return errno;
}

#define write_char(c)                                                          \
  send_cmd(0b100000 | (c >> 4));                                               \
  usleep(50);                                                                  \
  send_cmd(0b100000 | (c & 0b1111));                                           \
  usleep(50);                                                                  \
  lcd->cursorx++;                                                              \
  if (lcd->cursorx == lcd->linelen) {                                          \
    moveLcdCursor(lcd, 0, lcd->cursory + 1);                                   \
  }

int writeToLcd(LetsScreenI2C *lcd, char text[]) {
  bool done = false;
  int errno = 0;

  for (int i = 0; !done; i++) {
    uint8_t c = text[i];
    if (c == 0) {
      done = true;

    } else if (c < 0b100000) {
      if (c == '\n') { // Line break with '\n'
        moveLcdCursor(lcd, 0, lcd->cursory + 1);
      } else
        printf("[LSI2C] WARNING: ASCII character Nr. %d not supported by "
               "display!\n",
               c);

    } else if (c < 0b10000000) { // Normal ASCII char
      // The display ROM maps ASCII backslash (\) to Yen symbol.
      // Let's map it to normal slash (/) instead.
      if (c == '\\')
        c = '/';
      write_char(c);

    } else if (c < 0b11000000) { // UTF-8 continuation; must not happen
      printf("[LSI2C] ERROR: unexpected UTF-8 continuation! This may be a bug "
             "in LSI2C! Please open an issue: "
             "https://github.com/Letsric/LSI2C/issues/new\n");
      return -2;

    } else if (c < 0b11100000) { // UTF-8 two bit encoding
      uint16_t full = (c << 8) + (uint8_t)text[i + 1];
      i += 1; // Skip next byte, already processed
      switch (full) {
      case 0b1100001010100101:
        write_char(0b01011100); // Map ¥
        break;
      case 0b1100001110100100:
        write_char(0b11100001); // Map ä
        break;
      case 0b1100001110110110:
        write_char(0b11101111); // Map ö
        break;
      case 0b1100001110111100:
        write_char(0b11110101); // Map ü
        break;
      default: // I'm too lazy to map all supported symbols, also, that would
               // require a more sophisticated approach
        printf(
            "[LSI2C] WARNING: character may be printable to display, but LSI2C "
            "doesn't support it. skipping.\n");
      }

    } else if (c < 0b11110000) { // UTF-8 three bit encoding
      uint32_t full =
          (c << 16) + ((uint8_t)text[i + 1] << 8) + (uint8_t)text[i + 2];
      i += 2; // Skip next two bytes, already processed
      switch (full) {
      case 0b111000101001011010100001:
        write_char(0b11011011); // Map □
        break;
      case 0b111000101001011010001000:
        write_char(0b11111111); // Map █
        break;
      default: // I'm too lazy to map all supported symbols, also, that would
               // require a more sophisticated approach
        printf(
            "[LSI2C] WARNING: character may be printable to display, but LSI2C "
            "doesn't support it. skipping.\n");
      }

    } else if (c < 0b11111000) { // UTF-8 four bit encoding
      printf(
          "[LSI2C] WARNING: character may be printable to display, but LSI2C "
          "doesn't support it. skipping.\n");
      i += 3; // Skip next two bytes, already "processed"

    } else {
      printf("[LSI2C] ERROR: character to write has non UTF-8 encoding!\n");
      return -2;
    }
  }

  return errno;
}

int closeLcd(LetsScreenI2C *lcd) {
  if (close(lcd->file) < 0) {
    printf("Error closing device file\n");
    return -1;
  }
  return 0;
}
