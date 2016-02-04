/*
 * Soubor:  proj2.c
 * Datum:   2010/10/21
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program pro vypocet hyperbolickeho tangens, obecneho logaritmu,
 *          vazeneho aritmetickeho a kvadratickeho prumeru za pomoci zakladnich
 *          matematickych operaci a standartniho vstupu/vystupu.
 */

#include "proj2.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// return EXIT_FAILURE; pokud funkce nevratila E_OK
#define CHECK_ERROR(err) { \
    if (err != E_OK) { \
        printError(err); \
        return EXIT_FAILURE; \
    } \
}

#define RETURN_IOFMT_ERROR(fin, fout) { \
    if (ferror(fin) || ferror(fout)) { \
        return E_IOFMT; \
    } \
}

// funkce nacte ze streamu fin v cyklu hodnoty typu double a na stream fout
// vypise na samostatny radek hyperbolicky tangens nactene hodnoty s presnosti
// epsylon, ktera je zadana parametrem eps
// vraci E_IOFMT pokud se vyskytl na vstupu neocekavany znak nebo pokud nastala
// chyba ferror na vstupu ci vystupu; jinak vrati E_OK
Error computeTanh(FILE* fin, FILE* fout, double eps)
{
    assert(fin != NULL);
    assert(fout != NULL);
    assert(eps > 0);

    while (!feof(fin) && !ferror(fin)) {
        double tempValue;
        int result = fscanf(fin, "%lf",
            &tempValue);
        RETURN_IOFMT_ERROR(fin, fout);

        if (result == 1) {
#ifdef DEBUG
            fprintf(fout, "%.5e", tempValue);
            fprintf(fout, "\t%.10e", tanh(tempValue));
            fprintf(fout, "\t%.10e", izp_tanh(tempValue, eps));
            fprintf(fout, "\n");
#else
            fprintf(fout, "%.10e\n", izp_tanh(tempValue, eps));
#endif
        }
        else {
            return feof(fin) ? E_OK : E_IOFMT;
        }
    }

    return E_OK;
}

// funkce nacte ze streamu fin v cyklu hodnoty typu double a na stream fout
// vypise na samostatny radek logaritmus nactenych hodnot o zakladu zadanem
// parametrem logBase s presnosti epsylon, ktera je predana parametrem eps
// vraci E_IOFMT pokud se vyskytl na vstupu neocekavany znak nebo pokud nastala
// chyba ferror na vstupu ci vystupu; jinak vrati E_OK
Error computeLog(FILE* fin, FILE* fout, double logBase, double eps)
{
    assert(fin != NULL);
    assert(fout != NULL);
    assert(eps > 0);

    //fprintf(fout, "readed\t\t\tmath.h\t\t\tmyimpl\n");
    while (!feof(fin) && !ferror(fin)) {
        double tempValue;
        int result = fscanf(fin, "%lf",
            &tempValue);
        RETURN_IOFMT_ERROR(fin, fout);

        if (result == 1) {
#ifdef DEBUG
            fprintf(fout, "%.5e", tempValue);
            fprintf(fout, "\t%.10e", log(tempValue) / log(logBase));
            fprintf(fout, "\t%.10e", izp_log(tempValue, logBase, eps));
            fprintf(fout, "\n");
#else
            fprintf(fout, "%.10e\n", izp_log(tempValue, logBase, eps));
#endif
        }
        else {
            return feof(fin) ? E_OK : E_IOFMT;
        }
    }

    return E_OK;
}

// funkce pro vypocet presnosti epsylon, ktera se pocita z poctu platnych mist
// zadanych v parametru sigdig. rozsah hodnoty sigdig je 1..DBL_DIG
// vrati 1e-sigdig, protoze funkce pouzite pro vypocet zacinaji vzdy 1.
double sigdigToEpsylon(unsigned long sigdig)
{
    assert(sigdig > 0);
    assert(sigdig <= DBL_DIG);
    double eps = 1;

    for (unsigned long i = 1; i < sigdig; i++) {
        eps *= 0.1f;
    }

    return eps;
}

// funkce nacte ze streamu fin v cyklu dvojice hodnot typu double a na stream fout
// vypisuje prumernou hodnotu funkci uvedenou parametrem weightedMean
// prvni hodnota dvojice je prvkem posloupnosti z niz se dany pruemr pocita
// a druha hodnota je nezaporna vaha tohoto prvku
// skonci-li posloupnost predcasne, poslednim vysledkem bude NAN
// pokud se na vstupu objevi zaporna vaha prvku, poslednim vysledkem bude NAN
// vraci E_NOT_IMPLEMENTED pokud byl zadan neplatny parametr weightedMean
// vraci E_IOFMT pokud se vyskytl na vstupu neocekavany znak nebo pokud nastala
// chyba ferror na vstupu ci vystupu; jinak vrati E_OK
Error computeWeightedMean(FILE* fin, FILE* fout, WeightedMean weightedMean)
{
    assert(fin != NULL);
    assert(fout != NULL);

    WEIGHTED_MEAN_FN fnWeightedMean = NULL;

    switch (weightedMean) {
        case WM_ARITHMETIC:
            fnWeightedMean = &izp_wam;
            break;

        case WM_QUADRATIC:
            fnWeightedMean = &izp_wqm;
            break;

        default:
            return E_NOT_IMPLEMENTED;
    }

    WeightedValue sumValue = { .weight = .0, .value = .0 };

    while (!feof(fin) && !ferror(fin)) {
        WeightedValue tempValue;
        int result = fscanf(fin, "%lf %lf",
            &tempValue.value,
            &tempValue.weight);
        RETURN_IOFMT_ERROR(fin, fout);

        if (result == 2) {
            double sum = fnWeightedMean(&sumValue, &tempValue);
            fprintf(fout, "%g\n", sum);
            RETURN_IOFMT_ERROR(fin, fout);

            if (sum == NAN) {
                return E_OK;
            }
        }
        else if (result == 1) {
            // Skonci-li vstupni posloupnost predcasne,
            // bude poslednim vysledkem hodnota NAN.
            if (feof(fin)) {
                fprintf(fout, "%g\n", NAN);
                RETURN_IOFMT_ERROR(fin, fout);
                return E_OK;
            }
            else {
                return E_IOFMT;
            }
        }
        else {
            // pokud nenastalo EOF tak je neocekavany znak na vstupu..
            return feof(fin) ? E_OK : E_IOFMT;
        }
    }
    
    return E_OK;
}

// Vytiskne napovedu
void printHelp()
{
    printf(
        "Iteracni vypocty, projekt c. 2 pro predmet IZP.\n"
        "\n"
        "Program pro vypocet hyperbolickeho tangens, obecneho logaritmu,\n"
        "vazeneho aritmetickeho a kvadratickeho prumeru za pomoci zakladnich\n"
        "matematickych operaci a standartniho vstupu/vystupu.\n"
        "Autor: Radek Sevcik (c) 2010\n"
        "\n"
        "Pouziti: proj2 -h\n"
        "         proj2 --logax <sigdig> <a>\n"
        "         proj2 --tanh <sigdig>\n"
        "         proj2 --wam\n"
        "         proj2 --wqm\n"
        "\n"
        "Popis parametru:\n"
        "  -h                     Vypise tuto napovedu.\n"
        "  --logax <sigdig> <a>   Spocita logaritmus o zaklade a s presnosti na\n"
        "                         sigdig cislic\n"
        "  --tanh <sigdig>        Spocita hyperbolicky tangens s presnosti na\n"
        "                         sigdig cislic\n"
        "  --wam                  Vypise prubezne hodnoty vazeneho aritmetickeho prumeru\n"
        "                         z jiz nactene posloupnosti hodnot\n"
        "  --wqm                  Vypise prubezne hodnoty vazeneho kvadratickeho prumeru\n"
        "                         z jiz nactene posloupnosti hodnot\n"
        "\n"
        "Presnost sigdig <1..%d>\n", DBL_DIG);
}

// Vytiskne chybu
void printError(Error err)
{
    switch (err) {
        case E_NOT_IMPLEMENTED:
            fprintf(stderr, "Operace neni implementovana.\n");
            break;

        case E_TOO_MANY_ARGS:
            fprintf(stderr, "Zadali jste prilis mnoho parametru.\n");
            break;

        case E_UNKNOWN_PARAM:
            fprintf(stderr, "Neznamy prikaz.\n");
            break;

        case E_INVALID_PARAM:
            fprintf(stderr, "Neplatna hodnota parametru sigdig nebo zaklad logaritmu.\n");
            break;

        case E_IOFMT:
            fprintf(stderr, "Nastala chyba pri zpracovani vstupu/vystupu.\n");
            break;

        case E_OK:
            fprintf(stderr, "OK.\n");
            break;

        default:
            fprintf(stderr, "Neznama chyba.\n");
            break;
    }
}

// parsuje parametry prikazove radky do struktury ProgParams
Error parseParams(int argc, char* argv[], ProgParams* const params)
{
    assert(argc >= 1);
    assert(argv != NULL);
    assert(params != NULL);

    // unsigned long int strtoul (const char* str, char** endptr, int base);
    // On success, the function returns the converted integral number as a long int value.
    // If no valid conversion could be performed, a zero value is returned.
    // If the correct value is out of the range of representable values, ULONG_MAX is returned, an the global variable errno is set to ERANGE.

    // float strtof(const char* nptr, char** endptr);
    // If no conversion is performed, zero is returned and the value of nptr is stored in the location referenced by endptr.
    // If the correct value would cause overflow, plus or minus HUGE_VAL (HUGE_VALF, HUGE_VALL) is returned (according to the sign of the value), and ERANGE is stored in errno.
    // If the correct value would cause underflow, zero is returned and ERANGE is stored in errno.

    if (argc > 4 ) {
        return E_TOO_MANY_ARGS;
    }
    else if (argc == 4) {
        if (strcmp(argv[1], PARAMS_LOGARITHM) == 0) {
            params->progState = PS_LOGARITHM;
            params->sigdig = strtoul(argv[2], NULL, 10);
            params->logBase = strtod(argv[3], NULL);

            if (params->sigdig == 0 ||
                params->sigdig == ULONG_MAX ||
                params->sigdig > DBL_DIG)
            {   // cislo se neprevedlo nebo nastal outofrange
                return E_INVALID_PARAM;
            }
            if (params->logBase == 0 ||
                params->logBase == HUGE_VALF ||
                params->logBase == HUGE_VALL)
            {   // cislo se neprevedlo nebo nastal overflow/underflow
                return E_INVALID_PARAM;
            }
            if (params->logBase <= 0.0f ||
                params->logBase == 1.0f)
            {   // zaklad logaritmu kladna R - {1}
                return E_INVALID_PARAM;
            }

            return E_OK;
        }
    }
    else if (argc == 3) {
        if (strcmp(argv[1], PARAMS_HYPERBOLIC_TANGENT) == 0) {
            params->progState = PS_HYPERBOLIC_TANGENT;
            params->sigdig = strtoul(argv[2], NULL, 10);

            if (params->sigdig == 0 ||
                params->sigdig == ULONG_MAX ||
                params->sigdig > DBL_DIG)
            {   // cislo se neprevedlo nebo nastal outofrange
                return E_INVALID_PARAM;
            }

            return E_OK;
        }
    }
    else if (argc == 2) {
        if (strcmp(argv[1], PARAMS_WEIGHTED_ARITHMETIC_MEAN) == 0) {
            params->progState = PS_WEIGHTED_ARITHMETIC_MEAN;
            return E_OK;
        }
        else if (strcmp(argv[1], PARAMS_WEIGHTED_QUADRATIC_MEAN) == 0) {
            params->progState = PS_WEIGHTED_QUADRATIC_MEAN;
            return E_OK;
        }
        else if (strcmp(argv[1], PARAMS_HELP) == 0) {
            params->progState = PS_HELP;
            return E_OK;
        }
    }
    else {
        params->progState = PS_HELP;
        return E_OK;
    }

    return E_UNKNOWN_PARAM;
}

// hlavni funkce main
int main(int argc, char* argv[])
{
    ProgParams state;
    Error err = parseParams(argc, argv, &state);
    CHECK_ERROR(err);

    switch (state.progState) {
        case PS_HELP:
            printHelp();
            break;

        case PS_WEIGHTED_ARITHMETIC_MEAN:
            err = computeWeightedMean(stdin, stdout, WM_ARITHMETIC);
            CHECK_ERROR(err);
            break;

        case PS_WEIGHTED_QUADRATIC_MEAN:
            err = computeWeightedMean(stdin, stdout, WM_QUADRATIC);
            CHECK_ERROR(err);
            break;

        case PS_HYPERBOLIC_TANGENT:
            err = computeTanh(stdin, stdout, sigdigToEpsylon(state.sigdig));
            CHECK_ERROR(err);
            break;

        case PS_LOGARITHM:
            err = computeLog(stdin, stdout, state.logBase, sigdigToEpsylon(state.sigdig));
            CHECK_ERROR(err);
            break;

        default:
            printError(E_NOT_IMPLEMENTED);
            return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
