/*
 * output.c
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2014/12/05
 *
 * This file is part of svetla.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _XTAL_FREQ 1843200UL

#include <xc.h>
#include <stdint.h>

/* ==================================================================== */
#define BAUDRATE CBR_19200
#define PARITY EVENPARITY

/* ==================================================================== */
// RA0-3: OUT5-8
// RA4: LED_Y
// RA5: n/c
// RA6,7: crystal
// RB0: LED_R
// RB1: Rx
// RB2: Tx n/c
// RB3: LED_G
// RB4-7: OUT1-4

#define LED_R RB0
#define LED_G RB3
#define LED_Y RA4

/* ==================================================================== */
#pragma config BOREN=ON
#pragma config CPD=OFF
#pragma config CP=OFF
#pragma config LVP=OFF
#pragma config MCLRE=OFF
#pragma config PWRTE=ON
#pragma config WDTE=ON
#pragma config FOSC=XT

#pragma config IDLOC0=0
#pragma config IDLOC1=0
#pragma config IDLOC2=0
#pragma config IDLOC3=0

/* ==================================================================== */
static unsigned char buffer[80];
static unsigned char length = 0;
static uint16_t outputs = 0;
static bit frame_ok = 0;

/* ==================================================================== */
#define MASK_REGISTER(data, and_mask, or_mask) \
    (((data) & (and_mask)) | ((or_mask) & ~(and_mask)))

/* ==================================================================== */
static const uint8_t
baudrates_spbrg[] = {
#define CBR_600         0
        191,
#define CBR_1200        1
        95,
#define CBR_2400        2
        47,
#define CBR_4800        3
        23,
#define CBR_9600        4
        11,
#define CBR_14400       5
        7,
#define CBR_19200       6
        5,
#define CBR_38400       7
        2,
#define CBR_57600       8
        1,
#define CBR_115200      9
        0,
};

#define BAUD_SPBRG(x) (baudrates_spbrg[x])

/* ==================================================================== */
/* http://www.piclist.com/techref/microchip/math/bit/parity.htm         */
/*                                                                      */
/* Unknown author from microchip forum post by Dario Greggio:           */
/* http://forum.microchip.com/tm.aspx?m=295647&mpage=1&key=&#297219     */

static char parity(char c)
{
    asm("swapf (parity@c), w"); // John's idea: reducing byte to nibble
    asm("xorwf (parity@c), w");
    asm("addlw 41H"); // bit 1 becomes B0^B1 and bit 7 becomes B6^B7
    asm("iorlw 7CH"); // for carry propagation from bit 1 to bit 7
    asm("addlw 2");   // Done! the parity bit is bit 7 of W
    asm("andlw 80H"); // set NZ if odd parity, and leave 00 or 80 in W
    asm("movwf (parity@c)");
    return c;
}

static bit parity_even(char c) {
    return 0 == parity(c);
}

static bit parity_odd(char c) {
    return 0x80 == parity(c);
}

static bit parity_no(char c) {
    return 1;
}

static bit parity_check(char c)
{
    static bit (*check_helper [])(char) = {
#define NOPARITY 0
        &parity_no,
#define EVENPARITY 1
        &parity_even,
#define ODDPARITY 2
        &parity_odd,
    };
    
    static unsigned char what_parity = PARITY;
    return check_helper[what_parity](c);
}

/* ==================================================================== */
static unsigned char ascii_to_byte(unsigned char c)
{
    if ('0' <= c && c <= '9')
        return (c - '0') + 0x00;
    if ('A' <= c && c <= 'F')
        return (c - 'A') + 0x0A;
    return (unsigned char)~0;
}

/* ==================================================================== */
static unsigned char getc()
{
    unsigned char c;
    uint16_t my_delay;

  repeat:
    my_delay = 65535;
    while (!RCIF && --my_delay)
        CLRWDT();

    if (!RCIF)
        goto timedout;

    RCIF = 0;

    if (FERR) {
        (void)RCREG;
      timedout:
        frame_ok = 0;
        LED_R = !1;
        goto repeat;
    }

    if (OERR) {
        CREN = 0;
        CREN = 1;
        frame_ok = 0;
        LED_R = !1;
        goto repeat;
    }

    c = RCREG;
    if (!parity_check(c)) {
        frame_ok = 0;
        LED_R = !1;
        goto repeat;
    }

    return (c &= 0x7f);
}

/* ==================================================================== */
void main(void)
{
    CMCON = 0x07;       // comparators off
    
    INTCON = 0b00000000; //  GIE PEIE T0IE INTE RBIE   T0IF   INTF   RBIF
    PIE1 = 0b00000000;   // EEIE CMIE RCIE TXIE ---- CCP1IE TMR2IE TMR1IE

    PORTA = 0b00010000;
    PORTB = 0b00000111;

    TRISA = 0b11100000;
    TRISB = 0b00000010;

    SPBRG = BAUD_SPBRG(BAUDRATE);
    TXSTA = 0b00000100; // CSRC TX9 TXEN SYNC ---- BRGH TRMT TX9D
    RCSTA = 0b10010000; // SPEN RX9 SREN CREN ADEN FERR OERR RX9D

    PR2 = 0xFF;
    CCPR1L = 0x42;
    CCP1CON = 0x2C;
    T2CON = 0x07;
    LED_G = 1;

    CLRWDT();

    while (1) {
        unsigned char c;

        /* ============================================================ */
      s_idle:
        CLRWDT();
        switch (c = getc()) {
        case ':':
            goto start_of_frame;
        default:
            goto s_idle;
        }

        /* ============================================================ */
      s_reception:
        CLRWDT();
        switch (c = getc()) {
        case ':':
            goto start_of_frame;
        case '\r':
            goto s_waiting_eof;
        default:
            goto concat_char;
        }

        /* ============================================================ */
      s_waiting_eof:
        CLRWDT();
        switch (c = getc()) {
        case '\n':
            goto control_frame;
        case ':':
            goto start_of_frame;
        default:
            frame_ok = 0;
            goto s_waiting_eof;
        }

        /* ============================================================ */
      start_of_frame:
        frame_ok = 1;
        length = 0;
        LED_Y = !1;
        LED_R = !0;
        goto s_reception;

        /* ============================================================ */
      concat_char:
        if (length < sizeof buffer) {
            buffer[length++] = c;
        }
        else {
            frame_ok = 0;
            LED_R = !1;
        }
        goto s_reception;

        /* ============================================================ */
      control_frame:
        if (!frame_ok)
            goto delete_frame;
        if (length % 2 != 0)
            goto delete_frame;
        if (length < 6)
            goto delete_frame;

        do {
            unsigned char lrc = 0;
            for (unsigned char *p = buffer, *q = buffer;
                 q < &buffer[length]; ++p)
            {
                unsigned char nibble_high = ascii_to_byte(*q++);
                unsigned char nibble_low = ascii_to_byte(*q++);

                if (nibble_high == (unsigned char)~0 ||
                    nibble_low == (unsigned char)~0)
                { goto delete_frame; }

                *p = ((nibble_high & 0x0f) << 4)
                   | ((nibble_low & 0x0f) << 0);
                lrc += *p;

                CLRWDT();
            }

            if (lrc != 0)
                goto delete_frame;
        } while (0);

        CLRWDT();

        do {
            unsigned char *p = &buffer[0];
            if (*p++ != 0x00)   // broadcast
                goto delete_frame;
            if (*p++ != 0x0F)   // Write Multiple Coils
                goto delete_frame;
            if (length - 6 != 14)
                goto delete_frame;
            if (*(uint16_t *)p != 0xE803)   // 1000h address
                goto delete_frame;
            p += 2;
            if (*(uint16_t *)p != 0x1000)   // 16 coils
                goto delete_frame;
            p += 2;
            if (*p++ != 0x02)   // 2 bytes follow
                goto delete_frame;

            outputs = *p | (*(p+1) << 8);
            p += 2;
        } while (0);

        CLRWDT();
        goto process_frame;

        /* ============================================================ */
      process_frame:
        LED_Y = !0;
        do {
            unsigned char my_output = (outputs & 0x00FF);
            PORTA = MASK_REGISTER(PORTA, 0xF0, my_output >> 4);
            PORTB = MASK_REGISTER(PORTB, 0x0F, my_output << 4);
        } while (0);
        goto s_idle;

        /* ============================================================ */
      delete_frame:
        LED_Y = !0;
        LED_R = !1;
        goto s_idle;
    }
}
