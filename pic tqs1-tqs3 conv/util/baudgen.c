/*
 * baudgen.c:      TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <fenv.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tgmath.h>

/* ==================================================================== */
static const struct {
    unsigned long Bd;
    const char *text;
} BaudRates[] = {

#define X(str, val, ...) \
    { val, #str },

#include "config_baud.def"
X_BAUDRATES

#undef X

};

/* ==================================================================== */
#define PRESCALER_BRG 64.0f
#define PRESCALER_BRGH 16.0f
#define PRESCALER_BRG16 16.0f
#define PRESCALER_BRGH16 4.0f

/* ==================================================================== */
typedef struct {
    bool BRGH;
    bool BRG16;
    uint16_t SPBRG;

    long error;
    unsigned long freq;
} brg_t;

/* ==================================================================== */
static inline uint_least16_t
brg_toregister(double prescaler,
               unsigned long baud)
{
    fesetround(FE_TONEAREST);
    return lrint((_XTAL_FREQ / (prescaler * baud)) - 1.0f);
}

/* ==================================================================== */
static inline unsigned long
brg_tobaud(double prescaler,
           uint_least16_t spbrg)
{
    fesetround(FE_TOWARDZERO);
    return lrint(_XTAL_FREQ / (prescaler * (spbrg + 1U)));
}

/* ==================================================================== */
static inline ldiv_t
brg_errori(unsigned long calculated,
           unsigned long desired)
{
    return ldiv(calculated - desired, desired);
}

/* ==================================================================== */
static brg_t
getBestFrequency(unsigned long baud)
{
    uint_least16_t spbrg[] = {
        brg_toregister(PRESCALER_BRG, baud),
        brg_toregister(PRESCALER_BRGH, baud),
        brg_toregister(PRESCALER_BRG16, baud),
        brg_toregister(PRESCALER_BRGH16, baud),
    };

    unsigned long bauds[] = {
        brg_tobaud(PRESCALER_BRG, spbrg[0]),
        brg_tobaud(PRESCALER_BRGH, spbrg[1]),
        brg_tobaud(PRESCALER_BRG16, spbrg[2]),
        brg_tobaud(PRESCALER_BRGH16, spbrg[3]),
    };

    uint_least16_t spbrg_max[] = {
        UINT8_MAX,
        UINT8_MAX,
        UINT16_MAX,
        UINT16_MAX,
    };

    ldiv_t errors[] = {
        brg_errori(bauds[0], baud),
        brg_errori(bauds[1], baud),
        brg_errori(bauds[2], baud),
        brg_errori(bauds[3], baud),
    };

    /* ================================================================ */
    size_t picked = -1;
    for (size_t i = 0; i < _countof(spbrg); ++i) {
        if (spbrg[i] > spbrg_max[i]) continue; // register limit
        if (errors[i].quot != 0) continue; // error > 100%
        if (labs(errors[i].rem) > baud / 100) continue; // error > 1%

        if (picked == -1
            || labs(errors[i].rem) < labs(errors[picked].rem))
        {
            picked = i;
        }
    }

    /* ================================================================ */
    if (picked != -1) {
        return (brg_t) { .BRGH = picked & 0x01,
                         .BRG16 = picked & 0x02,
                         .SPBRG = spbrg[picked],
                         .error = errors[picked].rem,
                         .freq = bauds[picked],
        };
    }
    else {
        return (brg_t) { 0 };
    }
}

/* ==================================================================== */
int
main(int argc, char* argv[])
{
#ifndef _XTAL_FREQ
    _Static_assert(0, "_XTAL_FREQ is not defined");
#endif

    fprintf(stdout, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n\n");
    fprintf(stdout, "/* #define _XTAL_FREQ %lu */\n\n", _XTAL_FREQ);
    fprintf(stdout, "/* #undef X(Bd, SPBRG, BRGH, BRG16, pctERROR) */\n");
    fprintf(stdout, "#define X_SPBRG");

    for (size_t i = 0; i < _countof(BaudRates); ++i) {
        brg_t brg = getBestFrequency(BaudRates[i].Bd);

        const char *fmt_8 = "X(%s /* %lu Bd */, 0x%02hX, %hhu, %hhu, %.2f)";
        const char *fmt_16 = "X(%s /* %lu Bd */, 0x%04hX, %hhu, %hhu, %.2f)";

        if (brg.SPBRG == 0) continue;

        fprintf(stdout, " \\\n");
        fprintf(stdout, brg.BRG16 ? fmt_16 : fmt_8,
                BaudRates[i].text, brg.freq,
                brg.BRG16 ? brg.SPBRG : brg.SPBRG & 0xff,
                brg.BRGH, brg.BRG16,
                (double)brg.error / (double)(BaudRates[i].Bd / 100));
    }

    fprintf(stdout, "\n\n");
    return 0;
}
