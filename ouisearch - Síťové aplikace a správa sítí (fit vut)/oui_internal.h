/*
 * oui_internal.h: Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *                 
 * Date:           2012/10/14
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifndef OUI_INTERNAL_H
#define OUI_INTERNAL_H

#include <stdarg.h>
#include <stdbool.h>

#define EOK         0
#define EHELP       1
#define EARGNET     2
#define EARGOUI     3
#define EINVALARG   4

#if defined(__GNUC__)
# define __MYFUNCTION__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
# define __MYFUNCTION__ __FUNCSIG__
#else // C99
# define __MYFUNCTION__ __func__
#endif

#ifndef __cplusplus
# define _oui_verbose(...) _oui_log(__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*oui_dbg_fn)(const char*);

extern volatile bool _terminate;

bool _oui_debug();
void _oui_set_dbg(bool);
void _oui_set_dbg_fn(oui_dbg_fn);

void
_oui_log(
        const char* fmt,
        ...)
#ifdef __GNUC__
  __attribute__ ((format (printf, 1, 2)));
#else
  ;
#endif

#ifdef __cplusplus
}
#endif

#endif //OUI_INTERNAL_H
