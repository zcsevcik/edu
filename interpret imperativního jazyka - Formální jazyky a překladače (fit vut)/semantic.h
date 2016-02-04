/*
 * semantic.h:     Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef SEMANTIC_H
# define SEMANTIC_H

# include "syntree.h"

# define SEM_EOK 0
# define SEM_ENOMEM 1
# define SEM_ERESERVEDNAME 2
# define SEM_ECONVNUM 3
# define SEM_EINVALTYPE 4
# define SEM_EINVALVALUE 5
# define SEM_EVARNOTDEF 6
# define SEM_EFUNCNOTDEF 7
# define SEM_EDIVZERO 8
# define SEM_EFUNCREDEF 9
# define SEM_EPARAM 10
# define SEM_ERANGE 11
# define SEM_ESUBSTR 12

int semantic(ast_t *);

#endif                          // SEMANTIC_H
