/*
 * prvocisla.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-02-26
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU1 a).
 */

#include "bit-array.h"
#include "eratosthenes.h"
#include "error.h"

#include <stdio.h>
#include <stdint.h>

/* ==================================================================== */
#define N 101000000UL
#define LAST_N 10

/* ==================================================================== */
int main(void)
{
    BitArray(primes, N);
    Eratosthenes(primes);

    unsigned long last_primes[LAST_N] = { 0 };
    for (size_t i = N, ii = LAST_N; --i, ii; ) {
        if (!GetBit(primes, i))
            last_primes[--ii] = i;
    }

    for (size_t i = 0; i < LAST_N; ++i) {
        fprintf(stdout, "%lu\n", last_primes[i]);
    }

    return 0;
}

/* ==================================================================== */
