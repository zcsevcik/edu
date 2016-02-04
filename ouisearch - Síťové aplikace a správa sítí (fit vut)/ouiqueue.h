/*
 * ouiqueue.h:     Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/18
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifndef OUIQUEUE_H
#define OUIQUEUE_H

#include <stdbool.h>
#include <time.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ouiqueue_init();
void ouiqueue_cleanup();
bool ouiqueue_push(struct in6_addr, struct timespec*);
bool ouiqueue_pop(struct in6_addr*);


#ifdef __cplusplus
}
#endif

#endif //OUIQUEUE_H
