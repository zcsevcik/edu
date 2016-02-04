/*
 * frame.c:        TQS1-TQS3 adapter.
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
#include <stddef.h>
#include <stdint.h>

#include "frame.h"

/* ==================================================================== */
static char _buf[80];
static uint8_t _front;
static uint8_t _back;
static volatile bit _ovf;

/* ==================================================================== */
char frame_front()
{
    return _buf[_front];
}

/* ==================================================================== */
char frame_back()
{
    return _buf[_back - 1];
}

/* ==================================================================== */
char *frame_begin()
{
    return &_buf[_front];
}

/* ==================================================================== */
char *frame_end()
{
    return &_buf[_back];
}

/* ==================================================================== */
bit frame_empty()
{
    return _back == _front;
}

/* ==================================================================== */
bit frame_full()
{
    return _back + 1 == _front;
}

/* ==================================================================== */
uint8_t frame_size()
{
    return _back - _front;
}

/* ==================================================================== */
uint8_t frame_max_size()
{
    return _countof(_buf);
}

/* ==================================================================== */
void frame_clear()
{
    _front = 0;
    _back = 0;
    _ovf = 0;
}

/* ==================================================================== */
void frame_push(char c)
{
#if 0
    if (_back >= _countof(_buf)) {
        _ovf = 1;
        return;
    }
#endif

    _buf[_back++] = c;
}

/* ==================================================================== */
void frame_pop()
{
#if 0
    if (_front >= _countof(_buf)) {
        _ovf = 1;
        return;
    }
#endif

    _front++;
}

/* ==================================================================== */
bit frame_overflow()
{
    return _ovf;
}

/* ==================================================================== */
char *frame_lock()
{
    _ovf = 0;
    _front = 0;
    return _buf;
}

/* ==================================================================== */
void frame_unlock(uint8_t size)
{
    _back = size;
}

