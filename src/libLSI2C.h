#ifndef LETSSCREENLCD_H
#define LETSSCREENLCD_H

#include <stdbool.h>

typedef struct LetsScreenI2C {
  int address;
  int file;
  int lines;
  int linelen;
  bool backlight;
  bool blink;
  bool cursor;
  int *pinmapping;
} LetsScreenI2C;

int setupLcd(LetsScreenI2C *lcd, char *devicefile, int adress, bool backlight,
             bool blink, bool cursor, int lines, int linelengh, bool font5x10,
             int pinmapping[]);
int closeLcd(LetsScreenI2C *lcd);
int clearLcd(LetsScreenI2C *lcd);
int moveLcdCursor(LetsScreenI2C *lcd, int x, int y);
int reconfigureLcd(LetsScreenI2C *lcd, bool backlight, bool blink, bool cursor);
int writeToLcd(LetsScreenI2C *lcd, char text[]);

#endif
