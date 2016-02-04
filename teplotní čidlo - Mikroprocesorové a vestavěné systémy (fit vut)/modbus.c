/*
 * modbus.c:       Inteligentni teplotni cidlo s mcu PIC,
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "modbus.h"
#include "modbusproto.h"

#include <stdbool.h>
#include <htc.h>

#include "byteswap.h"
#include "crc.h"
#include "io.h"

#define to_uint16(p) (*(uint16_t*)(p))

/* -------------------------------------------------------------------- */
/*      F  U  N  C  T  I  O  N     C  O  D  E  S                        */
/* -------------------------------------------------------------------- */
#define MB_FNCODE_RD_COIL               0x01
#define MB_FNCODE_RD_DISCRETE_INPUT     0x02
#define MB_FNCODE_RD_HOLDING_REGISTER   0x03
#define MB_FNCODE_RD_INPUT_REGISTER     0x04
#define MB_FNCODE_WR_SINGLE_COIL        0x05
#define MB_FNCODE_WR_SINGLE_REGISTER    0x06

/* -------------------------------------------------------------------- */
/*      D  A  T  A     M  A  C  R  O  S                                 */
/* -------------------------------------------------------------------- */
#define mb_getaddr() (mb_frame[0])
#define mb_getfunc() (mb_frame[1])
#define mb_getdataaddr() (ntohs(to_uint16(mb_frame + 2)))
#define mb_getdatalen() (ntohs(to_uint16(mb_frame + 4)))
#define mb_getdataval() (ntohs(to_uint16(mb_frame + 4)))
#define mb_getcrc() (to_uint16(&mb_frame[mb_framelen - 2]))

#define mb_isbroadcast() (mb_getaddr() == MB_ADDR_BROADCAST)

/* -------------------------------------------------------------------- */
/*      M  O  D  B  U  S     I  N  S  T  A  N  C  E                     */
/* -------------------------------------------------------------------- */
volatile MB_STATUS mb_state;
volatile uint8_t mb_framelen;
uint8_t mb_addr;
uint8_t mb_baud;
uint8_t mb_parity;
uint8_t mb_stopb;
uint8_t mb_frame[MB_MAXFRAMELEN];
bit mb_ferr;

/* -------------------------------------------------------------------- */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* -------------------------------------------------------------------- */
void init_modbus()
{
    // set timer2
    switch (mb_baud) {
        case CBR_300:
            PR2 = 0x5B;
            T2CON = 0x7A;
            break;

        case CBR_1200:
            PR2 = 0x50;
            T2CON = 0x85;
            break;

        case CBR_2400:
            PR2 = 0x0D;
            T2CON = 0x6A;
            break;

        case CBR_4800:
            PR2 = 0x1C;
            T2CON = 0x61;
            break;

        case CBR_9600:
            PR2 = 0x0D;
            T2CON = 0x69;
            break;

        default:
        case CBR_19200:
            PR2 = 0x07;
            T2CON = 0x61;
            break;

        //for greater values
            //PR2 = 0x7D;
            //T2CON = 0x00;
    }

    // set tris
    ADDR_EN_TRIS = 1;

    mb_state = MB_INIT;
    RCIE = 1;
}

bit mb_checkframe()
{
    if (mb_framelen < 4 ||
        mb_framelen > MB_MAXFRAMELEN)
    { goto return_error; }
    if (mb_getaddr() != mb_addr &&
        !mb_isbroadcast())
    { goto return_error; }
    if (mb_getcrc() != crc16(mb_frame, mb_framelen - 2)) {
        goto return_error;
    }

    mb_ferr = false;
    return true;

return_error:
    mb_ferr = true;
    return false;
}

bit mb_processframe()
{
    uint8_t err;
    uint16_t val;

    uint16_t dataaddr = mb_getdataaddr();
    uint16_t dataval = mb_getdataval();

    switch (mb_getfunc()) {
        case MB_FNCODE_RD_COIL:
            err = proto_get_rw_byte(dataaddr, dataval, (uint8_t*)&val);
            goto mb_check;

        case MB_FNCODE_RD_DISCRETE_INPUT:
            err = proto_get_ro_byte(dataaddr, dataval, (uint8_t*)&val);
            goto mb_check;

        case MB_FNCODE_RD_HOLDING_REGISTER:
            err = proto_get_rw_word(dataaddr, dataval, &val);
            goto mb_check;

        case MB_FNCODE_RD_INPUT_REGISTER:
            err = proto_get_ro_word(dataaddr, dataval, &val);
            goto mb_check;

        case MB_FNCODE_WR_SINGLE_COIL: {
            switch (dataval) {
                default:
                    err = MB_EVAL;
                    goto mb_check;

                case 0x0000:
                    val = false;
                    break;

                case 0xFF00:
                    val = true;
                    break;
            }

            err = proto_set_rw_byte(dataaddr, val);
        }   goto mb_check;

        case MB_FNCODE_WR_SINGLE_REGISTER: {
            val = dataval;
            err = proto_set_rw_word(dataaddr, val);
        }   goto mb_check;

        default:
            err = MB_EFUNC;
            break;
    }

    err = MB_EFUNC;

mb_check:
    if (mb_isbroadcast()) {
        return false;
    }

    mb_frame[0] = mb_addr;

    if (err != MB_EOK) {
        mb_frame[1] = mb_getfunc() + 0x80;
        mb_frame[2] = err;
        mb_framelen = 3;
    }
    else {
        mb_frame[1] = mb_getfunc();

        switch (mb_getfunc()) {
            default:
                err = MB_EFUNC;
                goto mb_check;

            case MB_FNCODE_RD_COIL:
            case MB_FNCODE_RD_DISCRETE_INPUT:
                mb_frame[2] = (uint8_t)dataval;
                mb_frame[3] = (uint8_t)val;
                mb_framelen = 4;
                break;

            case MB_FNCODE_RD_HOLDING_REGISTER:
            case MB_FNCODE_RD_INPUT_REGISTER:
                mb_frame[2] = (uint8_t)(2 * dataval);
                to_uint16(&mb_frame[3]) = htons(val);
                mb_framelen = 5;
                break;

            case MB_FNCODE_WR_SINGLE_COIL:
                to_uint16(&mb_frame[2]) = htons(dataaddr);
                to_uint16(&mb_frame[4]) = htons(dataval);
                mb_framelen = 6;
                break;

            case MB_FNCODE_WR_SINGLE_REGISTER:
                to_uint16(&mb_frame[2]) = htons(dataaddr);
                to_uint16(&mb_frame[4]) = htons(dataval);
                mb_framelen = 6;
                break;
        }
    }

    to_uint16(&mb_frame[mb_framelen]) = crc16(mb_frame, mb_framelen);
    mb_framelen += 2;

    return true;
}
