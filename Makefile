DESTINATION ?= /usr/local/lib/lua/5.1/

LIBRARY = signal.so
OBJECTS = $(LIBRARY)

INSTALL = install -c -v
CC = gcc
CFLAGS = -Wall -fPIC -O2

all: signal.so

install: $(LIBRARY)
	$(INSTALL) -m 755 $(LIBRARY) $(DESTINATION)

clean:
	rm -f $(OBJECTS)

signal.so: lsignal.c
	$(CC) $(CFLAGS) -shared -o $@ $^
