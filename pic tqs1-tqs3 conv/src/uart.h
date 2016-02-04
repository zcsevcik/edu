/*
 * uart.h:         TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>

#include "bitop.h"

/* ==================================================================== */
/*      B  A  U  D     R  A  T  E  S                                    */
/* ==================================================================== */
enum {
#define X(name, baud, value) \
    name = value,

#include "config_baud.def"
    X_BAUDRATES

#undef X
};

/* ==================================================================== */
/*      P  A  R  I  T  Y                                                */
/* ==================================================================== */
enum {
    NOPARITY = 0,
    ODDPARITY = 1,
    EVENPARITY = 2,
    MARKPARITY = 3,
    SPACEPARITY = 4,
};

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
extern void init_uart();
extern void close_uart();

extern void uart_config_port1(uint8_t baud, uint8_t parb);
extern void uart_config_port2(uint8_t baud, uint8_t parb);

extern void uart_activate_port1_rx();
extern void uart_activate_port1_tx();
extern void uart_activate_port2_rx();
extern void uart_activate_port2_tx();
extern void uart_activate_port12_tx();

extern bit  uart_write(const char *, size_t);
extern void uart_int_tx();

extern bit  check_parity(uint8_t c, bool parb);
extern bit  count_parity(uint8_t c);

#endif //UART_H
