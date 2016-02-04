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
##        make clean-doxygen
##        make doxygen
##        make pack
##        make run

PACKNAME = xsevci44
EXEC_GUI = dama2013
EXEC_CLI = dama2013-cli

## ==================================================================== ##
SUBDIRS = src
BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

## ==================================================================== ##
all: $(BUILDDIRS) $(EXEC_GUI) $(EXEC_CLI)
$(SUBDIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

## ==================================================================== ##
doxygen:
	mkdir -p doc/code/
	doxygen doxyfile

## ==================================================================== ##
run: build-src
	./$(EXEC_GUI)

## ==================================================================== ##
pack:
	git archive -v --format tgz -o $(PACKNAME).tgz HEAD

## ==================================================================== ##
clean: $(CLEANDIRS) clean-doxygen
	rm -f $(EXEC_GUI) $(EXEC_CLI)

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean

## ==================================================================== ##
clean-doxygen:
	rm -fr doc/code/

## ==================================================================== ##
QT_PREFIX = /usr/local/share/Qt
QT_LIBDIR = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --variable=libdir QtCore)
PKG_CONFIG = pkg-config
PKG_CONFIG_PATH = $(QT_PREFIX)/lib/pkgconfig

## ==================================================================== ##
CXX = $(shell which g++-4.7 g++47 g++ 2>/dev/null | head -1)
LDFLAGS = -Wl,-rpath,$(QT_LIBDIR)

## ==================================================================== ##
GUI_LIBS = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --libs QtCore QtGui QtXml)

GUI_OBJ = \
    src/gui/main_gui.a \
    src/core.a

$(EXEC_GUI): build-src
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) $(GUI_OBJ) $(LOADLIBES) $(LDLIBS) $(GUI_LIBS) -o $@

## ==================================================================== ##
CLI_LIBS = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG) \
    --libs QtCore QtXml)

CLI_OBJ = \
    src/cli/main_cli.a \
    src/core.a

$(EXEC_CLI): build-src
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) $(CLI_OBJ) $(LOADLIBES) $(LDLIBS) $(CLI_LIBS) -o $@

## ==================================================================== ##
.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all clean clean-doxygen doxygen pack run
