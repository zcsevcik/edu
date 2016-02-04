/*
 * osc.h:           TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef OSC_H
#define OSC_H

#include <stdbool.h>
#include <stdint.h>

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
extern void init_osc(int32_t freq, bool intosc);
extern void osc_waitstable_lf();
extern void osc_waitstable_hf();
extern void osc_waitstable_ext();

#endif //OSC_H
