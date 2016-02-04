/*
 * parser_int.h:   Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef PARSER_INT_H
# define PARSER_INT_H

# include "scanner.h"
# include "syntree.h"

// -----------------------------------------------------------------------
// internal error
// -----------------------------------------------------------------------
extern int _parse_errno;

// -----------------------------------------------------------------------
// token
// -----------------------------------------------------------------------
# define NEXT_TOKEN() do { \
        _tok = getNextToken(&_attr); \
        if (_tok == TOK_ENOMEM) { \
            _parse_errno = PARSE_ENOMEM; \
            return false; \
        } else if (_tok < _TOK_EMAX) { \
            _parse_errno = PARSE_ELEXICAL; \
            return false; \
        } \
} while (0)

extern tokens_t _tok;
extern void *_attr;

extern bool parse_expr(expr_t **);

#endif                          // PARSER_INT_H
