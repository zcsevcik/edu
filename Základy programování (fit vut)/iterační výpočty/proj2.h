/*
 * Soubor:  proj2.h
 * Datum:   2010/10/21
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program pro vypocet hyperbolickeho tangens, obecneho logaritmu,
 *          vazeneho aritmetickeho a kvadratickeho prumeru za pomoci zakladnich
 *          matematickych operaci a standartniho vstupu/vystupu.
 */

#ifndef PROJ2_H
#define	PROJ2_H

#include "proj2math.h"
#include <float.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif

// konstanty prikazove radky
const char* const PARAMS_HELP = "-h";
const char* const PARAMS_WEIGHTED_ARITHMETIC_MEAN = "--wam";
const char* const PARAMS_WEIGHTED_QUADRATIC_MEAN = "--wqm";
const char* const PARAMS_HYPERBOLIC_TANGENT = "--tanh";
const char* const PARAMS_LOGARITHM = "--logax";

// ukazatel na funkci, ktera pocita vazeny prumer
typedef double (*WEIGHTED_MEAN_FN)(WeightedValue* const, const WeightedValue* const);

// vazeny prumer
typedef enum {
    WM_ARITHMETIC = 0,              // vazeny aritmeticky prumer
    WM_QUADRATIC,                   // vazeny kvadraticky prumer
} WeightedMean;

// stav programu
typedef enum {
    PS_HELP,                        // napoveda
    PS_WEIGHTED_ARITHMETIC_MEAN,    // vazeny aritmeticky prumer
    PS_WEIGHTED_QUADRATIC_MEAN,     // vazeny kvadraticky prumer
    PS_HYPERBOLIC_TANGENT,          // hyperbolicky tangens
    PS_LOGARITHM,                   // logaritmus
} ProgState;

// chyba
typedef enum {
    E_OK,                           // v poradku
    E_IOFMT,                        // chyba na vstupu/vystupu
    E_TOO_MANY_ARGS,                // prilis mnoho parametru
    E_UNKNOWN_PARAM,                // neznamy parametr
    E_NOT_IMPLEMENTED,              // neimplementovano
    E_INVALID_PARAM,                // chybny parametr
} Error;

// struktura pro parsovani z prikazove radky
typedef struct {
    ProgState progState;
    unsigned long sigdig;
    double logBase;
} ProgParams;

// deklarace funkci
Error parseParams(int, char*[], ProgParams* const);
Error computeWeightedMean(FILE*, FILE*, WeightedMean);
Error computeTanh(FILE*, FILE*, double);
Error computeLog(FILE*, FILE*, double, double);
double sigdigToEpsylon(unsigned long);
void printHelp();
void printError(Error);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ2_H */
