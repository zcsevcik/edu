/*
 * uart.c:         TQS1-TQS3 adapter.
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
#include <stddef.h>
#include <stdint.h>
#include <htc.h>

#include "uart.h"
#include "crc.h"

/* ==================================================================== */
/*      G  L  O  B  A  L     V  A  R  I  A  B  L  E  S                  */
/* ==================================================================== */
static uint8_t _port1_cfg;
static uint8_t _port2_cfg;
static uint8_t _cur_cfg;

static volatile const char *_sendbuf;
static volatile uint8_t _sendlen;

/* ==================================================================== */
/*      B  A  U  D     R  A  T  E     G  E  N  E  R  A  T  O  R         */
/* ==================================================================== */
#define BRG(fosc, baud) ((fosc)/(64UL * (baud)) - 1)
#define BRGH(fosc, baud) ((fosc)/(16UL * (baud)) - 1)
#define BRG16(fosc, baud) ((fosc)/(16UL * (baud)) - 1)
#define BRGH16(fosc, baud) ((fosc)/(4UL * (baud)) - 1)

/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
#define UART_EN(rx1, tx1, rx2, tx2) \
    (PORTC = MASK_REGISTER(PORTC, 0xCC, (!(rx1) << 5) | \
                                        (!!(tx1) << 4) | \
                                        (!(rx2) << 1) | \
                                        (!!(tx2) << 0)))

/* ==================================================================== */
#define UART_CFG(p_baud, p_parb) \
    ((((p_baud) << 4) & 0xF0) | ((p_parb) & 0x0F))

/* ==================================================================== */
#define UART_CFG_BAUD(p_cfg) \
    (((p_cfg) >> 4) & 0x0F)

/* ==================================================================== */
#define UART_CFG_PARB(p_cfg) \
    ((p_cfg) & 0x0F)

/* ==================================================================== */
#pragma warning push
#pragma warning disable 350
void _uart_load(uint8_t p_cfg)
{
    _cur_cfg = p_cfg;

    switch (UART_CFG_BAUD(p_cfg)) {

#define X(p_Bd, p_spbrg, p_brgh, p_brg16, p_err) \
    l_##p_Bd: \
    case p_Bd: \
        asm("; " #p_Bd " (" #p_err "%)"); \
        asm("; BRGH=" #p_brgh " BRG16=" #p_brg16); \
        asm("; SPBRG=" #p_spbrg); \
        BRGH = p_brgh; \
        BRG16 = p_brg16; \
        SPBRGH = p_spbrg >> 8; \
        SPBRG = p_spbrg & 0xFF; \
        break;

#include "x_spbrg.def"
    X_SPBRG

#undef X

    default:
        goto l_CBR_19200;
    }

    TX9 = (UART_CFG_PARB(p_cfg) != NOPARITY);
    RX9 = TX9;
}
#pragma warning pop

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
void init_uart()
{
    _sendbuf = NULL;
    _sendlen = 0;

    _port1_cfg = UART_CFG(CBR_19200, EVENPARITY);
    _port2_cfg = UART_CFG(CBR_19200, EVENPARITY);

    TXSTA = MASK_REGISTER(TXSTA, 0x8B, 0x44);
    RCSTA = MASK_REGISTER(RCSTA, 0x2F, 0xC0);

    _uart_load(UART_CFG(CBR_19200, EVENPARITY));
}

/* ==================================================================== */
void close_uart()
{
    UART_EN(0, 0, 0, 0);
    SPEN = 0;

    _cur_cfg = 0x00;
}

/* ==================================================================== */
void uart_config_port1(uint8_t baud, uint8_t parb)
{
    _port1_cfg = UART_CFG(baud, parb);
}

/* ==================================================================== */
void uart_config_port2(uint8_t baud, uint8_t parb)
{
    _port2_cfg = UART_CFG(baud, parb);
}

/* ==================================================================== */
void uart_activate_port1_rx()
{
    TXEN = 0;
#if 0
    UART_EN(1, 0, 0, 0);
#else
    RC5 = !1; /* UART1 RX DISABLE */
    RC4 = 0; /* UART1 TX ENABLE */
    RC1 = !0; /* UART2 RX DISABLE */
    RC0 = 0; /* UART2 TX ENABLE */
#endif
    _uart_load(_port1_cfg);
    CREN = 1;
}

/* ==================================================================== */
void uart_activate_port1_tx()
{
    CREN = 0;
#if 0
    UART_EN(0, 1, 0, 0);
#else
    RC5 = !0; /* UART1 RX DISABLE */
    RC4 = 1; /* UART1 TX ENABLE */
    RC1 = !0; /* UART2 RX DISABLE */
    RC0 = 0; /* UART2 TX ENABLE */
#endif
    _uart_load(_port1_cfg);
    TXEN = 1;
}

/* ==================================================================== */
void uart_activate_port2_rx()
{
    TXEN = 0;
#if 0
    UART_EN(0, 0, 1, 0);
#else
    RC5 = !0; /* UART1 RX DISABLE */
    RC4 = 0; /* UART1 TX ENABLE */
    RC1 = !1; /* UART2 RX DISABLE */
    RC0 = 0; /* UART2 TX ENABLE */
#endif
    _uart_load(_port2_cfg);
    CREN = 1;
}

/* ==================================================================== */
void uart_activate_port2_tx()
{
    CREN = 0;
#if 0
    UART_EN(0, 0, 0, 1);
#else
    RC5 = !0; /* UART1 RX DISABLE */
    RC4 = 0; /* UART1 TX ENABLE */
    RC1 = !0; /* UART2 RX DISABLE */
    RC0 = 1; /* UART2 TX ENABLE */
#endif
    _uart_load(_port2_cfg);
    TXEN = 1;
}

/* ==================================================================== */
void uart_activate_port12_tx()
{
    UART_EN(0, 1, 0, 1);
    _uart_load(_port1_cfg);
    CREN = 0, TXEN = 1;
 }

/* ==================================================================== */
/*    P  A  R  I  T  Y     C  H  E  C  K                                */
/* ==================================================================== */
bit check_parity(uint8_t c, bool parb)
{
    return !count_parity(c) ^ parb;
}

/* ==================================================================== */
bit count_parity(uint8_t c)
{
    switch (UART_CFG_PARB(_cur_cfg)) {
    /* ================================================================ */
    default:
    case NOPARITY:
        return 0;
    /* ================================================================ */
    case ODDPARITY:
        return !crc1(c);
    /* ================================================================ */
    case EVENPARITY:
        return crc1(c);
    /* ================================================================ */
    case MARKPARITY:
        return 1;
    /* ================================================================ */
    case SPACEPARITY:
        return 0;
    /* ================================================================ */
    }
}

/* ==================================================================== */
/*    T  E  R  M  I  N  A  L     I  N  P  U  T  /  O  U  T  P  U  T     */
/* ==================================================================== */
void putch(unsigned char byte)
{
    while (!TXIF);
    TX9D = count_parity(byte);
    TXREG = byte;
    NOP();
}

/* ==================================================================== */
unsigned char getch()
{
    bool parb;
    uint8_t c;

    do {
        while (!RCIF);
        parb = RX9D;
        c = RCREG;
    } while (!check_parity(c, parb));

    return c;
}

/* ==================================================================== */
unsigned char getche()
{
    uint8_t c = getch();
    return putch(c), c;
}

/* ==================================================================== */
#define _uart_int_tx_inl() do { \
    while (!TXIF); \
    TXIE = (_sendlen-- >= 2); \
    TX9D = count_parity(*_sendbuf); \
    TXREG = *_sendbuf++; \
} while (0)

/* ==================================================================== */
bit uart_write(const char *buf, size_t len)
{
    if (!buf) return false;
    if (len == 0) return false;
    if (_sendlen != 0) return false;

    _sendbuf = buf;
    _sendlen = len;

    _uart_int_tx_inl();
    return true;
}

/* ==================================================================== */
void uart_int_tx()
{
    _uart_int_tx_inl();
}
