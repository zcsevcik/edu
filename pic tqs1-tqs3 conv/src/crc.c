/*
 * crc.c:          TQS1-TQS3 adapter.
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

#include "crc.h"
#include "bitop.h"

/* ==================================================================== */
/*      C  R  C  -  1                                                   */
/* ==================================================================== */
bit crc1(uint8_t c)
{
    /* This algorithm comes from a book titled "Efficient C", by
     * Thomas Plum and Jim Brodie. They credit Kernighan and Ritchie
     * for the original insight.
     */
    uint8_t cbits = 0;
    while (c != 0) {
        cbits++;
        c &= c - 1;
    }

    return cbits & 0x01;
}

/* ==================================================================== */
/*      C  R  C  -  8  -  D  a  l  l  a  s  /  M  a  x  i  m            */
/* ==================================================================== */
uint8_t crc8(const uint8_t* buffer, uint8_t buffer_length)
{
    /* CRC-8 assembler alg. by T. Scott Dattalo (18JAN03) */
    uint8_t crc = 0x00;

    while (buffer_length--) {
        crc ^= *buffer++;

        asm("clrw");

        asm("btfsc (crc8@crc),0");
        asm("xorlw 0x5e");

        asm("btfsc (crc8@crc),1");
        asm("xorlw 0xbc");

        asm("btfsc (crc8@crc),2");
        asm("xorlw 0x61");

        asm("btfsc (crc8@crc),3");
        asm("xorlw 0xc2");

        asm("btfsc (crc8@crc),4");
        asm("xorlw 0x9d");

        asm("btfsc (crc8@crc),5");
        asm("xorlw 0x23");

        asm("btfsc (crc8@crc),6");
        asm("xorlw 0x46");

        asm("btfsc (crc8@crc),7");
        asm("xorlw 0x8c");

        asm("movwf (crc8@crc)");
    }

    return crc;
}

/* ==================================================================== */
/*      C  R  C  -  1  6  -  I  B  M  (  A  N  S  I  )                  */
/* ==================================================================== */
uint16_t crc16(const uint8_t* buffer, uint8_t buffer_length)
{
    uint16_t crc = 0xFFFF;

    while (buffer_length--) {
        crc ^= (uint16_t) *buffer++;

        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 0x0001)
                crc >>= 1, crc ^= 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}
