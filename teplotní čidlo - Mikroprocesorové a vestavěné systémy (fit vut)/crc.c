/*
 * crc.c:          Inteligentni teplotni cidlo s mcu PIC,
 *                 kompatibilni s protokolem tqs3 Modbus RTU
 *                 IMP projekt: Vlastni tema
 *
 * Date:           Wed, 12 Dec 2012 23:46:02 +0100
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * This file is part of mytqs.
 *
 * CRC-8 assembler alg. by T. Scott Dattalo (18JAN03)
 * Original.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "crc.h"

#include <htc.h>

/* -------------------------------------------------------------------- */
/*      C  R  C  -  8  -  D  a  l  l  a  s  /  M  a  x  i  m            */
/* -------------------------------------------------------------------- */
uint8_t crc8(const uint8_t* buffer, uint8_t buffer_length)
{
    uint8_t crc = 0x00;

    while (buffer_length--) {
        crc ^= *buffer++;

        asm("clrw");

        if (crc & 0x01) {
            asm("xorlw 0x5E");
        }
        if (crc & 0x02) {
            asm("xorlw 0xBC");
        }
        if (crc & 0x04) {
            asm("xorlw 0x61");
        }
        if (crc & 0x08) {
            asm("xorlw 0xC2");
        }
        if (crc & 0x10) {
            asm("xorlw 0x9D");
        }
        if (crc & 0x20) {
            asm("xorlw 0x23");
        }
        if (crc & 0x40) {
            asm("xorlw 0x46");
        }
        if (crc & 0x80) {
            asm("xorlw 0x8C");
        }

        asm("movwf (crc8@crc)");
    }

    return crc;
}

/* -------------------------------------------------------------------- */
/*      C  R  C  -  1  6  -  I  B  M  (  A  N  S  I  )                  */
/* -------------------------------------------------------------------- */
uint16_t crc16(const uint8_t* buffer, uint8_t buffer_length)
{
    uint8_t crcl = 0xFF;
    uint8_t crch = 0xFF;

    while (buffer_length--) {
        crcl ^= *buffer++;

        for (uint8_t i = 0; i < 8; ++i) {
            asm("clrc");
            asm("rrf (crc16@crch),f");
            asm("rrf (crc16@crcl),f");

            if (CARRY) {
                crch ^= 0xA0;
                crcl ^= 0x01;
            }
        }
    }

    return (crch << 8 | crcl);
}
