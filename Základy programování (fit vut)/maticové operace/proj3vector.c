/*
 * Soubor:  proj3vector.c
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#include "proj3vector.h"
#include <assert.h>
#include <stdlib.h>

bool vector_alloc(Vector* vect)
{
    assert(vect != NULL);
    assert(vect->count > 0);

    vect->items = (int *)calloc(vect->count, sizeof(int));
    return vect->items != NULL;
}

void vector_free(Vector* vect)
{
    assert(vect != NULL);
    free(vect->items);
    vect->items = NULL;
}

void vector_add(const Vector* lhs, const Vector* rhs, Vector* dest)
{
    assert(lhs != NULL && lhs->count > 0 && lhs->items != NULL);
    assert(rhs != NULL && rhs->count > 0 && rhs->items != NULL);
    assert(dest != NULL && dest->count > 0 && dest->items != NULL);
    assert(lhs->count == rhs->count && rhs->count == dest->count);

    int* pv1 = lhs->items;
    int* pv2 = rhs->items;
    int* pvd = dest->items;

    for (size_t i = 0; i < dest->count; ++i) {
        *pvd++ = *pv1++ + *pv2++;
    }
}

int vector_scalar_mult(const Vector* lhs, const Vector* rhs)
{
    assert(lhs != NULL && lhs->count > 0 && lhs->items != NULL);
    assert(rhs != NULL && rhs->count > 0 && rhs->items != NULL);
    assert(lhs->count == rhs->count);

    int* pv1 = lhs->items;
    int* pv2 = rhs->items;
    int scalar = 0;

    for (size_t i = 0; i < lhs->count; ++i) {
        scalar += *pv1++ * *pv2++;
    }

    return scalar;
}

bool vm_alloc(VectorOfMatrices* vect)
{
    assert(vect != NULL);
    assert(vect->count > 0);
    assert(vect->rows > 0);
    assert(vect->cols > 0);

    vect->items = (int *)calloc(vect->count * vect->rows * vect->cols, sizeof(int));
    return vect->items != NULL;
}

void vm_free(VectorOfMatrices* vect)
{
    assert(vect != NULL);
    free(vect->items);
    vect->items = NULL;
}
