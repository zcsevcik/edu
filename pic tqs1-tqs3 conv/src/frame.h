/*
 * frame.h:        TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef FRAME_H
# define FRAME_H

# include <htc.h>

/* ==================================================================== */
extern char frame_front();
extern char frame_back();

/* ==================================================================== */
extern char *frame_begin();
extern char *frame_end();

/* ==================================================================== */
extern char *frame_lock();
extern void frame_unlock(uint8_t size);

/* ==================================================================== */
extern bit frame_overflow();
extern bit frame_empty();
extern bit frame_full();
extern uint8_t frame_size();
extern uint8_t frame_max_size();

/* ==================================================================== */
extern void frame_clear();
extern void frame_push(char c);
extern void frame_pop();

#endif                          //FRAME_H
