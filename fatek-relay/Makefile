MAKEFLAGS += -rR --no-print-directory
.PHONY: all clean
.SUFFIXES:
.SUFFIXES: .o .h .cc

## ==================================================================== ##
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Weffc++ -O3 -march=native
CPPFLAGS = $(DEFS) $(INCLUDES)
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
INCLUDES =
LDLIBS =
LDFLAGS =

## ==================================================================== ##
OBJS = address.o main.o packet.o serialport.o udpserver.o
OUTPUT := fatek-relay

$(OUTPUT): $(OBJS)
address.o: address.cc address.h
main.o: main.cc address.h buffer.h packet.h serialport.h udpserver.h
packet.o: packet.cc packet.h
serialport.o: serialport.cc serialport.h
udpserver.o: udpserver.cc udpserver.h address.h

## ==================================================================== ##
all: $(OUTPUT)

## ==================================================================== ##
clean:
	rm -f $(OUTPUT) $(OBJS)

## ==================================================================== ##
$(OUTPUT):
	@echo "  LD\t$@"
	@$(cmd_link_o_target)

.cc.o:
	@echo "  CXX\t$@"
	@$(cmd_cxx_o_cc)

cmd_cxx_o_cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
cmd_link_o_target = $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

