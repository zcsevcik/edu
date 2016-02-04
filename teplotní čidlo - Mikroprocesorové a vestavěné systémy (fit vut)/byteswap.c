/*
 * byteswap.c:     Inteligentni teplotni cidlo s mcu PIC,
 *                 kompatibilni s protokolem tqs3 Modbus RTU
 *                 IMP projekt: Vlastni tema
 *
 * Date:           Wed, 12 Dec 2012 23:46:02 +0100
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * This file is part of mytqs.
 *
 * Original.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "byteswap.h"

#include <htc.h>

uint16_t _byteswap_ushort(uint16_t val)
{
    asm("movf (__byteswap_ushort@val),w");    // W := A
    asm("xorwf (__byteswap_ushort@val+1),w"); // W := A ^ B
    asm("xorwf (__byteswap_ushort@val),f");   // A := A ^ B ^ A = B
    asm("xorwf (__byteswap_ushort@val+1),f"); // B := B ^ B ^ A = A
    return val;
}
