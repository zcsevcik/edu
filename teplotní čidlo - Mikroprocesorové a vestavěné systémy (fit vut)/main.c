/*
 * main.c:         Inteligentni teplotni cidlo s mcu PIC,
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

#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <htc.h>

#include "crc.h"
#include "io.h"
#include "modbus.h"
#include "w1.h"

/* ==================================================================== */
/*      C  O  N  F  I  G  U  R  A  T  I  O  N     B  I  T  S            */
/* ==================================================================== */
__CONFIG(FOSC_INTOSCIO &        // internal oscillator; RA6, RA7 as IO
         WDTE_OFF &             // watchdog timer
         PWRTE_ON &             // power-up timer
         MCLRE_OFF &            // RA5/~MCLR/Vpp as IO
         BOREN_ON &             // brown-out reset
         LVP_OFF &              // low-voltage programming
         CP_OFF &               // code program memory protection
         CPD_OFF);              // data code protection

/* ==================================================================== */
/*      E  E  P  R  O  M     M  E  M  O  R  Y     D  A  T  A            */
/* ==================================================================== */
//__EEPROM_DATA('1', CBR_19200, EVENPARITY, ONESTOPBIT, 0, 0, 0, 0);
__EEPROM_DATA('1', CBR_9600, NOPARITY, ONESTOPBIT, 0, 0, 0, 0);

#define EEADDR_ADDR         0x00
#define EEADDR_BAUD         0x01
#define EEADDR_PAR          0x02
#define EEADDR_STOPB        0x03

/* -------------------------------------------------------------------- */
/*      T  I  M  E  R                                                   */
/* -------------------------------------------------------------------- */
#define W1_REFRESH_TIME     30000

// 4Hz ~ 250ms
#define DIVIDER_TMR1        250
#define setup_tmr1() do { \
    TMR1H = 0x85; \
    TMR1L = 0xEE; \
} while (0)

//#define tmr2_en() do { \
//    PR2 = 0xFA; \
//    TMR2 = 0; \
//    T2CON = 0x26; \
//} while (0)

#define tmr2_en() do { \
    PR2 = 0x01; \
    TMR2 = 0; \
    T2CON = 0x66; \
} while (0)

static volatile bit _tmr_conv;
static volatile bit _tmr_read;
static volatile uint8_t _mb_counter;

/* ==================================================================== */
/*      I  N  T  E  R  U  P  T     H  A  N  D  L  E  R                  */
/* ==================================================================== */
void interrupt isr()
{
    if (TMR1IF) {
        TMR1IF = 0;

        /* ------------------------------------------------------------ */
        /*      T  I  M  E  R  1     I  N  T  E  R  R  U  P  T          */
        /* ------------------------------------------------------------ */
        static uint8_t cnt = 0;
        cnt++;

        switch (w1_status) {
            case W1_WAITING: {
                /* ---------------------------------------------------- */
                /*      w1 temperature WAITING state                    */
                /* ---------------------------------------------------- */
                if (cnt >= W1_REFRESH_TIME / DIVIDER_TMR1) {
                    _tmr_conv = true;
                    cnt = 0;
                }
            }   break;

            case W1_PENDING: {
                /* ---------------------------------------------------- */
                /*      w1 temperature PENDING state                    */
                /* ---------------------------------------------------- */
                if (cnt >= W1_TCONV / DIVIDER_TMR1) {
                    LED = 0;

                    w1_status = W1_WAITING;
                    _tmr_read = true;
                    cnt = 0;
                }
            }   break;

            default:
                break;
        }

        setup_tmr1();
    }
    else if (TMR2IF) {
        TMR2IF = 0;

        /* ------------------------------------------------------------ */
        /*      T  I  M  E  R  2     I  N  T  E  R  R  U  P  T          */
        /* ------------------------------------------------------------ */
        _mb_counter++;

        bool tmr_timedout_char = _mb_counter >= MB_TIMEOUT_CHAR;
        bool tmr_timedout_frame = _mb_counter >= MB_TIMEOUT_FRAME;

        switch (mb_state) {
            case MB_INIT:
                /* ---------------------------------------------------- */
                /*      Modbus INIT state                               */
                /* ---------------------------------------------------- */
                if (tmr_timedout_frame) {
                    mb_state = MB_IDLE;
                    _mb_counter = 0;
                    TMR2ON = 0;
                }
                break;

            default:
            case MB_IDLE:
                /* ---------------------------------------------------- */
                /*      Modbus IDLE state                               */
                /* ---------------------------------------------------- */
                break;

            case MB_RECEPTION:
                /* ---------------------------------------------------- */
                /*      Modbus RECEPTION state                          */
                /* ---------------------------------------------------- */
                if (tmr_timedout_char) {
                    mb_state = MB_CONTROL_WAITING;
                }
                break;

            case MB_CONTROL_WAITING:
                /* ---------------------------------------------------- */
                /*      Modbus CONTROL-WAITING state                    */
                /* ---------------------------------------------------- */
                if (tmr_timedout_frame) {
                    mb_state = MB_IDLE;
                    _mb_counter = 0;
                    LED = 0;
                    TMR2ON = 0;
                }
                break;

            case MB_EMISSION:
                /* ---------------------------------------------------- */
                /*      Modbus EMISSION state                           */
                /* ---------------------------------------------------- */
                if (tmr_timedout_frame) {
                    mb_state = MB_IDLE;
                    _mb_counter = 0;
                    TMR2ON = 0;
                }
                break;
        }
    }
    else if (RCIF) {
        RCIF = 0;

        /* ------------------------------------------------------------ */
        /*      U S A R T   R E C E I V E   I N T E R R U P T           */
        /* ------------------------------------------------------------ */
        if (OERR) {
            /* -------------------------------------------------------- */
            /*      USART overrun error                                 */
            /* -------------------------------------------------------- */
            CREN = 0;
            CREN = 1;
            mb_ferr = true;
            return;
        }
        if (FERR) {
            /* -------------------------------------------------------- */
            /*      USART frame error                                   */
            /* -------------------------------------------------------- */
            mb_ferr = true;
            (void)RCREG;
            return;
        }

        uint8_t parb = RCSTA & 0x01;    // parity bit
        uint8_t c = RCREG;              // received byte
        uint8_t* p;

        switch (mb_state) {
            default:
            case MB_INIT: {
                /* ---------------------------------------------------- */
                /*      Modbus INIT state                               */
                /* ---------------------------------------------------- */
                _mb_counter = 0;
            }   return;

            case MB_IDLE: {
                /* ---------------------------------------------------- */
                /*      Modbus IDLE state                               */
                /* ---------------------------------------------------- */
                _mb_counter = 0;
                mb_ferr = false;
                mb_state = MB_RECEPTION;
                p = mb_frame;
                mb_framelen = 0;
                LED = 1;
                tmr2_en();
            }   break;

            case MB_RECEPTION: {
                /* ---------------------------------------------------- */
                /*      Modbus RECEPTION state                          */
                /* ---------------------------------------------------- */
                _mb_counter = 0;
                p = &mb_frame[mb_framelen];
            }   break;

            case MB_CONTROL_WAITING: {
                /* ---------------------------------------------------- */
                /*      Modbus CONTROL-WAITING state                    */
                /* ---------------------------------------------------- */
                mb_ferr = true;
            }   return;

            case MB_EMISSION: {
                /* ---------------------------------------------------- */
                /*      Modbus EMISSION state                           */
                /* ---------------------------------------------------- */
                _mb_counter = 0;
            }   return;
        }

        // check for Modbus frame length
        if (mb_framelen >= MB_MAXFRAMELEN) {
            mb_ferr = true;
            return;
        }

        // save byte
        *p = c;
        mb_framelen++;

        // TODO: implement EVEN and ODD parity check
        if (mb_parity != NOPARITY) {
            mb_ferr = true;
            return;
            //uint8_t myparb = c;
            //for (uint8_t i = 1; i < 8; ++i) {
            //    c >>= 1;
            //    myparb ^= c;
            //}

            //myparb ^= parb;

            //if (myparb & 0x01 == 0) {
            //    mb_ferr = true;
            //    return;
            //}
        }
    }

    return;
}

/* -------------------------------------------------------------------- */
/*      T  I  M  E  R     i  n  i  t                                    */
/* -------------------------------------------------------------------- */
static
void init_timer()
{
    setup_tmr1();
    tmr2_en();

    _mb_counter = 0;

    T1CON = 0x3D;

    TMR1IE = 1;
    TMR2IE = 1;
}

/* -------------------------------------------------------------------- */
/*      E  E  P  R  O  M     l  o  a  d     c  o  n  f  i  g            */
/* -------------------------------------------------------------------- */
static
void init_eeconfig()
{
    mb_addr = eeprom_read(EEADDR_ADDR);
    mb_baud = eeprom_read(EEADDR_BAUD);
    mb_parity = eeprom_read(EEADDR_PAR);
    mb_stopb = eeprom_read(EEADDR_STOPB);
}

/* ==================================================================== */
/*      E    N    T    R    Y         P    O    I    N    T             */
/* ==================================================================== */
void main()
{
    // disable comparator
    CMCON = 0x07;

    // GIE, PEIE
    INTCON = 0xC0;

    LED_TRIS = 0;
    _tmr_conv = true;
    _tmr_read = false;

    init_eeconfig();
    init_uart();
    init_modbus();
    init_w1();
    init_timer();

w1_detect:
    if (w1_readrom()) {
        w1_status = W1_WAITING;
        for ( ; ; ) {
            /* -------------------------------------------------------- */
            /*      main loop                                           */
            /* -------------------------------------------------------- */
            if (_tmr_read) {
                /* ---------------------------------------------------- */
                /*      w1 temperature converted                        */
                /* ---------------------------------------------------- */
                _tmr_read = false;
                w1_readdata();
            }
            if (_tmr_conv) {
                _tmr_conv = false;

                /* ---------------------------------------------------- */
                /*      w1 timer elapsed; convert temperature           */
                /* ---------------------------------------------------- */
                LED = 1;
                w1_status = W1_PENDING;
                w1_convert();
            }

            switch (mb_state) {
                case MB_IDLE:
                    /* ------------------------------------------------ */
                    /*      Modbus IDLE state                           */
                    /* ------------------------------------------------ */
                    break;

                case MB_RECEPTION: {
                    /* ------------------------------------------------ */
                    /*      Modbus RECEPTION state                      */
                    /* ------------------------------------------------ */
                    while (mb_state != MB_CONTROL_WAITING);
                    if (mb_ferr) {
                        break;
                    }
                    if (!mb_checkframe()) {
                        break;
                    }

                    while (mb_state != MB_IDLE);
                    if (mb_ferr) {
                        break;
                    }
                    if (mb_processframe()) {
                        mb_state = MB_EMISSION;
                    }
                }   break;

                case MB_EMISSION: {
                    /* ------------------------------------------------ */
                    /*      Modbus EMISSION state                       */
                    /* ------------------------------------------------ */
                    if (mb_framelen == 0) {
                        break;
                    }

                    CREN = 0;
                    RS485_RE = 0;
                    __delay_ms(1);
                    TXEN = 1;

                    for (uint8_t *p = mb_frame; mb_framelen > 0; mb_framelen--) {
                        putch(*p++);
                    }

                    while (!TRMT);

                    TXEN = 0;
                    RS485_RE = 1;
                    _nop();
                    CREN = 1;

                    _mb_counter = 0;
                    tmr2_en();
                }   break;

                default:
                    /* ------------------------------------------------ */
                    /*      other states                                */
                    /* ------------------------------------------------ */
                    break;
            }

        }
    }
    else {
        /* ------------------------------------------------------------ */
        /*      w1 temperature sensor not detected                      */
        /*      -- underclock to 32kHz                                  */
        /*      -- blinking an LED                                      */
        /*      -- after 30s, try to redetect                           */
        /* ------------------------------------------------------------ */
        uint8_t zINTCON = INTCON;
        INTCON = 0;
        OSCF = 0;

        #undef _XTAL_FREQ
        #define _XTAL_FREQ 32000

        for (uint8_t i = 0; i < 15; i++) {
            LED = 1;
            __delay_ms(1000);
            LED = 0;
            __delay_ms(1000);
        }

        OSCF = 1;
        INTCON = zINTCON;

        goto w1_detect;
    }

}
