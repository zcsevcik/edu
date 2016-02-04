/*
 * bitop.h:        TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef BITOP_H
#define BITOP_H

#define GETB(x, i) \
    ((uint8_t *) &(x))[i]

#define MSB(x) \
    GETB(x, 1)

#define LSB(x) \
    GETB(x, 0)

#define MASK_REGISTER(data, and_mask, or_mask) \
    (((data) & (and_mask)) | ((or_mask) & ~(and_mask)))

#endif //BITOP_H
