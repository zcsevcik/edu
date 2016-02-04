/*
 * w1.c:           Inteligentni teplotni cidlo s mcu PIC,
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

#include "w1.h"

#include <stdbool.h>
#include <stdint.h>
#include <htc.h>

#include "crc.h"
#include "io.h"

/* -------------------------------------------------------------------- */
/*      C  O  M  M  A  N  D  S                                          */
/* -------------------------------------------------------------------- */
#define W1_ROM_SEARCH     0xF0
#define W1_ROM_READ       0x33
#define W1_ROM_MATCH      0x55
#define W1_ROM_SKIP       0xCC
#define W1_ROM_ALARM      0xEC
#define W1_FN_CONV        0x44
#define W1_FN_WR          0x4E
#define W1_FN_RD          0xBE
#define W1_FN_CP          0x48
#define W1_FN_RECALL      0xB8
#define W1_FN_PWR_SUPPLY  0xB4

/* -------------------------------------------------------------------- */
/*      S  I  G  N  A  L     O  U  T  P  U  T                           */
/* -------------------------------------------------------------------- */
#define w1_lo() do { \
    W1_TRIS = 0; \
    W1 = 0; \
} while (0)

#define w1_hiz() do { \
    W1_TRIS = 1; \
} while (0)

/* -------------------------------------------------------------------- */
/*      M  E  M  O  R  Y     M  A  P                                    */
/* -------------------------------------------------------------------- */
struct w1_data_st {
    int16_t temperature;
    int8_t alarm_hi;
    int8_t alarm_lo;
    unsigned : 1;
    uint8_t resolution : 2;
    unsigned : 5;
    uint24_t;
    uint8_t crc;
};

/* -------------------------------------------------------------------- */
/*      6 4 - b i t   L A S E R E D   R O M   C O D E                   */
/* -------------------------------------------------------------------- */
struct w1_rom_st {
    uint8_t family;
    uint8_t sn[6];
    uint8_t crc;
};

/* ==================================================================== */
/*      I  N  P  U  T  /  O  U  T  P  U  T                              */
/* ==================================================================== */
static
bit w1_reset()
{
    bool active;

    w1_lo();
    __delay_us(500);

    w1_hiz();
    __delay_us(70);

    active = !W1;
    __delay_us(430);

    return active;
}

static
void w1_putch(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; ++i) {
        w1_lo();
        __delay_us(2);

        if (byte & 0x01) {
            w1_hiz();
        }
        byte >>= 1;

        __delay_us(15);

        w1_hiz();
        __delay_us(40);
    }

    return;
}

static
uint8_t w1_getch()
{
    uint8_t byte;

    for (uint8_t i = 0; i < 8; ++i) {
        w1_lo();
        __delay_us(2);

        w1_hiz();
        __delay_us(3);

        byte = byte >> 1 | W1 << 7;
        __delay_us(50);
    }

    return byte;
}

/* ==================================================================== */
/*      w1     I  N  S  T  A  N  C  E                                   */
/* ==================================================================== */
volatile W1_STATUS      w1_status;
struct w1_rom_st        _w1_rom;
struct w1_data_st       _w1_data;

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
void init_w1()
{
    w1_status = W1_NOTINIT;
    w1_hiz();
}

bit w1_convert()
{
    if (w1_reset()) {
        w1_putch(W1_ROM_SKIP);
        w1_putch(W1_FN_CONV);
        return true;
    }

    return false;
}

bit w1_readrom()
{
    if (w1_reset()) {
        w1_putch(W1_ROM_READ);

        for (uint8_t *p = (uint8_t*)&_w1_rom,
             i = 0; i < sizeof(struct w1_rom_st); ++i)
        { *p++ = w1_getch(); }

        return 0 == crc8((uint8_t*)&_w1_rom, sizeof(struct w1_rom_st));
    }

    return false;
}

bit w1_readdata()
{
    if (w1_reset()) {
        w1_putch(W1_ROM_SKIP);
        w1_putch(W1_FN_RD);

        for (uint8_t *p = (uint8_t*)&_w1_data,
             i = 0; i < sizeof(struct w1_data_st); ++i)
        { *p++ = w1_getch(); }

        return 0 == crc8((uint8_t*)&_w1_data, sizeof(struct w1_data_st));
    }

    return false;
}

/* -------------------------------------------------------------------- */
/*      P  R  O  P  E  R  T  I  E  S                                    */
/* -------------------------------------------------------------------- */
uint8_t w1_getfamily()
{
    return _w1_rom.family;
}

const uint8_t* w1_getserial()
{
    return _w1_rom.sn;
}

int16_t w1_gettemperature()
{
    return _w1_data.temperature;
}

int8_t w1_getalarm_hi()
{
    return _w1_data.alarm_hi;
}

int8_t w1_getalarm_lo()
{
    return _w1_data.alarm_lo;
}

uint8_t w1_getresolution()
{
    return 9 + _w1_data.resolution;
}

uint8_t w1_getconverttime()
{
    return W1_TCONV >> (3 - _w1_data.resolution);
}
