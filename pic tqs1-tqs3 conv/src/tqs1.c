/*
 * tqs1.c:         TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <htc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tqs1.h"

/* ==================================================================== */
#define TQS1_EOF '\r'

/* ==================================================================== */
static char _tqs1_buf[16];

/* ==================================================================== */
#define _tonum(c) ((c) & 0x0f)

/* ==================================================================== */
static bool _tqs1_temperature(const char *msg, int16_t *val)
{
    if (msg[2] == 'E') return false;
    *val = _tonum(msg[7]);
    *val += _tonum(msg[5]) * 10;
    *val += _tonum(msg[4]) * 100;
    *val += _tonum(msg[3]) * 1000;
    if (msg[2] == '-') *val = -*val;
    return true;
}

/* ==================================================================== */
static bool _tqs1_okerr(const char *msg)
{
    if (msg[2] == 'O' && msg[3] == 'K') return true;
    return false;
}

/* ==================================================================== */
bool tqs1_addr_isvalid(char addr)
{
    return ('A' <= addr && addr <= 'S')
        || ('U' <= addr && addr <= 'Z')
        || ('a' <= addr && addr <= 'z')
        || ('0' <= addr && addr <= '9');
}

/* ==================================================================== */
void tqs1_measure_all()
{
    _tqs1_buf[0] = 'T';
    _tqs1_buf[1] = '$';
    _tqs1_buf[2] = 'C';
    _tqs1_buf[3] = '\0';
    _tqs1_write(_tqs1_buf);
}

/* ==================================================================== */
bool tqs1_measure_imm(char addr, int16_t *val)
{
    if (!tqs1_addr_isvalid(addr)) {
        return false;
    }
    if (!val) {
        return false;
    }

    _tqs1_buf[0] = 'T';
    _tqs1_buf[1] = addr;
    _tqs1_buf[2] = 'I';
    _tqs1_buf[3] = '\0';

    if (!_tqs1_write(_tqs1_buf)) {
        return false;
    }
    if (!_tqs1_read(_tqs1_buf, _countof(_tqs1_buf), TQS1_EOF))
    {
        return false;
    }

    return _tqs1_temperature(_tqs1_buf, val);
}

/* ==================================================================== */
bool tqs1_measure(char addr)
{
    if (!tqs1_addr_isvalid(addr)) {
        return false;
    }

    _tqs1_buf[0] = 'T';
    _tqs1_buf[1] = addr;
    _tqs1_buf[2] = 'C';
    _tqs1_buf[3] = '\0';

    if (!_tqs1_write(_tqs1_buf)) {
        return false;
    }
    if (!_tqs1_read(_tqs1_buf, _countof(_tqs1_buf), TQS1_EOF))
    {
        return false;
    }

    return _tqs1_okerr(_tqs1_buf);
}

/* ==================================================================== */
bool tqs1_read(char addr, int16_t *val)
{
    if (!tqs1_addr_isvalid(addr)) {
        return false;
    }
    if (!val) {
        return false;
    }

    _tqs1_buf[0] = 'T';
    _tqs1_buf[1] = addr;
    _tqs1_buf[2] = 'R';
    _tqs1_buf[3] = '\0';

    if (!_tqs1_write(_tqs1_buf)) {
        return false;
    }
    if (!_tqs1_read(_tqs1_buf, _countof(_tqs1_buf), TQS1_EOF))
    {
        return false;
    }

    return _tqs1_temperature(_tqs1_buf, val);
}

/* ==================================================================== */
bool tqs1_setaddr(char addr)
{
    if (!tqs1_addr_isvalid(addr)) {
        return false;
    }

    _tqs1_buf[0] = 'T';
    _tqs1_buf[1] = '#';
    _tqs1_buf[2] = addr;
    _tqs1_buf[3] = '\0';

    if (!_tqs1_write(_tqs1_buf)) {
        return false;
    }
    if (!_tqs1_read(_tqs1_buf, _countof(_tqs1_buf), TQS1_EOF))
    {
        return false;
    }

    return _tqs1_okerr(_tqs1_buf);
}

/* ==================================================================== */
const char *tqs1_info(char addr)
{
    if (!tqs1_addr_isvalid(addr)) {
        return NULL;
    }

    _tqs1_buf[0] = 'T';
    _tqs1_buf[1] = addr;
    _tqs1_buf[2] = '?';
    _tqs1_buf[3] = '\0';

    if (!_tqs1_write(_tqs1_buf)) {
        return NULL;
    }
    if (!_tqs1_read(_tqs1_buf, _countof(_tqs1_buf), TQS1_EOF))
    {
        return NULL;
    }

    return _tqs1_buf;
}

