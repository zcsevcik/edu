# Datum:   2010/11/24
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Maticove operace, projekt c. 3 pro predmet IZP

# Pouziti: make
#          make all
#          make clean
#          make pack

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

PROJNAME = proj3
AUTHOR = xsevci44
FACULTY = fit
OBJFILES = $(PROJNAME).o $(PROJNAME)io.o $(PROJNAME)matrix.o $(PROJNAME)vector.o
SOURCEFILES = makefile $(PROJNAME).c $(PROJNAME).h \
    $(PROJNAME)io.c $(PROJNAME)io.h \
    $(PROJNAME)matrix.c $(PROJNAME)matrix.h \
    $(PROJNAME)vector.c $(PROJNAME)vector.h
PACKFILES = $(SOURCEFILES)

.PHONY: all clean pack

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PROJNAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

all: $(PROJNAME)

pack:
	tar cvzf $(AUTHOR)-$(FACULTY).tar.gz $(PACKFILES)

clean:
	rm -f $(OBJFILES) $(PROJNAME)
