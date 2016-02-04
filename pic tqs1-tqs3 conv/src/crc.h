/*
 * crc.h:          TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef CRC_H
#define CRC_H

#include <stdint.h>

/* ==================================================================== */
/**
 * Funkce vypocita CRC-1 (paritni bit)
 * Polynomialni funkce x + 1
 */
extern
bit crc1(uint8_t c);

/* ==================================================================== */
/**
 * Funkce vypocita CRC-8-Dallas/Maxim
 * Polynomialni funkce x^8 + x^5 + x^4 + 1
 *
 * "Understanding and Using Cyclic Redundancy Checks with
 * Dallas Semiconductor iButton Products"
 * http://pdfserv.maxim-ic.com/en/an/AN27.pdf
 */
extern
uint8_t crc8(
        const uint8_t* buffer,
        uint8_t buffer_length);

/* ==================================================================== */
/**
 * Funkce vypocita CRC-16-IBM (CRC-16-ANSI)
 * Polynomialni funkce x^16 + x^15 + x^2 + 1
 *
 * "MODBUS over serial line specification and implementation guide V1.02"
 * http://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf
 */
extern
uint16_t crc16(
        const uint8_t* buffer,
        uint8_t buffer_length);

#endif //CRC_H
