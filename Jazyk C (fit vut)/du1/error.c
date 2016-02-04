/*
 * error.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-02-26
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU1 b).
 */

#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* ==================================================================== */
void Warning(const char *fmt, ...)
{
    fprintf(stderr, "CHYBA: ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

/* ==================================================================== */
void FatalError(const char *fmt, ...)
{
    fprintf(stderr, "CHYBA: ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    exit(1);
}

