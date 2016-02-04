##
## Makefile
##
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
## Date:           $Format:%aD$
##
## This file is part of ims13. 
##

## Usage: make
##        make all
##        make clean

## ==================================================================== ##
.PHONY: all clean
.SUFFIXES: .a .o .d .cc .h

## ==================================================================== ##
CXX := g++
CXXFLAGS := -pedantic -Wall -Wextra -O3
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = 
INCLUDES = -I. -I..
LDLIBS = -lm -lsimlib

## ==================================================================== ##
OBJ := \
    ../model.a

SRC := \
    accident_test.cc \
    exit_load.cc \
    speedway.cc \
    usek_load.cc \
    vehicle_generator.cc

OUTPUT := $(SRC:%.cc=%)

## ==================================================================== ##
all: $(OUTPUT)

## ==================================================================== ##
clean:
	rm -f $(OUTPUT)

## ==================================================================== ##
$(OUTPUT): %: %.cc $(OBJ)
	@echo "  CXXLD " $@
	@$(LINK.cpp) -o $@ $(LDLIBS) $+
