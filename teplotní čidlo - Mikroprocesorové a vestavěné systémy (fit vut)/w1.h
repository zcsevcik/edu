/*
 * w1.h:           Inteligentni teplotni cidlo s mcu PIC,
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

#ifndef W1_H
#define W1_H

#include <stdint.h>

/* ==================================================================== */
/*      Dallas 1-wire temperature sensor                                */
/*      -- http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf      */
/* ==================================================================== */
#define W1_TCONV          750   // 12-bit conversion time (ms)

typedef enum {
    W1_NOTINIT,
    W1_WAITING,
    W1_PENDING,
} W1_STATUS;

extern volatile W1_STATUS w1_status;

extern void             init_w1();
extern bit              w1_convert();
extern bit              w1_readrom();
extern bit              w1_readdata();

extern uint8_t          w1_getfamily();
extern const uint8_t*   w1_getserial();

extern int16_t          w1_gettemperature();
extern int8_t           w1_getalarm_hi();
extern int8_t           w1_getalarm_lo();
extern uint8_t          w1_getresolution();

extern uint8_t          w1_getconverttime();

#endif //W1_H
