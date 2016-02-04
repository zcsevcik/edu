/*
 * ouidb.h:        Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/14
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifndef OUIDB_H
#define OUIDB_H

#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ouidb_foreach_fn)(const uint8_t oui[3],
                                 const char* name,
                                 void* obj);

bool ouidb_init(const char* filename);
void ouidb_cleanup();
const char* ouidb_search(struct in6_addr);
void ouidb_foreach(ouidb_foreach_fn, void* obj);

#ifdef __cplusplus
}
#endif

#endif //OUIDB_H
