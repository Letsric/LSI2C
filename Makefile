CC = gcc
CFLAGS = 
dir_guard=@mkdir -p $(@D) # Credit: https://stackoverflow.com/a/1951111/18406170

all: tools lib example

clean:
	rm -r build


tools: build/tools/backlightCtl

lib: build/lib/libLSI2C.so

example: build/example/example


# TOOLS
build/tools/backlightCtl: tools/backlightCtl.c
	$(dir_guard)
	$(CC) $(CFLAGS) -o build/tools/backlightCtl tools/backlightCtl.c -l i2c

# LIB
build/lib/libLSI2C.o: src/libLSI2C.c src/libLSI2C.h
	$(dir_guard)
	$(CC) $(CFLAGS) -c -fPIC -o build/lib/libLSI2C.o src/libLSI2C.c
build/lib/libLSI2C.so: build/lib/libLSI2C.o
	$(dir_guard)
	$(CC) $(CFLAGS) -shared -o build/lib/libLSI2C.so build/lib/libLSI2C.o -lm -li2c

# EXAMPLE
build/example/example: example/example.c build/lib/libLSI2C.so
	$(dir_guard)
	$(CC) $(CFLAGS) -o build/example/example example/example.c -L./build/lib -l LSI2C
