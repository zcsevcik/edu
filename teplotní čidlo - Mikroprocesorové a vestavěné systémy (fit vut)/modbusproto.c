/*
 * modbusproto.c:  Inteligentni teplotni cidlo s mcu PIC,
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

#include "modbusproto.h"

#include <stddef.h>
#include <stdlib.h>
#include <htc.h>

#include "io.h"
#include "modbus.h"
#include "w1.h"

/* -------------------------------------------------------------------- */
/*      ADDRESSES:                                                      */
/*      -- Read Holding Registers 03h                                   */
/*      -- Write Single Registers 06h                                   */
/* -------------------------------------------------------------------- */
#define MBP_ADDR_ENCONF     0x0000
#define MBP_ADDR_ID         0x0001
#define MBP_ADDR_BAUD       0x0002
#define MBP_ADDRALT_TEMPC   0x000A
#define MBP_ADDRALT_TEMPI   0x000B

/* -------------------------------------------------------------------- */
/*      ADDRESSES:                                                      */
/*      -- Read Input Registers 04h                                     */
/* -------------------------------------------------------------------- */
#define MBP_ADDR_TEMPC      0x0000
#define MBP_ADDR_TEMPI      0x0001

/* ==================================================================== */
/*      Convert w1 temperature to 16-bit format (temp := val / 10)      */
/* ==================================================================== */
static
const int8_t _flt_part[] = {
    0, 1, 1, 2,
    3, 3, 4, 4,
    5, 6, 6, 7,
    8, 8, 9, 9
};

static
int16_t getmbtemp()
{
    // w1-12bit ::= SSSSIIII IIIIDDDD
    // S ::= sign
    // I ::= integer part
    // D ::= decimal part

    int16_t intpt = w1_gettemperature();
    int8_t decpt = _flt_part[(intpt & 0x000F)];

    // intpt = 10 * (intpt >> 4);
    intpt &= 0xFFF0;
    intpt >>= 1;
    intpt += intpt >> 2;

    return intpt + decpt;
}

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
uint8_t proto_get_rw_byte(uint16_t addr, uint16_t num, uint8_t* value)
{
    return MB_EADDR;
}

uint8_t proto_get_ro_byte(uint16_t addr, uint16_t num, uint8_t* value)
{
    return MB_EADDR;
}

uint8_t proto_get_ro_word(uint16_t addr, uint16_t num, uint16_t* value)
{
    if (value == NULL) {
        return MB_EDEVFAIL;
    }
    if (num != 1) {
        return MB_EADDR;
    }

    switch (addr) {
        case MBP_ADDR_TEMPC:
        case MBP_ADDR_TEMPI:
            *value = getmbtemp();
            break;

        default:
            return MB_EADDR;
    }

    return MB_EOK;
}

uint8_t proto_get_rw_word(uint16_t addr, uint16_t num, uint16_t* value)
{
    if (value == NULL) {
        return MB_EDEVFAIL;
    }
    if (num != 1) {
        return MB_EADDR;
    }

    switch (addr) {
        case MBP_ADDR_ENCONF:
            if (ADDR_EN) {
                *value = 0x00FF;
            }
            else {
                *value = 0x0000;
            }
            break;

        case MBP_ADDR_ID:
            *value = mb_addr;
            break;

        case MBP_ADDR_BAUD:
            *value = mb_baud;
            break;

        case MBP_ADDRALT_TEMPC:
        case MBP_ADDRALT_TEMPI:
            *value = getmbtemp();
            break;

        default:
            return MB_EADDR;
    }

    return MB_EOK;
}

uint8_t proto_set_rw_byte(uint16_t addr, bool value)
{
    return MB_EADDR;
}

uint8_t proto_set_rw_word(uint16_t addr, uint16_t value)
{
    switch (addr) {
        case MBP_ADDR_ENCONF:
            if (ADDR_EN != value) {
                return MB_EDEVFAIL;
            }

            return MB_EOK;

        case MBP_ADDR_ID:
            if (!ADDR_EN) {
                return MB_EDEVFAIL;
            }
            if (value == 0) {
                return MB_EVAL;
            }
            if (value > 247) {
                return MB_EVAL;
            }

            mb_addr = value;
            return MB_EADDR;

        case MBP_ADDR_BAUD:
            return MB_EADDR;

        default:
            return MB_EADDR;
    }
}
