/*
 * tqs1.h:         TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef TQS1_H
#define TQS1_H

#include <htc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==================================================================== */
extern bool _tqs1_read(char *buf, size_t len, char eof);
extern bool _tqs1_write(const char *);

/* ==================================================================== */
extern bool tqs1_addr_isvalid(char addr);
extern void tqs1_measure_all();
extern bool tqs1_measure_imm(char addr, int16_t *);
extern bool tqs1_measure(char addr);
extern bool tqs1_read(char addr, int16_t *);
extern bool tqs1_setaddr(char addr);
extern const char *tqs1_info(char addr);

#endif //TQS1_H
