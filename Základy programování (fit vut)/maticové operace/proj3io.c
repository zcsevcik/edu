/*
 * Soubor:  proj3io.c
 * Datum:   2010/11/24
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Maticove operace, projekt c. 3 pro predmet IZP
 */

#include "proj3io.h"
#include <assert.h>

#define CHECK_IO(success, lParams, jmp) { \
    if (success == EOF || success != lParams) { goto jmp; }\
}

bool test(const char* filename, FILE* fout)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        return false;
    }

    int fmt;

    int suc = fscanf(fp, "%d", &fmt);
    if (suc == EOF || suc != 1) {
        fclose(fp);
        return false;
    }
    if (fclose(fp) != 0) {
        perror(filename);
    }

    switch (fmt) {
        case FMT_VECTOR: {
            Vector vect = { .count = 0, .items = NULL };
            if (!vector_load(&vect, filename)) {
                return false;
            }

            vector_print(&vect, fout);
            vector_free(&vect);
        }   break;

        case FMT_MATRIX: {
            Matrix mat = { .rows = 0, .cols = 0, .items = NULL };
            if (!matrix_load(&mat, filename)) {
                return false;
            }

            matrix_print(&mat, fout);
            matrix_free(&mat);
        }   break;

        case FMT_VECTOR_OF_MATRICES: {
            VectorOfMatrices vm = { .count = 0, .rows = 0, .cols = 0, .items = NULL };
            if (!vm_load(&vm, filename)) {
                return false;
            }

            vm_print(&vm, fout);
            vm_free(&vm);
        }   break;

        default:
            return false;
    }

    return true;
}

bool vector_load(Vector* vect, const char* filename)
{
    assert(vect != NULL);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        goto return_false;
    }

    int fmt;
    unsigned long count;

    int suc = fscanf(fp, "%d", &fmt);
    CHECK_IO(suc, 1, return_false);
    if (fmt != FMT_VECTOR) {
        goto return_false;
    }

    suc = fscanf(fp, "%lu", &count);
    CHECK_IO(suc, 1, return_false);
    if (count <= 0) {
        goto return_false;
    }
    vect->count = count;
    if (!vector_alloc(vect)) {
        goto return_false;
    }

    int* pVect = vect->items;
    for (size_t i = 0; i < vect->count; ++i) {
        suc = fscanf(fp, "%d", pVect++);
        CHECK_IO(suc, 1, return_false);
    }

    if (fclose(fp) != 0) {
        perror(filename);
    }

    return true;

return_false:
    if (fp != NULL && fclose(fp) != 0) {
        perror(filename);
    }

    vector_free(vect);
    return false;
}

void vector_print(const Vector* vect, FILE* fp)
{
    assert(vect != NULL);
    assert(fp != NULL);
    assert(vect->count > 0);
    assert(vect->items != NULL);

    fprintf(fp, "%d\n", FMT_VECTOR);
    fprintf(fp, "%zu\n", vect->count);

    int* pVect = vect->items;
    for (size_t i = 0; i < vect->count; ++i) {
        fprintf(fp, "%d ", *pVect++);
    }
}

void noop_print(FILE* fp)
{
    assert(fp != NULL);
    fprintf(fp, "false\n");
}

bool matrix_load(Matrix* mat, const char* filename)
{
    assert(mat != NULL);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        goto return_false;
    }

    int fmt;
    unsigned long rows;
    unsigned long cols;

    int suc = fscanf(fp, "%d", &fmt);
    CHECK_IO(suc, 1, return_false);
    if (fmt != FMT_MATRIX) {
        goto return_false;
    }

    suc = fscanf(fp, "%lu %lu", &rows, &cols);
    CHECK_IO(suc, 2, return_false);
    if (rows <= 0 || cols <= 0) {
        goto return_false;
    }
    mat->rows = rows;
    mat->cols = cols;
    if (!matrix_alloc(mat)) {
        goto return_false;
    }

    int* pMat = mat->items;
    size_t count = rows * cols;
    for (size_t i = 0; i < count; ++i) {
        suc = fscanf(fp, "%d", pMat++);
        CHECK_IO(suc, 1, return_false);
    }

    if (fclose(fp) != 0) {
        perror(filename);
    }

    return true;

return_false:
    if (fp != NULL && fclose(fp) != 0) {
        perror(filename);
    }

    matrix_free(mat);
    return false;
}

void matrix_print(const Matrix* mat, FILE* fp)
{
    assert(mat != NULL);
    assert(fp != NULL);
    assert(mat->rows > 0);
    assert(mat->cols > 0);
    assert(mat->items != NULL);

    fprintf(fp, "%d\n", FMT_MATRIX);
    fprintf(fp, "%zu %zu\n", mat->rows, mat->cols);

    int* pMat = mat->items;
    size_t count = mat->rows * mat->cols;

    for (size_t i = 0; i < count; ++i) {
        fprintf(fp, "%d ", *pMat++);
        if (i % mat->cols == mat->cols - 1) {
            fprintf(fp, "\n");
        }
    }
}

bool vm_load(VectorOfMatrices* vm, const char* filename)
{
    assert(vm != NULL);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        goto return_false;
    }

    int fmt;
    unsigned long count;
    unsigned long rows;
    unsigned long cols;

    int suc = fscanf(fp, "%d", &fmt);
    CHECK_IO(suc, 1, return_false);
    if (fmt != FMT_VECTOR_OF_MATRICES) {
        goto return_false;
    }

    suc = fscanf(fp, "%lu %lu %lu", &count, &rows, &cols);
    CHECK_IO(suc, 3, return_false);
    if (rows <= 0 || cols <= 0 || count <= 0) {
        goto return_false;
    }
    vm->rows = rows;
    vm->cols = cols;
    vm->count = count;
    if (!vm_alloc(vm)) {
        goto return_false;
    }

    int* pvm = vm->items;
    size_t iter_count = rows * cols * count;
    for (size_t i = 0; i < iter_count; ++i) {
        suc = fscanf(fp, "%d", pvm++);
        CHECK_IO(suc, 1, return_false);
    }

    if (fclose(fp) != 0) {
        perror(filename);
    }

    return true;

return_false:
    if (fp != NULL && fclose(fp) != 0) {
        perror(filename);
    }

    vm_free(vm);
    return false;
}

void vm_print(const VectorOfMatrices* vm, FILE* fp)
{
    assert(vm != NULL);
    assert(fp != NULL);
    assert(vm->rows > 0);
    assert(vm->cols > 0);
    assert(vm->count > 0);
    assert(vm->items != NULL);

    fprintf(fp, "%d\n", FMT_VECTOR_OF_MATRICES);
    fprintf(fp, "%zu %zu %zu\n", vm->count, vm->rows, vm->cols);

    int* pvm = vm->items;
    size_t count = vm->rows * vm->cols * vm->count;

    for (size_t i = 0; i < count; ++i) {
        fprintf(fp, "%d ", *pvm++);
        if (i % vm->cols == vm->cols - 1) {
            fprintf(fp, "\n");
        }
    }
}
