/*
 * syntree.h:      Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef SYNTREE_H
# define SYNTREE_H

# include <stdbool.h>

# include "ial.h"
# include "str.h"
# include "types.h"

/* ==================================================================== */
/*     A  B  S  T  R  A  C  T     S  Y  N  T  A  X     T  R  E  E       */
/* ==================================================================== */
typedef struct _stmt_st stmt_t;
typedef struct _expr_st expr_t;

/* -------------------------------------------------------------------- */
/* expression tree                                                      */
/* -------------------------------------------------------------------- */
typedef enum {
    EXPR_CALL,
    EXPR_SUBSTR,

    EXPR_MINUS,
    EXPR_SUM,
    EXPR_SUB,
    EXPR_MULT,
    EXPR_DIV,
    EXPR_POW,

    EXPR_CMP_LT,
    EXPR_CMP_LE,
    EXPR_CMP_GT,
    EXPR_CMP_GE,
    EXPR_CMP_EQ,
    EXPR_CMP_NEQ,

    EXPR_AND,
    EXPR_OR,
    EXPR_NOT,
    EXPR_IN,
    EXPR_NOTIN,

    EXPR_ID,
    EXPR_STR,
    EXPR_NUM,
    EXPR_BOOL,
    EXPR_NIL,
} expr_type;

struct _expr_tbl_st;
typedef struct {
    string_t *fn_name;
    struct _expr_tbl_st *params;
} expr_call_t;

typedef struct {
    expr_t *val;
    string_t *from;
    string_t *to;
} expr_substr_t;

typedef struct {
    expr_t *lhs;
    expr_t *rhs;
} expr_binop_t;

struct _expr_st {
    expr_type type;
    union {
        expr_call_t __p_call;
        expr_substr_t __p_substr;
        expr_binop_t __p_binop;
        expr_t *__p_unop;

        string_t *__p_str;
        string_t *__p_id;
        string_t *__p_num;
        bool __p_bool;
        void *__p_nil;
    } __u;
# define p_call __u.__p_call
# define p_substr __u.__p_substr
# define p_binop __u.__p_binop
# define p_unop __u.__p_unop
# define p_str __u.__p_str
# define p_id __u.__p_id
# define p_num __u.__p_num
# define p_bool __u.__p_bool
# define p_nil __u.__p_nil
};


expr_t *expr_new(expr_type);
void expr_delete_all(expr_t *);

/* -------------------------------------------------------------------- */
/* expression tree single-linked list                                   */
/* -------------------------------------------------------------------- */
struct _expr_tbl_st {
    struct _expr_tbl_st *next;
    expr_t *expr;
};

struct _expr_tbl_st *_alloc_expr_tbl(struct _expr_tbl_st **);
void _free_expr_tbl(struct _expr_tbl_st *);
struct _expr_tbl_st *_alloc_expr_tbl_prepend(struct _expr_tbl_st *);

/* -------------------------------------------------------------------- */
/* statement single-linked list                                         */
/* -------------------------------------------------------------------- */
typedef enum {
    STMT_NOP,
    STMT_ASSIGN,
    STMT_BRANCH,
    STMT_WHILE,
    STMT_LOOP,
    STMT_FUNCTION,
    STMT_RETURN,
} stmt_type;

typedef struct {
    string_t *lhs;
    expr_t *rhs;
} stmt_assign_t;

typedef struct {
    expr_t *cond;
    stmt_t *stmt_i;
    stmt_t *stmt_o;
} stmt_branch_t;

typedef struct {
    expr_t *cond;
    stmt_t *stmt;
} stmt_loop_t;

typedef struct {
    expr_t *expr;
} stmt_return_t;

typedef struct {
    string_t *name;
    string_list_t *params;
    stmt_t *stmt;
} stmt_function_t;

struct _stmt_st {
    struct _stmt_st *next;
    stmt_type type;
    union {
        stmt_assign_t __p_assign;
        stmt_branch_t __p_branch;
        stmt_loop_t __p_loop;
        stmt_function_t __p_function;
        stmt_return_t __p_return;
    } __u;

#define p_assign __u.__p_assign
#define p_branch __u.__p_branch
#define p_loop __u.__p_loop
#define p_function __u.__p_function
#define p_return __u.__p_return
};

stmt_t *stmt_append(stmt_t *, stmt_type);
void stmt_delete_all(stmt_t *);

typedef struct {
    struct _symbol_tbl_st {
        struct _symbol_tbl_st *next;
        bst_t *bst;
    } *sym;
    struct _variable_tbl_st {
        struct _variable_tbl_st *next;
        typeval_t typeval;
    } *var;
    stmt_t *stmt;
} ast_t;

/* -------------------------------------------------------------------- */
/* symbol_tbl single-linked list                                        */
/* -------------------------------------------------------------------- */
struct _symbol_tbl_st *_alloc_sym_tbl(struct _symbol_tbl_st **);
void _free_sym_tbl(struct _symbol_tbl_st *);
struct _symbol_tbl_st *_alloc_sym_tbl_append(struct _symbol_tbl_st *);

/* -------------------------------------------------------------------- */
/* variable_tbl single-linked list                                      */
/* -------------------------------------------------------------------- */
struct _variable_tbl_st *_alloc_var_tbl(struct _variable_tbl_st **);
void _free_var_tbl(struct _variable_tbl_st *);
struct _variable_tbl_st *_alloc_var_tbl_append(struct _variable_tbl_st *);

#endif                          // SYNTREE_H
