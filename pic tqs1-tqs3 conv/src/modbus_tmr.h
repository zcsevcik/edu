/*
 * modbus_tmr.h:   TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef MODBUS_TMR_H
#define MODBUS_TMR_H

#include <htc.h>
#include <stdint.h>

/* ==================================================================== */
#define MODBUS_TMR_REGISTER     TMR2
#define MODBUS_TMR_PERIOD       PR2
#define MODBUS_TMR_CONTROL      T2CON
#define MODBUS_TMR_ON           TMR2ON
#define MODBUS_TMR_INT_EN       TMR2IE
#define MODBUS_TMR_INT_REQ      TMR2IF

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
extern void init_modbus_tmr(uint8_t baud);
extern void modbus_tmr_elapsed();
extern void modbus_tmr_runonce();

#endif //MODBUS_TMR_H
