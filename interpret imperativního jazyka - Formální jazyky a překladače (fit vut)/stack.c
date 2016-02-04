/*
 * stack.c:        Implementace interpretu imperativniho jazyka IFJ12
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

#include "stack.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static inline size_t _max(size_t a, size_t b)
{
    return a > b ? a : b;
}

struct _stack_st {
    size_t alloced;
    size_t grow_size;
    char *top;
    char base[1];
};

stack_t *stack_new(size_t grow_size)
{
    stack_t *s = (stack_t *) malloc(sizeof(stack_t) + grow_size);
    if (s) {
        s->top = s->base;
        s->alloced = grow_size;
        s->grow_size = grow_size;
    }

    return s;
}

void stack_dispose(stack_t * s)
{
    free(s);
}

bool stack_empty(stack_t * s)
{
    return s && s->top == s->base;
}

size_t stack_size(stack_t * s)
{
    if (s) {
        return s->top - s->base;
    }

    return 0;
}

/*
char *stack_top(stack_t * s, size_t size)
{
    if (s) {
        if (s->top >= s->base + size) {
            return s->top - size;
        }
    }

    return NULL;
}
*/

char *stack_at(stack_t * s, size_t offset, size_t size)
{
    if (s) {
        if (s->top - offset - size >= s->base) {
            return s->top - offset - size;
        }
    }

    return NULL;
}

bool stack_pop(stack_t * s, char *p, size_t size)
{
    if (s) {
        if (s->top >= s->base + size) {
            if (p) {
                memcpy(p, s->top - size, size);
            }

            s->top -= size;
            return true;
        }
    }

    return false;
}

bool stack_pop_front(stack_t * s, char *p, size_t size)
{
    if (s) {
        if (s->top >= s->base + size) {
            if (p) {
                memcpy(p, s->base, size);
                memmove(s->base, s->base + size, s->top - s->base - size);
            }

            s->top -= size;
            return true;
        }
    }

    return false;
}

/*
bool stack_push(stack_t ** s, const char *p, size_t size)
{
    if (s && *s) {
        stack_t *s_ = *s;
        if (s_->top + size > s_->base + s_->alloced) {
            size_t toalloc = s_->alloced + _max(s_->grow_size, size);

            s_ = realloc(s_, sizeof(stack_t) + toalloc);
            if (!s_) {
                return false;
            }

            s_->alloced = toalloc;
            *s = s_;
        }

        memcpy(s_->top, p, size);
        s_->top += size;

        return true;
    }

    return false;
}
*/

bool stack_insert(stack_t ** s, size_t offset, const char *p, size_t size)
{
    if (s && *s) {
        stack_t *s_ = *s;
        if (s_->top + size > s_->base + s_->alloced) {
            size_t toalloc = s_->alloced + _max(s_->grow_size, size);
            ptrdiff_t sp_offset = s_->top - s_->base;

            s_ = (stack_t *) realloc(s_, sizeof(stack_t) + toalloc);
            if (!s_) {
                return false;
            }

            s_->top = s_->base + sp_offset;
            s_->alloced = toalloc;
            *s = s_;
        }

        // presuneme polozky k vrcholu zasobniku
        memmove(s_->top - offset + size, s_->top - offset, offset);

        // nakopirujeme nove polozky
        memcpy(s_->top - offset, p, size);
        s_->top += size;

        return true;
    }

    return false;
}
