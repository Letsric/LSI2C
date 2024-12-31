LSI2C
======

A C Library for Liquid Crystal screens with I²C Backpack for Linux

Compatible with HD44780 screens and PCF574 based backpacks

# Demo video

[![demo video](http://img.youtube.com/vi/vd55fGJ4qyo/0.jpg)](http://www.youtube.com/watch?v=vd55fGJ4qyo)

# Usage

## Requirements

- An LCD with I²C Backpack (like [this one](https://www.az-delivery.de/products/hd44780-2004-lcd-display-bundle-4x20-zeichen-mit-i2c-schnittstelle-gruen?_pos=5&_sid=8ff954a91&_ss=r)) 1-Line and 2-Line displays *should* word, but **only a 4-Line display is tested and confirmed to word!** If you notice unintended behavior, please open an Issue on Github.
- A I²C capable Linux device (make sure to enable I²C in `raspi-config` for Raspberry Pi)
- build essentials (gcc, make, ...)
- Linux headers
- libi2c (for `i2c/smbus.h`)
- i2c-tools (only needed for scanning) (In some distros, this also includes `i2c/smbus.h`)

For Raspberry Pi OS:
```bash
sudo apt install build-essential raspberrypi-kernel-headers libi2c-dev i2c-tools
```

## Building

simply run `make` to build everything.

## Other make targets

- **lib**: compile `libLSI2C.so`
- **tools**: compile files in tools/ folder
- **example**: compile example
- **run**: compile and run example

# Documentation

## Datasheets & Resources

- **HD44780**: <https://www.sparkfun.com/datasheets/LCD/HD44780.pdf>
- **PCF574**: <https://www.ti.com/lit/ds/symlink/pcf8574.pdf>
- **Linux I²C docs**: <https://docs.kernel.org/i2c/index.html> (especially "Implementing I2C device drivers in userspace")

## I²C Device File and Address

The I²C device file corresponds to your I²C controller. To list them, run:
```bash
i2cdetect -l

# Example Output:
# i2c-1   i2c         bcm2835 (i2c@7e804000)            I2C adapter
# i2c-20  i2c         fef04500.i2c                      I2C adapter
# i2c-21  i2c         fef09500.i2c                      I2C adapter
```

You can then use `i2cdetect <bus-number>` to scan a bus:
```bash
i2cdetect 1
# Example output:
# WARNING! This program can confuse your I2C bus, cause data loss and worse!
# I will probe file /dev/i2c-1.
# I will probe address range 0x08-0x77.
# Continue? [Y/n]
#      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
# 00:                         -- -- -- -- -- -- -- --
# 10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 20: -- -- -- -- -- -- -- 27 -- -- -- -- -- -- -- --
# 30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 70: -- -- -- -- -- -- -- --

i2cdetect 20
# Example output:
# WARNING! This program can confuse your I2C bus, cause data loss and worse!
# I will probe file /dev/i2c-20.
# I will probe address range 0x08-0x77.
# Continue? [Y/n]
#      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
# 00:                         08 09 0a 0b 0c 0d 0e 0f
# 10: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
# 20: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
# 30: -- -- -- -- -- -- -- -- 38 39 3a 3b 3c 3d 3e 3f
# 40: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f
# 50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 60: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f
# 70: 70 71 72 73 74 75 76 77
```

In this case, you can see that there is a single device with address `0x27` on the 
I²C bus `/dev/i2c-1`. In my case, this is likely the display, because I have no 
other devices on that bus. If your bus is more like the second example, that's 
probably not it. If you want to be extra sure, unplug your screen from the bus
and run `i2cdetect <bus-number>` again to see if it disappeared.

## Pin mappings

This is a bit strange: These backpack boards are apparently all just clones off 
each other, and it's very hard to find documentation.

Try running the example first, it might just work.

If not, there are still some solutions:
- The [Adafruit I²C / SPI character LCD backpack](https://www.adafruit.com/product/292) has a [helpful schematic](https://cdn-learn.adafruit.com/assets/assets/000/118/702/large1024/arduino_compatibles_schem.png)
- Try looking for any text on your board and search the web for that (sometimes you get lucky in obscure forums)
- Use a multimeter

If you have any Problems, you can also open an Issue on Github.
