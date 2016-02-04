#=============================================================================
# Name        : Makefile
# Author      : Ondrej Rysavy
# Date        : Feb 26, 2009
# Copyright   : (c) Brno University of Technology
# Description : Makefile for IPK project 3. Assigned in spring term 
#               of 2008/2009.
#
#               It makes two executable: udtdemo and idtproxy.
#               udtdemo - Demostrates principles of a UDP communication.
#               udtproxy - Implements UDP proxy that can drop packets, 
#                          reorded packets, and injects bit errors.
#
# Usage:        make [build] - for building the project 
#               make clean   - for cleaning the projects immediate files and
#                              executables.
#
#============================================================================
CC=gcc

demo=udtdemo
proxy=udtproxy

OBJ=udtdemo.o udtproxy.o
SRC=udtdemo.c udtproxy.c
HEAD=udt.h

.PHONY: build
.PHONY: clean

build: ${OBJ}
	${CC} -o ${demo} udtdemo.o
	${CC} -o ${proxy} udtproxy.o

clean:
	rm -f *.o *.core ${demo} ${proxy}
