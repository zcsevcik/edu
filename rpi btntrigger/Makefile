## make all
## make clean

CC=gcc
CFLAGS=-std=c99 -O2 -Wall -Wextra -pedantic -I.
LDLIBS=-lpifacedigital -lmcp23s17
LDFLAGS=-L.

all: btntrigger
btntrigger: btntrigger.c

clean:
	rm btntrigger

.PHONY: all clean
