# Datum:   2012/04/14
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK

# Pouziti: make
#          make all
#          make clean
#          make pack

AUTHOR = xsevci44
NAME = rdt

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g -O2
DEFS = -DHAVE_CONFIG_H
INCLUDES = -I. -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lexpat -lrt

OBJFILES = \
    base64.o \
    log.o \
    rdt.o \
    sock.o \
    udt.o

OBJFILES_CLI = \
    $(OBJFILES) \
    rdtclient.o

OBJFILES_SRV = \
    $(OBJFILES) \
    rdtserver.o

PACKFILES = \
    alloca.h \
    base64.c \
    base64.h \
    config.h \
    log.c \
    log.h \
    Makefile \
    rdt.c \
    rdt.h \
    rdtclient.c \
    rdtserver.c \
    readme.xml \
    sock.c \
    sock.h \
    udt.c \
    udt.h

.PHONY: all clean doc pack

all: rdtclient rdtserver


rdtserver: $(OBJFILES_SRV)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES_SRV) $(LDFLAGS)

rdtclient: $(OBJFILES_CLI)
	$(CC) $(CFLAGS) -o $@ $(OBJFILES_CLI) $(LDFLAGS)

base64.o: base64.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

log.o: log.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

rdt.o: rdt.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

sock.o: sock.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

udt.o: udt.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

rdtclient.o: rdtclient.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

rdtserver.o: rdtserver.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

pack:
	tar cvzf $(AUTHOR).tar.gz --exclude=.svn $(PACKFILES)

clean:
	rm -f $(OBJFILES_CLI) rdtclient
	rm -f $(OBJFILES_SRV) rdtserver

