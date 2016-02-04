/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifndef IPK_RDT_H
#define IPK_RDT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RDT_FLAG_WND 0x04
#define RDT_FLAG_SEQ 0x08
#define RDT_FLAG_ACK 0x10
#define RDT_FLAG_RST 0x20
#define RDT_FLAG_SYN 0x40
#define RDT_FLAG_FIN 0x80

#define RDT_WND_FLAG_USABLE 0x00 // not yet send
#define RDT_WND_FLAG_BUFFERED 0x01 // sent but not acked
#define RDT_WND_FLAG_ACKED 0x02 // acked

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _rdt_st rdt_t;

rdt_t*
rdt_new();

void
rdt_dispose(
        rdt_t*);

bool
rdt_data_write(
        rdt_t*,
        char* buf,
        size_t len);

size_t
rdt_data_read(
        rdt_t*,
        char* buf,
        size_t len);

bool
rdt_client_start(
        rdt_t*);

void
rdt_client_step(
        rdt_t*,
        const char* inbuf,
        size_t inbuflen,
        char* outbuf,
        size_t* outbuflen);

bool
rdt_client_done(
        rdt_t*);

bool
rdt_server_start(
        rdt_t*,
        uint8_t window_size);

void
rdt_server_step(
        rdt_t*,
        const char* inbuf,
        size_t inbuflen,
        char* outbuf,
        size_t* outbuflen);

void
rdt_server_done(
        rdt_t*);

#ifdef __cplusplus
}
#endif

#endif //IPK_RDT_H
