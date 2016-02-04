/*
 * Soubor:  proj3matrix.c
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#include "proj3matrix.h"
#include <assert.h>
#include <stdlib.h>

bool matrix_alloc(Matrix* mat)
{
    assert(mat != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);

    mat->items = (int *)calloc(mat->rows * mat->cols, sizeof(int));
    return mat->items != NULL;
}

void matrix_free(Matrix* mat)
{
    assert(mat != NULL);
    free(mat->items);
    mat->items = NULL;
}

int matrix_getitem(const Matrix* mat, size_t row, size_t col)
{
    assert(mat != NULL);
    assert(mat->items != NULL);
    assert(row < mat->rows);
    assert(col < mat->cols);

    return mat->items[row * mat->cols + col];
}

void matrix_setitem(Matrix* mat, size_t row, size_t col, int value)
{
    assert(mat != NULL);
    assert(mat->items != NULL);
    assert(row < mat->rows);
    assert(col < mat->cols);

    mat->items[row * mat->cols + col] = value;
}

void matrix_mult(const Matrix* lhs, const Matrix* rhs, Matrix* dest)
{
    assert(lhs != NULL && lhs->items != NULL);
    assert(rhs != NULL && rhs->items != NULL);
    assert(dest != NULL && dest->items != NULL);
    assert(dest->rows == lhs->rows);
    assert(dest->cols == rhs->cols);

    int* pdest = dest->items;
    for (size_t i = 0; i < dest->rows; ++i) {
        for (size_t j = 0; j < dest->cols; ++j, ++pdest) {
            int* plhs = &lhs->items[i * lhs->cols];
            int* prhs = &rhs->items[j];
            *pdest = 0;
            for (size_t k = 0; k < lhs->cols; ++k, ++plhs, prhs += rhs->cols) {
                *pdest += *plhs * *prhs;
            }
        }
    }
}
