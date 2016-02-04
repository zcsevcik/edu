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
QT_PREFIX = /usr/local/share/Qt
PKG_CONFIG = pkg-config
PKG_CONFIG_PATH = $(QT_PREFIX)/lib/pkgconfig

MOC = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --variable=moc_location QtCore)
UIC = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --variable=uic_location QtCore)

## ==================================================================== ##
CXX = $(shell which g++-4.7 g++47 g++ 2>/dev/null | head -1)
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra -g -O3
CXXFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --cflags QtCore QtGui)
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -DHAVE_CONFIG_H -DNDEBUG -DQT_NO_DEBUG
INCLUDES = -I. -I.. -I...

## ==================================================================== ##
HDR = \
    modelmoves.h \
    slotforwarder.h \
    wdgcheckerboard.h \
    wdggame.h \
    wdglocal.h \
    wdgmulti.h \
    wdgreplay.h \
    wdgsingle.h \
    wndmain.h
    
SRC = \
    main.cpp \
    modelmoves.cpp \
    wdgcheckerboard.cpp \
    wdggame.cpp \
    wdglocal.cpp \
    wdgmulti.cpp \
    wdgreplay.cpp \
    wdgsingle.cpp \
    wndmain.cpp
    
FORMS = \
    wdggame.ui \
    wdgreplay.ui \
    wndmain.ui
    
QOBJ = \
    modelmoves.h \
    slotforwarder.h \
    wdgcheckerboard.h \
    wdggame.h \
    wdglocal.h \
    wdgmulti.h \
    wdgreplay.h \
    wdgsingle.h \
    wndmain.h
    
OBJ = \
    $(SRC:.cpp=.o) \
    $(addprefix moc_,$(QOBJ:.h=.o))

GEN = \
    $(addprefix ui_,$(FORMS:.ui=.h)) \
    $(addprefix moc_,$(QOBJ:.h=.cpp))
    
OUTPUT = main_gui.a

## ==================================================================== ##
all: $(OUTPUT)

## ==================================================================== ##
clean:
	rm -f $(OBJ) $(GEN) $(OUTPUT)

## ==================================================================== ##
$(OUTPUT): $(HDR) $(GEN) $(OBJ)
	ar rcs $@ $(OBJ)
	
## ==================================================================== ##
moc_%.cpp: %.h
	$(MOC) $(DEFS) $(INCLUDES) $< -o $@
	
## ==================================================================== ##
ui_%.h: %.ui
	$(UIC) $< -o $@

## ==================================================================== ##
.PHONY: all clean
	