/*
 * fold.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-04-06
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU2 1a).
 */

/* ==================================================================== */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* ==================================================================== */
#define WORD_LENGTH 4096
#define str(x) # x
#define xstr(x) str(x)

static int width = 80;
static int cut = 0;
static char filename[FILENAME_MAX] = { 0 };

/* ==================================================================== */
static int process_file(FILE *fp)
{
    char word[WORD_LENGTH];
    int cursor = 0;
    int first_cut = 1;

    while (EOF != fscanf(fp, "%" xstr(WORD_LENGTH) "s", word)) {
        int wordlen = (int)strlen(word);
        if (cut && wordlen > width) {
            wordlen = width;

            if (first_cut) {
                fprintf(stderr, "slovo bylo oriznuto\n");
                first_cut = 0;
            }
        }

        if (cursor + wordlen + 1 > width) {
            printf("\n");
            cursor = 0;
        }
        else if (cursor != 0) {
            printf(" ");
            ++cursor;
        }

        printf("%.*s", wordlen, word);
        cursor += wordlen;

        int ch_ = '\0';
        int ch;
        while ((ch = fgetc(fp)) != EOF) {
            if (ch == '\n' && ch_ == '\n') {
                if (cursor != 0) printf("\n");
                printf("\n");
                cursor = 0;
            }
            else if (!isspace(ch)) {
                ungetc(ch, fp);
                break;
            }

            ch_ = ch;
        }
    }

    printf("\n");

    if (ferror(fp)) {
        fprintf(stderr, "chyba pri cteni\n");
        return 1;
    }

    return 0;
}

/* ==================================================================== */
static int parse_cmdargs(int argc, char *argv[])
{
    while (--argc) {
        const char *p = *(++argv);

        if (strcmp(p, "-c") == 0) {
            cut = 1;
        }
        else if (strcmp(p, "-w") == 0) {
            if (--argc == 0) return 1;
            if (1 != sscanf(*(++argv), "%d", &width)) { return 1; }
            if (width <= 0 || width > WORD_LENGTH) { return 1; }
        }
        else if (strncmp(p, "-", 1) == 0) {
            return 2;
        }
        else {
            break;
        }
    }

    /* ================================================================ */
    if (argc == 0) {
        return 0;               // read from stdin
    }
    else if (argc == 1) {
        strncpy(filename, *argv, sizeof filename);// read from file
        filename[sizeof filename - 1] = '\0';
        return 0;
    }
    else {
        return 2;
    }
}

/* ==================================================================== */
int main(int argc, char *argv[])
{
    switch (parse_cmdargs(argc, argv)) {
    /* ================================================================ */
    case 0:
        break;
    /* ================================================================ */
    default:
        return 1;
    /* ================================================================ */
    case 1:
        fprintf(stderr, "chyba pri zadani parametru -w\n");
        return 1;
    /* ================================================================ */
    case 2:
        fprintf(stderr, "chyba, neznamy parametr\n");
        return 1;
    /* ================================================================ */
    }

    FILE *fp;
    if (*filename) {
        fp = fopen(filename, "r");
        if (!fp) {
            perror(filename);
            return 1;
        }
    }
    else fp = stdin;

    /* ================================================================ */
    process_file(fp);

    /* ================================================================ */
    if (fp != stdin) {
        if (EOF == fclose(fp))
            perror(filename);
    }

    return 0;
}
