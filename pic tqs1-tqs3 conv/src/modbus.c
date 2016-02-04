/*
 * modbus.c:       TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _HOSTED 1
#include <conio.h>

#include <stdbool.h>
#include <stdint.h>
#include <htc.h>

#include "modbus.h"
#include "modbus_tmr.h"
#include "frame.h"
#include "byteswap.h"
#include "crc.h"
#include "uart.h"

/* ==================================================================== */
#define TO_UINT8(reg) (* (uint8_t *) & (reg))
#define TO_UINT16_i(reg, i) (* (uint16_t *) & (reg)[(i)])

/* ==================================================================== */
/*      S  T  A  T  E                                                   */
/* ==================================================================== */
enum {
    MB_SINIT = 0,
    MB_SIDLE,
    MB_SRECEPTION,
    MB_SCONTROL_WAITING,
    MB_SEMISSION,
};

/* ==================================================================== */
/*      A  C  T  I  O  N                                                */
/* ==================================================================== */
enum {
    MB_AINIT = 0,
    MB_AIDLE,
    MB_ACONTROL,
    MB_APROCESS,
};

/* ==================================================================== */
/*      A  D  D  R  E  S  S  E  S                                       */
/* ==================================================================== */
enum {
    MB_ADDR_BROADCAST = 0,
    MB_ADDR_SLAVE = 1,
    MB_ADDR_RESERVED = 248,
};

/* ==================================================================== */
/*      M  O  D  B  U  S     I  N  S  T  A  N  C  E                     */
/* ==================================================================== */
static struct {
    unsigned state : 4;
    unsigned action : 4;
} _fsm;

/* ==================================================================== */
static volatile struct {
    unsigned tmr_1_5 : 1;
    unsigned tmr_3_5 : 1;
    unsigned sent : 1;
    unsigned rcvd : 1;
    unsigned err_parity : 1;
    unsigned err_frame : 1;
    unsigned err_ovr : 1;
} _flags;

/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
#define _get_slave_id(buf, count) \
    ((uint8_t) (buf)[0])

/* ==================================================================== */
#define _get_pdu(buf, count) \
    ((void *) & (buf)[1])

/* ==================================================================== */
#define _get_pdu_len(buf, count) \
    ((count) - 2 - 1)

/* ==================================================================== */
#define _get_crc(buf, count) \
    TO_UINT16_i(buf, (count) - 2)

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
void init_modbus()
{
    TO_UINT8(_fsm) = 0;
    TO_UINT8(_flags) = 0;
}

/* ==================================================================== */
void modbus_event(uint8_t t, uint8_t param)
{
    switch (t) {
    /* ================================================================ */
    default:
        break;
    /* ================================================================ */
    case MB_EVENT_RCVD:
        _flags.rcvd = 1;
        frame_push(param);
        break;
    /* ================================================================ */
    case MB_EVENT_SENT:
        _flags.sent = 1;
        break;
    /* ================================================================ */
    case MB_EVENT_ERROR:
        switch (param) {
        /* ============================================================ */
        default:
            break;
        /* ============================================================ */
        case MB_EPARITY:
            _flags.err_parity = 1;
            break;
        /* ============================================================ */
        case MB_EFRAME:
            _flags.err_frame = 1;
            break;
        /* ============================================================ */
        case MB_EOVERRUN:
            _flags.err_ovr = 1;
            break;
        /* ============================================================ */
        }
        break;
    /* ================================================================ */
    case MB_EVENT_T_1_5:
        _flags.tmr_1_5 = 1;
        break;
    /* ================================================================ */
    case MB_EVENT_T_3_5:
        _flags.tmr_3_5 = 1;
        break;
    /* ================================================================ */
    }

    return;
}

/* ==================================================================== */
void modbus_process()
{
#if 0
    RB6 = (MB_SIDLE != _fsm.state);
    RC6 = _flags.err_frame ||
          _flags.err_ovr ||
          _flags.err_parity ||
          frame_overflow();
#endif

    switch (_fsm.state) {
    /* ================================================================ */
    default:
        break;
    /* ================================================================ */
    case MB_SINIT: {
        if (_flags.tmr_3_5) {
            _flags.tmr_3_5 = 0;

            _fsm.state = MB_SIDLE;
        }
        else if (_flags.rcvd) {
            _flags.rcvd = 0;

            modbus_tmr_runonce();
        }
        else {
            TO_UINT8(_flags) = 0;
        }
    }   break;
    /* ================================================================ */
    case MB_SIDLE: {
        if (_flags.rcvd) {
            _flags.rcvd = 0;

            _flags.err_frame = 0;
            TO_UINT8(_flags) &= 0x0f;

            _fsm.state = MB_SRECEPTION;
            modbus_tmr_runonce();
        }
        else if (_flags.sent) {
            _flags.sent = 0;

            _fsm.state = MB_SEMISSION;
            modbus_tmr_runonce();
        }
        else {
            TO_UINT8(_flags) = 0;
        }
    }   break;
    /* ================================================================ */
    case MB_SRECEPTION: {
        if (_flags.rcvd) {
            _flags.rcvd = 0;

            modbus_tmr_runonce();
        }
        else if (_flags.tmr_1_5) {
            _flags.tmr_1_5 = 0;

            _fsm.state = MB_SCONTROL_WAITING;
            _fsm.action = MB_ACONTROL;
        }
    }   break;
    /* ================================================================ */
    case MB_SCONTROL_WAITING: {
        if (_flags.rcvd) {
            _flags.rcvd = 0;

            _flags.err_frame = 1;
        }
        else if (_flags.tmr_3_5) {
            _flags.tmr_3_5 = 0;

            _fsm.state = MB_SIDLE;
            _fsm.action = MB_APROCESS;
        }
    }   break;
    /* ================================================================ */
    case MB_SEMISSION: {
        if (_flags.sent) {
            _flags.sent = 0;

            modbus_tmr_runonce();
        }
        else if (_flags.tmr_3_5) {
            _flags.tmr_3_5 = 0;

            _fsm.state = MB_SIDLE;

            while (!TRMT);
            frame_clear();
            _modbus_emission_done();
        }
    }   break;
    /* ================================================================ */
    }

    switch (_fsm.action) {
    /* ================================================================ */
    default:
    case MB_AIDLE:
        return;
    /* ================================================================ */
    case MB_AINIT:
        frame_clear();
        modbus_tmr_runonce();
        break;
    /* ================================================================ */
    case MB_ACONTROL: {
        if (_flags.err_frame || frame_overflow()) {
            break;
        }

        char *buf = frame_begin();
        uint8_t len = frame_size();

        uint16_t crc_their = _get_crc(buf, len);
        uint16_t crc_ours = crc16(buf, len - 2);

        if (crc_ours != crc_their) {
            _flags.err_frame = 1;
        }
    }   break;
    /* ================================================================ */
    case MB_APROCESS: {
        if (_flags.err_frame || frame_overflow()) {
            break;
        }

        char *buf = frame_begin();
        uint8_t len = frame_size();

        void *pdu = _get_pdu(buf, len);
        uint8_t pdulen = _get_pdu_len(buf, len);
        uint8_t addr = _get_slave_id(buf, len);

        /* get request */
        uint8_t res = _modbus_request_pdu(pdu, pdulen, addr);
        frame_clear();

        /* do nothing ? */
        if (MRP_DONOTHING == res) break;

        /* init write buffer */
        buf = frame_lock();
        pdu = _get_pdu(buf, len);
        len = 0;

        /* write address */
        buf[len++] = addr;

        /* write response */
        switch (res) {
        /* ============================================================ */
        default:
            break;
        /* ============================================================ */
        case MRP_REPLY:
            pdulen = _modbus_response_pdu(pdu);
            break;
        /* ============================================================ */
        case MRP_REPLY_ERROR:
            pdulen = _modbus_exception_pdu(pdu);
            break;
        /* ============================================================ */
        }

        if (!pdulen) break;

        len += pdulen;

        /* write crc */
        uint16_t crc = crc16(buf, len);
        TO_UINT16_i(buf, len) = crc;
        len += sizeof(uint16_t);
        frame_unlock(len);

        _modbus_emission_start();
        uart_write(buf, len);
    }   break;
    /* ================================================================ */
    }

    _fsm.action = MB_AIDLE;
    return;
}
