/*
 * Soubor:  proj3matrix.h
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#ifndef PROJ3MATRIX_H
#define	PROJ3MATRIX_H

#include <stdbool.h>
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    size_t rows;
    size_t cols;
    int* items;
} Matrix;

bool matrix_alloc(Matrix*);
void matrix_free(Matrix*);
int matrix_getitem(const Matrix*, size_t, size_t);
void matrix_setitem(Matrix*, size_t, size_t, int);
void matrix_mult(const Matrix*, const Matrix*, Matrix*);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ3MATRIX_H */
