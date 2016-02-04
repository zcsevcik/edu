/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifndef IPK_UDT_H
#define IPK_UDT_H

#include <stdbool.h>
#include "sock.h"

#define UDT_PACKET_MAXLEN 512

#ifdef __cplusplus
extern "C" {
#endif

ipk_sock_t
udt_init(
        char* local_port);

bool
udt_connect(
        ipk_sock_t udt,
        char* remote_port);

int
udt_recv(
        ipk_sock_t udt,
        char* buf,
        size_t nbytes);

int
udt_send(
        ipk_sock_t udt,
        char* buf,
        size_t nbytes);

#ifdef __cplusplus
}
#endif

#endif //IPK_UDT_H
