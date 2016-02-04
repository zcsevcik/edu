/*
 * io.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-04-06
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU2 2).
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "io.h"

/* ==================================================================== */
int fgetw(char *s, int max, FILE *f)
{
    if (!s || max <= 0) return 0;
    if (!f) return EOF;

    char fmt[32];
    snprintf(fmt, sizeof fmt, "%%%ds%%*[^ \n\t\v\f\r]", max - 1);

    int result = fscanf(f, fmt, s);
    if (result == 1) return strlen(s);
    else             return result;
}

/* ==================================================================== */
