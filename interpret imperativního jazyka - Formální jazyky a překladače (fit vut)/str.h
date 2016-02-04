/*
 * str.h:          Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef STR_H
# define STR_H

# include <stdbool.h>
# include <stddef.h>

/* -------------------------------------------------------------------- */
/* reallocable string                                                   */
/* -------------------------------------------------------------------- */
typedef struct _string_st string_t;

string_t *string_new();
void string_dispose(string_t *);

const char *string_getstr(const string_t *);
size_t string_getlen(const string_t *);

bool string_append(string_t **, char);
bool string_concat(string_t **, const char *);

/* -------------------------------------------------------------------- */
/* reallocable string -- single-linked list                             */
/* -------------------------------------------------------------------- */
typedef struct _string_list_st {
    struct _string_list_st *next;
    string_t *str;
} string_list_t;

string_list_t *string_list_append(string_list_t *, string_t *);
void string_list_delete_all(string_list_t *);

#endif                          // STR_H
