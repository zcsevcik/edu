/*
 * ouinetwork.h:   Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/18
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifndef OUINETWORK_H
#define OUINETWORK_H

#include <stdbool.h>
#include <stdint.h>

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*in6_found_fn)(struct in6_addr, void* arg);

int _addr2name6(
        struct in6_addr addr,
        char* buf, size_t len);

bool oui_scan_network(
        struct in6_addr prefix,
        uint8_t prefixlen,
        uint16_t packets_per_sec,
        in6_found_fn, void*);

#ifdef __cplusplus
}
#endif

#endif //OUINETWORK_H
