/*
 * byteswap.c:     TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <htc.h>

#include "byteswap.h"

/* ==================================================================== */
uint16_t _byteswap_ushort(uint16_t val)
{
    asm("movf (__byteswap_ushort@val),w");    // W := A
    asm("xorwf (__byteswap_ushort@val+1),w"); // W := A ^ B
    asm("xorwf (__byteswap_ushort@val),f");   // A := A ^ B ^ A = B
    asm("xorwf (__byteswap_ushort@val+1),f"); // B := B ^ B ^ A = A
    return val;
}
