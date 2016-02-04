# Soubor:  makefile
# Datum:   2011/04/10
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Synchronizace vlaken, projekt c. 2 pro predmet IOS

PROGRAM = barbers
SRC = main.c
OBJ = main.o
PACKFILES = makefile $(SRC) 

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS = -lrt

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(PROGRAM) $(LDLIBS)

.PHONY: all
.PHONY: build
.PHONY: clean	
.PHONY: pack

all: $(PROGRAM)

build: $(PROGRAM)

pack:
	zip xsevci44.zip $(PACKFILES)

clean:
	rm -f *.o $(PROGRAM)

