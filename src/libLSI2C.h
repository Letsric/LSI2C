#ifndef LETSSCREENLCD_H
#define LETSSCREENLCD_H

#include <stdbool.h>

typedef struct LetsScreenLCD {
  int address;
  int file;
  bool backlight;
  bool blink;
  bool cursor;
  int *pinmapping;
} LetsScreenLCD;

int LetsScreenI2CInit(LetsScreenLCD *lcd, char *devicefile, int adress,
                      bool backlight, bool blink, bool cursor, int lines,
                      bool font5x10, int pinmapping[]);
int closeLcd(LetsScreenLCD *lcd);

#endif
