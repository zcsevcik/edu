/*
 * Soubor:  proj3io.h
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#ifndef PROJ3IO_H
#define	PROJ3IO_H

#include "proj3matrix.h"
#include "proj3vector.h"
#include <stdbool.h>
#include <stdio.h>

#define FMT_VECTOR 1
#define FMT_MATRIX 2
#define FMT_VECTOR_OF_MATRICES 3

#ifdef	__cplusplus
extern "C" {
#endif

bool test(const char*, FILE*);
bool vector_load(Vector*, const char*);
void vector_print(const Vector*, FILE*);
bool matrix_load(Matrix*, const char*);
void matrix_print(const Matrix*, FILE*);
void noop_print(FILE*);
bool vm_load(VectorOfMatrices*, const char*);
void vm_print(const VectorOfMatrices*, FILE*);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ3IO_H */
