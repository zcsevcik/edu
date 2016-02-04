/*
 * wordcount.c
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include "htable.h"
#include "io.h"

/* prvocislo; slusna prumerna delka synonym pro seq 1000000 2000000 */
#define HTAB_SIZE 100003

#define WORD_BUFFER_SIZE 128

/* ==================================================================== */
static htab_t *t = NULL;

/* ==================================================================== */
static void clean_table()
{
    htab_free(t);
}

/* ==================================================================== */
/* _Noreturn */ static void exit_w_enomem()
{
    fprintf(stderr, "%s\n", strerror(ENOMEM));
    exit(1);
}

/* ==================================================================== */
static void print_table(const char *key, void *data)
{
    printf("%s\t%" PRIuPTR "\n", key, (uintptr_t)data);
}

/* ==================================================================== */
static unsigned fill_table(htab_t *t, FILE *fp)
{
    static unsigned words_cut = 0;
    unsigned words = 0;

    int result = EOF;
    char buf[WORD_BUFFER_SIZE];
    while ((result = fgetw(buf, sizeof(buf), fp)) != EOF) {
        if (result == sizeof buf - 1 && ++words_cut == 1)
            fputs("slovo zkraceno\n", stderr);

        htab_listitem *item = htab_lookup(t, buf);
        if (!item) exit_w_enomem();

        uintptr_t *data = (uintptr_t *)&item->data;
        (*data)++;

        ++words;
    }

    return words;
}

/* ==================================================================== */
int main(void)
{
    if (!!atexit(&clean_table)) {
        fprintf(stderr, "cannot set exit function\n");
    }

    if (!(t = htab_init(HTAB_SIZE)))
        exit_w_enomem();

    fill_table(t, stdin);
    htab_foreach(t, &print_table);

#ifndef NDEBUG
    htab_statistics(t);
#endif

    return 0;
}
