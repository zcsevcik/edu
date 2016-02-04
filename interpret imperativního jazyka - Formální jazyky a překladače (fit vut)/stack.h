/*
 * stack.h:        Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef STACK_H
# define STACK_H

# include <stdbool.h>
# include <stddef.h>

typedef struct _stack_st stack_t;

stack_t *stack_new(size_t grow_size);
void stack_dispose(stack_t *);

bool stack_insert(stack_t **, size_t, const char *, size_t);
//bool stack_push(stack_t **, const char *, size_t);
bool stack_pop(stack_t *, char *, size_t);
//char *stack_top(stack_t *, size_t);
char *stack_at(stack_t *, size_t, size_t);

bool stack_pop_front(stack_t *, char *, size_t);

#define stack_top(s, l) stack_at(s, 0, l)
#define stack_push(s, p, l) stack_insert(s, 0, p, l)

bool stack_empty(stack_t *);
size_t stack_size(stack_t *);

#endif                          // STACK_H
