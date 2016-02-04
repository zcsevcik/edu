/*
 * Soubor:  proj1.h
 * Datum:   2010/10/07
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Jednoducha komprese textu, projekt c. 1 pro predmet IZP
 * Popis:   Program komprimuje nebo dekomprimuje text podle zadane delky bloku.
 */

#ifndef PROJ1_H
#define PROJ1_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// delka bloku a pocet opakovani
const size_t REPEAT_MAX = 9;

// parametry prikazove radky
const char* const PARAMS_HELP = "-h";
const char* const PARAMS_COMPRESS = "-c";
const char* const PARAMS_DECOMPRESS = "-d";
const char* const PARAMS_EXTRA = "--extra";

// stavy programu
typedef enum {
    PS_HELP,                    // napoveda
    PS_COMPRESS,                // komprimace
    PS_DECOMPRESS,              // dekomprimace
    PS_EXTRA,                   // vlastni rozsireni
} ProgState;

// chybova hlaseni
typedef enum {
    E_OK,                       // v poradku
    E_IOFMT,                    // chyba na vstupu/vystupu
    E_TOO_MANY_ARGS,            // prilis mnoho parametru
    E_UNKNOWN_PARAM,            // neznamy parametr
    E_NOT_IMPLEMENTED,          // neimplementovano
    E_INVALID_BLOCK_LENGTH,     // nespravna delka bloku
    E_ALLOCATION_FAILED,        // chyba pri alokaci pameti
} Error;

// struktura pro parsovani parametru
typedef struct {
    ProgState progState;        // stav programu
    unsigned long blockLength;  // delka bloku
} ProgParams;

// deklarace funkci
void printHelp();
void printError(Error);
bool checkBlockLength(unsigned long);
Error parseParams(int argc, char* argv[], ProgParams* const);
Error compress(const unsigned long, FILE* fin, FILE* fout);
Error decompress(const unsigned long, FILE* fin, FILE* fout);

#ifdef __cplusplus
}
#endif

#endif // PROJ1_H
