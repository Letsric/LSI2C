CC ?= gcc
CFLAGS ?= -O3 -Wall
dir_guard=@mkdir -p $(@D) # Credit: https://stackoverflow.com/a/1951111/18406170

all: tools lib example

clean:
	cd i2c-tools && make clean
	rm -r build

run: build/example/example
	./build/example/example


tools: build/tools/backlightCtl

lib: build/lib/libLSI2C.a

example: build/example/example


# TOOLS
build/tools/backlightCtl: tools/backlightCtl.c build/i2c-tools/libi2c.a
	$(dir_guard)
	$(CC) $(CFLAGS) -I i2c-tools/include -o build/tools/backlightCtl tools/backlightCtl.c build/i2c-tools/libi2c.a

# LIB
build/lib/LSI2C.ao: lib/LSI2C.c include/LSI2C.h
	$(dir_guard)
	$(CC) $(CFLAGS) -I i2c-tools/include -I include -fPIC -c -o build/lib/LSI2C.ao lib/LSI2C.c
build/lib/libLSI2C.a: build/lib/LSI2C.ao build/i2c-tools/smbus.ao
	$(dir_guard)
	ar rcs build/lib/libLSI2C.a build/lib/LSI2C.ao build/i2c-tools/smbus.ao

# EXAMPLE
build/example/example: example/example.c build/lib/libLSI2C.a
	$(dir_guard)
	$(CC) $(CFLAGS) -I include -o build/example/example example/example.c build/lib/libLSI2C.a

# I2C-TOOLS
build/i2c-tools/smbus.ao:
	$(dir_guard)
	cd i2c-tools && make all-lib
	cp i2c-tools/lib/smbus.ao build/i2c-tools/
build/i2c-tools/libi2c.a: build/i2c-tools/smbus.ao
	$(dir_guard)
	cp i2c-tools/lib/libi2c.a build/i2c-tools/
