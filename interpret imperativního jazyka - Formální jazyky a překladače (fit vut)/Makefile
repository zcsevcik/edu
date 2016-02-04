##
## Makefile:       Implementace interpretu imperativniho jazyka IFJ12
##
## Team:           Daniel Berek <xberek00@stud.fit.vutbr.cz>
##                 Dusan Fodor  <xfodor01@stud.fit.vutbr.cz>
##                 Pavol Jurcik <xjurci06@stud.fit.vutbr.cz>
##                 Peter Pritel <xprite01@stud.fit.vutbr.cz>
##                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
## Date:           $Format:%aD$
##
## This file is part of ifj12_b2i.
##

## Usage: make
##        make all
##        make clean
##        make clean-doc
##        make doc
##        make pack

.PHONY: all clean clean-doc doc pack

LEADER = xsevci44
EXECUTABLE = ifj12

CC = gcc
CFLAGS = $(CPPFLAGS) -std=c99 -pedantic -Wall -Wextra -g -O2
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -DHAVE_CONFIG_H -DNDEBUG
INCLUDES = -I.
LDFLAGS =
LDLIBS = -lm

SOURCES = \
    defs.c \
    expr.c \
    ial.c \
    ifj12.c \
    parser.c \
    scanner.c \
    semantic.c \
    stack.c \
    str.c \
    syntree.c

HEADERS = \
    config.h \
    builtin.def \
    defs.h \
    ial.h \
    keywords.def \
    parser.h \
    parser_int.h \
    scanner.h \
    semantic.h \
    stack.h \
    str.h \
    syntree.h \
    types.def \
    types.h

PACKFILES = \
    $(HEADERS) \
    $(SOURCES) \
    BSDmakefile \
    Makefile \
    authors \
    dokumentace.pdf \
    rozdeleni \
    rozsireni

OBJFILES = \
    defs.o \
    expr.o \
    ial.o \
    ifj12.o \
    parser.o \
    scanner.o \
    semantic.o \
    stack.o \
    str.o \
    syntree.o

all: $(EXECUTABLE)

doc:
	mkdir -p doc/code
	doxygen doxyfile

pack:
	git archive -v --format tgz -o $(LEADER).tgz HEAD

clean:
	rm -f $(OBJFILES) $(EXECUTABLE)

clean-doc:
	rm -fr doc/code

$(EXECUTABLE): $(OBJFILES)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $(OBJFILES) $(LOADLIBES) $(LDLIBS) -o $@
