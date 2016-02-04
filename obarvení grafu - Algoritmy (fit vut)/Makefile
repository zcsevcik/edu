# Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
# $Id: Makefile 29 2011-12-11 22:33:42Z zcsevcik $

# Pouziti: make
#          make all
#          make doc
#          make clean
#          make pack

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g -O2
DEFS = -DHAVE_CONFIG_H
INCLUDES = -I.
LDFLAGS = 

NAME = ial11
OBJFILES = \
    graph.o \
    ial11.o \
    matrix.o \
    vertex.o

PACKFILES = \
    alloca.h \
    config.h \
    Doxyfile \
    graph.c \
    graph.h \
    ial11.c \
    Makefile \
    matrix.c \
    matrix.h \
    vertex.c \
    vertex.h \
    doc/* \
    tests/matgen.c \
    tests/*.txt \
    win32/ial11.sln \
    win32/ial11.vcxproj \
    win32/stdbool.h

.PHONY: all clean doc pack

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJFILES) -o $@

all: $(NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o tests/matgen tests/matgen.c

doc:
	doxygen Doxyfile

pack:
	tar cvzf $(NAME).tar.gz --exclude=.svn $(PACKFILES)

clean:
	rm -f $(OBJFILES) $(NAME)
	rm -f tests/matgen

