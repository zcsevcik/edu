/*
 * htable.c
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htable.h"

/* ==================================================================== */
struct htab_t {
    unsigned count;
    htab_listitem *items[];
};

/* ==================================================================== */
inline htab_listitem * listitem_init(const char *key)
{
    size_t keylen = strlen(key) + 1;

    htab_listitem *p = (htab_listitem *)
            malloc(sizeof(htab_listitem) + keylen);

    if (p) {
        p->data = NULL;
        p->next = NULL;
        memcpy((char *)p->key, key, keylen);
    }

    return p;
}

/* ==================================================================== */
inline void listitem_free(htab_listitem *p)
{
    free(p);
}

/* ==================================================================== */
htab_t * htab_init(size_t p_size)
{
    htab_t *t = (htab_t *)malloc(
            sizeof(htab_t) +
            sizeof(htab_listitem *) * p_size);
    if (t) {
        t->count = p_size;
        memset(t->items, '\0', sizeof(htab_listitem *) * p_size);
    }

    return t;
}

/* ==================================================================== */
void htab_free(htab_t *t)
{
    /* adept na inline; avsak opaque pointer je vyhodnejsi */

    htab_clear(t);
    free(t);
}

/* ==================================================================== */
htab_listitem * htab_lookup(htab_t *t, const char *key)
{
    if (t && key) {
        unsigned offset = hash_function(key, t->count);
        assert(offset < t->count);

        htab_listitem **q = &t->items[offset];
        htab_listitem *p = *q;

        for ( ; p && strcmp(p->key, key) != 0; q = &p->next, p = *q) ;
        return !p ? *q = listitem_init(key) : p;
    }

    return NULL;
}

/* ==================================================================== */
void htab_foreach(htab_t *t, void (*p_fn)(const char *, void *))
{
    if (t && p_fn) {
        for (htab_listitem **pp_i = t->items;
                             pp_i < &t->items[t->count];
                           ++pp_i)
        {
            for (htab_listitem *p = *pp_i; p; p = p->next)
                p_fn(p->key, p->data);
        }
    }
}

/* ==================================================================== */
int htab_remove(htab_t *t, const char *key)
{
    if (t && key) {
        unsigned offset = hash_function(key, t->count);
        assert(offset < t->count);

        htab_listitem **q = &t->items[offset];
        htab_listitem *p = *q;

        for ( ; p && strcmp(p->key, key) != 0; q = &p->next, p = *q) ;

        if (p) {
            *q = p->next;
            listitem_free(p);
            return 1;
        }
    }

    return 0;
}

/* ==================================================================== */
void htab_clear(htab_t *t)
{
    if (t) {
        for (htab_listitem **pp_i = t->items;
                             pp_i < &t->items[t->count];
                           ++pp_i)
        {
            for (htab_listitem *q, *p = *pp_i; p; p = q) {
                q = p->next;
                listitem_free(p);
            }

            *pp_i = NULL;
        }

    }
}

/* ==================================================================== */
void htab_statistics(htab_t *t)
{
    if (t) {
        unsigned min = -1u;
        unsigned max = 0;
        unsigned total = 0;
        unsigned empty = 0;

        size_t bytes_keys = 0;

        for (htab_listitem **pp_i = t->items;
                             pp_i < &t->items[t->count];
                           ++pp_i)
        {
            unsigned count_at_i = 0;
            for (htab_listitem *p = *pp_i; p; p = p->next) {
                ++count_at_i;
                bytes_keys += strlen(p->key) + 1;
            }

            total += count_at_i;
            if (count_at_i < min) min = count_at_i;
            if (count_at_i > max) max = count_at_i;
            if (count_at_i == 0) ++empty;
        }

        size_t bytes_table = (char *)&t->items[t->count] - (char *)t;
        size_t bytes_items = total * sizeof(htab_listitem) + bytes_keys;
        size_t bytes_total = bytes_table + bytes_items;

        float avg = (float)total / (float)t->count;
        fprintf(stderr, "-- table size: %u\n", t->count);
        fprintf(stderr, "-- items count: %u\n", total);
        fprintf(stderr, "-- empty count: %u\n", empty);
        fprintf(stderr, "-- minimum length: %u\n", min);
        fprintf(stderr, "-- maximum length: %u\n", max);
        fprintf(stderr, "-- average length: %g\n", avg);
        fprintf(stderr, "-- memory usage: %zu\n", bytes_total);
    }
}
