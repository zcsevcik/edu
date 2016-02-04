/*
 * Soubor:  proj4.h
 * Datum:   2010/12/13
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Ceske razeni, projekt c. 4 pro predmet IZP
 */

#ifndef PROJ4_H
#define	PROJ4_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif

// konstanty prikazove radky
const char* const PARAMS_HELP = "-h";
const char* const PARAMS_SORT = "--sort";
const char* const PARAMS_USORT = "--usort";
const char* const PARAMS_PRINT = "--print";
const char* const PARAMS_ROW_BEFORE = "--before";
const char* const PARAMS_ROW_AFTER = "--after";
const char* const PARAMS_CODEPAGE = "--codepage";
const char* const PARAMS_CODEPAGE_CP28592 = "iso-8859-2";
const char* const PARAMS_CODEPAGE_CP1250 = "win1250";
const char* const PARAMS_CODEPAGE_CP852 = "cp852";

const int PRIMARY_CP28592[256];
const int SECONDARY_CP28592[256];
const int PRIMARY_CP1250[256];
const int SECONDARY_CP1250[256];
const int PRIMARY_CP852[256];
const int SECONDARY_CP852[256];

const int* PRIMARY_CP[3];
const int* SECONDARY_CP[3];

// stav programu
typedef enum {
    PS_HELP,
    PS_PRINT,
} ProgState;

// chyba
typedef enum {
    E_OK,                           // v poradku
    E_UNKNOWN_ERROR,                // blize nespecifikovana chyba v programu
    E_UNKNOWN_PARAM,                // neznamy parametr
    E_NOT_IMPLEMENTED,              // neimplementovano
    E_INVALID_PARAM,                // chybny parametr
    E_ALLOC_FAILED,
    E_IO,
    E_BAD_COLLUMN,
} Error;

typedef enum {
    RS_AFTER,
    RS_BEFORE,
    RS_ALL,
} RowSelect;

typedef enum {
    SORT_NONE,
    SORT_UNIQUE,
    SORT_BUBBLE,
} Sort;

typedef enum {
    CP28592,
    CP1250,
    CP852,
} Codepage;

typedef struct {
    ProgState progState;
    Sort sort;
    char* printCol;
    RowSelect selectRow;
    char* selectCol;
    char* selectString;
    Codepage codepage;
    char* inputFile;
    char* outputFile;
} ProgParams;

typedef struct {
    size_t id;
    char* name;
} Header;

typedef char* Record;

typedef struct {
    size_t rows;
    size_t cols;
    Header* headers;
    Record* records;
} Table;

int mystrcmp(const char*, const char*, Codepage);
void sort(Record*, size_t, Codepage);
void usort(Record*, size_t*, Codepage);
Error select(const Table*, Record**, size_t*, size_t, RowSelect, size_t, const char*, Codepage);
Error table_load(FILE*, Table*);
void table_free(Table*);
Record table_getitem(const Table*, size_t, size_t);
void record_print(const Record*, size_t, FILE*);
size_t getCollumnID(char*, Header*, size_t);
Error parseParams(int, char*[], ProgParams* const);
void printHelp();
void printError(Error);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ4_H */
