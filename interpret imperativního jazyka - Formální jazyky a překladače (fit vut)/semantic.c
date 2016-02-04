/*
 * semantic.c:     Implementace interpretu imperativniho jazyka IFJ12
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

#include "semantic.h"

#include <assert.h>
#include <fenv.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

#include "defs.h"
#include "stack.h"
#include "syntree.h"

#define CHECKED(cmd) do { \
        int hr = (cmd); \
        if (hr != SEM_EOK) \
            return hr; \
} while (0)

/* ==================================================================== */
/* BUILTIN FUNCTIONS                                                    */
/* ==================================================================== */
double _builtin_typeof(typeval_t * t)
{
    if (!t) {
        return 0.0;
    }

    switch (t->type) {
    default:
        return 0.0;

#define X(a,b) \
    case TYPE_##a: \
        return (double)(b);
#include "types.def"
        X_TYPES
#undef X
    }
}

int _builtin_numeric(const char *s, double *num)
{
    _ifj_assert_if(s != NULL);
    _ifj_assert_if(num != NULL);

    char *endptr;
    *num = strtod_l(s, &endptr, c_locale);
    if (endptr == s) {
        return SEM_ECONVNUM;
    }
    else if (errno == ERANGE) {
        return SEM_ERANGE;
    }

    return SEM_EOK;
}

void _builtin_print(typeval_t * val)
{
    _ifj_assert_if(val != NULL);

    switch (val->type) {
    case TYPE_NIL:
        fprintf(stdout, "%s", "Nil");
        break;
    case TYPE_BOOLEAN:
        fprintf(stdout, "%s", val->v_bool ? "true" : "false");
        break;
    case TYPE_NUMERIC: {
        // hack pro locale, na nekterych systemech neni fprintf_l
        // a cislo bysme meli vypsat s desetinou teckou
        struct lconv *cur_locale = localeconv();
        if (*cur_locale->decimal_point == ',') {
            char buf[255];
            sprintf(buf, "%g", val->v_num);
            char *decpt = strchr(buf, ',');
            if (decpt)
                *decpt = '.';
            fprintf(stdout, "%s", buf);
        }
        else {
            fprintf(stdout, "%g", val->v_num);
        }
    }   break;
    case TYPE_FUNCTION:
        fprintf(stdout, "%s", string_getstr(val->v_str));
        break;
    case TYPE_STRING:
        fprintf(stdout, "%s", string_getstr(val->v_str));
        break;
    }
}

int _builtin_input(typeval_t * val)
{
    _ifj_assert_if(val != NULL);

    switch (val->type) {
    default:
        break;
    case TYPE_STRING:
        string_dispose(val->v_str);
        break;
    }

    val->type = TYPE_STRING;
    val->v_str = string_new();
    if (!val->v_str) {
        return SEM_ENOMEM;
    }

    char buf[255];
    for (bool eol = false; !eol; ) {
        fgets(buf, countof(buf), stdin);
        char *eol_l = strrchr(buf, '\n');
        if (eol_l) {
            *eol_l = '\0';
            eol = true;
        }
        if (!string_concat(&val->v_str, buf))
            return SEM_ENOMEM;
    }

    return SEM_EOK;
}

int _builtin_sort(typeval_t * val)
{
    _ifj_assert_if(val != NULL);
    if (val->type != TYPE_STRING) {
        return SEM_EINVALTYPE;
    }

    if (!ial_sort(val->v_str)) {
        return SEM_ENOMEM;
    }

    return SEM_EOK;
}

double _builtin_find(string_t *str, string_t *substr)
{
    _ifj_assert_if(str != NULL);
    _ifj_assert_if(substr != NULL);

    char* p = ial_strstr(str, substr);
    if (!p) {
        return -1.0;
    }
    else if (p == string_getstr(str)) {
        return 0.0;
    }
    else {
        return (double)(p - string_getstr(str));
    }
}

double _builtin_len(typeval_t * val)
{
    _ifj_assert_if(val != NULL);

    switch (val->type) {
    default:
        return 0.0;
    case TYPE_STRING:
        return string_getlen(val->v_str);
    }
}

/* ==================================================================== */
/* INSTRUCTIONS                                                         */
/* ==================================================================== */
#define X_ARITHM_LOG \
X(_op_sum, +, double) \
X(_op_sub, -, double) \
X(_op_mult, *, double) \
X(_op_div, /, double) \
X(_op_and, &&, bool) \
X(_op_or, ||, bool)

#define X(fn, op, t) \
static t (fn)(t a, t b) \
{ \
    return a op b; \
}
        X_ARITHM_LOG
#undef X

static double _op_pow(double a, double b)
{
    return pow(a, b);
}

static bool _op_not(bool b)
{
    return !b;
}

static double _op_minus(double d)
{
    return -d;
}

static bool _op_in(string_t * a, string_t * b)
{
    return isgreaterequal(_builtin_find(b, a), 0.0);
}

static bool _op_notin(string_t * a, string_t * b)
{
    return !_op_in(a, b);
}

static string_t *_op_concat(string_t * a, string_t * b)
{
    string_t *r = string_new();
    if (!r) {
        return NULL;
    }
    if (!string_concat(&r, string_getstr(a))) {
        goto return_null;
    }
    if (!string_concat(&r, string_getstr(b))) {
        goto return_null;
    }

    return r;

  return_null:
    string_dispose(r);
    return NULL;
}

static string_t *_op_str_n(string_t * str, double times)
{
    string_t *r = string_new();
    if (!r) {
        return NULL;
    }

    if (!islessgreater(times, 0.0)) {
        // retezec ^ 0 = prazdny retezec
        return r;
    }
    else if (isless(times, 0.0)) {
        goto return_null;
    }
    else {
        size_t n = (size_t) trunc(times);
        while (n--) {
            if (!string_concat(&r, string_getstr(str))) {
                goto return_null;
            }
        }
    }

    return r;

  return_null:
    string_dispose(r);
    return NULL;
}

static bool _op_eq(typeval_t *l, typeval_t *r)
{
    _ifj_assert_if(l != NULL);
    _ifj_assert_if(r != NULL);

    if (l->type != r->type)
        return false;

    switch (l->type) {
    default:
        _ifj_assert();
        return false;
    case TYPE_NIL:
        return true;
    case TYPE_NUMERIC:
        return !islessgreater(l->v_num, r->v_num);
    case TYPE_STRING:
        return !strcmp(string_getstr(l->v_str), string_getstr(r->v_str));
    case TYPE_BOOLEAN:
        return l->v_bool == r->v_bool;
    }
}

static bool _op_neq(typeval_t *l, typeval_t *r)
{
    return !_op_eq(l, r);
}

#define X_REL_OP \
X(_op_lt, isless, <) \
X(_op_le, islessequal, <=) \
X(_op_gt, isgreater, >) \
X(_op_ge, isgreaterequal, >=)

#define X(fn, cmp_num, cmp_str) \
static int (fn)(typeval_t *l, typeval_t *r, bool *b) \
{ \
    _ifj_assert_if(l != NULL); \
    _ifj_assert_if(r != NULL); \
    _ifj_assert_if(b != NULL); \
\
    if (l->type != r->type) \
        return SEM_EINVALTYPE; \
\
    if (l->type == TYPE_NUMERIC) { \
        *b = cmp_num(l->v_num, r->v_num); \
    } \
    else if (l->type == TYPE_STRING) { \
        *b = strcmp(string_getstr(l->v_str), string_getstr(r->v_str)) cmp_str 0; \
    } \
    else { \
        return SEM_EINVALTYPE; \
    } \
\
    return SEM_EOK; \
}

X_REL_OP
#undef X

/* ==================================================================== */
/* SEMANTIC CHECK FUNCTIONS                                             */
/* ==================================================================== */
static bool _not_reserved_name(const char *s)
{

    if (0) {
    }

#define X(a,b) \
    else if (!strcmp(s, (b))) { \
        return false; \
    }

#include "builtin.def"
#include "keywords.def"

    X_KEYWORD X_KEYWORD_RESERVED X_BUILTIN_FN X(0, "_main")
#undef X
        else {
        return true;
    }

}

/* ==================================================================== */
/* EXPRESSION EVALUATIION                                               */
/* ==================================================================== */
static int _interpret(stmt_t * stmt,
                      struct _symbol_tbl_st *sym_g,
                      struct _symbol_tbl_st *sym_l,
                      struct _variable_tbl_st *var_tbl);

static int _eval(expr_t * e,
                 struct _symbol_tbl_st *sym_g,
                 struct _symbol_tbl_st *sym_l,
                 typeval_t * t, struct _variable_tbl_st *var_tbl);

int _eval(expr_t * e,
          struct _symbol_tbl_st *sym_g,
          struct _symbol_tbl_st *sym_l,
          typeval_t * t, struct _variable_tbl_st *var_tbl)
{
    _ifj_assert_if(e != NULL);
    _ifj_assert_if(t != NULL);
    _ifj_assert_if(sym_g != NULL);
    _ifj_assert_if(sym_l != NULL);

    switch (e->type) {

    case EXPR_SUM:{
            typeval_t tv_l = { 0 }, tv_r = { 0 };
            CHECKED(_eval(e->p_binop.lhs, sym_g, sym_l, &tv_l, var_tbl));
            CHECKED(_eval(e->p_binop.rhs, sym_g, sym_l, &tv_r, var_tbl));
            if (tv_l.type == TYPE_NUMERIC && tv_r.type == TYPE_NUMERIC) {
                t->type = TYPE_NUMERIC;
                t->v_num = _op_sum(tv_l.v_num, tv_r.v_num);
            }
            else if (tv_l.type == TYPE_STRING && tv_r.type == TYPE_STRING) {
                struct _variable_tbl_st *loc_vartbl =
                        _alloc_var_tbl_append(var_tbl);
                if (!loc_vartbl)
                    return SEM_ENOMEM;
                t->type = TYPE_STRING;
                t->v_str = _op_concat(tv_l.v_str, tv_r.v_str);
                loc_vartbl->typeval.type = TYPE_STRING;
                loc_vartbl->typeval.v_str = t->v_str;
                if (!t->v_str) {
                    return SEM_ENOMEM;
                }
            }
            else if (tv_l.type == TYPE_STRING) {
                struct _variable_tbl_st *loc_vartbl =
                    _alloc_var_tbl_append(var_tbl);
                if (!loc_vartbl)
                    return SEM_ENOMEM;

                string_t* tmpstr = string_new();

                switch (tv_r.type) {
                case TYPE_NIL:
                    if (!string_concat(&tmpstr, "Nil"))
                        return SEM_ENOMEM;
                    break;

                case TYPE_BOOLEAN:
                    if (!string_concat(&tmpstr, tv_r.v_bool ? "true" : "false"))
                        return SEM_ENOMEM;
                    break;

                case TYPE_NUMERIC: {
                    // hack pro locale, na nekterych systemech neni fprintf_l
                    // a cislo bysme meli vypsat s desetinou teckou
                    char buf[255];
                    struct lconv *cur_locale = localeconv();

                    sprintf(buf, "%g", tv_r.v_num);

                    if (*cur_locale->decimal_point == ',') {
                        char *decpt = strchr(buf, ',');
                        if (decpt)
                            *decpt = '.';
                    }

                    if (!string_concat(&tmpstr, buf))
                        return SEM_ENOMEM;
                }   break;

                case TYPE_FUNCTION:
                case TYPE_STRING:
                    if (!string_concat(&tmpstr, string_getstr(tv_r.v_str)))
                        return SEM_ENOMEM;
                    break;
                }

                t->type = TYPE_STRING;
                t->v_str = _op_concat(tv_l.v_str, tmpstr);
                loc_vartbl->typeval.type = TYPE_STRING;
                loc_vartbl->typeval.v_str = t->v_str;
                if (!t->v_str) {
                    return SEM_ENOMEM;
                }
                string_dispose(tmpstr);
            }
            else {
                return SEM_EINVALTYPE;
            }
        }
        break;

    case EXPR_MULT:{
            typeval_t tv_l = { 0 }
            , tv_r = {
            0};
            CHECKED(_eval(e->p_binop.lhs, sym_g, sym_l, &tv_l, var_tbl));
            CHECKED(_eval(e->p_binop.rhs, sym_g, sym_l, &tv_r, var_tbl));
            if (tv_l.type == TYPE_NUMERIC && tv_r.type == TYPE_NUMERIC) {
                t->type = TYPE_NUMERIC;
                t->v_num = _op_mult(tv_l.v_num, tv_r.v_num);
            }
            else if (tv_l.type == TYPE_STRING && tv_r.type == TYPE_NUMERIC) {
                struct _variable_tbl_st *loc_vartbl =
                    _alloc_var_tbl_append(var_tbl);
                if (!loc_vartbl)
                    return SEM_ENOMEM;
                t->type = TYPE_STRING;
                t->v_str = _op_str_n(tv_l.v_str, tv_r.v_num);
                loc_vartbl->typeval.type = TYPE_STRING;
                loc_vartbl->typeval.v_str = t->v_str;
                if (!t->v_str) {
                    return SEM_ENOMEM;
                }
            }
            else {
                return SEM_EINVALTYPE;
            }
        }
        break;

#define X_BINOP \
X(EXPR_SUB, TYPE_NUMERIC, TYPE_NUMERIC, v_num, v_num, &_op_sub) \
X(EXPR_DIV, TYPE_NUMERIC, TYPE_NUMERIC, v_num, v_num, &_op_div) \
X(EXPR_POW, TYPE_NUMERIC, TYPE_NUMERIC, v_num, v_num, &_op_pow) \
X(EXPR_AND, TYPE_BOOLEAN, TYPE_BOOLEAN, v_bool, v_bool, &_op_and) \
X(EXPR_OR, TYPE_BOOLEAN, TYPE_BOOLEAN, v_bool, v_bool, &_op_or) \
X(EXPR_IN, TYPE_BOOLEAN, TYPE_STRING, v_bool, v_str, &_op_in) \
X(EXPR_NOTIN, TYPE_BOOLEAN, TYPE_STRING, v_bool, v_str, &_op_notin)

#define X_UNOP \
X(EXPR_NOT, TYPE_BOOLEAN, TYPE_BOOLEAN, v_bool, v_bool, &_op_not) \
X(EXPR_MINUS, TYPE_NUMERIC, TYPE_NUMERIC, v_num, v_num, &_op_minus)

#define X_RELOP_T \
X(EXPR_CMP_EQ, &_op_eq) \
X(EXPR_CMP_NEQ, &_op_neq)

#define X_RELOP \
X(EXPR_CMP_LT, &_op_lt) \
X(EXPR_CMP_LE, &_op_le) \
X(EXPR_CMP_GT, &_op_gt) \
X(EXPR_CMP_GE, &_op_ge)

#define X(expr, t_res, t_acc, p_res, p_acc, fn) \
    case expr: { \
        typeval_t tv_l = { 0 }, tv_r = { 0 }; \
        CHECKED(_eval(e->p_binop.lhs, sym_g, sym_l, &tv_l, var_tbl)); \
        CHECKED(_eval(e->p_binop.rhs, sym_g, sym_l, &tv_r, var_tbl)); \
        if (tv_l.type != (t_acc) || tv_r.type != (t_acc)) \
            return SEM_EINVALTYPE; \
        t->type = (t_res); \
        feclearexcept(FE_ALL_EXCEPT); \
        t->p_res = (fn)(tv_l.p_acc, tv_r.p_acc); \
        if (fetestexcept(FE_DIVBYZERO)) \
            return SEM_EDIVZERO; \
        if (fetestexcept(FE_OVERFLOW | FE_UNDERFLOW)) \
            return SEM_ERANGE; \
    }   break;
#if 0
    if ((t_res) == TYPE_NUMERIC) { \
        if (fpclassify(t->p_res) == FP_NAN || \
            fpclassify(t->p_res) == FP_INFINITE) { \
            return SEM_EDIVZERO; \
            } \
    }
#endif

        X_BINOP
#undef X
#define X(expr, t_res, t_acc, p_res, p_acc, fn) \
    case expr: { \
        typeval_t tv = { 0 }; \
        CHECKED(_eval(e->p_unop, sym_g, sym_l, &tv, var_tbl)); \
        if (tv.type != (t_acc)) \
            return SEM_EINVALTYPE; \
        t->type = (t_res); \
        t->p_res = (fn)(tv.p_acc); \
    }   break;
            X_UNOP
#undef X

#define X(expr, fn) \
    case expr: { \
        typeval_t tv_l = { 0 }, tv_r = { 0 }; \
        CHECKED(_eval(e->p_binop.lhs, sym_g, sym_l, &tv_l, var_tbl)); \
        CHECKED(_eval(e->p_binop.rhs, sym_g, sym_l, &tv_r, var_tbl)); \
        t->type = TYPE_BOOLEAN; \
        t->v_bool = (fn)(&tv_l, &tv_r); \
    }   break;
        X_RELOP_T
#undef X

#define X(expr, fn) \
    case expr: { \
        typeval_t tv_l = { 0 }, tv_r = { 0 }; \
        CHECKED(_eval(e->p_binop.lhs, sym_g, sym_l, &tv_l, var_tbl)); \
        CHECKED(_eval(e->p_binop.rhs, sym_g, sym_l, &tv_r, var_tbl)); \
        t->type = TYPE_BOOLEAN; \
        CHECKED((fn)(&tv_l, &tv_r, &t->v_bool)); \
    }   break;
    X_RELOP
#undef X

    case EXPR_NUM:
        t->type = TYPE_NUMERIC;
        CHECKED(_builtin_numeric(string_getstr(e->p_num), &t->v_num));
        break;

    case EXPR_STR: {
        string_t *newstr = string_new();
        if (!newstr)
            return SEM_ENOMEM;
        if (!string_concat(&newstr, string_getstr(e->p_str)))
            return SEM_ENOMEM;

        struct _variable_tbl_st *loc_vartbl =
            _alloc_var_tbl_append(var_tbl);
        loc_vartbl->typeval.type = TYPE_STRING;
        loc_vartbl->typeval.v_str = newstr;
        t->type = TYPE_STRING;
        t->v_str = newstr;
    }   break;

    case EXPR_BOOL:
        t->type = TYPE_BOOLEAN;
        t->v_bool = e->p_bool;
        break;

    case EXPR_NIL:
        t->type = TYPE_NIL;
        break;

    case EXPR_SUBSTR: {
        typeval_t tv = { 0 };
        CHECKED(_eval(e->p_substr.val, sym_g, sym_l, &tv, var_tbl));
        if (tv.type != TYPE_STRING)
            return SEM_ESUBSTR;
        double from = 0.0, to = string_getlen(tv.v_str);
        if (e->p_substr.from)
            CHECKED(_builtin_numeric(string_getstr(e->p_substr.from), &from));
        if (e->p_substr.to)
            CHECKED(_builtin_numeric(string_getstr(e->p_substr.to), &to));
        if (to < from) {
            from = 0.0;
            to = 0.0;
        }

        struct _variable_tbl_st *var =
            _alloc_var_tbl_append(var_tbl);
        if (!var)
            return SEM_ENOMEM;
        var->typeval.type = TYPE_STRING;
        var->typeval.v_str = string_new();
        if (!var->typeval.v_str)
            return SEM_ENOMEM;

        size_t nfrom = (size_t) trunc(from);
        size_t nto = (size_t) trunc(to);
        const char *str = string_getstr(tv.v_str);

        if (nto > string_getlen(tv.v_str))
            nto = string_getlen(tv.v_str);

        if (nfrom < string_getlen(tv.v_str)) {
            size_t n = nto - nfrom;
            str += nfrom;

            while (n--) {
                if (!string_append(&var->typeval.v_str, *str++))
                    return SEM_ENOMEM;
            }
        }

        memcpy(t, &var->typeval, sizeof(typeval_t));
    }   break;

    case EXPR_CALL:{
            stmt_function_t *fn;
            if (!bst_search
                (sym_g->bst, string_getstr(e->p_call.fn_name),
                 (void **) &fn)) {
                return SEM_EFUNCNOTDEF;
            }

            // alokujeme tabulku symbolu pro funkci
            struct _symbol_tbl_st *sym_fn = _alloc_sym_tbl_append(sym_l);
            if (!sym_fn) {
                return SEM_ENOMEM;
            }

            // navratova hodnota
            struct _variable_tbl_st *fn_retval =
                _alloc_var_tbl_append(var_tbl);
            if (!fn_retval)
                return SEM_ENOMEM;

            if (!bst_insert(&sym_fn->bst, "return", &fn_retval->typeval)) {
                return SEM_ENOMEM;
            }

            // vychozi navratova hodnota
            fn_retval->typeval.type = TYPE_NIL;

            if (!strcmp(string_getstr(e->p_call.fn_name), "typeOf")) {
                if (!e->p_call.params || !e->p_call.params->expr) {
                    fn_retval->typeval.v_num = _builtin_typeof(NULL);
                }
                else if (e->p_call.params->expr->type == EXPR_ID) {
                    if (bst_search
                        (sym_g->bst,
                         string_getstr(e->p_call.params->expr->p_id),
                         NULL)) {
                        fn_retval->typeval.type = TYPE_FUNCTION;
                    }
                    else {
                        CHECKED(_eval
                                (e->p_call.params->expr, sym_g, sym_l,
                                 &fn_retval->typeval, var_tbl));
                    }

                    fn_retval->typeval.v_num =
                        _builtin_typeof(&fn_retval->typeval);
                }
                else {
                    CHECKED(_eval
                            (e->p_call.params->expr, sym_g, sym_l,
                             &fn_retval->typeval, var_tbl));
                    fn_retval->typeval.v_num =
                        _builtin_typeof(&fn_retval->typeval);
                }
                fn_retval->typeval.type = TYPE_NUMERIC;
            }
            else if (!strcmp(string_getstr(e->p_call.fn_name), "numeric")) {
                if (!e->p_call.params || !e->p_call.params->expr) {
                    return SEM_ECONVNUM;
                }

                CHECKED(_eval
                        (e->p_call.params->expr, sym_g, sym_l,
                         &fn_retval->typeval, var_tbl));

                if (fn_retval->typeval.type == TYPE_NUMERIC) {
                    /* fn_retval->typeval.p_num = fn_retval->typeval.p_num; */
                }
                else if (fn_retval->typeval.type == TYPE_STRING) {
                    CHECKED(_builtin_numeric
                            (string_getstr(fn_retval->typeval.v_str),
                             &fn_retval->typeval.v_num));
                }
                else {
                    return SEM_ECONVNUM;
                }

                fn_retval->typeval.type = TYPE_NUMERIC;
            }
            else if (!strcmp(string_getstr(e->p_call.fn_name), "print")) {
                struct _expr_tbl_st *params_val = e->p_call.params;
                while (params_val) {
                    CHECKED(_eval
                            (params_val->expr, sym_g, sym_l,
                             &fn_retval->typeval, var_tbl));
                    _builtin_print(&fn_retval->typeval);
                    params_val = params_val->next;
                }

                fn_retval->typeval.type = TYPE_NIL;
            }
            else if (!strcmp(string_getstr(e->p_call.fn_name), "input")) {
                CHECKED(_builtin_input(&fn_retval->typeval));
            }
            else if (!strcmp(string_getstr(e->p_call.fn_name), "sort")) {
                if (!e->p_call.params)
                    return SEM_EINVALTYPE;
                CHECKED(_eval
                    (e->p_call.params->expr, sym_g, sym_l,
                    &fn_retval->typeval, var_tbl));
                CHECKED(_builtin_sort(&fn_retval->typeval));
            }
            else if (!strcmp(string_getstr(e->p_call.fn_name), "find")) {
                struct _variable_tbl_st *fn_str =
                    _alloc_var_tbl_append(var_tbl);
                struct _variable_tbl_st *fn_substr =
                    _alloc_var_tbl_append(var_tbl);

                struct _expr_tbl_st *params_val = e->p_call.params;

                if (!fn_str || !fn_substr)
                    return SEM_ENOMEM;
                if (!params_val || !params_val->next)
                    return SEM_EINVALTYPE;

                CHECKED(_eval
                    (params_val->expr, sym_g, sym_l,
                     &fn_str->typeval, var_tbl));
                CHECKED(_eval
                    (params_val->next->expr, sym_g, sym_l,
                     &fn_substr->typeval, var_tbl));

                if (fn_str->typeval.type != TYPE_STRING &&
                    fn_substr->typeval.type != TYPE_STRING)
                    return SEM_EINVALTYPE;

                fn_retval->typeval.v_num = _builtin_find(fn_str->typeval.v_str, fn_substr->typeval.v_str);
                fn_retval->typeval.type = TYPE_NUMERIC;
            }
            else if (!strcmp(string_getstr(e->p_call.fn_name), "len")) {
                if (e->p_call.params) {
                    CHECKED(_eval
                        (e->p_call.params->expr, sym_g, sym_l,
                        &fn_retval->typeval, var_tbl));
                }
                else {
                    fn_retval->typeval.type = TYPE_NIL;
                }

                fn_retval->typeval.v_num = _builtin_len(&fn_retval->typeval);
                fn_retval->typeval.type = TYPE_NUMERIC;
            }
            else {

                // vlozime promenne do lokalni tabulky
                string_list_t *params_name = fn->params;
                struct _expr_tbl_st *params_val = e->p_call.params;
                while (params_name || params_val) {
                    struct _variable_tbl_st *tbl = NULL;
                    if (params_name) {
                        if (!(tbl = _alloc_var_tbl_append(var_tbl))) {
                            return SEM_ENOMEM;
                        }

                        tbl->typeval.type = TYPE_NIL;

                        if (!bst_insert
                            (&sym_fn->bst, string_getstr(params_name->str),
                             &tbl->typeval)) {
                            return SEM_ENOMEM;
                        }

                        if (params_val) {
                            CHECKED(_eval
                                    (params_val->expr, sym_g, sym_l,
                                     &tbl->typeval, var_tbl));

                            params_val = params_val->next;
                        }

                        params_name = params_name->next;
                    }
                }

                CHECKED(_interpret(fn->stmt, sym_g, sym_fn, var_tbl));
            }

            // vratime navratovou hodnotu
            memcpy(t, &fn_retval->typeval, sizeof(typeval_t));
        }
        break;

    case EXPR_ID:{
            typeval_t *p = NULL;
            if (!bst_search
                (sym_l->bst, string_getstr(e->p_id), (void **) &p)) {
                return SEM_EVARNOTDEF;
            }
            memcpy(t, p, sizeof(typeval_t));
        }
        break;
    }

    return SEM_EOK;
}

int _interpret(stmt_t * stmt,
               struct _symbol_tbl_st *sym_g,
               struct _symbol_tbl_st *sym_l,
               struct _variable_tbl_st *var_tbl)
{
    for (; stmt; stmt = stmt->next) {
        switch (stmt->type) {
        case STMT_NOP:
            continue;

        case STMT_RETURN:{
            typeval_t *var = NULL;
            if (!bst_search(sym_l->bst, "return", (void **) &var)) {
                return SEM_EVARNOTDEF;
            }

            CHECKED(_eval
                    (stmt->p_return.expr, sym_g, sym_l, var, var_tbl));
            return SEM_EOK;
        }   break;

        case STMT_WHILE: {
            typeval_t var = { 0 };
            for (;;) {
                CHECKED(_eval
                    (stmt->p_loop.cond, sym_g, sym_l, &var, var_tbl));
                if (var.type != TYPE_BOOLEAN)
                    return SEM_EINVALTYPE;
                if (!var.v_bool)
                    break;

                CHECKED(_interpret(stmt->p_loop.stmt, sym_g, sym_l, var_tbl));
            }
        }   break;

        case STMT_LOOP: {
            typeval_t var = { .type = TYPE_BOOLEAN, .v_bool = true };
            do {
                if (stmt->p_loop.cond) {
                    CHECKED(_eval
                    (stmt->p_loop.cond, sym_g, sym_l, &var, var_tbl));
                    if (var.type != TYPE_BOOLEAN)
                        return SEM_EINVALTYPE;
                }
                CHECKED(_interpret(stmt->p_loop.stmt, sym_g, sym_l, var_tbl));
            } while (var.v_bool);
        }   break;

        case STMT_BRANCH: {
            typeval_t var = { 0 };
            CHECKED(_eval
                (stmt->p_branch.cond, sym_g, sym_l, &var, var_tbl));
            if (var.type != TYPE_BOOLEAN)
                return SEM_EINVALTYPE;

            stmt_t *next_stmt = var.v_bool ? stmt->p_branch.stmt_i : stmt->p_branch.stmt_o;
            CHECKED(_interpret(next_stmt, sym_g, sym_l, var_tbl));
        }   break;

        case STMT_ASSIGN: {
            if (!_not_reserved_name(string_getstr(stmt->p_assign.lhs))) {
                return SEM_ERESERVEDNAME;
            }

            if (bst_search(sym_g->bst, string_getstr(stmt->p_assign.lhs), NULL)) {
                // promenna nemuze mit stejne jmeno jako funkce
                return SEM_EFUNCREDEF;
            }

            struct _variable_tbl_st *tbl =
                _alloc_var_tbl_append(var_tbl);
            if (!tbl) {
                return SEM_ENOMEM;
            }

            typeval_t *var = NULL;
            if (!bst_search
                (sym_l->bst, string_getstr(stmt->p_assign.lhs),
                 (void **) &var)) {

                var = &tbl->typeval;

                if (!bst_insert
                    (&sym_l->bst, string_getstr(stmt->p_assign.lhs),
                     (void *) var)) {
                    return SEM_ENOMEM;
                }
            }

            CHECKED(_eval(stmt->p_assign.rhs, sym_g, sym_l, var, var_tbl));
        }   break;

        case STMT_FUNCTION:
            if (!_not_reserved_name(string_getstr(stmt->p_function.name))) {
                return SEM_ERESERVEDNAME;
            }

            if (bst_search
                (sym_g->bst, string_getstr(stmt->p_function.name), NULL)) {
                // redefinice funkce
                return SEM_EFUNCREDEF;
            }

            // vlozime do tabulky globalnich symbolu
            if (!bst_insert
                (&sym_g->bst, string_getstr(stmt->p_function.name),
                 (void *) &stmt->p_function)) {
                return SEM_ENOMEM;
            }
            break;
        }

    }

    return SEM_EOK;
}

/* ==================================================================== */
/*    P   U   B   L   I   C       I   N   T   E   R   F   A   C   E     */
/* ==================================================================== */
int semantic(ast_t * t)
{
    int retval = SEM_EOK;

    // alokujeme tabulku globalnich symbolu
    if (!_alloc_sym_tbl(&t->sym)) {
        goto return_enomem;
    }

    // alokujeme tabulku pro _main
    if (!_alloc_sym_tbl(&t->sym->next)) {
        goto return_enomem;
    }

    // alokujeme tabulku pro uchovani hodnot promennych
    if (!_alloc_var_tbl(&t->var)) {
        goto return_enomem;
    }

    // vyplnime tabulku globalnich symbolu
    if (!bst_insert(&t->sym->bst, "_main", t->sym->next)) {
        goto return_enomem;
    }

#define X(a,b) \
    if (!bst_insert(&t->sym->bst, (b), NULL)) { \
        goto return_enomem; \
    }
#include "builtin.def"
    X_BUILTIN_FN
#undef X
        // naplnime lokalni tabulku _main
        retval = _interpret(t->stmt, t->sym, t->sym->next, t->var);

  cleanup:
    return retval;

  return_enomem:
    retval = SEM_ENOMEM;
    goto cleanup;

  return_reserved_name:
    retval = SEM_ERESERVEDNAME;
    goto cleanup;
}
