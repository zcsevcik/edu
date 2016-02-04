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

## ==================================================================== ##
CXX = $(shell which g++-4.7 g++47 g++ 2>/dev/null | head -1)
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra -g -O3
CXXFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --cflags QtCore QtXml)
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -DHAVE_CONFIG_H -DNDEBUG -DQT_NO_DEBUG
INCLUDES = -I. -I..

## ==================================================================== ##
HDR = \
    Ai.h \
    AiRandom.h \
    AlgebraicNotation.h \
    BinaryNotation.h \
    Color.h \
    Game.h \
    Game_bot.h \
    Game_io.h \
    Game_net.h \
    Game_p.h \
    Game_replay.h \
    GameCmd.h \
    CheckerBoard.h \
    CheckerBoardGen.h \
    Log.h \
    Move.h \
    MsgBase.h \
    MsgFactory.h \
    MsgChallenge.h \
    MsgChallengeResponse.h \
    MsgChat.h \
    MsgMove.h \
    MsgMoveAck.h \
    MsgSurrender.h \
    MyEndian.h \
    Notation.h \
    Piece.h \
    XmlNotation.h
    
SRC = \
    AiRandom.cpp \
    AlgebraicNotation.cpp \
    BinaryNotation.cpp \
    Game.cpp \
    Game_bot.cpp \
    Game_net.cpp \
    Game_replay.cpp \
    GameCmd.cpp \
    Log.cpp \
    MsgFactory.cpp \
    XmlNotation.cpp
    
OBJ = $(SRC:.cpp=.o)

OUTPUT = core.a

## ==================================================================== ##
SUBDIRS = cli gui
BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

## ==================================================================== ##
all: $(OUTPUT) $(BUILDDIRS)
$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

## ==================================================================== ##
clean: $(CLEANDIRS)
	rm -f $(OBJ) $(OUTPUT)
	
$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean

## ==================================================================== ##
$(OUTPUT): $(HDR) $(OBJ)
	ar rcs $@ $(OBJ)

## ==================================================================== ##
.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all clean
