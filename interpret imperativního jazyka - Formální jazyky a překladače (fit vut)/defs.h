/*
 * defs.h:         Implementace interpretu imperativniho jazyka IFJ12
 *
 * Team:           Daniel Berek <xberek00@stud.fit.vutbr.cz>
 *                 Dusan Fodor  <xfodor01@stud.fit.vutbr.cz>
 *                 Pavol Jurcik <xjurci06@stud.fit.vutbr.cz>
 *                 Peter Pritel <xprite01@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           Sun, 2 Dec 2012 16:01:29 +0100
 *
 * This file is part of ifj12_b2i.
 */

#ifndef DEFS_H
#define DEFS_H

#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define IFJ_EOK         0   /**< bez chyb */
#define IFJ_ELEXICAL    1   /**< chyba lexikalni analyzy */
#define IFJ_ESYNTAX     2   /**< chyba syntakticke analyzy */
#define IFJ_EUNDEFVAR   3   /**< nedefinovana promenna */
#define IFJ_EUNDEFFUNC  4   /**< nedefinovana funkce */
#define IFJ_ESEMANTIC   5   /**< blize nespecifikovana semanticka chyba */
#define IFJ_EDIVBYZERO  10  /**< deleni nulou */
#define IFJ_EINCOMPTYPE  11 /**< nekompatibilni typy */
#define IFJ_ECAST       12  /**< chyba pretypovani na cislo */
#define IFJ_ERUNTIME    13  /**< blize nespecifikovana behova chyba */
#define IFJ_EINTERNAL   99  /**< interni chyba */

#if defined(__GNUC__)
#define __MYFUNCTION__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define __MYFUNCTION__ __FUNCSIG__
#else                           // C99
#define __MYFUNCTION__ __func__
#endif

#define _ifj_assert() \
    _ifj_debug_log("ASSERT\n\n%s at %s:%d", \
                   __MYFUNCTION__, __FILE__, __LINE__)

#define _ifj_assert_if(x) do { \
    if (!(x)) \
        _ifj_debug_log("ASSERT\n\n%s at %s:%d", \
                       __MYFUNCTION__, __FILE__, __LINE__); \
} while (0)

#define _ifj_assert_val(x) \
    _ifj_debug_log("ASSERT(#%d)\n\n%s at %s:%d\n", \
                   (int)(x), __MYFUNCTION__, __FILE__, __LINE__)

#define _ifj_perror(s) \
    _ifj_error_log("%s: %s", \
                   (s), strerror(errno))


#define _ifj_error_log(...) LEVEL(1, __VA_ARGS__)
#define _ifj_debug_log(...) LEVEL(2, __VA_ARGS__)
#define _ifj_lexical_log(...) LEVEL(3, __VA_ARGS__)
#define _ifj_syntax_log(...) LEVEL(4, __VA_ARGS__)
#define _ifj_semantic_log(...) LEVEL(5, __VA_ARGS__)
#define _ifj_interpret_log(...) LEVEL(6, __VA_ARGS__)

#define LEVEL(l, ...) do { \
    if (_ifj_log_lvl >= l || _ifj_log_lvl > 9) \
        _ifj_log(l, __VA_ARGS__); \
} while (0)

#define LEVEL_EQ(l, ...) do { \
    if (_ifj_log_lvl == l || _ifj_log_lvl > 9) \
        _ifj_log(l, __VA_ARGS__); \
} while (0)

#ifdef __cplusplus
extern          "C" {
#endif

    extern char    *program_name;
    extern FILE    *src_f;
    extern char    *src_name;

    extern locale_t c_locale;

    typedef void    (*ifj_log_fn) (int level, const char *);
    extern int      _ifj_log_lvl;
    extern ifj_log_fn _ifj_log_fn;


    void            _ifj_log(int level, const char *fmt, ...)
#ifdef __GNUC__
        __attribute__ ((format(printf, 2, 3)));
#else
                   ;
#endif

#ifdef __cplusplus
}
#endif
#endif                          // DEFS_H
