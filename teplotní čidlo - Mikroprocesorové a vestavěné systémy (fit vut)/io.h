/*
 * io.h:           Inteligentni teplotni cidlo s mcu PIC,
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

#ifndef IO_H
#define IO_H

#include <htc.h>

/* -------------------------------------------------------------------- */
/*      PIC16f628a PIN assignments                                      */
/*      -- RA2/AN2/Vref                                                 */
/*      -- RA3/AN3/CMP1                                                 */
/*      -- RB0/INT                                                      */
/*      -- RB1/RX/DT                                                    */
/*      -- RB2/TX/CK                                                    */
/*      -- RB5                                                          */
/* -------------------------------------------------------------------- */
#define W1_BIT              0
#define LED_BIT             2
#define ADDR_EN_BIT         5
#define RS485_RE_BIT        3
#define RS485_TX_BIT        2
#define RS485_RX_BIT        1

#define W1                  RB0
#define LED                 RA2
#define ADDR_EN             RB5
#define RS485_RE            RA3
#define RS485_TX            RB2
#define RS485_RX            RB1

#define W1_TRIS             TRISB0
#define LED_TRIS            TRISA2
#define ADDR_EN_TRIS        TRISB5
#define RS485_RE_TRIS       TRISA3
#define RS485_TX_TRIS       TRISB2
#define RS485_RX_TRIS       TRISB1

#endif //IO_H
