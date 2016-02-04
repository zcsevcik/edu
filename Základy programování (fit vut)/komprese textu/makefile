# Datum:   2010/10/07
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: Jednoducha komprese textu, projekt c. 1 pro predmet IZP

# Pouziti: make
#          make all
#          make clean
#          make pack

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

NAME = proj1
AUTHOR = xsevci44
FACULTY = fit
OBJFILES = $(NAME).o $(NAME)circularbuffer.o
SOURCEFILES = makefile $(NAME).c $(NAME).h $(NAME)circularbuffer.c $(NAME)circularbuffer.h
PACKFILES = $(SOURCEFILES)

.PHONY: all clean pack

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

all: $(NAME)

pack:
	tar cvzf $(AUTHOR)-$(FACULTY).tar.gz $(PACKFILES)

clean:
	rm -f *.o
	rm -f $(NAME)