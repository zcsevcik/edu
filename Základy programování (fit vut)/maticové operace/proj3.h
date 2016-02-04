/*
 * Soubor:  proj3.h
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#ifndef PROJ3_H
#define	PROJ3_H

#include "proj3matrix.h"
#include "proj3vector.h"

#ifdef	__cplusplus
extern "C" {
#endif

// konstanty prikazove radky
const char* const PARAMS_HELP = "-h";
const char* const PARAMS_TEST = "--test";
const char* const PARAMS_VECTOR_ADDITION = "--vadd";
const char* const PARAMS_VECTOR_SCALAR_MULTIPLICATION = "--vscal";
const char* const PARAMS_MATRIX_MULTIPLICATION = "--mmult";
const char* const PARAMS_MATRIX_EXPRESSION = "--mexpr";
const char* const PARAMS_EIGHT = "--eight";
const char* const PARAMS_BUBBLES = "--bubbles";
const char* const PARAMS_BUBBLES_3D = "--extbubbles";
const char* const PARAMS_MAZE = "--maze";
const char* const PARAMS_CAROM = "--carom";

// stav programu
typedef enum {
    PS_HELP,
    PS_TEST,
    PS_VECTOR_ADDITION,
    PS_VECTOR_SCALAR_MULTIPLICATION,
    PS_MATRIX_MULTIPLICATION,
    PS_MATRIX_EXPRESSION,
    PS_EIGHT,
    PS_BUBBLES,
    PS_BUBBLES_3D,
    PS_MAZE,
    PS_CAROM,
} ProgState;

// chyba
typedef enum {
    E_OK,                           // v poradku
    E_UNKNOWN_ERROR,                // blize nespecifikovana chyba v programu
    E_UNKNOWN_PARAM,                // neznamy parametr
    E_NOT_IMPLEMENTED,              // neimplementovano
    E_INVALID_PARAM,                // chybny parametr
} Error;

// struktura pro parsovani z prikazove radky
typedef struct {
    ProgState progState;
    char* filename1;
    char* filename2;
} ProgParams;

typedef enum {
    LeftToRight,
    RightToLeft,
    TopDown,
    BottomUp,
    TopRight,
    TopLeft,
    BottomLeft,
    BottomRight,
} Direction;

typedef struct {
    struct Coordinates {
        size_t row;
        size_t col;
    } *coords;
    size_t count;
} CoordArr;

// deklarace funkci
int bubbles(const Matrix*);
bool bubbles_isMarked(const CoordArr*, size_t, size_t);
void bubbles_markNeighbor(const Matrix*, size_t, size_t, CoordArr*);
void bubbles_markNeighborDir(const Matrix*, size_t, size_t, CoordArr*, Direction);
bool eight(const Matrix*, const Vector*);
bool eight_check(const Matrix*, const Vector*, size_t, size_t, Direction);
Error parseParams(int, char*[], ProgParams* const);
void printHelp();
void printError(Error);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ3_H */
