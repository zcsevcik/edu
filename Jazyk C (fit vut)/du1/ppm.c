/*
 * ppm.c
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

#include "ppm.h"
#include "error.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================================================================== */
#define PPM_MAGIC "P6"

/* ==================================================================== */
struct ppm *ppm_read(const char *filename)
{
    struct ppm *p = NULL;
    int result;

    /* ==== I N I T =================================================== */
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        Warning("\"%s\" %s", filename, strerror(errno));
        goto l_error;
    }

    /* ==== M A G I C ================================================= */
    char magic_number[3];
    result = fscanf(fp, "%2s ", magic_number);
    if (result != 1) {
        Warning("chyba cteni magic_number");
        goto l_error;
    }

    if (0 != strncmp(magic_number, PPM_MAGIC, 2)) {
        Warning("neni PPM");
        goto l_error;
    }

    /* ==== X S I Z E ================================================= */
    unsigned width;
    result = fscanf(fp, "%u ", &width);
    if (result != 1) {
        Warning("chyba cteni xsizetxt");
        goto l_error;
    }

    /* ==== Y S I Z E ================================================= */
    unsigned height;
    result = fscanf(fp, "%u ", &height);
    if (result != 1) {
        Warning("chyba cteni ysizetxt");
        goto l_error;
    }

    /* ==== C O L O R ================================================= */
    unsigned color;
    result = fscanf(fp, "%u", &color);
    if (result != 1) {
        Warning("chyba cteni color_max");
        goto l_error;
    }

    if (color != 255) {
        Warning("color_max: %d je nepodporovano", color);
        goto l_error;
    }

    int ws = fgetc(fp);
    if (!isspace(ws)) {
        Warning("ocekavan whitespace");
        goto l_error;
    }

    /* ==== R A S T E R =============================================== */
    const size_t datalen = 3 * width * height;
    p = malloc(sizeof(struct ppm) + datalen);
    if (!p) {
        Warning("%s", strerror(errno));
        goto l_error;
    }

    p->xsize = width;
    p->ysize = height;
    result = fread(p->data, sizeof(char), datalen, fp);
    if (datalen != (size_t)result) {
        Warning("neocekavany konec dat");
        goto l_error;
    }

    /* ==== E O F ===================================================== */
    if (EOF != fgetc(fp) && !feof(fp)) {
        Warning("nedosazeno konce souboru");
        goto l_error;
    }

    /* ==== D O N E =================================================== */
    if (fp) { fclose(fp); }
    return p;

    /* ==== E R R O R ================================================= */
l_error:
    if (fp) { fclose(fp); }
    if (p) { free(p); }
    return NULL;
}

/* ==================================================================== */
int ppm_write(struct ppm *p, const char *filename)
{
    if (!p) {
        Warning("prazdna struktura");
        return -1;
    }

    int result;
    int written = 0;

    /* ==== I N I T =================================================== */
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        Warning("\"%s\" %s", filename, strerror(errno));
        goto l_error;
    }

    /* ==== M A G I C ================================================= */
    result = fprintf(fp, "%.2s\n", PPM_MAGIC);
    if (result < 0) {
        Warning("nelze zapsat magic_number");
        goto l_error;
    }
    written += result;

    /* ==== S I Z E =================================================== */
    result = fprintf(fp, "%u %u\n", p->xsize, p->ysize);
    if (result < 0) {
        Warning("nelze zapsat size");
        goto l_error;
    }
    written += result;

    /* ==== C O L O R ================================================= */
    result = fprintf(fp, "255\n");
    if (result < 0) {
        Warning("nelze zapsat ysize");
        goto l_error;
    }
    written += result;

    /* ==== R A S T E R =============================================== */
    const size_t datalen = 3 * p->xsize * p->ysize;
    result = fwrite(p->data, sizeof(char), datalen, fp);
    if (datalen != (size_t)result) {
        Warning("nelze zapsat rastr");
        goto l_error;
    }
    written += result;

    /* ==== D O N E =================================================== */
    if (fp) { fclose(fp); }
    return written;

    /* ==== E R R O R ================================================= */
l_error:
    if (fp) { fclose(fp); }
    return -1;
}
