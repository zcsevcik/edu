/*
 * oui_internal.c: Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/18
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "oui_internal.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------
static bool _oui_dbg;
static oui_dbg_fn _oui_dbg_fn;
volatile bool _terminate;

// ----------------------------------------------------------------------------
bool _oui_debug()
{ return _oui_dbg; }

void _oui_set_dbg(bool value)
{ _oui_dbg = value; }

void _oui_set_dbg_fn(oui_dbg_fn value)
{ _oui_dbg_fn = value; }

void _oui_log(const char* fmt, ...)
{
    va_list args;
    char *str = NULL;
    int ret = 0;

    if (!_oui_dbg)
        return;
    if (_oui_dbg_fn == NULL)
        return;

    va_start(args, fmt);
    ret = vasprintf(&str, fmt, args);

    assert(str != NULL);
    assert(ret > 0);

    va_end(args);

    if (ret > 0) {
        _oui_dbg_fn(str);
    }

    free(str);
}
