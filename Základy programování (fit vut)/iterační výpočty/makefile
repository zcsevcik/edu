# Datum:   2010/10/21
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP

# Pouziti: make
#          make all
#          make clean
#          make pack

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g -D $(PREPROCESSOR)
PREPROCESSOR = NDEBUG

PROJNAME = proj2
AUTHOR = xsevci44
FACULTY = fit
OBJFILES = $(PROJNAME).o $(PROJNAME)math.o
SOURCEFILES = makefile $(PROJNAME).c $(PROJNAME).h $(PROJNAME)math.c $(PROJNAME)math.h
PACKFILES = $(SOURCEFILES)

.PHONY: all clean pack

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PROJNAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -lm -o $@

all: $(PROJNAME)

pack:
	tar cvzf $(AUTHOR)-$(FACULTY).tar.gz $(PACKFILES)

clean:
	rm -f $(OBJFILES) $(PROJNAME)
