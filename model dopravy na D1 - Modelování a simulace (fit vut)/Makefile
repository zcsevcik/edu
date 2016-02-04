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
##        make run
##        make dist

## ==================================================================== ##
.PHONY: all clean run dist
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.SUFFIXES: .a .o .d .cc .h

## ==================================================================== ##
CXX := g++
CXXFLAGS := -pedantic -Wall -Wextra -O3
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = 
INCLUDES = -I.

## ==================================================================== ##
HDR := \
    accident_generator.h \
    auto_incr.h \
    countof.h \
    model.h \
    model_impl.h \
    route_generator.h \
    timeconv.h \
    vehicle.h \
    vehicle_generator.h

SRC := \
    accident_generator.cc \
    route_generator.cc \
    vehicle.cc \
    vehicle_generator.cc

OBJ := $(SRC:.cc=.o)

DEPS := $(OBJ:.o=.d)

OUTPUT := model.a
EXEC := speedway

## ==================================================================== ##
define make_depend
    $(CXX) -MM -MF $3 -MP -MT $2 $(CXXFLAGS) $(CPPFLAGS) $1
endef

## ==================================================================== ##
SUBDIRS := stats
BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

## ==================================================================== ##
all: $(OUTPUT) $(BUILDDIRS)
$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS): $(OUTPUT)
	@$(MAKE) -C $(@:build-%=%)

## ==================================================================== ##
clean: $(CLEANDIRS)
	rm -f $(OUTPUT) $(OBJ) $(DEPS)
$(CLEANDIRS):
	@$(MAKE) -C $(@:clean-%=%) clean

## ==================================================================== ##
run: build-stats
	stats/$(EXEC) 8 20

dist:
	git archive --format tgz -o 08_xsevci44_xkidon00.tar.gz HEAD

## ==================================================================== ##
%.a:
	@echo "  AR    " $@
	@$(AR) $(ARFLAGS) $@ $?

## ==================================================================== ##
.cc.o:
	@echo "  CXX   " $@
	@$(call make_depend, $<, $@, $(@:.o=.d))
	@$(COMPILE.cpp) -o $@ $<

## ==================================================================== ##
$(OUTPUT): $(OBJ)

## ==================================================================== ##
-include $(DEPS)
