/*
 * modbus_tmr.c:   TQS1-TQS3 adapter.
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

#include "modbus_tmr.h"
#include "modbus.h"
#include "uart.h"

/* ==================================================================== */
static uint8_t _cur_cfg;
static volatile uint8_t _ctr;

static volatile struct state_t {
    unsigned t1_5 : 1;
    unsigned t3_5 : 1;
} _prevstate;

/* ==================================================================== */
#pragma warning push
#pragma warning disable 350
void init_modbus_tmr(uint8_t baud)
{
    switch (baud) {

#define X(p_Bd, p_pr, p_ckps, p_outps, p_err) \
    l_##p_Bd: \
    case p_Bd: \
        asm("; " #p_Bd " (" #p_err "%)"); \
        asm("; OUTPS=" #p_outps); \
        asm("; CKPS=" #p_ckps); \
        asm("; PR=" #p_pr); \
        MODBUS_TMR_CONTROL = ((p_outps << 3) & 0x78) | \
                             ((p_ckps) & 0x03); \
        MODBUS_TMR_PERIOD = p_pr; \
        _cur_cfg = MODBUS_TMR_CONTROL; \
        break;

#include "x_mbtmr.def"
    X_MBTMR
#undef X

    default:
        goto l_CBR_19200;
    }

    return;
}
#pragma warning pop

/* ==================================================================== */
void modbus_tmr_elapsed()
{
    _ctr++;

    struct state_t state;
    state.t1_5 = (_ctr >= 3);
    state.t3_5 = (_ctr >= 7);

    if (!_prevstate.t1_5 && state.t1_5) {
        _prevstate.t1_5 = 1;
        modbus_event(MB_EVENT_T_1_5, NULL);
    }
    if (!_prevstate.t3_5 && state.t3_5) {
        _prevstate.t3_5 = 1;
        modbus_event(MB_EVENT_T_3_5, NULL);

        /* run once */
        MODBUS_TMR_ON = 0;
        MODBUS_TMR_INT_EN = 0;
    }

    return;
}

/* ==================================================================== */
void modbus_tmr_runonce()
{
    MODBUS_TMR_INT_EN = 0;

    _ctr = 0;
    _prevstate.t1_5 = 0;
    _prevstate.t3_5 = 0;

    MODBUS_TMR_REGISTER = 0;
    MODBUS_TMR_CONTROL = _cur_cfg;
    MODBUS_TMR_ON = 1;
    MODBUS_TMR_INT_EN = 1;
}

