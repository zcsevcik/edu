/*
 * str.c:          Implementace interpretu imperativniho jazyka IFJ12
 *
 * Team:           Daniel Berek <xberek00@stud.fit.vutbr.cz>
 *                 Dusan Fodor  <xfodor01@stud.fit.vutbr.cz>
 *                 Pavol Jurcik <xjurci06@stud.fit.vutbr.cz>
 *                 Peter Pritel <xprite01@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ifj12_b2i.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "str.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

#define STR_LEN_INC 1024

/* memory management -- tohle byla posledni nejrychlejsi moznost        */
/* nez dlouze hledat, kde se mi uvolnuje neco dva a vice-krat           */

static string_t *g_first = NULL;
static string_t *g_last = NULL;

struct _string_st {
    struct _string_st *prev;
    struct _string_st *next;

    size_t alloced;
    size_t len;
    char text[1];
};

static inline size_t _max(size_t a, size_t b)
{
    return a > b ? a : b;
}

static void _string_fix_ptr(string_t *p)
{
    if (p) {
        if (p->prev)
            p->prev->next = p;
        if (p->next)
            p->next->prev = p;
    }
}

static string_t* _string_realloc(string_t **p_, size_t req_size)
{
    _ifj_assert_if(p_ != NULL);
    _ifj_assert_if(*p_ != NULL);

    string_t *p = *p_;
    if (p->alloced < p->len + req_size) {
        size_t toalloc = p->alloced + _max(STR_LEN_INC, req_size);
        p = (string_t *) realloc(p, sizeof(string_t) + toalloc);
        if (!p) {
            return NULL;
        }

        p->alloced += toalloc;
        _string_fix_ptr(p);
        *p_ = p;
    }

    return p;
}

string_t *string_new()
{
    string_t *s = (string_t *) malloc(STR_LEN_INC + sizeof(string_t));
    if (s) {
        s->alloced = STR_LEN_INC;
        s->len = 0;
        s->text[0] = '\0';

        s->prev = g_last;
        s->next = NULL;

        if (!g_first)
            g_first = s;
        if (g_last)
            g_last->next = s;

        g_last = s;
    }

    return s;
}

void string_dispose(string_t * p)
{ }


void _string_dispose(string_t * s)
{
    if (s) {
        if (s->prev)
            s->prev->next = s->next;
        if (s->next)
            s->next->prev = s->prev;
    }
    free(s);
}

extern void _string_collect()
{
    for (string_t *p = g_first, *q; p; p = q) {
        q = p->next;
        _string_dispose(p);
    }

    /*
    g_first = NULL;
    g_last = NULL;
    */
}

size_t string_getlen(const string_t * s)
{
    assert(s != NULL);
    return s->len;
}

const char *string_getstr(const string_t * s)
{
    assert(s != NULL);
    return s->text;
}

bool string_append(string_t ** s, char c)
{
    if (s && *s) {
        string_t *p = *s;
        if (!_string_realloc(s, 1))
            return false;

        char *pstr = &p->text[p->len];
        *pstr++ = c;
        *pstr = '\0';

        p->len += 1;

        return true;
    }

    return false;
}

bool string_concat(string_t ** s, const char *d)
{
    if (s && *s && d) {
        string_t *p = *s;
        size_t l = strlen(d);
        if (!_string_realloc(s, l))
            return false;

        strcpy(p->text + p->len, d);
        p->len += l;

        return true;
    }

    return false;
}

string_list_t *string_list_append(string_list_t * li, string_t * s)
{
    string_list_t *p = (string_list_t *) malloc(sizeof(string_list_t));
    if (p) {
        p->next = NULL;
        p->str = s;

        if (li) {
            string_list_t *n = li;
            while ((li = li->next))
                n = li;

            n->next = p;
        }
    }

    return p;
}

void string_list_delete_all(string_list_t * s)
{
    for (string_list_t * p = s; p; p = s) {
        s = p->next;
        string_dispose(p->str);
        free(p);
    }
}

