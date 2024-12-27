#ifndef LETSSCREENLCD_H
#define LETSSCREENLCD_H

#include <stdbool.h>

typedef struct LetsScreenI2C {
  int address;
  int file;
  bool backlight;
  bool blink;
  bool cursor;
  int *pinmapping;
} LetsScreenI2C;

int setupLcd(LetsScreenI2C *lcd, char *devicefile, int adress, bool backlight,
             bool blink, bool cursor, int lines, bool font5x10,
             int pinmapping[]);
int closeLcd(LetsScreenI2C *lcd);
int reconfigureLcd(LetsScreenI2C *lcd, bool backlight, bool blink, bool cursor);

#endif
