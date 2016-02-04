/*
 * modbus.h:       TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

/* ==================================================================== */
/* MODBUS over serial line specification and implementation guide V1.02 */
/*     -- http://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf      */
/* ==================================================================== */
#ifndef MODBUS_H
# define MODBUS_H

# include <stdint.h>

# include "modbus_pdu.h"

/* ==================================================================== */
#define MB_cBOOL_FALSE 0x00
#define MB_cBOOL_TRUE 0xff
#define MB_wBOOL_FALSE 0x0000
#define MB_wBOOL_TRUE 0xff00

/* ==================================================================== */
enum {
    MB_EVENT_RCVD,
    MB_EVENT_SENT,
    MB_EVENT_ERROR,
    MB_EVENT_T_1_5,
    MB_EVENT_T_3_5,
};

/* ==================================================================== */
enum {
    MB_EPARITY,
    MB_EFRAME,
    MB_EOVERRUN,
};

/* ==================================================================== */
#define MRP_DONOTHING 0
#define MRP_REPLY 1
#define MRP_REPLY_ERROR -1

extern uint8_t _modbus_request_pdu(
        struct mb_req_pdu *pdu,
        uint8_t len,
        uint8_t addr);

extern uint8_t _modbus_response_pdu(
        struct mb_rsp_pdu *pdu);

extern uint8_t _modbus_exception_pdu(
        struct mb_excep_rsp_pdu *pdu);

/* ==================================================================== */
extern void _modbus_emission_start();
extern void _modbus_emission_done();

/* ==================================================================== */
extern void init_modbus();

extern void modbus_event(uint8_t type, uint8_t param);
extern void modbus_process();

#endif                          //MODBUS_H
