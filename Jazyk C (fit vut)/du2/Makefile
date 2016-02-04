##
## Makefile
##
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
## Date:           2014-04-06
##
## Faculty of Information Technology
## Brno University of Technology
##
## This file is part of IJC-DU2.
##

## ==================================================================== ##
.SUFFIXES:

.DEFAULT_GOAL: all
all:
.PHONY: all

## ==================================================================== ##
AR := ar
ARFLAGS := rcs
CC := gcc
CFLAGS := -O2 -std=c99 -pedantic -Wall -Wextra -g \
#          -MMD -MP
CPPFLAGS := -DNDEBUG
CXX := g++
CXXFLAGS := -O2 -std=c++11 -pedantic -Wall -Wextra -g \
#            -MMD -MP
RM := rm -f

## ==================================================================== ##
# deklarace cilu
bin-y :=
lib-y :=
obj-y :=
deps-y :=

## ==================================================================== ##
htable.o: htable.c htable.h
htable_hash.o: htable_hash.c htable.h

htable.o htable_hash.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -c -o $@ $<

libhtable.a: htable.o htable_hash.o
	$(AR) $(ARFLAGS) $@ $?

libhtable.so: htable.o htable_hash.o
	$(CC) $(CFLAGS) -shared -o $@ $+

fold: fold.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $+

fold2: fold2.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $+

io.o: io.c io.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

wordcount: wordcount.c io.o libhtable.a
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ wordcount.c io.o libhtable.a

wordcount-dynamic: wordcount.c io.o libhtable.so
	$(CC) $(CFLAGS) $(CPPFLAGS) -L. -o $@ wordcount.c io.o -lhtable

bin-y += fold
bin-y += fold2
bin-y += wordcount
bin-y += wordcount-dynamic

lib-y += libhtable.a
lib-y += libhtable.so

obj-y += htable.o htable_hash.o
obj-y += io.o
obj-y += wordcount.o

deps-y += $(obj-y:.o=.d)

## ==================================================================== ##
# pravidla
.SUFFIXES: .c .cc .h .o .a .so

cmd_rm_target = $(RM) $1
cmd_cc_o_c = $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
cmd_cxx_o_cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
cmd_link_cc_o_target = $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)
cmd_link_cxx_o_target = $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)
cmd_link_l_target = $(AR) $(ARFLAGS) $@ $?

Makefile: ;

#.c.o:
#	$(call cmd_cc_o_c)

#%.so: LDFLAGS += -shared

#.cc.o:
#	$(call cmd_cxx_o_cc)

## ==================================================================== ##
all: $(lib-y) $(bin-y)

.PHONY: all

## ==================================================================== ##
clean:
	$(call cmd_rm_target,$(bin-y) $(lib-y) $(obj-y) $(deps-y))

.PHONY: clean

## ==================================================================== ##
dist:
	zip xsevci44.zip *.c *.cc *.h Makefile

.PHONY: dist

## ==================================================================== ##
#-include $(deps-y)