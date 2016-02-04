/*
 * crc.h:          Inteligentni teplotni cidlo s mcu PIC,
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

#ifndef CRC_H
#define CRC_H

#include <stdint.h>

/**
 * Funkce vypocita CRC-8-Dallas/Maxim
 * Polynomialni funkce x^8 + x^5 + x^4 + 1
 *
 * "Understanding and Using Cyclic Redundancy Checks with
 * Dallas Semiconductor iButton Products"
 * http://pdfserv.maxim-ic.com/en/an/AN27.pdf
 */
uint8_t crc8(
        const uint8_t* buffer,
        uint8_t buffer_length);

/**
 * Funkce vypocita CRC-16-IBM (CRC-16-ANSI)
 * Polynomialni funkce x^16 + x^15 + x^2 + 1
 *
 * "MODBUS over serial line specification and implementation guide V1.02"
 * http://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf
 */
uint16_t crc16(
        const uint8_t* buffer,
        uint8_t buffer_length);

#endif //CRC_H
