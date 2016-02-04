/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <alloca.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

int _ipk_log_level;
ipk_log_fn _ipk_log_fn;

void ipk_set_log_level(int level)
{ _ipk_log_level = level; }

void ipk_set_log_func(ipk_log_fn fn)
{ _ipk_log_fn = fn; }

void _ipk_log(int level, const char* fmt, ...)
{  
    va_list args;
    char *str = NULL;
    int ret = 0;
    
    if (_ipk_log_fn == NULL)
        return;
    
    va_start(args, fmt);
    
#ifdef _MSC_VER
    str = (char*)alloca((_vscprintf(fmt, args) + 1) * sizeof(char));
    ret = vsprintf(str, fmt, args);
#elif HAVE_VASPRINTF
    ret = vasprintf(&str, fmt, args);
#else
    str = (char*)alloca(4096 * sizeof(char));
    ret = vsprintf(str, fmt, args);
#endif
    
    assert(str != NULL);
    assert(ret > 0);

    va_end(args);
    
    if (ret > 0) {
        _ipk_log_fn(level, str);
    }

#if !defined(_MSC_VER) && defined(HAVE_VASPRINTF)
    free(str);
#endif
}


void ipk_perror(int error)
{
    if (error == 0)
        return;

    char* errmsg = strerror(error);

    if (errmsg) {
        _ipk_error_log("Chyba #%d: %s", error, errmsg);
    }
    else {
        _ipk_error_log("Chyba #%d", error);
    }
}

#ifdef _WIN32
void ipk_perror_win(unsigned long error)
{
    if (error == 0)
        return;

    char* errmsg = NULL;

    unsigned long hr = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        0U, error, LANG_NEUTRAL, (char *)&errmsg, 0U, NULL);
    assert(hr > 0);

    if (errmsg) {
        _ipk_error_log("Chyba #%d: %s", error, errmsg);
    }
    else {
        _ipk_error_log("Chyba #%d", error);
    }

    LocalFree(errmsg);
}
#endif
