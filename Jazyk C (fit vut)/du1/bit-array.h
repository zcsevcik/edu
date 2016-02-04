/*
 * bit-array.h
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

#ifndef BIT_ARRAY_H_
#define BIT_ARRAY_H_

#include <limits.h>

#include "error.h"

/* ==================================================================== */
typedef unsigned long BitArray_int, BitArray_t[];
#define BITARRAY_CEIL(x,y) ((x / y) + !!(x % y))
#define BITARRAY_BIT (sizeof(BitArray_int) * CHAR_BIT)

/* ==================================================================== */
#define BitArray(name, size) \
    BitArray_int name[1UL + BITARRAY_CEIL((size), BITARRAY_BIT)] = { \
        [0] = (size) \
    }

/* ==================================================================== */
#define BitArray_at(p, i) \
    (&p[1UL + ((i) / BITARRAY_BIT)])

#define BitArray_bit_n(p, i) \
    (1UL << ((i) % BITARRAY_BIT))

/* ==================================================================== */
#define DU1__GET_BIT(p, i) ( \
    !!(*BitArray_at(p, i) & BitArray_bit_n(p, i)) \
)

#define DU1__SET_BIT(p, i, b) ( \
    !!(b) ? (*BitArray_at(p, i) |= BitArray_bit_n(p, i)) \
          : (*BitArray_at(p, i) &= ~BitArray_bit_n(p, i)) \
)

/* ==================================================================== */
#define BitArray_erange(index, mez) \
    FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)mez)

#if ! USE_INLINE
/* ==================================================================== */
#define BitArraySize(p) \
    (p[0])

#define SetBit(p, i, expr) \
    ((i > BitArraySize(p)) ? BitArray_erange(i, BitArraySize(p)), 0 \
                           : DU1__SET_BIT(p, i, expr))

#define GetBit(p, i) \
    ((i > BitArraySize(p)) ? BitArray_erange(i, BitArraySize(p)), 0 \
                           : DU1__GET_BIT(p, i))

#else /* USE_INLINE */
/* ==================================================================== */
#include <stdbool.h>
#include <stddef.h>

static inline size_t BitArraySize(BitArray_t p)
{
    return p[0];
}

static inline void SetBit(BitArray_t p, size_t i, bool expr)
{
    if (i > BitArraySize(p))
        BitArray_erange(i, BitArraySize(p));

    DU1__SET_BIT(p, i, expr);
}

static inline bool GetBit(BitArray_t p, size_t i)
{
    if (i > BitArraySize(p))
        BitArray_erange(i, BitArraySize(p));

    return DU1__GET_BIT(p, i);
}
/* ==================================================================== */
#endif  /* USE_INLINE */

#endif                          //BIT_ARRAY_H_
