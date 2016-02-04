/*
 * syntree.c:      Implementace interpretu imperativniho jazyka IFJ12
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

#include "syntree.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "types.h"

/* -------------------------------------------------------------------- */
/* statement single-linked list                                         */
/* -------------------------------------------------------------------- */
static void _stmt_free_props(stmt_t * p)
{
    if (!p) {
        return;
    }

    switch (p->type) {
    default:
        _ifj_assert();
        break;
    case STMT_ASSIGN:
        string_dispose(p->p_assign.lhs);
        expr_delete_all(p->p_assign.rhs);
        break;
    case STMT_BRANCH:
        stmt_delete_all(p->p_branch.stmt_i);
        stmt_delete_all(p->p_branch.stmt_o);
        expr_delete_all(p->p_branch.cond);
        break;
    case STMT_WHILE:
    case STMT_LOOP:
        stmt_delete_all(p->p_loop.stmt);
        expr_delete_all(p->p_loop.cond);
        break;
    case STMT_FUNCTION:
        string_dispose(p->p_function.name);
        string_list_delete_all(p->p_function.params);
        stmt_delete_all(p->p_function.stmt);
        break;
    case STMT_RETURN:
        expr_delete_all(p->p_return.expr);
        break;
    case STMT_NOP:
        break;
    }

    return;
}

stmt_t *stmt_append(stmt_t * s, stmt_type t)
{
    stmt_t *p = (stmt_t *) malloc(sizeof(stmt_t));
    if (p) {
        p->type = t;
        p->next = NULL;
        memset(&p->__u, '\0', sizeof(p->__u));

        if (s) {
            s->next = p;
        }
    }

    return p;
}

void stmt_delete_all(stmt_t * s)
{
    for (stmt_t * p = s; p; p = s) {
        s = p->next;
        _stmt_free_props(p);
        free(p);
    }
}

/* -------------------------------------------------------------------- */
/* expression tree                                                      */
/* -------------------------------------------------------------------- */
static void _expr_free_props(expr_t * p)
{
    if (!p) {
        return;
    }

    switch (p->type) {
    default:
        _ifj_assert();
        break;

    case EXPR_SUM:
    case EXPR_SUB:
    case EXPR_MULT:
    case EXPR_DIV:
    case EXPR_POW:
    case EXPR_CMP_LT:
    case EXPR_CMP_LE:
    case EXPR_CMP_GT:
    case EXPR_CMP_GE:
    case EXPR_CMP_EQ:
    case EXPR_CMP_NEQ:
    case EXPR_AND:
    case EXPR_OR:
    case EXPR_IN:
    case EXPR_NOTIN:
        expr_delete_all(p->p_binop.lhs);
        expr_delete_all(p->p_binop.rhs);
        break;

    case EXPR_ID:
        string_dispose(p->p_id);
        break;
    case EXPR_STR:
        string_dispose(p->p_str);
        break;
    case EXPR_NUM:
        string_dispose(p->p_num);
        break;
    case EXPR_BOOL:
        break;
    case EXPR_NIL:
        break;

    case EXPR_MINUS:
    case EXPR_NOT:
        expr_delete_all(p->p_unop);
        break;

    case EXPR_CALL:
        string_dispose(p->p_call.fn_name);
        _free_expr_tbl(p->p_call.params);
        break;

    case EXPR_SUBSTR:
        expr_delete_all(p->p_substr.val);
        string_dispose(p->p_substr.from);
        string_dispose(p->p_substr.to);
        break;
    }

    return;
}

expr_t *expr_new(expr_type t)
{
    expr_t *p = (expr_t *) malloc(sizeof(expr_t));
    if (p) {
        p->type = t;
        memset(&p->__u, '\0', sizeof(p->__u));
    }

    return p;
}

void expr_delete_all(expr_t * e)
{
    _expr_free_props(e);
    free(e);
}

struct _expr_tbl_st *_alloc_expr_tbl(struct _expr_tbl_st **p)
{
    _ifj_assert_if(p != NULL);
    *p = (struct _expr_tbl_st *)
        malloc(sizeof(struct _expr_tbl_st));

    if (!*p) {
        return NULL;
    }

    (*p)->next = NULL;
    (*p)->expr = NULL;
    return *p;
}

void _free_expr_tbl(struct _expr_tbl_st *p)
{
    for (struct _expr_tbl_st * q; p; p = q) {
        q = p->next;
        expr_delete_all(p->expr);
        free(p);
    }
}

struct _expr_tbl_st *_alloc_expr_tbl_prepend(struct _expr_tbl_st *r)
{
    _ifj_assert_if(r != NULL);

    while (r->next)
        r = r->next;

    return _alloc_expr_tbl(&r->next);
}

/* -------------------------------------------------------------------- */
/* symbol table                                                         */
/* -------------------------------------------------------------------- */
struct _symbol_tbl_st *_alloc_sym_tbl(struct _symbol_tbl_st **p)
{
    _ifj_assert_if(p != NULL);
    *p = (struct _symbol_tbl_st *)
        malloc(sizeof(struct _symbol_tbl_st));

    if (!*p) {
        return NULL;
    }

    (*p)->next = NULL;
    (*p)->bst = bst_new();
    return *p;
}

void _free_sym_tbl(struct _symbol_tbl_st *p)
{
    for (struct _symbol_tbl_st * q; p; p = q) {
        q = p->next;
        bst_dispose(&p->bst, NULL);
        free(p);
    }
}

struct _symbol_tbl_st *_alloc_sym_tbl_append(struct _symbol_tbl_st *r)
{
    _ifj_assert_if(r != NULL);

    while (r->next)
        r = r->next;

    return _alloc_sym_tbl(&r->next);
}

/* -------------------------------------------------------------------- */
/* variable table                                                       */
/* -------------------------------------------------------------------- */
static void _free_var(typeval_t * p)
{
    if (p) {
        switch (p->type) {
        default:
            _ifj_assert();
            break;

        case TYPE_NUMERIC:
        case TYPE_BOOLEAN:
        case TYPE_NIL:
            break;

        case TYPE_STRING:
            string_dispose(p->v_str);
            break;
        }
    }
}

struct _variable_tbl_st *_alloc_var_tbl(struct _variable_tbl_st **p)
{
    _ifj_assert_if(p != NULL);
    *p = (struct _variable_tbl_st *)
        malloc(sizeof(struct _variable_tbl_st));

    if (!*p) {
        return NULL;
    }

    (*p)->next = NULL;
    memset(&(*p)->typeval, '\0', sizeof(typeval_t));
    return *p;
}

void _free_var_tbl(struct _variable_tbl_st *p)
{
    for (struct _variable_tbl_st * q; p; p = q) {
        q = p->next;
        _free_var(&p->typeval);
        free(p);
    }
}

struct _variable_tbl_st *_alloc_var_tbl_append(struct _variable_tbl_st *r)
{
    _ifj_assert_if(r != NULL);

    while (r->next)
        r = r->next;

    return _alloc_var_tbl(&r->next);
}
