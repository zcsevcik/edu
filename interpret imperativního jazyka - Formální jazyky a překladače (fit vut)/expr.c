/*
 * expr.c:         Implementace interpretu imperativniho jazyka IFJ12
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

#include "parser.h"
#include "parser_int.h"

#include "defs.h"
#include "scanner.h"
#include "syntree.h"
#include "stack.h"
#include "str.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------- */
/* precedence table                                                     */
/* -------------------------------------------------------------------- */

/* E ::= i()                                                            */
/* E ::= i(E)                                                           */
/* E ::= i(L)                                                           */
/* L ::= L, E                                                           */
/* L ::= E, E                                                           */
/*                                                                      */
/* E ::= (E)                                                            */
/* E ::= E + E                                                          */
/* E ::= E - E                                                          */
/* E ::= E * E                                                          */
/* E ::= E / E                                                          */
/* E ::= E ** E                                                         */
/*                                                                      */
/* E ::= E and E                                                        */
/* E ::= E or E                                                         */

/* E ::= -E                                                             */
/* E ::= not E                                                          */
/*                                                                      */
/* E ::= E > E                                                          */
/* E ::= E >= E                                                         */
/* E ::= E < E                                                          */
/* E ::= E <= E                                                         */
/* E ::= E == E                                                         */
/* E ::= E != E                                                         */
/* E ::= E in E                                                         */
/* E ::= E notin E                                                      */
/*                                                                      */
/* E ::= i                                                              */

/* <opt-num> ::= num                                                    */
/* <opt-num> ::= epsilon                                                */
/* <substr> ::= i [ <opt-num> : <opt-num> ]                             */

static const size_t _tbl_item[_KW_MAX] = {
//  [undefined] = 0,
    [TOK_OP_SUM] = 1,
    [TOK_OP_SUB] = 2,
    [TOK_OP_MULT] = 3,
    [TOK_OP_DIV] = 4,
    [TOK_OP_POW] = 5,
    [TOK_TYPE_NUM] = 6,
    [TOK_TYPE_STR] = 6,
    [TOK_TYPE_ID] = 6,
    [TOK_TYPE_FN] = 6,
    [TOK_TYPE_BOOL] = 6,
    [TOK_TYPE_NIL] = 6,
    [TOK_LPAR] = 7,
    [TOK_RPAR] = 8,
    [TOK_COMMA] = 9,
    [TOK_LBRACK] = 10,
    [TOK_RBRACK] = 11,
    [TOK_COLON] = 12,
    [KW_AND] = 13,
    [KW_OR] = 14,
    [KW_NOT] = 15,
    [KW_IN] = 16,
    [KW_NOTIN] = 17,
#if 0
    [FN_FIND] = 6,
    [FN_INPUT] = 6,
    [FN_LEN] = 6,
    [FN_NUMERIC] = 6,
    [FN_PRINT] = 6,
    [FN_SORT] = 6,
    [FN_TYPEOF] = 6,
#endif
    [TOK_OP_LT] = 18,
    [TOK_OP_LE] = 19,
    [TOK_OP_GT] = 20,
    [TOK_OP_GE] = 21,
    [TOK_OP_NEQ] = 22,
    [TOK_OP_EQ] = 23,
    [TOK_MINUS] = 24,
};

static const char _tbl[_KW_MAX][_KW_MAX] = {
/*            $    +    -    *    /    **   i    (    )    ,    [    ]    :   and   or  not  in  notin  <    <=   >   >=   !=   ==   -E */
/*     $ */ {'$', '<', '<', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<'},
/*     + */ {'>', '>', '>', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*     - */ {'>', '>', '>', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*     * */ {'>', '>', '>', '>', '>', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*     / */ {'>', '>', '>', '>', '>', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*    ** */ {'>', '>', '>', '>', '>', '>', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*     i */ {'>', '>', '>', '>', '>', '>', ' ', '=', '>', '>', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/*     ( */ {' ', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' ', ' ', ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<'},
/*     ) */ {'>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/*     , */ {' ', '>', '>', '>', '>', '>', '<', '>', '>', '>', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/*     [ */ {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
/*     ] */ {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
/*     : */ {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
/*   and */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<'},
/*    or */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '<', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<'},
/*   not */ {'>', '>', '>', '>', '>', '>', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*    in */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/* notin */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*     < */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*    <= */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*     > */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*    >= */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
/*    != */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<'},
/*    == */ {'>', '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<'},
/*    -E */ {'>', '>', '>', '>', '>', '>', '<', '<', '>', '>', ' ', ' ', ' ', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'},
};

#define X_FN_CALL(e_, l_) \
X(_e__i_call_arg0, (e_), ((char []) { TOK_TYPE_ID, TOK_LPAR, TOK_RPAR })) \
X(_e__i_call_arg1, (e_), ((char []) { TOK_TYPE_ID, TOK_LPAR, (e_), TOK_RPAR })) \
X(_e__i_call_argn, (e_), ((char []) { TOK_TYPE_ID, TOK_LPAR, (l_), TOK_RPAR })) \
X(_l__l_e, (l_), ((char []) { (l_), TOK_COMMA, (e_) })) \
X(_l__e_e, (l_), ((char []) { (e_), TOK_COMMA, (e_) }))
/*
X(_e__i_call_builtin_input, (e_), ((char []) { FN_INPUT, TOK_LPAR, TOK_RPAR })) \
X(_e__i_call_builtin_numeric, (e_), ((char []) { FN_NUMERIC, TOK_LPAR, (e_), TOK_RPAR })) \
X(_e__i_call_builtin_print, (e_), ((char []) { FN_PRINT, TOK_LPAR, (l_), TOK_RPAR })) \
X(_e__i_call_builtin_typeof, (e_), ((char []) { FN_TYPEOF, TOK_LPAR, (e_), TOK_RPAR })) \
X(_e__i_call_builtin_len, (e_), ((char []) { FN_LEN, TOK_LPAR, (e_), TOK_RPAR })) \
X(_e__i_call_builtin_find, (e_), ((char []) { FN_FIND, TOK_LPAR, (l_), TOK_RPAR })) \
X(_e__i_call_builtin_sort, (e_), ((char []) { FN_SORT, TOK_LPAR, (e_), TOK_RPAR })) \
*/

#define X_EXPR_OP(e_, l_) \
X(_e__e_par, (e_), ((char []) { TOK_LPAR, (e_), TOK_RPAR })) \
X(_e__e_add_e, (e_), ((char []) { (e_), TOK_OP_SUM, (e_) })) \
X(_e__e_sub_e, (e_), ((char []) { (e_), TOK_OP_SUB, (e_) })) \
X(_e__e_mult_e, (e_), ((char []) { (e_), TOK_OP_MULT, (e_) })) \
X(_e__e_div_e, (e_), ((char []) { (e_), TOK_OP_DIV, (e_) })) \
X(_e__e_pow_e, (e_), ((char []) { (e_), TOK_OP_POW, (e_) })) \
X(_e__e_gt_e, (e_), ((char []) { (e_), TOK_OP_GT, (e_) })) \
X(_e__e_ge_e, (e_), ((char []) { (e_), TOK_OP_GE, (e_) })) \
X(_e__e_lt_e, (e_), ((char []) { (e_), TOK_OP_LT, (e_) })) \
X(_e__e_le_e, (e_), ((char []) { (e_), TOK_OP_LE, (e_) })) \
X(_e__e_neq_e, (e_), ((char []) { (e_), TOK_OP_NEQ, (e_) })) \
X(_e__e_eq_e, (e_), ((char []) { (e_), TOK_OP_EQ, (e_) })) \
X(_e__min_e, (e_), ((char []) { TOK_MINUS, (e_) })) \
X(_e__e_and_e, (e_), ((char []) { (e_), KW_AND, (e_) })) \
X(_e__e_or_e, (e_), ((char []) { (e_), KW_OR, (e_) })) \
X(_e__not_e, (e_), ((char []) { KW_NOT, (e_) })) \
X(_e__e_in_e, (e_), ((char []) { (e_), KW_IN, (e_) })) \
X(_e__e_notin_e, (e_), ((char []) { (e_), KW_NOTIN, (e_) }))

#define X_EXPR_I(e_, l_) \
X(_e__i, (e_), ((char []) { TOK_TYPE_ID })) \
X(_e__i, (e_), ((char []) { TOK_TYPE_NUM })) \
X(_e__i, (e_), ((char []) { TOK_TYPE_STR })) \
X(_e__i, (e_), ((char []) { TOK_TYPE_NIL })) \
X(_e__i, (e_), ((char []) { TOK_TYPE_BOOL })) \
X(_e__i, (e_), ((char []) { TOK_TYPE_FN }))

typedef enum {
#define X(a,b,c) a,
    X_EXPR_OP(E_, L_)
    X_FN_CALL(E_, L_)
#undef X
    _e__s_substr,
    _e__i_substr,
    _e__i,
} expr_rules;

/* -------------------------------------------------------------------- */
/* debugging                                                            */
/* -------------------------------------------------------------------- */
#ifndef NDEBUG
struct _stack_st {
    size_t alloced;
    size_t grow_size;
    char *top;
    char base[1];
};

#define PRINT_STACK(s) do { \
        for (const char* p = s->base; p < s->top; ++p) \
            fprintf(stderr, "%02x ", (const unsigned char) *p); \
        fprintf(stderr, "<- SP <expr>\n"); \
} while (0)
#else
# define PRINT_STACK(s)
#endif

/* -------------------------------------------------------------------- */
/* stack operations                                                     */
/* -------------------------------------------------------------------- */
#define PUSH(s, p, t) do { \
        if (!stack_push(&(s), (char *) (p), sizeof(t))) { \
            _parse_errno = PARSE_ENOMEM; \
            goto return_false; \
        } \
} while (0)

#define INSERT(s, p, i, t) do { \
        if (!stack_insert(&(s), (i), (char * ) (p), sizeof(t))) { \
            _parse_errno = PARSE_ENOMEM; \
            goto return_false; \
        } \
} while (0)

#define AT(s, i) \
        stack_at((s), (i), sizeof(char))

#define POP(s, p, t) \
        stack_pop((s), (char *) (p), sizeof(t) )

#define POP_FRONT(s, p, t) \
        stack_pop_front((s), (char *) (p), sizeof(t) )

#define POP_N(s, i) \
        stack_pop((s), NULL, (i) * sizeof(char))

#define POP_RULE(s, p) \
        POP((s), &(p), rule_arg)

#define PUSH_LIT(s, c) \
        PUSH((s), (char []) { (char) (c) }, char)

#define PUSH_ADDR(s, p) \
        PUSH((s), (void * []) { (p) }, void *)

#define PUSH_RULE(s, p) \
        PUSH((s), &(p), rule_arg)

#define INSERT_LIT(s, c, i) \
        INSERT((s), (char []) { (char) (c) }, (i), char)

#define PUSH_ATTR(s) do { \
        if (_tok == TOK_TYPE_BOOL || \
            _tok == TOK_TYPE_ID || \
            _tok == TOK_TYPE_NIL || \
            _tok == TOK_TYPE_NUM || \
            _tok == TOK_TYPE_STR) \
        { PUSH_ADDR(s, _attr); } \
} while (0)

static char _stack_top_term(stack_t * s, size_t * at)
{
    size_t stsize = stack_size(s);
    for (size_t i = 0; i < stsize; ++i) {
        char c = *stack_at(s, i, sizeof(char));
        if (c >= 0) {
            *at = i;
            return c;
        }
    }

    return '\0';
}

static char _stack_top_stop(stack_t * s, size_t * at)
{
    const char endmarker_ = ~0;
    size_t stsize = stack_size(s);
    for (size_t i = 0; i < stsize; ++i) {
        char c = *stack_at(s, i, sizeof(char));
        if (c == endmarker_) {
            *at = i;
            return c;
        }
    }

    return '\0';
}

static void _free_token()
{
    switch (_tok) {
    default:
        break;
    case TOK_TYPE_ID:
    case TOK_TYPE_NUM:
    case TOK_TYPE_STR:
        string_dispose((string_t *) _attr);
        _attr = NULL;
        break;
    }

    return;
}

static void _free_string_stack(stack_t * s)
{
    while (!stack_empty(s)) {
        string_t* p;
        stack_pop(s, (char *) &p, sizeof(string_t*));
        string_dispose(p);
    }
}

/* -------------------------------------------------------------------- */
/* find derivation rule                                                 */
/* -------------------------------------------------------------------- */
typedef struct {
    expr_rules rule;
    tokens_t type;              // pro rozpoznani typu identifikatoru
} rule_arg;

static unsigned char _find_rule(rule_arg* r, const char *s, size_t len)
{
    const unsigned char E_ = -2;
    const unsigned char L_ = -3;

#ifndef NDEBUG
    fprintf(stderr, "Finding rule for: ");
    for (size_t i = 0; i < len; ++i) {
        fprintf(stderr, "%02x ", (unsigned char) s[i]);
    }
    fprintf(stderr, "\n");
#endif

    if (0) {

    }
#define X(a,b,c) \
    else if (!memcmp((c), s, len)) { \
        _ifj_debug_log("%s %02x", (#a), (a)); \
        r->rule = (a); \
        r->type = 0; \
        return (b); \
    }

    X_EXPR_OP(E_, L_)
#undef X

#define X(a,b,c) \
    else if (!memcmp((c), s, len)) { \
        _ifj_debug_log("%s %02x", (#a), (a)); \
        r->rule = (a); \
        r->type = (tokens_t) *s; \
        return (b); \
    }

    X_EXPR_I(E_, L_)
#undef X

#define X(a,b,c) \
    else if (!memcmp((c), s, len)) { \
        _ifj_debug_log("%s %02x", (#a), (a)); \
        r->rule = (a); \
        r->type = (tokens_t) *s; \
        return (b); \
}

    X_FN_CALL(E_, L_)
#undef X

else {
        return 0;
    }
}

static bool _read_substr(string_t **from, string_t **to)
{
    _ifj_assert_if(from != NULL);
    _ifj_assert_if(to != NULL);

    *from = NULL;
    *to = NULL;

    if (_tok != TOK_LBRACK) {
        return false;
    }
    NEXT_TOKEN();
    if (_tok == TOK_TYPE_NUM) {
        *from = _attr;
        NEXT_TOKEN();
    }
    if (_tok != TOK_COLON) {
        return false;
    }
    NEXT_TOKEN();
    if (_tok == TOK_TYPE_NUM) {
        *to = _attr;
        NEXT_TOKEN();
    }
    if (_tok != TOK_RBRACK) {
        return false;
    }
    NEXT_TOKEN();

    return true;
}

/* -------------------------------------------------------------------- */
/* infix to postfix using precedence table                              */
/* -------------------------------------------------------------------- */
static bool _infix_to_postfix(stack_t** s, stack_t** r, stack_t** ids)
{
    tokens_t _last_tok = _tok;
    void *_last_attr = _attr;

    NEXT_TOKEN();
    PUSH_ATTR(*ids);
    PUSH_LIT(*s, 0);        // '$'
    //PRINT_STACK(s);

    for (;;) {
        size_t i = 0;
        char row = _stack_top_term(*s, &i);

        char col;
        if (_tok == TOK_TYPE_KW ||
            _tok == TOK_TYPE_RKW ||
            _tok == TOK_TYPE_FN)
        {
            col = (char) (keywords_t) _attr;
        }
        else if (_tok == TOK_OP_SUB &&
            _last_tok != TOK_RPAR &&
            _last_tok != TOK_TYPE_NUM &&
            _last_tok != TOK_TYPE_STR &&
            _last_tok != TOK_TYPE_BOOL &&
            _last_tok != TOK_TYPE_ID)
        {
            // unary minus
            _tok = TOK_MINUS;
            col = (char) _tok;
        }
        else if (_tok == TOK_LBRACK &&
            (_last_tok == TOK_TYPE_STR ||
            _last_tok == TOK_TYPE_ID))
        {
            tokens_t lasttok = _last_tok;
            string_t *from, *to;
            if (!_read_substr(&from, &to))
                return PARSE_ESYNTAX;

            rule_arg rule = { 0 };
            if (lasttok == TOK_TYPE_STR) {
                rule.rule = _e__s_substr;
            }
            else if (lasttok == TOK_TYPE_ID) {
                rule.rule = _e__i_substr;
            }
            else {
                _ifj_assert();
            }

            PUSH_ADDR(*ids, from);
            PUSH_ADDR(*ids, to);
            PUSH_RULE(*r, rule);
            return true;
        }
        else if (_tok == TOK_RPAR &&
            _last_tok == TOK_COMMA)
        {   // chybi argument
            _parse_errno = PARSE_ESYNTAX;
            goto return_false;
        }
        else if (_tok == TOK_COMMA &&
            _last_tok == TOK_LPAR)
        {   // chybi argument
            _parse_errno = PARSE_ESYNTAX;
            goto return_false;
        }
        else if (_tok == TOK_COMMA &&
            _last_tok == TOK_COMMA)
        {   // chybi argument
            _parse_errno = PARSE_ESYNTAX;
            goto return_false;
        }
        else {
            col = (char) _tok;
        }

        const size_t pos_x = _tbl_item[(size_t) col];
        const size_t pos_y = _tbl_item[(size_t) row];
        const char op = _tbl[pos_y][pos_x];

        switch (op) {
            default:
                _parse_errno = PARSE_EEXPR;
                goto return_false;

            case '$':
                return true;

            case '<':
                //_ifj_debug_log("<");
                INSERT_LIT(*s, ~0, i);       // '<'
                PUSH_LIT(*s, col);

                //PRINT_STACK(s);
                _last_tok = _tok;
                _last_attr = _attr;
                NEXT_TOKEN();
                PUSH_ATTR(*ids);
                break;

            case '>':
                //_ifj_debug_log(">");
                row = _stack_top_stop(*s, &i);
                _ifj_assert_if(row == ~0);    // '<'
                _ifj_assert_if(i > 0);      // za '<' musi nasledovat pravidlo

                rule_arg rule = { 0 };
                row = _find_rule(&rule, AT(*s, i - 1), i);
                if (row == 0) {
                    _parse_errno = PARSE_EEXPR;
                    goto return_false;
                }

                PUSH_RULE(*r, rule);
                POP_N(*s, i + 1);
                PUSH_LIT(*s, row);
                //PRINT_STACK((*s));
                break;

            case '=':
                //_ifj_debug_log("=");
                PUSH_LIT(*s, col);
                //PRINT_STACK(s);
                _last_tok = _tok;
                _last_attr = _attr;
                NEXT_TOKEN();
                PUSH_ATTR(*ids);
                break;
        }
    }

  return_false:
    _free_token();
    return false;
}

/* -------------------------------------------------------------------- */
/* postfix to expression tree                                           */
/* -------------------------------------------------------------------- */
#define X_UNOP \
X(_e__not_e, EXPR_NOT, "!") \
X(_e__min_e, EXPR_MINUS, "-")

#define X_BINOP \
X(_e__e_add_e, EXPR_SUM, "+") \
X(_e__e_sub_e, EXPR_SUB, "-") \
X(_e__e_div_e, EXPR_DIV, "/") \
X(_e__e_mult_e, EXPR_MULT, "*") \
X(_e__e_pow_e, EXPR_POW, "**") \
X(_e__e_gt_e, EXPR_CMP_GT, ">") \
X(_e__e_ge_e, EXPR_CMP_GE, ">=") \
X(_e__e_lt_e, EXPR_CMP_LT, "<") \
X(_e__e_le_e, EXPR_CMP_LE, "<=") \
X(_e__e_neq_e, EXPR_CMP_NEQ, "!=") \
X(_e__e_eq_e, EXPR_CMP_EQ, "==") \
X(_e__e_and_e, EXPR_AND, "&&") \
X(_e__e_or_e, EXPR_OR, "||") \
X(_e__e_in_e, EXPR_IN, "in") \
X(_e__e_notin_e, EXPR_NOTIN, "notin")

#define X_I \
X(TOK_TYPE_ID, EXPR_ID, p_id, string_t *) \
X(TOK_TYPE_NUM, EXPR_NUM, p_num, string_t *) \
X(TOK_TYPE_BOOL, EXPR_BOOL, p_bool, bool) \
X(TOK_TYPE_STR, EXPR_STR, p_str, string_t *) \
X(TOK_TYPE_NIL, EXPR_NIL, p_nil, void *)

static bool _postfix_to_tree(stack_t * s_rule, stack_t * s_ids, expr_t ** e_)
{
    stack_t *s_ptr =
        stack_new(stack_size(s_rule) / sizeof(rule_arg) * sizeof(expr_t **));
    if (!s_ptr) {
        goto return_false;
    }

    PUSH(s_ptr, &e_, expr_t **);

    while (!stack_empty(s_rule) && !stack_empty(s_ptr)) {
        rule_arg r;
        POP_RULE(s_rule, r);

        expr_t** e;
        POP(s_ptr, &e, expr_t **);

        switch (r.rule) {
        default:
            _ifj_assert();
            break;

        case _e__i_call_arg0: {
            *e = expr_new(EXPR_CALL);
            string_t *name = NULL;
            POP(s_ids, &name, void *);
            (*e)->p_call.fn_name = (string_t *) name;
            (*e)->p_call.params = NULL;
        }   break;

        case _e__i_call_arg1: {
            *e = expr_new(EXPR_CALL);
            string_t *name = NULL;
            POP_FRONT(s_ids, &name, void *);
            (*e)->p_call.fn_name = (string_t *) name;
            if (!_alloc_expr_tbl(&(*e)->p_call.params)) {
                goto return_false;
            }
            PUSH_ADDR(s_ptr, &(*e)->p_call.params->expr);
        }   break;

        case _e__i_call_argn: {
            *e = expr_new(EXPR_CALL);
            string_t* name = NULL;
            POP_FRONT(s_ids, &name, void *);
            (*e)->p_call.fn_name = (string_t *) name;
            if (!_alloc_expr_tbl(&(*e)->p_call.params)) {
                goto return_false;
            }
            PUSH_ADDR(s_ptr, &(*e)->p_call.params);
        }   break;

        case _e__s_substr:
        case _e__i_substr: {
            *e = expr_new(EXPR_SUBSTR);
            string_t *val, *from, *to;
            POP(s_ids, &to, void *);
            POP(s_ids, &from, void *);
            POP(s_ids, &val, void *);
            (*e)->p_substr.from = from;
            (*e)->p_substr.to = to;

            if (r.rule == _e__s_substr) {
                (*e)->p_substr.val = expr_new(EXPR_STR);
            }
            else if (r.rule == _e__i_substr) {
                (*e)->p_substr.val = expr_new(EXPR_ID);
            }
            else {
                _ifj_assert();
            }
            (*e)->p_substr.val->p_str = val;
        }   break;

        case _l__e_e: {
            struct _expr_tbl_st *tbl = *(struct _expr_tbl_st **) e;
            if (!_alloc_expr_tbl(&tbl->next)) {
                goto return_false;
            }
            PUSH_ADDR(s_ptr, &tbl->next->expr);
            PUSH_ADDR(s_ptr, &tbl->expr);
        }   break;

        case _l__l_e: {
            struct _expr_tbl_st *tbl = *(struct _expr_tbl_st **) e;
            if (!_alloc_expr_tbl(&tbl->next)) {
                goto return_false;
            }
            PUSH_ADDR(s_ptr, &tbl->next);
            PUSH_ADDR(s_ptr, &tbl->expr);
        }   break;

#define X(a,b,c) \
        case a: \
            *e = expr_new(b); \
            PUSH_ADDR(s_ptr, &(*e)->p_binop.lhs); \
            PUSH_ADDR(s_ptr, &(*e)->p_binop.rhs); \
            break;

            X_BINOP
#undef X

#define X(a,b,c) \
        case a: \
            *e = expr_new(b); \
            PUSH_ADDR(s_ptr, &(*e)->p_unop); \
            break;

            X_UNOP
#undef X

        case _e__e_par:
            // push back
            PUSH_ADDR(s_ptr, e);
            break;

        case _e__i:
            switch (r.type) {
            default:
                _ifj_assert();
                return false;
#define X(a,b,c,t) \
            case a: { \
                *e = expr_new(b); \
                t param = NULL; \
                POP(s_ids, &param, void *); \
                (*e)->c = (t) param; \
            }   break;

                X_I
#undef X
            }
        }
    }

    bool retval = stack_empty(s_rule) && stack_empty(s_ptr);

  cleanup:
    stack_dispose(s_ptr);

    if (!retval) {
        _parse_errno = PARSE_EEXPR;
    }

    return retval;

  return_false:
    _free_token();
    retval = false;
    goto cleanup;
}

static void _print_tree(expr_t* expr)
{
    if (!expr) {
        return;
    }

    switch (expr->type) {
#define X(a,b,c) \
    case b: \
        fprintf(stderr, "("); \
        _print_tree(expr->p_binop.lhs); \
        fprintf(stderr, " %s ", c); \
        _print_tree(expr->p_binop.rhs); \
        fprintf(stderr, ")"); \
        break;
    X_BINOP
#undef X

#define X(a,b,c) \
    case b: \
        fprintf(stderr, "%s(", c); \
        _print_tree(expr->p_unop); \
        fprintf(stderr, ")"); \
        break;
    X_UNOP
#undef X

    case EXPR_ID:
    case EXPR_NUM:
        fprintf(stderr, "%s", string_getstr(expr->p_id));
        break;

    case EXPR_STR:
        fprintf(stderr, "\"%s\"", string_getstr(expr->p_str));
        break;

    case EXPR_BOOL:
        fprintf(stderr, "%s", expr->p_bool ? "true" : "false");
        break;

    case EXPR_NIL:
        fprintf(stderr, "Nil");
        break;

    case EXPR_SUBSTR:
        fprintf(stderr, "%s[%s:%s]", string_getstr(expr->p_substr.val->p_str),
                expr->p_substr.from ? string_getstr(expr->p_substr.from) : "",
                expr->p_substr.to ? string_getstr(expr->p_substr.to) : "");
        break;

    case EXPR_CALL:
        fprintf(stderr, "%s(", string_getstr(expr->p_call.fn_name));
        struct _expr_tbl_st *params = expr->p_call.params;
        while (params) {
            _print_tree(params->expr);
            fprintf(stderr, ", ");
            params = params->next;
        };

        fprintf(stderr, ")");
        break;
    }
}

static bool _fix_param_fn_argn(expr_t * expr)
{
    if (!expr) {
        return false;
    }

    stack_t *s = stack_new(8 * sizeof(expr_t *));
    if (!s) {
        return false;
    }

    switch (expr->type) {
#define X(a,b,c) \
    case b: \
        _fix_param_fn_argn(expr->p_binop.lhs); \
        _fix_param_fn_argn(expr->p_binop.rhs); \
        break;
    X_BINOP
#undef X

#define X(a,b,c) \
    case b: \
        _fix_param_fn_argn(expr->p_unop); \
        break;
    X_UNOP
#undef X

    default:
        break;

    case EXPR_CALL:
        for (struct _expr_tbl_st * p = expr->p_call.params; p; p = p->next) {
            PUSH_ADDR(s, p->expr);
        }
        for (struct _expr_tbl_st * p = expr->p_call.params; p; p = p->next) {
            POP(s, &p->expr, expr_t *);
        }

        break;
    }

    stack_dispose(s);
    return true;

  return_false:
    stack_dispose(s);
    return false;
}

/* ==================================================================== */
/*    P   U   B   L   I   C       I   N   T   E   R   F   A   C   E     */
/* ==================================================================== */
bool parse_expr(expr_t ** expr)
{
    _ifj_syntax_log("<expr>");

    stack_t *s = NULL, *r = NULL, *ids = NULL;

    if (!(s = stack_new(256 * sizeof(char)))) {
        _parse_errno = PARSE_ENOMEM;
        goto return_false;
    }
    if (!(r = stack_new(8 * sizeof(rule_arg)))) {
        _parse_errno = PARSE_ENOMEM;
        goto return_false;
    }
    if (!(ids = stack_new(8 * sizeof(string_t *)))) {
        _parse_errno = PARSE_ENOMEM;
        goto return_false;
    }

    if (!_infix_to_postfix(&s, &r, &ids)) {
        goto return_false;
    }

    if (!_postfix_to_tree(r, ids, expr)) {
        goto return_false;
    }
    if (!_fix_param_fn_argn(*expr)) {
        goto return_false;
    }

    bool retval;
#ifndef NDEBUG
    _print_tree(*expr);
#endif

    retval = true;
    goto cleanup;

  return_false:
    _free_string_stack(ids);
    retval = false;
    goto cleanup;

  cleanup:
    stack_dispose(ids);
    stack_dispose(r);
    stack_dispose(s);
    return retval;
}


