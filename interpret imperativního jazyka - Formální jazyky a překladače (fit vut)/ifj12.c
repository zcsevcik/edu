/*
 * ifj12.c:        Implementace interpretu imperativniho jazyka IFJ12
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
# include <config.h>
#endif

#include <assert.h>
#include <getopt.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "parser.h"
#include "semantic.h"
#include "syntree.h"


#define IFJ_ARGOK   0
#define IFJ_ARGERR  1
#define IFJ_ARGHELP 2


static int parseargs(int, char **, char **);
static void usage(FILE *);
static void verbose(int, const char *);
int main(int, char *[]);

extern void _string_collect();

int parseargs(int argc, char **argv, char **srcfile)
{
    assert(argc >= 0);
    assert(argv != NULL);
    assert(srcfile != NULL);

    int opt;
    int getopt_optind = 1;
    const struct option getopt_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {"debug", no_argument, &_ifj_log_lvl, 10},
        {0, 0, 0, 0}
    };

    opterr = 1;

    while ((opt = getopt_long(argc, argv, "h",
                              getopt_opts, &getopt_optind)) != -1) {
        switch (opt) {
        case 0:
            continue;

        default:
            return IFJ_ARGERR;

        case 'h':
            return IFJ_ARGHELP;
        }
    }

    *srcfile = argv[optind++];
    if (!*srcfile) {
        return IFJ_ARGERR;
    }
    if (optind != argc) {
        return IFJ_ARGERR;
    }

    return IFJ_ARGOK;
}

void usage(FILE * f)
{
    assert(f != NULL);

    fprintf(f,
            "Implementace interpretu imperativniho jazyka IFJ12\n"
            "Projekt do predmetu IFJ/2012\n"
            "\n"
            "Pouziti: %s SOUBOR\n"
            "\n"
            "Daniel Berek <xberek00@stud.fit.vutbr.cz>\n"
            "Dusan Fodor  <xfodor01@stud.fit.vutbr.cz>\n"
            "Pavol Jurcik <xjurci06@stud.fit.vutbr.cz>\n"
            "Peter Pritel <xprite01@stud.fit.vutbr.cz>\n"
            "Radek Sevcik <xsevci44@stud.fit.vutbr.cz>\n", program_name);
}

void verbose(int level, const char *msg)
{
    if (level == 1) {
        fprintf(stderr, "%s\n", msg);
    }
    else {
        fprintf(stderr, "##%d %s\n", level, msg);
    }
}

int main(int argc, char *argv[])
{
    int err_code = IFJ_EOK;

    // ulozi jmeno programu
    program_name = basename(argv[0]);

    // nastavi chybove hlasky
    _ifj_log_lvl = 1;
    _ifj_log_fn = &verbose;

    // nastavi systemove prostredi
    setlocale(LC_ALL, "");
    c_locale = newlocale(LC_ALL_MASK, "C", NULL);

    // parsne argumenty
    switch (parseargs(argc, argv, &src_name)) {
    case IFJ_ARGOK:
        break;

    case IFJ_ARGERR:
        usage(stderr);
        err_code = IFJ_EINTERNAL;
        goto cleanup;

    case IFJ_ARGHELP:
        usage(stdout);
        err_code = IFJ_EOK;
        goto cleanup;
    }

    // otevre zdrojovy soubor
    src_f = fopen(src_name, "r");
    if (!src_f) {
        _ifj_perror(src_name);
        err_code = IFJ_EINTERNAL;
        goto cleanup;
    }

    // udela syntaktickou analyzu
    ast_t ast;
    memset(&ast, '\0', sizeof(ast));

    int parse_hr = parse(&ast);
    if (parse_hr != PARSE_EOK) {
        switch (parse_hr) {
        case PARSE_ELEXICAL:
            err_code = IFJ_ELEXICAL;
            break;

        case PARSE_ESYNTAX:
        case PARSE_EEXPR:
            err_code = IFJ_ESYNTAX;
            break;

        default:
        case PARSE_ENOMEM:
            err_code = IFJ_EINTERNAL;
            break;
        }

        goto cleanup_ast;
    }

    // udela semantickou analyzu
    int sem_hr = semantic(&ast);
    if (sem_hr != SEM_EOK) {
        switch (sem_hr) {
        default:
            err_code = IFJ_ERUNTIME;
            break;
        case SEM_ERESERVEDNAME:
            _ifj_error_log("Chyba: Pouzito rezervovane jmeno.");
            err_code = IFJ_ESEMANTIC;
            break;
        case SEM_ECONVNUM:
            _ifj_error_log("Chyba pri pretypovani na cislo.");
            err_code = IFJ_ECAST;
            break;
        case SEM_EINVALTYPE:
            _ifj_error_log("Chyba: Pouzita operace nad nekompatibilnimi typy.");
            err_code = IFJ_EINCOMPTYPE;
            break;
        case SEM_ESUBSTR:
            _ifj_error_log("Chyba: Nekorektni operace vyberu podretezce.");
            err_code = IFJ_ESEMANTIC;
            break;
        case SEM_ENOMEM:
            _ifj_error_log("Chyba: %s.", strerror(ENOMEM));
            err_code = IFJ_EINTERNAL;
            break;
        case SEM_EVARNOTDEF:
            _ifj_error_log("Chyba: Pouzita nedefinovana promenna.");
            err_code = IFJ_EUNDEFVAR;
            break;
        case SEM_EFUNCNOTDEF:
            _ifj_error_log("Chyba: Pouzita nedefinovana funkce.");
            err_code = IFJ_EUNDEFFUNC;
            break;
        case SEM_EDIVZERO:
            _ifj_error_log("Chyba: Deleni nulou.");
            err_code = IFJ_EDIVBYZERO;
            break;
        case SEM_EFUNCREDEF:
            _ifj_error_log("Chyba: Redefinice funkce.");
            err_code = IFJ_ERUNTIME;
            break;
        case SEM_EPARAM:
            _ifj_error_log("Chyba: Chybny pocet argumentu pri volani funkce.");
            err_code = IFJ_ERUNTIME;
            break;
        case SEM_ERANGE:
            _ifj_error_log("Chyba: Cislo je mimo rozsah.");
            err_code = IFJ_ECAST;
            break;
        }

        goto cleanup_ast;
    }

    // zinterpretujeme nas program

    // uklidime po sobe
  cleanup_ast:
    stmt_delete_all(ast.stmt);
    _free_sym_tbl(ast.sym);
    _free_var_tbl(ast.var);
    _string_collect();

  //cleanup_close:
    if (0 != fclose(src_f)) {
        _ifj_perror(src_name);
    }

  cleanup:
    freelocale(c_locale);

    return err_code;
}
