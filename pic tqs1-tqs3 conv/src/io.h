/*
 * io.h:           TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef IO_H
#define IO_H

#include <htc.h>

/* ==================================================================== */
/*      PIC16F690 PIN assignments                                       */
/*      ... RA0 / ICSP DATA                                             */
/*      ... RA1 / ICSP CLOCK                                            */
/*      ... RA2 / nc                                                    */
/*      ... RA3 / VPP                                                   */
/*      ... RA4 / nc                                                    */
/*      ... RA5 / nc                                                    */
/*      ... RB4 / nc                                                    */
/*      ... RB5 / UART RX                                               */
/*      ... RB6 / nc                                                    */
/*      ... RB7 / UART TX                                               */
/*      ... RC0 / UART2 TX ENABLE                                       */
/*      ... RC1 / UART2 RX DISABLE                                      */
/*      ... RC2 / nc                                                    */
/*      ... RC3 / nc                                                    */
/*      ... RC4 / UART1 TX ENABLE                                       */
/*      ... RC5 / UART1 RX DISABLE                                      */
/*      ... RC6 / nc                                                    */
/*      ... RC7 / nc                                                    */
/* ==================================================================== */

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
extern void init_io();

#endif //IO_H
