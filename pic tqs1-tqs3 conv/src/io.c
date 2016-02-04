/*
 * io.c:           TQS1-TQS3 adapter.
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
#include <stddef.h>

#include "io.h"
#include "bitop.h"

/* ==================================================================== */
/*      I  /  O     I  N  I  T  I  A  L  I  Z  E                        */
/* ==================================================================== */
void init_io()
{
    /* no analog inputs */
    ANSELH = MASK_REGISTER(ANSELH, 0x00, 0x00);
    ANSEL = MASK_REGISTER(ANSEL, 0xF0, 0x00);

    /* UART ports */
    PORTB = MASK_REGISTER(PORTB, 0x5F, 0xFF);
    PORTC = MASK_REGISTER(PORTC, 0xCC, 0x22);

    TRISB = MASK_REGISTER(TRISB, 0x5F, 0x7F);
    TRISC = MASK_REGISTER(TRISC, 0xCC, 0x00);

    /* unused ports */
    PORTA = MASK_REGISTER(PORTA, 0xCB, 0x00);
    PORTB = MASK_REGISTER(PORTB, 0xAF, 0x00);
    PORTC = MASK_REGISTER(PORTC, 0x33, 0x00);

    TRISA = MASK_REGISTER(TRISA, 0xCB, 0x00);
    TRISB = MASK_REGISTER(TRISB, 0xAF, 0x00);
    TRISC = MASK_REGISTER(TRISC, 0x33, 0x00);
}
