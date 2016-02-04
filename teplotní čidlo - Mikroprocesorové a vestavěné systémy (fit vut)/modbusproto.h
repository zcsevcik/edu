/*
 * modbusproto.h:  Inteligentni teplotni cidlo s mcu PIC,
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

#ifndef MODBUSPROTO_H
#define MODBUSPROTO_H

#include <stdbool.h>
#include <stdint.h>

uint8_t proto_get_ro_byte(uint16_t addr, uint16_t num, uint8_t* value);
uint8_t proto_get_rw_byte(uint16_t addr, uint16_t num, uint8_t* value);
uint8_t proto_get_ro_word(uint16_t addr, uint16_t num, uint16_t* value);
uint8_t proto_get_rw_word(uint16_t addr, uint16_t num, uint16_t* value);
uint8_t proto_set_rw_byte(uint16_t addr, bool value);
uint8_t proto_set_rw_word(uint16_t addr, uint16_t value);

#endif //MODBUSPROTO_H
