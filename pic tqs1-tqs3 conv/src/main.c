/*
 * main.c:         TQS1-TQS3 adapter.
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

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <htc.h>

#include "io.h"
#include "osc.h"
#include "bitop.h"
#include "uart.h"
#include "tqs1.h"
#include "modbus.h"
#include "modbus_pdu.h"
#include "modbus_tmr.h"
#include "byteswap.h"

/* ==================================================================== */
/*      C  O  N  F  I  G  U  R  A  T  I  O  N     B  I  T  S            */
/* ==================================================================== */
__CONFIG(FCMEN_OFF &            // fail-safe clock monitor
         IESO_ON &              // internal/external switchover
         BOREN_OFF &            // brown-out reset
         CPD_OFF &              // data code protection
         CP_OFF &               // code program memory protection
         MCLRE_OFF &            // MCLR
         PWRTE_ON &             // power-up timer
         WDTE_ON &              // watchdog timer
         FOSC_XT);              // XT oscillator; crystal on RA4, RA5

/* ==================================================================== */
/*      E  E  P  R  O  M     M  E  M  O  R  Y     D  A  T  A            */
/* ==================================================================== */

/* ==================================================================== */
/*      D  E  V  I  C  E     I  D  E  N  T  I  F  I  C  A  T  I  O  N 	*/
/* ==================================================================== */
static const char *DEVICE_ID[] = {
    /* [MB_DEVICEID_VENDOR] = */ "Radek Sevcik",
    /* [MB_DEVICEID_CODE] = */ "00000000",
    /* [MB_DEVICEID_VERSION] = */ "2013/08/07",
    /* [MB_DEVICEID_URL] = */ "zcsevcik@gmail.com",
    /* [MB_DEVICEID_NAME] = */ "tqs1-tqs3 adapter",
    /* [MB_DEVICEID_MODEL] = */ "rev. 1",
    /* [MB_DEVICEID_APPNAME] = */ "",
};

/* ==================================================================== */
/*      G  L  O  B  A  L     D  A  T  A                                 */
/* ==================================================================== */
struct _prog_st {
    /* addressing */
    uint8_t mb_addr;
    uint8_t tqs_addr;
    uint8_t caddr;

    /* modbus link */
    uint8_t mb_speed;
    uint8_t mb_parity;

    /* tqs link */
    uint8_t tqs_speed;
    uint8_t tqs_parity;
};

static struct _prog_st _prog;

/* ==================================================================== */
static uint8_t _modbus_errno;
static uint8_t _modbus_fncode;
static uint8_t _modbus_addr;

enum {
    PROGA_ID,
    PROGA_LINK,
};

static uint8_t _modbus_action;

/* ==================================================================== */
/*      T  I  M  E  R     I  N  I  T  I  A  L  I  Z  E                  */
/* ==================================================================== */
#define TMR_PR_f(fosc, freq, prescaler, postscaler) \
    ((fosc)/(4UL * (freq) * (prescaler) * (postscaler)))

#define TMR_PR_usec(fosc, usec, prescaler, postscaler) \
    (((fosc) * (usec))/(4000000UL * (prescaler) * (postscaler)))

#define TMR_PR_msec(fosc, msec, prescaler, postscaler) \
    (((fosc) * (msec))/(4000UL * (prescaler) * (postscaler)))

#define TMR_PR_sec(fosc, sec, prescaler, postscaler) \
    (((fosc) * (sec))/(4UL * (prescaler) * (postscaler)))

/* ==================================================================== */
/*      M  O  D  B  U  S     H  A  N  D  L  E  R  S                     */
/* ==================================================================== */
#if 0
static uint8_t _mei_deviceid(mb_req_mei_deviceid * req,
                             mb_rsp_mei_deviceid * rsp, uint8_t * len)
{
    uint8_t i = 0;
    uint8_t end = 0;

#define COPY_STRING(dst, src) ( \
	strcpy((dst), (src)), \
	strlen((src)) + sizeof(char))

#define RESET_OID(oid) do { \
    if ((oid) >= _countof(DEVICE_ID)) \
	    (oid) = 0; \
} while (0)

    switch (req->code) {
    default:
        return MB_EVAL;

    case MB_DEVICEID_BASIC:
        RESET_OID(req->oid);
        i = req->oid;
        end = MB_DEVICEID_VERSION;
        break;

    case MB_DEVICEID_REGULAR:
        RESET_OID(req->oid);
        i = req->oid;
        end = MB_DEVICEID_APPNAME;
        break;

    case MB_DEVICEID_SPECIFIC:
        if (req->oid >= _countof(DEVICE_ID))
            return MB_EADDR;

        i = end = req->oid;
        break;
    }

    if (i > end) {
        end = i;
    };

    rsp->code = req->code;
    rsp->conformity_lvl = MB_DEVICEID_REGULAR_;
    rsp->more_follows = MB_cBOOL_FALSE;
    rsp->next_oid = 0;
    rsp->lObjects = end - i + 1;

    for (_mb_mei_object * p = rsp->objects; i <= end; ++i, ++p) {
        p->oid = i;
        p->cByte = COPY_STRING(p->data, DEVICE_ID[i]);

        *len += sizeof(*rsp->objects) + p->cByte;
        p = (_mb_mei_object *) ((char *) p + p->cByte);
    }

    *len += sizeof(mb_rsp_mei_deviceid);
    return 0;
}

static uint8_t _mei(mb_mei * req, mb_mei * rsp, uint8_t * len)
{
    uint8_t errno = MB_EFUNC;

    switch (req->type) {
    default:
        errno = MB_EFUNC;
        break;

    case MB_FMEI_DEVICEID:
        errno = _mei_deviceid((void *) req->data, (void *) rsp->data, len);
        break;
    }

    if (!errno) {
        rsp->type = req->type;
        *len += sizeof(mb_mei);
    }

    return errno;
}
#endif

/* ==================================================================== */
uint8_t _modbus_request_pdu(struct mb_req_pdu *pdu,
                            uint8_t len, uint8_t addr)
{
    _modbus_fncode = pdu->function_code;
    _modbus_addr = addr;

#define PDU_CAST(t) \
    ((t *) pdu->request_data)

    /* check addr */
    if (!(_prog.mb_addr <= addr && addr <= _prog.mb_addr + _prog.caddr))
        return MRP_DONOTHING;

    switch (pdu->function_code) {
    /* ================================================================ */
    case MB_FR_COIL:
    case MB_FR_INPUT:
    case MB_FR_INPUT_REG:
    case MB_FW_COIL:
    case MB_FW_REG:
    case MB_FW_COILS:
    case MB_FW_REGS:
    case MB_FRW_REGS:
    case MB_FR_SLAVEID:
    default:
        _modbus_errno = MB_EFUNC;
        return MRP_REPLY_ERROR;
    /* ================================================================ */
    case MB_FR_HOLDING_REG:
        if (PDU_CAST(mb_req_r_holding_reg)->quantity != be16toh_c(1)) {
            _modbus_errno = MB_EADDR;
            return MRP_REPLY_ERROR;
        }

        switch (PDU_CAST(mb_req_r_holding_reg)->address) {
        /* ============================================================ */
        default:
            _modbus_errno = MB_EADDR;
            return MRP_REPLY_ERROR;
        /* ============================================================ */
        case be16toh_c(0x01):
            _modbus_action = PROGA_ID;
            break;
        /* ============================================================ */
        case be16toh_c(0x02):
            _modbus_action = PROGA_LINK;
            break;
        /* ============================================================ */
        }
        break;
    /* ================================================================ */
#if 0
    case MB_FMEI:
        errno = _mei((void *) &pdu->request_data,
                     (void *) &pdu->request_data, len);
        break;
#endif
    /* ================================================================ */
    }

    return MRP_REPLY;

#undef PDU_CAST
}

/* ==================================================================== */
uint8_t _modbus_response_pdu(struct mb_rsp_pdu *pdu)
{
    pdu->function_code = _modbus_fncode;

#define PDU_CAST(t) \
    ((t *) pdu->response_data)

    switch (_modbus_action) {
    default:
        return 0;
    /* ================================================================ */
    case PROGA_ID:
        PDU_CAST(mb_rsp_r_holding_reg)->cByte = 1 * sizeof(uint16_t);
        PDU_CAST(mb_rsp_r_holding_reg)->wValues[0] = htobe16(_modbus_addr);
        return sizeof(struct mb_rsp_pdu) + sizeof(mb_rsp_r_holding_reg) + sizeof(uint16_t);
    /* ================================================================ */
    case PROGA_LINK:
        PDU_CAST(mb_rsp_r_holding_reg)->cByte = 1 * sizeof(uint16_t);
        PDU_CAST(mb_rsp_r_holding_reg)->cValues[0] = _prog.mb_parity;
        PDU_CAST(mb_rsp_r_holding_reg)->cValues[1] = _prog.mb_speed;
        return sizeof(struct mb_rsp_pdu) + sizeof(mb_rsp_r_holding_reg) + sizeof(uint16_t);
    /* ================================================================ */
    }

#undef PDU_CAST
}

/* ==================================================================== */
uint8_t _modbus_exception_pdu(struct mb_excep_rsp_pdu *pdu)
{
    pdu->function_code = _modbus_fncode + 0x80;
    pdu->exception_code = _modbus_errno;
    return sizeof(struct mb_excep_rsp_pdu);
}

/* ==================================================================== */
/*      I  N  T  E  R  U  P  T     H  A  N  D  L  E  R                  */
/* ==================================================================== */
void interrupt isr()
{
    if (MODBUS_TMR_INT_EN && MODBUS_TMR_INT_REQ) {
        /* ============================================================ */
        /*      M O D B U S   I N T E R R U P T                         */
        /* ============================================================ */
        MODBUS_TMR_INT_REQ = 0;
        modbus_tmr_elapsed();
    }
    else if (RCIE && RCIF) {
        /* ============================================================ */
        /*      U A R T   R E C E I V E   I N T E R R U P T             */
        /* ============================================================ */
        RCIF = 0;

        if (OERR) {
            /* ======================================================== */
            /*      UART overrun error                                  */
            /* ======================================================== */
            CREN = 0, CREN = 1;
            modbus_event(MB_EVENT_ERROR, MB_EOVERRUN);
            return;
        }
        if (FERR) {
            /* ======================================================== */
            /*      UART frame error                                    */
            /* ======================================================== */
            modbus_event(MB_EVENT_ERROR, MB_EFRAME);
            (void) RCREG;
            return;
        }

        uint8_t parb = RX9D;
        uint8_t c = RCREG;

        if (!check_parity(c, parb)) {
            modbus_event(MB_EVENT_ERROR, MB_EPARITY);
            return;
        }

        modbus_event(MB_EVENT_RCVD, c);
    }
    else if (TXIE && TXIF) {
        /* ============================================================ */
        /*      U A R T   T R A N S M I T   I N T E R R U P T           */
        /* ============================================================ */
        TXIF = 0;
        uart_int_tx();
        modbus_event(MB_EVENT_SENT, NULL);
    }

    return;
}

/* ==================================================================== */
bool _tqs1_read(char *buf, size_t len, char eof)
{
    uart_activate_port1_rx();
    char c;
    do {
        c = getch();
        if (len-- <= 1) return false;
        *buf++ = c;
    } while (c != eof);
    *buf++ = '\0';

    return true;
}

/* ==================================================================== */
bool _tqs1_write(const char *p)
{
    uart_activate_port1_tx();
    while (*p) putch(*p++);
    while (!TRMT);
    return true;
}

/* ==================================================================== */
char _tohex(char nibble)
{
    nibble &= 0x0f;
    if (nibble >= 0x0a)
        return ('A' - 0x0a) + nibble;
    else
        return '0' + nibble;
}

/* ==================================================================== */
enum {
    S_INIT,
    S_MB_RX,
    S_MB_TX,
    S_TQS1_TX,
    S_TQS1_RX,
};

static uint8_t state = S_INIT;
static uint8_t nextstate = S_MB_RX;

void _modbus_emission_start()
{
    nextstate = S_MB_TX;
    RCIE = 0;
    uart_activate_port1_tx();
}

void _modbus_emission_done()
{
    nextstate = S_MB_RX;
    uart_activate_port1_rx();
    RCIE = 1;
}

/* ==================================================================== */
/*      E    N    T    R    Y         P    O    I    N    T             */
/* ==================================================================== */
void main()
{
    init_osc(8000000, 0);
    init_io();

    init_uart();

    _prog.caddr = 4;
    _prog.mb_addr = 'A';
    _prog.mb_speed = CBR_38400;
    _prog.mb_parity = EVENPARITY;
    _prog.tqs_addr = 'A';
    _prog.tqs_speed = CBR_9600;
    _prog.tqs_parity = NOPARITY;

    uart_config_port1(_prog.mb_speed, _prog.mb_parity);
    uart_config_port2(_prog.tqs_speed, _prog.tqs_parity);

    init_modbus();
    init_modbus_tmr(_prog.mb_speed);

#if 0
    RC6 = 0;
    RC7 = 0;
    TRISC6 = 0;
    TRISC7 = 0;
#endif

    osc_waitstable_ext();
    uart_activate_port1_rx();

    RCIE = 1;
    PEIE = 1;
    ei();

    while (1) {
        CLRWDT();

#if 0
        if (nextstate != state) {
            switch (nextstate) {
            /* ======================================================== */
            default:
                break;
            /* ======================================================== */
            case S_MB_RX:
                uart_activate_port1_rx();
                break;
            /* ======================================================== */
            case S_MB_TX:
                uart_activate_port1_tx();
                break;
            /* ======================================================== */
            case S_TQS1_RX:
                uart_activate_port2_rx();
                break;
            /* ======================================================== */
            case S_TQS1_TX:
                uart_activate_port2_tx();
                break;
            /* ======================================================== */
            }
        }
#endif

        switch (state) {
        /* ============================================================ */
        default:
            break;
        /* ============================================================ */
        case S_MB_RX:
        case S_MB_TX:
            modbus_process();
            break;
        /* ============================================================ */
        case S_TQS1_RX:
        case S_TQS1_TX:
            break;
        /* ============================================================ */
        }

        state = nextstate;
    };

    return;
}
