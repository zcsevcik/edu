##
## Makefile:       TQS1-TQS3 adapter.
##
## Date:           $Format:%aD$
## Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
##
## This file is part of tqs-adapter.
##

PACKNAME = tqs_adapter

HEXFILE = bin/tqs_adapter.hex
CHIP = 16f690
TTY = /dev/ttyS0

## ==================================================================== ##
SUBDIRS = src util
BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

## ==================================================================== ##
all: $(BUILDDIRS)
$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%) CHIP=$(CHIP)
	
## ==================================================================== ##
burn:
	picprog -q -p $(TTY) -d pic$(CHIP) -i $(HEXFILE) --slow --reboot --erase --burn 
	
## ==================================================================== ##
pack:
	git archive -v --format tgz -o $(PACKNAME).tgz HEAD

## ==================================================================== ##
build-src: config create-bin

## ==================================================================== ##
create-bin:
	mkdir -p bin

## ==================================================================== ##
config: build-util
	util/baudgen >x_spbrg.def
	util/mbtmrgen >x_mbtmr.def

## ==================================================================== ##
clean: $(CLEANDIRS)
	rm -fr bin/
	rm -f x_spbrg.def x_mbtmr.def

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean

## ==================================================================== ##
.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: config
.PHONY: all burn pack clean
