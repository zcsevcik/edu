##
## GNUmakefile
##
## Author(s):   Ševčík Radek, Bc. <xsevci44@stud.fit.vutbr.cz>
## Date:        2014/11/04
##
## Soft Computing (SFC)
## Vizualizace Juliových množin
##
## Faculty of Information Technology
## Brno University of Technology
##
## This file is part of sfc2014.
##

MAKEFLAGS += -rR --no-print-directory
.SUFFIXES:
.SUFFIXES: .o .h .c

CC := gcc

CFLAGS += -std=c99 -pedantic -Wall -Wextra -O2
LIBS += -lGL -lglut -lGLEW

OBJS += main.o palletes.o shaders.o

.PHONY: all
all: sfc2014

.PHONY: clean
clean:
	rm sfc2014 $(OBJS)

.c.o:
	@echo "  CC\t$@"
	@$(cmd_cc_o_c)

sfc2014: $(OBJS)
	@echo "  LD\t$@"
	@$(cmd_link_o_target)

main.o: main.c palletes.h params.h shaders.h
palletes.o: palletes.c palletes.h
shaders.o: shaders.c shaders.h

cmd_cc_o_c = $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
cmd_link_o_target = $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $+ $(LIBS)
