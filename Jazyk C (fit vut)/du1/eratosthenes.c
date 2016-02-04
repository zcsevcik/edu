/*
 * eratosthenes.c
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

#include <math.h>
#include <stddef.h>

#include "bit-array.h"
#include "eratosthenes.h"

/* ==================================================================== */
void Eratosthenes(BitArray_t arr)
{
    SetBit(arr, 0, 1);
    SetBit(arr, 1, 1);

    size_t i_stop = ceil(sqrt(BitArraySize(arr)));
    for (size_t i = 2; i <= i_stop; ++i) {
        if (!GetBit(arr, i)) {
            for (size_t ii = 2 * i; ii < BitArraySize(arr); ii += i) {
                SetBit(arr, ii, 1);
            }
        }
    }
}

/* ==================================================================== */
