/*
 * ouidb.c:        Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/14
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ouidb.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUILISTGROWSIZE 10

// ----------------------------------------------------------------------------
struct ouilist {
    uint8_t key[3];
    char* value;
};

// ----------------------------------------------------------------------------
static struct ouilist* _ouilist;
static size_t _nouilist;
static size_t _alloced;

// ----------------------------------------------------------------------------
void ouidb_cleanup()
{
    for (size_t i = 0; i < _nouilist; ++i) {
        free(_ouilist[i].value);
    }

    free(_ouilist);

    _ouilist = NULL;
    _nouilist = 0;
    _alloced = 0;
}

bool ouidb_init(const char* filename)
{
    FILE *fp;
    size_t lineno = 0;
    char buf[255];
    char *p, *key;
    char *tail;

    _nouilist = 0;
    _alloced = 0;

    fp = fopen(filename, "r");
    if (!fp) {
        perror(filename);
        return false;
    }

    while (fgets(buf, _countof(buf), fp)) {
        ++lineno;

        // strip new lines
        for (p = &buf[strlen(buf) - 1]; *p == '\r' || *p == '\n'; --p) { *p = '\0'; }

        // strip leading spaces
        for (p = buf; *p && isspace(*p); ++p);

        // skip comments and empty lines
        if (!*p || *p == '#') { continue; }

        key = p;

        while (*p && (isxdigit(*p) || *p == ':')) {
            ++p;
        }

        if (*p && !isspace(*p)) {
            fprintf(stderr, "Syntakticka chyba v souboru %s:%zu.\n", filename, lineno);
            return false;
        }
        *p++ = '\0';

        while (*p && isspace(*p)) { ++p; }

        // strip trailing spaces
        tail = p + strlen(p) - 1;
        while (tail > p && isspace(*tail)) {
            *tail-- = '\0';
        }

        // reallocate
        if (_nouilist == _alloced) {
            _alloced += OUILISTGROWSIZE;
            _ouilist = (struct ouilist*)realloc(_ouilist,
                _alloced * sizeof(struct ouilist));

            if (!_ouilist) {
                fprintf(stderr, "%s\n", strerror(errno));
                return false;
            }
        }

        // save description
        if (*p) {
            _ouilist[_nouilist].value = strdup(p);
            if (!_ouilist[_nouilist].value) {
                fprintf(stderr, "%s\n", strerror(errno));
                return false;
            }
        }
        else {
            _ouilist[_nouilist].value = NULL;
        }

        // tokenize oui
        p = strtok(key, ":");
        for (size_t i = 0; i < 3; ++i) {
            if (!p) {
                fprintf(stderr, "Syntakticka chyba v souboru %s:%zu.\n", filename, lineno);
                return false;
            }
            long val = strtol(p, NULL, 16);
            if (errno == ERANGE || val < 0 || val > UINT8_MAX) {
                fprintf(stderr, "%s %s:%zu.\n", strerror(ERANGE), filename, lineno);
                return false;
            }
            _ouilist[_nouilist].key[i] = (uint8_t)val;

            p = strtok(NULL, ":");
        }

        if (strtok(NULL, ":")) {
            fprintf(stderr, "Syntakticka chyba v souboru %s:%zu.\n", filename, lineno);
            return false;
        }

        // parse successful
        ++_nouilist;
    }

    fclose(fp);
    return true;
}

static const char* _ouidb_search(uint8_t mac[6])
{
    for (size_t i = 0; i < _nouilist; ++i) {
        struct ouilist* curr = &_ouilist[i];
        if (0 != memcmp(curr->key, mac, 3 * sizeof(uint8_t))) {
            continue;
        }

        return curr->value;
    }

    return NULL;
}

const char* ouidb_search(struct in6_addr eui64)
{
    uint8_t mac[6] = {
        eui64.s6_addr[8] ^ 0x02,
        eui64.s6_addr[9],
        eui64.s6_addr[10],
        eui64.s6_addr[13],
        eui64.s6_addr[14],
        eui64.s6_addr[15]
    };

    return _ouidb_search(mac);
}

void ouidb_foreach(ouidb_foreach_fn callback, void* obj)
{
    if (!callback) {
        return;
    }

    for (size_t i = 0; i < _nouilist; ++i) {
        struct ouilist* curr = &_ouilist[i];
        callback(curr->key, curr->value, obj);
    }

    return;
}
