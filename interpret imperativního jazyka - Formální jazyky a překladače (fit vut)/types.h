/*
 * types.h:        Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef TYPES_H
# define TYPES_H

# include "str.h"

typedef struct {
    enum {
# define X(a,b) \
        TYPE_##a,
# include "types.def"
        X_TYPES
# undef X
    } type;

    union {
        string_t *__v_str;
        double __v_dbl;
        bool __v_bool;
    } __u;

# define v_str __u.__v_str
# define v_var __u.__v_var
# define v_num __u.__v_dbl
# define v_bool __u.__v_bool
} typeval_t;

#endif                          /// TYPES_H
