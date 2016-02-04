/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: matrix.h 14 2011-12-11 16:04:32Z zcsevcik $
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _matrix_st matrix_t;

matrix_t*
ial_matrix_new(
        size_t vertices);

void
ial_matrix_free(
        matrix_t*);

int
ial_matrix_get(
        const matrix_t*,
        size_t row,
        size_t col);

size_t
ial_matrix_get_vertices(
        const matrix_t*);

int
ial_matrix_set(
        matrix_t*,
        size_t row,
        size_t col,
        bool value);

int
ial_matrix_set_all(
        matrix_t*,
        size_t vertices,
        const bool* values);

int
ial_matrix_delete(
        matrix_t*,
        size_t vertex);

int
ial_matrix_join(
        matrix_t*,
        size_t vertex_lhs,
        size_t vertex_rhs);

const bool*
_matrix_raw(
        const matrix_t*);

#ifdef __cplusplus
}
#endif

#endif //MATRIX_H
