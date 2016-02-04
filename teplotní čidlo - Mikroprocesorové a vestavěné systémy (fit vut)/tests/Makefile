##
## Makefile:       Inteligentni teplotni cidlo s mcu PIC,
##                 kompatibilni s protokolem tqs3 Modbus RTU
##                 IMP projekt: Vlastni tema
##
## Date:           Wed, 12 Dec 2012 23:46:02 +0100
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##
## This file is part of mytqs.
##
## Original.
##

CC = gcc

LIBMODBUS_I = $(shell pkg-config --cflags libmodbus)
LIBMODBUS_LD = $(shell pkg-config --libs libmodbus)

INCLUDES = $(LIBMODBUS_I)
LDLIBS = 
DEFS = -DNDEBUG

CFLAGS = -std=c99 -Wall -pedantic -g -O2 $(INCLUDES) $(DEFS)
LDFLAGS = $(LIBMODBUS_LD)

executable = master_com

.PHONY: all clean

all: $(executable)

$(executable): master_com.o

clean:
	rm -f $(executable) master_com.o

