##
## Makefile:       Mapovani adresoveho prostoru IPv6 pomoci OUI
##                 ISA projekt #1: Programovani sitove sluzby
##
## Date:           2012/10/14
## Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
##
## This file is part of ouisearch.
##

## Usage: make
##        make all
##        make clean
##        make pack

.PHONY: all clean pack

AUTHOR = xsevci44
EXECUTABLE = ouisearch

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g -O2
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -DHAVE_CONFIG_H
INCLUDES = -I.
LDFLAGS =
LDLIBS = -lrt -lpthread

SOURCES = \
    oui_internal.c \
    ouidb.c \
    ouinetwork.c \
    ouisearch.c \
    ouiqueue.c

HEADERS = \
    config.h \
    oui_internal.h \
    ouidb.h \
    ouinetwork.h \
    ouiqueue.h

PACKFILES = \
    $(HEADERS) \
    $(SOURCES) \
    AUTHORS \
    BSDmakefile \
    Makefile \
    manual.pdf \
    README

OBJFILES = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

pack:
	tar cvf $(AUTHOR).tar --exclude=.svn --exclude=.git* $(PACKFILES)

clean:
	rm -f $(OBJFILES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJFILES)
