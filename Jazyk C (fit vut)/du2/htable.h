/*
 * htable.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-04-07
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU2 2).
 */

#ifndef HTABLE_H_
#define HTABLE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
typedef struct htab_t htab_t;

/* ==================================================================== */
typedef struct _htab_listitem_st {
    void *data;
    struct _htab_listitem_st *next;
    const char key[];
} htab_listitem;

/* ==================================================================== */
htab_t * htab_init(size_t);
void htab_free(htab_t *);
htab_listitem * htab_lookup(htab_t *, const char *key);
void htab_foreach(htab_t *, void (*)(const char *key, void *data));
int htab_remove(htab_t *, const char *key);
void htab_clear(htab_t *);
void htab_statistics(htab_t *);

/* ==================================================================== */
unsigned hash_function(const char *key, unsigned htab_size);

/* ==================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* HTABLE_H_ */
