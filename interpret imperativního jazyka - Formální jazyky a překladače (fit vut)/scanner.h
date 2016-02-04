/*
 * scanner.h:      Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef SCANNER_H
# define SCANNER_H

#include <stddef.h>

typedef enum {
    TOK_EINVAL,    /**< error: invalid character */
    TOK_ENOMEM,    /**< error: not enough memory */
    TOK_EEOF,      /**< error: unexpected end of file */
    TOK_ENUM,      /**< error: unexpected numeric format */
    _TOK_EMAX,
    TOK_TYPE_NUM,  /**< numeric */
    TOK_TYPE_STR,  /**< string */
    TOK_TYPE_ID,   /**< identifier */
    TOK_TYPE_KW,   /**< keyword */
    TOK_TYPE_RKW,  /**< reserved keyword */
    TOK_TYPE_FN,   /**< built-in function */
    TOK_TYPE_BOOL, /**< boolean */
    TOK_TYPE_NIL,  /**< nil */
    TOK_OP_SUM,    /**< sum */
    TOK_OP_SUB,    /**< subtraction */
    TOK_OP_LE,     /**< less or equal than */
    TOK_OP_LT,     /**< less than */
    TOK_OP_GE,     /**< greater or equal than */
    TOK_OP_GT,     /**< greater than */
    TOK_OP_EQ,     /**< equal */
    TOK_OP_NEQ,    /**< not equal */
    TOK_OP_ASSIGN, /**< assign */
    TOK_OP_MULT,   /**< multiplication */
    TOK_OP_POW,    /**< power */
    TOK_OP_DIV,    /**< division */
    TOK_COMMA,     /**< operator , */
    TOK_COLON,     /**< operator : */
    TOK_LPAR,      /**< operator ( */
    TOK_RPAR,      /**< operator ) */
    TOK_LBRACK,    /**< operator [ */
    TOK_RBRACK,    /**< operator ] */
    TOK_MINUS,     /**< operator unary - */
    TOK_EOL,       /**< end of line */
    TOK_EOF,       /**< end of file */
    _TOK_MAX
} tokens_t;

typedef enum {
    _KW_MIN = _TOK_MAX,

// vygeneruje konstanty ve tvaru KW_xxx
# define X(a, b) KW_##a,
# include "keywords.def"
    X_KEYWORD
# undef X

// vygeneruje konstanty ve tvaru RKW_xxx
# define X(a, b) RKW_##a,
# include "keywords.def"
    X_KEYWORD_RESERVED
# undef X

// vygeneruje konstanty ve tvaru FN_xxx
# define X(a, b) FN_##a,
# include "builtin.def"
    //X_BUILTIN_FN
# undef X

    _KW_MAX
} keywords_t;

/**
 * Vrati prave jeden token ze vstupniho souboru #src_f
 * @param attr V pripade #TOK_TYPE_NUM, #TOK_TYPE_STR a #TOK_TYPE_ID vrati
 *             #string_t*, ktery uvolni volajici;
 *             V pripade #TOK_TYPE_KW, #TOK_TYPE_RKW a #TOK_TYPE_FN vrati
 *             #keywords_t;
 *             V pripade #TOK_TYPE_BOOL vrati 0(false) nebo 1(true);
 *             V pripade #TOK_TYPE_NIL vrati NULL;
 *             Jinak vrati NULL
 * @return Token typu #tokens_t
 */
int getNextToken(void **attr);

/**
 * Vrati aktualne zpracovavany radek souboru #src_f
 */
size_t getCurrLine();

/**
 * Vrati aktualne zpracovavany sloupec souboru #src_f
 */
size_t getCurrCol();

#endif                          // SCANNER_H
