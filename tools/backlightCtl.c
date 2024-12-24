#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void printUsageAndExit() {
  printf("Usage: ./backlightCtl <device file> <adress> <on|off>\n");
  exit(1);
}

int main(int argc, char **argv) {

  //
  // Open I2C connection to display
  // docs: https://www.kernel.org/doc/html/latest/i2c/dev-interface.html
  //

  if (argc != 4)
    printUsageAndExit();

  __u8 msg;
  if (!strcmp(argv[3], "on"))
    msg = 0x08; // Send 0x08 to turn backlight on
  else if (!strcmp(argv[3], "off"))
    msg = 0x00; // Send 0x00 to turn backlight off
  else
    printUsageAndExit();

  int addr;
  if (sscanf(argv[2], "%x", &addr) < 1)
    printUsageAndExit();

  int file = open(argv[1], O_RDWR);
  if (file < 0) {
    printf("Error while opening device file\n");
    exit(1);
  }

  if (ioctl(file, I2C_SLAVE, addr) < 0) {
    printf("Error while initialising I2C\n");
    exit(1);
  }

  //
  // Turn backlight on/off
  //

  if (i2c_smbus_write_byte(file, msg) < 0) {
    printf("Error sending byte\n");
  }

  if (close(file) < 0) {
    printf("Error while closing device file\n");
    exit(1);
  }

  return 0;
}
