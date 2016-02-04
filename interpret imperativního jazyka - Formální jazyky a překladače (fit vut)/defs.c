/*
 * defs.c:         Implementace interpretu imperativniho jazyka IFJ12
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
#include <config.h>
#endif

#include <assert.h>
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

char           *program_name;
char           *src_name;
FILE           *src_f;

locale_t        c_locale;

int             _ifj_log_lvl;
ifj_log_fn      _ifj_log_fn;

void
_ifj_log(int level, const char *fmt, ...)
{
    va_list         args;
    char           *str = NULL;
    int             ret = 0;

    if (!_ifj_log_fn) {
        return;
    }

    va_start(args, fmt);
    ret = vasprintf(&str, fmt, args);

    assert(str != NULL);
    assert(ret > 0);

    va_end(args);

    if (ret > 0) {
        _ifj_log_fn(level, str);
    }

    free(str);
}
