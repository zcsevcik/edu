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

.SUFFIXES:
.SUFFIXES: .obj .lib .hex .lpp .c .as .asm .p1

PROJNAME = mytqs
OBJFILES = \
    byteswap.p1 \
    crc.p1 \
    main.p1 \
    modbus.p1 \
    modbusproto.p1 \
    uart.p1 \
    w1.p1

PACKFILES = \
    byteswap.c \
    byteswap.h \
    config.h \
    crc.c \
    crc.h \
    io.h \
    main.c \
    Makefile \
    modbus.c \
    modbus.h \
    modbusproto.c \
    modbusproto.h \
    uart.c \
    w1.c \
    w1.h

DEFS = -DHAVE_CONFIG_H
INCLUDES = -I.
LIBS =

CHIP = 16f628a
TTY = /dev/ttyS0

AS = picc
ASFLAGS = --chip=$(CHIP) -Q

CC = picc
CFLAGS = --chip=$(CHIP) -Q --ASMLIST

LD = picc
LDFLAGS = --chip=$(CHIP) -Q --ASMLIST

.PHONY: all burn pack clean

all: $(PROJNAME).hex

$(PROJNAME).hex: $(OBJFILES)
	$(LD) $(LDFLAGS) -O$@ $(OBJFILES) $(LIBS)

.c.p1:
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) --pass1 -O$@ $<

pack:
	tar cvzf $(PROJNAME).tar.gz --exclude=.svn --exclude=.git* $(PACKFILES)

clean:
	rm -f funclist *.hex *.obj *.p1 *.lst *.map *.cod *.cof *.hxl *.pre *.sdb *.sym *.rlf startup.*

burn:
	picprog -q -p $(TTY) -d pic$(CHIP) -i $(PROJNAME).hex --slow --reboot --erase --burn
