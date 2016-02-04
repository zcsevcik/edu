/*
 * parser.h:       Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef PARSER_H
# define PARSER_H

# include "syntree.h"

# define PARSE_EOK 0
# define PARSE_ELEXICAL 1
# define PARSE_ESYNTAX 2
# define PARSE_ENOMEM 3
# define PARSE_EEXPR 4

int parse(ast_t *);

#endif                          // PARSER_H
