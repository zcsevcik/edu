/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: graph.h 5 2011-12-10 02:08:28Z zcsevcik $
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "matrix.h"
#include "vertex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _graph_st graph_t;

graph_t*
ial_graph_new(
        size_t numvertices,
        vertex_t** vertices,
        matrix_t* matrix);

void
ial_graph_free(
        graph_t*);

int
ial_graph_complete(
        graph_t*);

size_t
ial_graph_count_vertices(
        const graph_t*);

vertex_t*
ial_graph_get_vertex(
        const graph_t*,
        size_t vertex);

matrix_t*
ial_graph_get_matrix(
        const graph_t*);

#ifdef __cplusplus
}
#endif

#endif //GRAPH_H
