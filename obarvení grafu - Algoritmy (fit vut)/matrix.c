/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: matrix.c 25 2011-12-11 20:52:44Z zcsevcik $
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "matrix.h"

#include <alloca.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
# include <algorithm>
using std::max;
using std::min;
#else
/**
 * Maximum.
 * @return Vrati vetsi ze dvou cisel.
 **/
size_t max(size_t a, size_t b)
{ return (a < b) ? b : a; }

/**
 * Minimum.
 * @return Vrati mensi ze dvou cisel.
 **/
size_t min(size_t a, size_t b)
{ return (a < b) ? a : b; }
#endif

/**
 * Struktura matice.
 **/
struct _matrix_st {
    /**
     * Matice sousednosti.
     * Implementovana jako trojuhelnikove pole. 
     **/
    bool* m;

    size_t vertices; ///< Pocet vrcholu v matici.
};

static size_t _matrix_count(size_t vertices);
static size_t _matrix_at(const matrix_t*, size_t row, size_t col);

size_t _matrix_count(size_t vertices)
{
    return (vertices * vertices / 2) - (vertices / 2);
}

size_t _matrix_at(const matrix_t* matrix, size_t row, size_t col)
{
    assert(matrix != NULL);

    size_t x = max(row, col);
    size_t y = min(row, col);

    size_t pos = y * matrix->vertices + x;
    for (size_t i = y + 1; i > 0; --i) {
        pos -= i;
    }

    return pos;
}

/**
 * Vytvori ctvercovou matici sousednosti.
 *
 * @relates _matrix_st
 * @param vertices Pocet vrcholu.
 * @return Matici nebo NULL.
 **/
matrix_t* ial_matrix_new(size_t vertices)
{
    if (vertices == 0) {
        return NULL;
    }

    size_t count = _matrix_count(vertices);
    size_t byteCount = sizeof(matrix_t) + count * sizeof(bool);
    matrix_t* matrix = (matrix_t*)malloc(byteCount);

    if (matrix != NULL) {
        memset(matrix, '\0', byteCount);
        matrix->vertices = vertices;
        matrix->m = (bool*) (((char*)matrix) + sizeof(matrix_t));

        return matrix;
    }

    return NULL;
}

/**
 * Odstrani matici.
 *
 * @relates _matrix_st
 **/
void ial_matrix_free(matrix_t* matrix)
{
    free(matrix);
}

/**
 * Vrati pocet vrcholu v matici.
 *
 * @relates _matrix_st
 * @return Pocet vrcholu.
 **/
size_t ial_matrix_get_vertices(const matrix_t* matrix)
{
    if (matrix == NULL) {
        return 0;
    }

    return matrix->vertices;
}

/**
 * Vrati prvek matice.
 *
 * @relates _matrix_st
 * @param row Urcuje radek matice.
 * @param col Urcuje sloupec matice.
 * @return Prvek matice na indexu s hodnotou 0 nebo 1; pri chybe vrati -1.
 **/
int ial_matrix_get(const matrix_t* matrix, size_t row, size_t col)
{
    if (matrix == NULL) {
        return -1;
    }
    if (row == col) {
        // na diagonale vzdy 0
        return 0;
    }
    if (row >= matrix->vertices) {
        return -1;
    }
    if (col >= matrix->vertices) {
        return -1;
    }

    size_t at = _matrix_at(matrix, row, col);
    return matrix->m[at];
}

/**
 * Nastavi prvek matice.
 *
 * @relates _matrix_st
 * @param row Urcuje radek matice.
 * @param col Urcuje sloupec matice.
 * @param value Hodnota.
 * @return true pri uspechu; jinak false.
 **/
int ial_matrix_set(matrix_t* matrix, size_t row, size_t col, bool value)
{
    if (matrix == NULL) {
        return false;
    }
    if (row == col) {
        return true;
    }
    if (row >= matrix->vertices) {
        return false;
    }
    if (col >= matrix->vertices) {
        return false;
    }

    size_t at = _matrix_at(matrix, row, col);
    matrix->m[at] = value;

    return true;
}

/**
 * Nastavi vsechny prvky matice z dvourozmerneho pole.
 *
 * @relates _matrix_st
 * @param vertices Pocet vrcholu.
 * @param values Dvourozmerne pole prvku matice.
 * @return true pri uspechu; jinak false.
 **/
int ial_matrix_set_all(matrix_t* matrix, size_t vertices, const bool* values)
{
    if (matrix == NULL) {
        return false;
    }
    if (values == NULL) {
        return false;
    }
    if (vertices != matrix->vertices) {
        return false;
    }

    bool* m = &matrix->m[0];
    for (size_t row = 0; row < vertices; ++row) {
        for (size_t col = row + 1; col < vertices; ++col) {
            *m++ = values[row * vertices + col];
        }
    }

    return true;
}

/**
 * Odstrani urcity radek a sloupec z matice.
 *
 * @relates _matrix_st
 * @param vertex Vrchol k odstraneni.
 * @return true pri uspechu; jinak false.
 **/
int ial_matrix_delete(matrix_t* matrix, size_t vertex)
{
    if (matrix == NULL) {
        return false;
    }
    if (vertex >= matrix->vertices) {
        return false;
    }

    size_t at_len = (matrix->vertices - 1);
    size_t* at = (size_t*)alloca(at_len * sizeof(size_t));

    for (size_t row, col, i = 0; i < at_len; ++i) {
        if (i < vertex) {
            row = i;
            col = vertex;
        }
        else {
            row = vertex;
            col = i + 1;
        }

        at[i] = _matrix_at(matrix, row, col);
    }

    bool* dst = NULL;
    bool* src = NULL;
    size_t len = 0;

    if (vertex == 0) {
        dst = &matrix->m[0];
    }
    else {
        size_t podminka = (vertex + 1 == matrix->vertices) ? vertex - 2 : vertex;

        for (size_t i = 0; i < podminka; ++i) {
            dst = &matrix->m[at[i] - i];
            src = &matrix->m[at[i] + 1];
            len = at[i + 1] - at[i] - 1;

            memmove(dst, src, len);
            dst += len;
        }
    }

    // zkopiruje zbytek pod horizontalni carou
    if (vertex + 1 != matrix->vertices) {
        src = &matrix->m[at[at_len - 1]] + 1;
        len = _matrix_count(matrix->vertices) - at[at_len - 1] - 1;

        memmove(dst, src, len);
    }

    matrix->vertices--;

    return true;
}

/**
 * Spoji hrany dvou vrcholu v matici.
 *
 * @relates _matrix_st
 * @param vertex_lhs Vrchol, do ktereho se ulozi vysledek spojeni.
 * @param vertex_rhs Druhy vrchol, se kterym se spojuje.
 * @return true pri uspechu; jinak false.
 **/
int ial_matrix_join(matrix_t* matrix, size_t vertex_lhs, size_t vertex_rhs)
{
    if (matrix == NULL) {
        return false;
    }
    if (vertex_lhs >= matrix->vertices) {
        return false;
    }
    if (vertex_rhs >= matrix->vertices) {
        return false;
    }

    for (size_t i = 0; i < matrix->vertices; ++i) {
        int lhs_val = ial_matrix_get(matrix, vertex_lhs, i);
        int rhs_val = ial_matrix_get(matrix, vertex_rhs, i);

        if (lhs_val == -1) {
            return false;
        }
        if (rhs_val == -1) {
            return false;
        }

        if (!ial_matrix_set(matrix, vertex_lhs, i, (bool)(lhs_val || rhs_val))) {
            return false;
        }
    }

    return true;
}

/**
 * Vrati interni reprezentaci dat matice.
 *
 * @relates _matrix_st
 * @return interni data pri uspechu; jinak NULL.
 **/
const bool* _matrix_raw(const matrix_t* matrix)
{
    if (matrix != NULL) {
        return matrix->m;
    }
    else {
        return NULL;
    }
}
