/*
 * osc.c:          TQS1-TQS3 adapter.
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
#include <stdint.h>

#include "osc.h"

/* ==================================================================== */
#define X_CLOCK /* (desc, freq, ircf, stable_bit) */ \
X("8MHz", 8000000, 0x07, HTS) \
X("4MHz", 4000000, 0x06, HTS) \
X("2MHz", 2000000, 0x05, HTS) \
X("1MHz", 1000000, 0x04, HTS) \
X("500kHz", 500000, 0x03, HTS) \
X("250kHz", 250000, 0x02, HTS) \
X("125kHz", 125000, 0x01, HTS) \
X("31kHz", 31000, 0x00, LTS)

/* ==================================================================== */
void init_osc(int32_t freq, bool intosc)
{
    if (0);
#define X(x_desc, x_freq, x_ircf, x_stable_bit) \
    else if (freq == x_freq) { \
        asm("; " #x_desc); \
        OSCCON = ((x_ircf << 4) & 0x70) | \
                 ((intosc) & 0x01); \
    }

    X_CLOCK
#undef X
        ;
    OSCTUNE = 0x0f;             // maximum frequency
}

/* ==================================================================== */
void osc_waitstable_lf()
{
    while (!LTS);
}

/* ==================================================================== */
void osc_waitstable_hf()
{
    while (!HTS);
}

/* ==================================================================== */
void osc_waitstable_ext()
{
    while (!OSTS);
}
