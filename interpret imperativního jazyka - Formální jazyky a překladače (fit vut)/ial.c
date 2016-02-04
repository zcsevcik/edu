/*
 * ial.c:          Implementace interpretu imperativniho jazyka IFJ12
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

#include "ial.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------
// BINARNI VYHLEDAVACI STROM
// -----------------------------------------------------------------------
struct _bst_st {
    void *data;
    struct _bst_st *node_l;
    struct _bst_st *node_r;
    char key[1];
};

bst_t *bst_new()
{
    return NULL;
}

void bst_dispose(bst_t ** p, void (*item_dispose) (void *item))
{
    if (p && *p) {
        bst_t *p_ = *p;

        if (item_dispose) {
            item_dispose(p_->data);
        }

        bst_dispose(&p_->node_l, item_dispose);
        bst_dispose(&p_->node_r, item_dispose);

        free(p_);
        *p = NULL;
    }
}

bool bst_insert(bst_t ** p, const char *key, void *item)
{
    if (p) {
        for (;;) {
            bst_t *p_ = *p;

            if (!p_) {
                size_t lkey = strlen(key);
                p_ = (bst_t *) malloc(sizeof(bst_t) + lkey);
                if (!p_) {
                    return false;
                }

                *p = p_;
                strcpy(p_->key, key);
                p_->data = item;
                p_->node_l = NULL;
                p_->node_r = NULL;
                return true;
            }
            else {
                int cmp_hr = strcmp(key, p_->key);
                if (!cmp_hr) {
                    p_->data = item;
                    return true;
                }
                else if (cmp_hr > 0) {
                    p = &p_->node_r;
                    continue;
                }
                else {
                    p = &p_->node_l;
                    continue;
                }
            }
        }
    }

    return false;
}

bool bst_search(const bst_t * p, const char *key, void **item)
{
    while (p) {
        int cmp_hr = strcmp(key, p->key);
        if (!cmp_hr) {
            if (item) {
                *item = p->data;
            }
            return true;
        }
        else if (cmp_hr > 0) {
            p = p->node_r;
        }
        else {
            p = p->node_l;
        }
    }

    return false;
}


// -----------------------------------------------------------------------
// VYHLEDANI PODRETEZCE
// -----------------------------------------------------------------------
char *ial_strstr(const string_t * s1, const string_t * s2)
{
    return strstr(string_getstr(s1), string_getstr(s2));
}

// -----------------------------------------------------------------------
// RAZENI
// -----------------------------------------------------------------------
static int _cmp_str(const void * l, const void *r)
{
    return strncmp((const char *)l, (const char *)r, 1);
}

bool ial_sort(string_t *s)
{
    qsort((void *)string_getstr(s), string_getlen(s), sizeof(char), &_cmp_str);
    return true;
}
