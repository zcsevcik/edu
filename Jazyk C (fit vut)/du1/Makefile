##
## Makefile
##
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
## Date:           2014-02-26
##
## Faculty of Information Technology
## Brno University of Technology
##
## This file is part of IJC-DU1.
##

## ==================================================================== ##
.SUFFIXES:

.DEFAULT_GOAL: all
all:
.PHONY: all

## ==================================================================== ##
X :=
#X := .exe

## ==================================================================== ##
CC := gcc
CFLAGS := -O2 -std=c99 -Wall -pedantic
CPP := gcc -E
CPPFLAGS :=
RM := rm -f

## ==================================================================== ##
LDFLAGS :=
LDLIBS := -lm

## ==================================================================== ##
# pro ladeni
#CFLAGS += -O0 -g -Wextra

## ==================================================================== ##
# deklarace cilu
bin-y :=
obj-y :=
deps-y :=

## ==================================================================== ##
# zavislosti
eratosthenes-obj := eratosthenes.o
eratosthenes-inl-obj := eratosthenes-inline.o
error-obj := error.o
ppm-obj := ppm.o

prvocisla-bin := prvocisla$(X)
prvocisla-obj := prvocisla.o
$(prvocisla-bin): \
    $(eratosthenes-obj) \
    $(error-obj) \
    $(prvocisla-obj) \

prvocisla-inl-bin := prvocisla-inline$(X)
prvocisla-inl-obj := prvocisla-inline.o
$(prvocisla-inl-bin): \
    $(eratosthenes-inl-obj) \
    $(error-obj) \
    $(prvocisla-inl-obj) \

steg-decode-bin := steg-decode$(X)
steg-decode-obj := steg-decode.o
$(steg-decode-bin): \
    $(eratosthenes-inl-obj) \
    $(error-obj) \
    $(ppm-obj) \
    $(steg-decode-obj) \

bin-y += \
    $(prvocisla-bin) \
    $(prvocisla-inl-bin) \
    $(steg-decode-bin) \

obj-y += \
    $(eratosthenes-obj) \
    $(eratosthenes-inl-obj) \
    $(error-obj) \
    $(ppm-obj) \
    $(prvocisla-obj) \
    $(prvocisla-inl-obj) \
    $(steg-decode-obj) \

deps-y += \
    $(eratosthenes-obj:.o=.d) \
    $(eratosthenes-inl-obj:.o=.d) \
    $(error-obj:.o=.d) \
    $(ppm-obj:.o=.d) \
    $(prvocisla-obj:.o=.d) \
    $(prvocisla-inl-obj:.o=.d) \
    $(steg-decode-obj:.o=.d) \

## ==================================================================== ##
# pravidla
.SUFFIXES: .c .h .o
CFLAGS += -MMD -MP

cmd_rm_target = $(RM) $1
cmd_cc_o_c = $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
cmd_link_o_target = $(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

Makefile: ;

.c.o:
	$(call cmd_cc_o_c)

%-inline.o: CPPFLAGS += -DUSE_INLINE
%-inline.o: %.c
	$(call cmd_cc_o_c)

$(bin-y):
	$(call cmd_link_o_target)

## ==================================================================== ##
all: $(bin-y)

.PHONY: all

## ==================================================================== ##
clean:
	$(call cmd_rm_target,$(bin-y) $(obj-y) $(deps-y))

.PHONY: clean

-include $(deps-y)
