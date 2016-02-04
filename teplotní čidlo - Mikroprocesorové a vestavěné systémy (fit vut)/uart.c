/*
 * uart.c:         Inteligentni teplotni cidlo s mcu PIC,
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

#include <conio.h>
#include <htc.h>

#include "io.h"
#include "modbus.h"

/* -------------------------------------------------------------------- */
/*      B  A  U  D     R  A  T  E     G  E  N  E  R  A  T  O  R         */
/* -------------------------------------------------------------------- */
#define BRGHI(fosc, baud) ((fosc)/(16UL * (baud)) - 1)
#define BRGLO(fosc, baud) ((fosc)/(64UL * (baud)) - 1)

/* -------------------------------------------------------------------- */
/*      U  A  R  T     i  n  i  t                                       */
/* -------------------------------------------------------------------- */
void init_uart()
{
    // set tris
    RS485_TX_TRIS = 1;
    RS485_RX_TRIS = 1;
    RS485_RE_TRIS = 0;

    // async
    SYNC = 0;

    switch (mb_baud) {
        case CBR_300:
            BRGH = 0;
            SPBRG = BRGLO(_XTAL_FREQ, 300);
            break;

        case CBR_1200:
            BRGH = 0;
            SPBRG = BRGLO(_XTAL_FREQ, 1200);
            break;

        case CBR_2400:
            BRGH = 0;
            SPBRG = BRGLO(_XTAL_FREQ, 2400);
            break;

        case CBR_4800:
            BRGH = 0;
            SPBRG = BRGLO(_XTAL_FREQ, 4800);
            break;

        case CBR_9600:
            BRGH = 1;
            SPBRG = BRGHI(_XTAL_FREQ, 9600);
            break;

        default:
        case CBR_19200:
            BRGH = 1;
            SPBRG = BRGHI(_XTAL_FREQ, 19200);
            break;
    }

    TX9 = !(mb_stopb == ONESTOPBIT
        && mb_parity == NOPARITY);

    RX9 = TX9;
    SPEN = 1;

    RS485_RE = 1;
    CREN = 1;
}

/* -------------------------------------------------------------------- */
/*    T  E  R  M  I  N  A  L     I  N  P  U  T  /  O  U  T  P  U  T     */
/* -------------------------------------------------------------------- */
void putch(unsigned char byte)
{
    while (!TXIF);
    _nop();
    TXREG = byte;
}

unsigned char getch()
{
    while (!RCIF);
    _nop();
    return RCREG;
}

unsigned char getche()
{
    uint8_t c;
    putch(c = getch());
    return c;
}
