/*
 * parser.c:       Implementace interpretu imperativniho jazyka IFJ12
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

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "defs.h"
#include "scanner.h"
#include "stack.h"
#include "str.h"

// -----------------------------------------------------------------------
// internal error
// -----------------------------------------------------------------------
int _parse_errno;

// -----------------------------------------------------------------------
// abstract syntax tree
// -----------------------------------------------------------------------
static ast_t *_ast;
static stack_t *_stmt_s;

#define GET_LAST_STMT(s) \
        ((stmt_t **) stack_top((s), sizeof(stmt_t **)))

#define POP_STMT(s) do { \
        stack_pop(s, NULL, sizeof(stmt_t **)); \
        _ifj_syntax_log("==STACK== pop stmt_t** level:%zu", \
                        stack_size(s) / sizeof(stmt_t **)); \
} while (0)

#define PUSH_STMT(s, st) do { \
        *(st) = stmt_append(NULL, STMT_NOP); \
        if (!*(st)) { \
            _parse_errno = PARSE_ENOMEM; \
            return false; \
        } \
        if (!stack_push(&(s), (char *) &*(st), sizeof(stmt_t **))) { \
            _parse_errno = PARSE_ENOMEM; \
            return false; \
        } \
        _ifj_syntax_log("==STACK== push stmt_t** level:%zu", \
                        stack_size(s) / sizeof(stmt_t **)); \
} while (0)

// -----------------------------------------------------------------------
// token
// -----------------------------------------------------------------------
tokens_t _tok = TOK_EOF;
void *_attr = NULL;

/* ==================================================================== */
/*                  R    U    L    E    S                               */
/* ==================================================================== */

// -----------------------------------------------------------------------
// function prototypes
// -----------------------------------------------------------------------

/* <program> ::= <function-or-stmt-list>                                */
static bool _r_program();

/* <opt-expr> ::= <expr>                                                */
/* <opt-expr> ::= epsilon                                               */
static bool _r_opt_expr();

/* <function> ::=                                                       */
/*         function id ( <param-list> ) EOL <stmt-list> <end-stmt>      */
static bool _r_function();

/* <end-stmt> ::= end EOL                                               */
static bool _r_end_stmt();

/* <end-stmt-opt-expr> ::= end <opt-expr> EOL                           */
static bool _r_end_stmt_opt_expr();

/* <if-stmt> ::= if <expr> EOL <stmt-list>                              */
static bool _r_if_stmt();

/* <else-stmt> ::= else EOL <stmt-list>                                 */
static bool _r_else_stmt();

/* <if-else-stmt> ::= <if-stmt> <else-stmt> <end-stmt>                  */
static bool _r_if_else_stmt();

/* <loop-stmt> ::= loop EOL <stmt-list> <end-stmt-opt-expr>             */
static bool _r_loop_stmt();

/* <while-stmt> ::= while <expr> EOL <stmt-list> <end-stmt>             */
static bool _r_while_stmt();

/* <return-stmt> ::= return <expr> EOL                                  */
static bool _r_return_stmt();

/* <assign-stmt> ::= id = <expr> EOL                                    */
static bool _r_assign_stmt();

/* <stmt> ::= <if-else-stmt>                                            */
/* <stmt> ::= <loop-stmt>                                               */
/* <stmt> ::= <while-stmt>                                              */
/* <stmt> ::= <return-stmt>                                             */
/* <stmt> ::= <assign-stmt>                                             */
static bool _r_stmt();

/* <stmt-list> ::= <stmt> <stmt-list>                                   */
/* <stmt-list> ::= epsilon                                              */
static bool _r_stmt_list();

/* <function-or-stmt-list> ::= <stmt> <function-or-stmt-list>           */
/* <function-or-stmt-list> ::= <function> <function-or-stmt-list>       */
/* <function-or-stmt-list> ::= epsilon                                  */
static bool _r_function_or_stmt_list();

/* <param-n> ::= , id <param-n>                                         */
/* <param-n> ::= epsilon                                                */
static bool _r_param_n();

/* <param-list> ::= id <param-n>                                        */
/* <param-list> ::= epsilon                                             */
static bool _r_param_list();

// -----------------------------------------------------------------------
// <program>
// -----------------------------------------------------------------------
bool _r_program()
{
    _ifj_syntax_log("<program>");
    NEXT_TOKEN();

    switch (_tok) {
    case TOK_TYPE_KW:
        switch ((keywords_t) _attr) {
        case KW_FUNCTION:
            return _r_function_or_stmt_list();
        case KW_IF:
            return _r_function_or_stmt_list();
        case KW_LOOP:
            return _r_function_or_stmt_list();
        case KW_WHILE:
            return _r_function_or_stmt_list();
        case KW_RETURN:
            return _r_function_or_stmt_list();
        default:
            _parse_errno = PARSE_ESYNTAX;
            return false;
        }

    case TOK_EOL:
        return _r_function_or_stmt_list();

    case TOK_EOF:
        return _r_function_or_stmt_list();

    case TOK_TYPE_ID:
        return _r_function_or_stmt_list();

    default:
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
}

// -----------------------------------------------------------------------
// <function>
// -----------------------------------------------------------------------
bool _r_function()
{
    _ifj_syntax_log("<function>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_FUNCTION) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    NEXT_TOKEN();
    if (_tok != TOK_TYPE_ID) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    *last = stmt_append(*last, STMT_FUNCTION);
    (*last)->p_function.name = (string_t *) _attr;

    NEXT_TOKEN();
    if (_tok != TOK_LPAR) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    NEXT_TOKEN();

    if (!_r_param_list()) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    if (_tok != TOK_RPAR) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    NEXT_TOKEN();
    if (_tok != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    PUSH_STMT(_stmt_s, &(*last)->p_function.stmt);
    NEXT_TOKEN();
    return _r_stmt_list() && _r_end_stmt();
}

// -----------------------------------------------------------------------
// <function-or-stmt-list>
// -----------------------------------------------------------------------
bool _r_function_or_stmt_list()
{
    _ifj_syntax_log("<function-or-stmt-list>");

    switch (_tok) {
    case TOK_EOF:
        return true;

    case TOK_EOL:
        NEXT_TOKEN();
        return _r_function_or_stmt_list();

    case TOK_TYPE_KW:
        switch ((keywords_t) _attr) {
        case KW_FUNCTION:
            return _r_function() && _r_function_or_stmt_list();
        case KW_IF:
            return _r_stmt() && _r_function_or_stmt_list();
        case KW_LOOP:
            return _r_stmt() && _r_function_or_stmt_list();
        case KW_WHILE:
            return _r_stmt() && _r_function_or_stmt_list();
        case KW_RETURN:
            return _r_stmt() && _r_function_or_stmt_list();
        default:
            _parse_errno = PARSE_ESYNTAX;
            return false;
        }

    case TOK_TYPE_ID:
        return _r_stmt() && _r_function_or_stmt_list();

    default:
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
}

// -----------------------------------------------------------------------
// <if-else-stmt>
// -----------------------------------------------------------------------
bool _r_if_else_stmt()
{
    _ifj_syntax_log("<if-else-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_IF) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    *last = stmt_append(*last, STMT_BRANCH);

    return _r_if_stmt() && _r_else_stmt() && _r_end_stmt();
}

// -----------------------------------------------------------------------
// <loop-stmt>
// -----------------------------------------------------------------------
bool _r_loop_stmt()
{
    _ifj_syntax_log("<loop-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_LOOP) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    *last = stmt_append(*last, STMT_LOOP);

    NEXT_TOKEN();
    if (_tok != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    PUSH_STMT(_stmt_s, &(*last)->p_loop.stmt);
    NEXT_TOKEN();
    return _r_stmt_list() && _r_end_stmt_opt_expr();
}

// -----------------------------------------------------------------------
// <while-stmt>
// -----------------------------------------------------------------------
bool _r_while_stmt()
{
    _ifj_syntax_log("<while-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_WHILE) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    *last = stmt_append(*last, STMT_WHILE);
    (*last)->p_loop.cond = NULL;

    if (!parse_expr(&(*last)->p_loop.cond)) {
        return false;
    }
    if ((_tok /*= getNextToken(&_attr)*/ ) != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    PUSH_STMT(_stmt_s, &(*last)->p_loop.stmt);
    NEXT_TOKEN();
    return _r_stmt_list() && _r_end_stmt();
}

// -----------------------------------------------------------------------
// <assign-stmt>
// -----------------------------------------------------------------------
bool _r_assign_stmt()
{
    _ifj_syntax_log("<assign-stmt>");

    if (_tok != TOK_TYPE_ID) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    _ifj_syntax_log("id \"%s\"", string_getstr(_attr));

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    *last = stmt_append(*last, STMT_ASSIGN);
    (*last)->p_assign.lhs = (string_t *) _attr;
    (*last)->p_assign.rhs = NULL;

    NEXT_TOKEN();
    if (_tok != TOK_OP_ASSIGN) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    if (!parse_expr(&(*last)->p_assign.rhs)) {
        return false;
    }
    if ((_tok /*= getNextToken(&_attr)*/ ) != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    NEXT_TOKEN();
    return true;
}

// -----------------------------------------------------------------------
// <return-stmt>
// -----------------------------------------------------------------------
bool _r_return_stmt()
{
    _ifj_syntax_log("<return-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_RETURN) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    *last = stmt_append(*last, STMT_RETURN);
    (*last)->p_return.expr = NULL;

    if (!parse_expr(&(*last)->p_return.expr)) {
        return false;
    }
    if ((_tok /*= getNextToken(&_attr)*/ ) != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    NEXT_TOKEN();
    return true;
}

// -----------------------------------------------------------------------
// <opt-expr>
// -----------------------------------------------------------------------
bool _r_opt_expr()
{
    _ifj_syntax_log("<opt-expr>");

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    _ifj_assert_if((*last)->type == STMT_LOOP);

    return parse_expr(&(*last)->p_loop.cond);
}

// -----------------------------------------------------------------------
// <end-stmt>
// -----------------------------------------------------------------------
bool _r_end_stmt()
{
    _ifj_syntax_log("<end-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_END) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    NEXT_TOKEN();
    if (_tok != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    POP_STMT(_stmt_s);
    NEXT_TOKEN();
    return true;
}

// -----------------------------------------------------------------------
// <end-stmt-opt-expr>
// -----------------------------------------------------------------------
bool _r_end_stmt_opt_expr()
{
    _ifj_syntax_log("<end-stmt-opt-expr>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_END) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    POP_STMT(_stmt_s);
    stmt_t **last = GET_LAST_STMT(_stmt_s);
    _ifj_assert_if((*last)->type == STMT_LOOP);
    (*last)->p_loop.cond = NULL;

    if (!_r_opt_expr()) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    if ((_tok /*= getNextToken(&_attr)*/ ) != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    NEXT_TOKEN();
    return true;
}

// -----------------------------------------------------------------------
// <if-stmt>
// -----------------------------------------------------------------------
bool _r_if_stmt()
{
    _ifj_syntax_log("<if-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_IF) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    stmt_t **last = GET_LAST_STMT(_stmt_s);
    _ifj_assert_if((*last)->type == STMT_BRANCH);
    (*last)->p_branch.cond = NULL;

    if (!parse_expr(&(*last)->p_branch.cond)) {
        return false;
    }
    if ((_tok /*= getNextToken(&_attr)*/ ) != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    PUSH_STMT(_stmt_s, &(*last)->p_branch.stmt_i);
    NEXT_TOKEN();
    return _r_stmt_list();
}

// -----------------------------------------------------------------------
// <else-stmt>
// -----------------------------------------------------------------------
bool _r_else_stmt()
{
    _ifj_syntax_log("<else-stmt>");

    if (_tok != TOK_TYPE_KW && (keywords_t) _attr != KW_ELSE) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
    NEXT_TOKEN();
    if (_tok != TOK_EOL) {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }

    POP_STMT(_stmt_s);
    stmt_t **last = GET_LAST_STMT(_stmt_s);
    _ifj_assert_if((*last)->type == STMT_BRANCH);

    PUSH_STMT(_stmt_s, &(*last)->p_branch.stmt_o);
    NEXT_TOKEN();
    return _r_stmt_list();
}

// -----------------------------------------------------------------------
// <param-n>
// -----------------------------------------------------------------------
bool _r_param_n()
{
    _ifj_syntax_log("<param-n>");

    if (_tok == TOK_RPAR) {
        return true;
    }
    else if (_tok == TOK_COMMA) {
        NEXT_TOKEN();
        if (_tok != TOK_TYPE_ID) {
            _parse_errno = PARSE_ESYNTAX;
            return false;
        }

        stmt_t **last = GET_LAST_STMT(_stmt_s);
        _ifj_assert_if((*last)->type == STMT_FUNCTION);

        string_list_t **params = &(*last)->p_function.params;
        string_list_append(*params, (string_t *) _attr);

        NEXT_TOKEN();
        return _r_param_n();
    }
    else {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
}

// -----------------------------------------------------------------------
// <param-list>
// -----------------------------------------------------------------------
bool _r_param_list()
{
    _ifj_syntax_log("<param-list>");

    if (_tok == TOK_RPAR) {
        return true;
    }
    else if (_tok == TOK_TYPE_ID) {
        stmt_t **last = GET_LAST_STMT(_stmt_s);
        _ifj_assert_if((*last)->type == STMT_FUNCTION);

        string_list_t **params = &(*last)->p_function.params;
        *params = string_list_append(*params, (string_t *) _attr);

        NEXT_TOKEN();
        return _r_param_n();
    }
    else {
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
}

// -----------------------------------------------------------------------
// <stmt>
// -----------------------------------------------------------------------
bool _r_stmt()
{
    _ifj_syntax_log("<stmt>");

    switch (_tok) {
    case TOK_TYPE_KW:
        switch ((keywords_t) _attr) {
        case KW_IF:
            return _r_if_else_stmt();
        case KW_LOOP:
            return _r_loop_stmt();
        case KW_WHILE:
            return _r_while_stmt();
        case KW_RETURN:
            return _r_return_stmt();
        default:
            _parse_errno = PARSE_ESYNTAX;
            return false;
        }

    case TOK_EOL:
        return true;

    case TOK_TYPE_ID:
        return _r_assign_stmt();

    default:
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
}

// -----------------------------------------------------------------------
// <stmt-list>
// -----------------------------------------------------------------------
bool _r_stmt_list()
{
    _ifj_syntax_log("<stmt-list>");

    switch (_tok) {
    case TOK_EOF:
        return true;

    case TOK_EOL:
        NEXT_TOKEN();
        return _r_stmt_list();

    case TOK_TYPE_KW:
        switch ((keywords_t) _attr) {
        case KW_IF:
            return _r_stmt() && _r_stmt_list();
        case KW_ELSE:
            return true;
        case KW_LOOP:
            return _r_stmt() && _r_stmt_list();
        case KW_WHILE:
            return _r_stmt() && _r_stmt_list();
        case KW_RETURN:
            return _r_stmt() && _r_stmt_list();
        case KW_END:
            return true;
        default:
            _parse_errno = PARSE_ESYNTAX;
            return false;
        }

    case TOK_TYPE_ID:
        return _r_stmt() && _r_stmt_list();

    default:
        _parse_errno = PARSE_ESYNTAX;
        return false;
    }
}

/* ==================================================================== */
/*    P   U   B   L   I   C       I   N   T   E   R   F   A   C   E     */
/* ==================================================================== */
int parse(ast_t * ast)
{
    assert(ast != NULL);
    _ast = ast;
    _parse_errno = PARSE_EOK;

    if (!(_stmt_s = stack_new(8 * sizeof(stmt_t **)))) {
        return PARSE_ENOMEM;
    }

    PUSH_STMT(_stmt_s, &ast->stmt);
    _r_program();
    POP_STMT(_stmt_s);

    stack_dispose(_stmt_s);

    if (_parse_errno == PARSE_ESYNTAX) {
        _ifj_error_log("%s: Nastala chyba pri syntakticke analyze, "
                       "nechybi token?", src_name);
    }
    else if (_parse_errno == PARSE_EEXPR) {
        _ifj_error_log("%s: Nastala chyba pri parsovani vyrazu, "
                       "nenalezeno vhodne pravidlo.", src_name);
    }

    return _parse_errno;
}
