/*
 * Soubor:  proj3vector.h
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#ifndef PROJ3VECTOR_H
#define	PROJ3VECTOR_H

#include <stddef.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    size_t count;
    int* items;
} Vector;

typedef struct {
    size_t rows;
    size_t cols;
    size_t count;
    int* items;
} VectorOfMatrices;

bool vector_alloc(Vector*);
void vector_free(Vector*);
void vector_add(const Vector*, const Vector*, Vector*);
int vector_scalar_mult(const Vector*, const Vector*);

bool vm_alloc(VectorOfMatrices*);
void vm_free(VectorOfMatrices*);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ3VECTOR_H */
