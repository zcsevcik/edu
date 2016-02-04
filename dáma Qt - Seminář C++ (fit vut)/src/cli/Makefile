##
## Makefile:       Navrhnete a implementujte aplikaci pro hru Dama
##
## Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
##                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
## Date:           $Format:%aD$
##
## This file is part of ICP-Dama.
##

## Usage: make
##        make all
##        make clean

## ==================================================================== ##
CXX = $(shell which g++-4.7 g++47 g++ 2>/dev/null | head -1)
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra -g -O3
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -DHAVE_CONFIG_H -DNDEBUG
INCLUDES = -I. -I.. -I...

## ==================================================================== ##
HDR = \
    ConApp.h \
    ConCtx.h \
    ConWnd.h \
    FileCtx.h \
    GameCtx.h \
    LocalCtx.h \
    MenuCtx.h \
    MultiCtx.h \
    SingleCtx.h \
    ReplayCtx.h
    
SRC = \
    main.cpp \
    ConApp.cpp \
    ConCtx.cpp \
    ConWnd.cpp \
    FileCtx.cpp \
    GameCtx.cpp \
    LocalCtx.cpp \
    MenuCtx.cpp \
    MultiCtx.cpp \
    SingleCtx.cpp \
    ReplayCtx.cpp

OBJ = $(SRC:.cpp=.o)

OUTPUT = main_cli.a

## ==================================================================== ##
all: $(OUTPUT)

## ==================================================================== ##
clean:
	rm -f $(OBJ) $(OUTPUT)

## ==================================================================== ##
$(OUTPUT): $(HDR) $(OBJ)
	ar rcs $@ $(OBJ)

## ==================================================================== ##
.PHONY: all clean
