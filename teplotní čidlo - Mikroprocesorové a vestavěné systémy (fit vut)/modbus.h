/*
 * modbus.h:       Inteligentni teplotni cidlo s mcu PIC,
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

#ifndef MODBUS_H
#define MODBUS_H

#include <stdbool.h>
#include <stdint.h>

/* -------------------------------------------------------------------- */
/*      M  E  M  O  R  Y     L  I  M  I  T  S                           */
/* -------------------------------------------------------------------- */
#define MB_MAXDATALEN 4
#define MB_MAXFRAMELEN (4 + MB_MAXDATALEN)

/* -------------------------------------------------------------------- */
/*      B  A  U  D     R  A  T  E  S                                    */
/* -------------------------------------------------------------------- */
#define CBR_300     0x02
#define CBR_1200    0x03
#define CBR_2400    0x04
#define CBR_4800    0x05
#define CBR_9600    0x06
#define CBR_19200   0x07
#define CBR_38400   0x08
#define CBR_57600   0x09
#define CBR_115200  0x0A

/* -------------------------------------------------------------------- */
/*      P  A  R  I  T  Y                                                */
/* -------------------------------------------------------------------- */
#define NOPARITY    0
#define ODDPARITY   1
#define EVENPARITY  2

/* -------------------------------------------------------------------- */
/*      S  T  O  P     B  I  T  S                                       */
/* -------------------------------------------------------------------- */
#define ONESTOPBIT  0
#define TWOSTOPBITS 2

/* -------------------------------------------------------------------- */
/*      A  D  D  R  E  S  S  E  S                                       */
/* -------------------------------------------------------------------- */
#define MB_ADDR_BROADCAST   0

/* -------------------------------------------------------------------- */
/*      T  I  M  E  R  2     C  O  N  S  T  A  N  T  S                  */
/* -------------------------------------------------------------------- */
#define MB_TIMEOUT_FRAME    8
#define MB_TIMEOUT_CHAR     4
#define MB_TIMEOUT_FRAME_HI 5
#define MB_TIMEOUT_CHAR_HI  2

/* -------------------------------------------------------------------- */
/*      E  R  R  O  R     C  O  D  E  S                                 */
/* -------------------------------------------------------------------- */
#define MB_EOK          0
#define MB_EFUNC        1
#define MB_EADDR        2
#define MB_EVAL         3
#define MB_EDEVFAIL     4
#define MB_EACK         5
#define MB_EDEVBUSY     6
#define MB_EMEMPAR      8
#define MB_EGATEPATH    10
#define MB_EGATETRGT    11

/* ==================================================================== */
/*     MODBUS Serial Line protocol                                      */
/*     -- http://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf      */
/*     -- http://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf */
/* ==================================================================== */
typedef enum {
    MB_INIT,
    MB_IDLE,
    MB_RECEPTION,
    MB_CONTROL_WAITING,
    MB_EMISSION,
} MB_STATUS;

extern volatile MB_STATUS mb_state;
extern volatile uint8_t   mb_framelen;

extern uint8_t      mb_addr;
extern uint8_t      mb_baud;
extern uint8_t      mb_parity;
extern uint8_t      mb_stopb;
extern uint8_t      mb_frame[MB_MAXFRAMELEN];
extern bit          mb_ferr;

extern void         init_modbus();

extern bit          mb_checkframe();
extern bit          mb_processframe();

#endif //MODBUS_H
