##
## Makefile:       TQS1-TQS3 adapter.
##
## Date:           $Format:%aD$
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##
## This file is part of tqs-adapter.
##

.SUFFIXES:
.SUFFIXES: .obj .lib .hex .lpp .c .as .asm .p1

PROJNAME = io

HEXFILE = ../bin/tqs_adapter.hex

OBJFILES = \
    byteswap.p1 \
    crc.p1 \
    frame.p1 \
    main.p1 \
    modbus.p1 \
    modbus_tmr.p1 \
    io.p1 \
    osc.p1 \
    uart.p1 \
    tqs1.p1

DEFS = -DHAVE_CONFIG_H
INCLUDES = -I. -I..
LIBS =

AS = picc
ASFLAGS = --chip=$(CHIP) -Q

CC = picc
CFLAGS = --chip=$(CHIP) -Q --ASMLIST

LD = picc
LDFLAGS = --chip=$(CHIP) -Q --ASMLIST

.PHONY: all clean

all: $(HEXFILE)

$(HEXFILE): $(OBJFILES)
	$(LD) $(LDFLAGS) -O$@ $(OBJFILES) $(LIBS)

.c.p1:
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) --pass1 -O$@ $<

clean:
	rm -f funclist *.p1 *.pre
