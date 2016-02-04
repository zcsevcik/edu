# Datum:   2010/12/13
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Ceske razeni, projekt c. 4 pro predmet IZP

# Pouziti: make
#          make all
#          make clean
#          make pack

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

PROJNAME = proj4
AUTHOR = xsevci44
FACULTY = fit
OBJFILES = $(PROJNAME).o
SOURCEFILES = makefile $(PROJNAME).c $(PROJNAME).h
PACKFILES = $(SOURCEFILES) README

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
