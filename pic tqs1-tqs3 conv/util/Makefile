##
## Makefile:       TQS1-TQS3 adapter.
##
## Date:           $Format:%aD$
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##
## This file is part of tqs-adapter.
##

.PHONY = all clean

CC = gcc
CFLAGS = -std=c99 -g -O2
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -DHAVE_CONFIG_H
INCLUDES = -I. -I..
LIBS =
LDFLAGS = 
LDLIBS = -lm

all: mbtmrgen baudgen

clean:
	rm -f mbtmrgen mbtmrgen.exe
	rm -f baudgen baudgen.exe

