##
## Makefile
##
## Author(s):      Radek Sevcik <zcsevcik@gmail.com>
## Date:           2014/01/06
##
## This file is part of bf. 
##

## Usage: make
##        make all
##        make clean

## ==================================================================== ##
.PHONY: all clean
.SUFFIXES: .cpp

## ==================================================================== ##
CXX ?= g++
CXXFLAGS ?= -std=c++11 -pedantic -Wall -Wextra -O2
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = 
INCLUDES = 
LDLIBS = 

## ==================================================================== ##
OUTPUT := bf bftmpl

## ==================================================================== ##
all: $(OUTPUT)

## ==================================================================== ##
clean:
	rm -f $(OUTPUT)

## ==================================================================== ##
bftmpl: maintmpl.cpp
bf: main.cpp
$(OUTPUT):
	@echo "  CXXLD " $@
	@$(LINK.cpp) -o $@ $(LDLIBS) $+
