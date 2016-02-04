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
typedef struct {
    long error;
    unsigned long error_div;
    unsigned long freq;
    uint8_t PR;

    uint8_t OUTPS;
    uint8_t CKPS;
} tmr_t;

/* ==================================================================== */
static inline
uint8_t
tmr_toregister(char ckps, char outps, unsigned long baud)
{
    fesetround(FE_TONEAREST);
    return lrint(_XTAL_FREQ / (4UL * baud * ckps * outps));
}

/* ==================================================================== */
static inline
unsigned long
tmr_tofrequency(char ckps, char outps, uint8_t period)
{
    fesetround(FE_TOWARDZERO);
    return lrint(_XTAL_FREQ / (4U * period * ckps * outps));
}

/* ==================================================================== */
static inline
ldiv_t
tmr_errori(unsigned long calculated, unsigned long desired)
{
    return ldiv(calculated - desired, desired);
}

/* ==================================================================== */
static
tmr_t
getBestFrequency(unsigned long baud)
{
    /* >19200 Bd/s
     * t(1.5) =   750 us
     * t(3.5) = 1.750 ms
     *
     *   i.e.     250 us interrupt
     *            t(1) = 500 us
     *            f(t(1)) = 2 kHz
     */
    if (baud > 19200) baud = 2000;

#define OUTPS_MAX 16
#define CKPS_MAX 3

    uint8_t pr[CKPS_MAX][OUTPS_MAX], *p_pr = &pr[0][0];
    unsigned long freq[CKPS_MAX][OUTPS_MAX], *p_freq = &freq[0][0];
    ldiv_t err[CKPS_MAX][OUTPS_MAX], *p_err = &err[0][0];

    for (int ckps = 1; ckps <= 16; ckps *= 4) {
        for (int outps = 1; outps <= 16; outps += 1) {
            *p_pr = tmr_toregister(ckps, outps, baud * 2);
            if (*p_pr == 0) *p_pr = 1; /* DIV_BY_ZERO */

            *p_freq = tmr_tofrequency(ckps, outps, *p_pr);
            if (*p_freq == 0) *p_freq = 1; /* DIV_BY_ZERO */

            *p_err = tmr_errori(*p_freq, baud * 2);

            ++p_pr, ++p_freq, ++p_err;
        }
    }

    /* ================================================================ */
    size_t picked_ckps = -1;
    size_t picked_outps = -1;

    for (size_t i_ckps = CKPS_MAX; i_ckps--;) {
        for (size_t i_outps = OUTPS_MAX; i_outps--;) {
            p_pr = &pr[i_ckps][i_outps];
            p_freq = &freq[i_ckps][i_outps];
            p_err = &err[i_ckps][i_outps];

            if (p_err->quot != 0) continue; // error > 100%
            if (p_err->rem < 0) continue; // at least t(3.5)

            if ((picked_outps == -1 && picked_ckps == -1)
                || (p_err->rem) < err[picked_ckps][picked_outps].rem)
            {
                picked_outps = i_outps;
                picked_ckps = i_ckps;
            }
        }
    }

    /* ================================================================ */
    if (picked_ckps == -1 && picked_outps == -1)
        return (tmr_t) { 0 };

    p_pr = &pr[picked_ckps][picked_outps];
    p_freq = &freq[picked_ckps][picked_outps];
    p_err = &err[picked_ckps][picked_outps];

    return (tmr_t) { .PR = *p_pr,
                     .CKPS = picked_ckps,
                     .OUTPS = picked_outps,
                     .error = p_err->rem,
                     .error_div = baud * 2 / 100,
                     .freq = *p_freq,
    };
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
    fprintf(stdout, "/* Modbus intervals are t(1.5), t(3.5)\n");
    fprintf(stdout, " * gcd(1.5, 3.5) := 0.5;\n");
    fprintf(stdout, " * t(0.5) := 2 * f;\n");
    fprintf(stdout, " */\n\n");
    fprintf(stdout, "/* #undef X(Bd, PR, CKPS, OUTPS, pctERROR) */\n");
    fprintf(stdout, "#define X_MBTMR");

    for (size_t i = 0; i < _countof(BaudRates); ++i) {
        tmr_t tmr = getBestFrequency(BaudRates[i].Bd);

        if (tmr.PR == 0) continue;

        int ckps = 1;
        int outps = 1;

        for (uint8_t i = 0; i < tmr.CKPS; ++i) ckps *= 4;
        for (uint8_t i = 0; i < tmr.OUTPS; ++i) outps += 1;

        fprintf(stdout, " \\\n");
        fprintf(stdout, "X(%s /* %lu Hz */, 0x%02hhX, 0x%02hhX /* 1:%d */, 0x%02hhX /* 1:%d */, %.2f)",
                BaudRates[i].text, tmr.freq,
                tmr.PR,
                tmr.CKPS, ckps,
                tmr.OUTPS, outps,
                (double)tmr.error / (double)tmr.error_div);
    }

    fprintf(stdout, "\n\n");
    return 0;
}
