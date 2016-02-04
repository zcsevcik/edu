/*
 * steg-decode.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-02-26
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU1 b).
 */

#include "bit-array.h"
#include "eratosthenes.h"
#include "error.h"
#include "ppm.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* ==================================================================== */
#define IMG_SIZE_LIMIT (1000 * 1000)

/* ==================================================================== */
void decode(const unsigned char *data, size_t len)
{
    BitArray(primes, IMG_SIZE_LIMIT * 3);
    Eratosthenes(primes);

    unsigned char c = 0;
    unsigned char bit = 1;

    for (size_t i = 1; i < BitArraySize(primes); ++i) {
        if (!GetBit(primes, i)) {
            if (i > len) {
                FatalError("Neukonceny retezec");
            }

            unsigned char lsb = -(data[i] & 0x01);
            c = (c & ~bit) | (lsb & bit);
            bit = (bit << 1) | !!(bit & 0x80);  // rotate-left

            if (bit == 1) {
                if (c == '\0') break;
                if (!isprint(c)) FatalError("Objeven netisknutelny znak");

                fprintf(stdout, "%c", c);
            }
        }
    }

    fprintf(stdout, "\n");
}

/* ==================================================================== */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        FatalError("Nezadan vstupni obrazek");
    }

    const char *filename = argv[1];

    struct ppm *p = ppm_read(filename);
    if (!p) {
        FatalError("Chyba pri cteni souboru %s", filename);
    }
    if (p->xsize * p->ysize > IMG_SIZE_LIMIT) {
        free(p);
        FatalError("Prekrocen implementacni limit");
    }

    decode(p->data, p->xsize * p->ysize * 3);
    free(p);
    return 0;
}

/* ==================================================================== */
