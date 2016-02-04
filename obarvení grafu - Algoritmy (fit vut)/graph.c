/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: graph.c 25 2011-12-11 20:52:44Z zcsevcik $
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "graph.h"

#include <stdlib.h>
#include <string.h>

/**
 * Struktura grafu.
 **/
struct _graph_st {
    matrix_t* matrix;    ///< Matice sousednosti
    vertex_t** vertices; ///< Vrcholy grafu.
    size_t numvertices;  ///< Pocet vrcholu grafu.
};

static int _graph_find_non_neighbour_vertices(
        graph_t*,
        size_t* vertex_1st,
        size_t* vertex_2nd);

/**
 * Vytvori novy graf.
 *
 * @relates _graph_st
 * @param numvertices Pocet vrcholu grafu.
 * @param vertices Pole vrcholu.
 * @param matrix Matice sousednosti.
 * @return Graf nebo NULL.
 **/
graph_t* ial_graph_new(size_t numvertices, vertex_t** vertices, matrix_t* matrix)
{
    if (numvertices == 0) {
        return NULL;
    }
    if (vertices == NULL) {
        return NULL;
    }
    if (matrix == NULL) {
        return NULL;
    }
    if (ial_matrix_get_vertices(matrix) != numvertices) {
        return NULL;
    }

    graph_t* graph = (graph_t*)malloc(sizeof(graph_t));
    if (graph != NULL) {
        graph->numvertices = numvertices;
        graph->vertices = vertices;
        graph->matrix = matrix;

        return graph;
    }

    return NULL;
}

/**
 * Uvolni graf, matici a jednotlive vrcholy.
 *
 * @relates _graph_st
 **/
void ial_graph_free(graph_t* graph)
{
    if (graph != NULL) {
        for (size_t i = 0; i < graph->numvertices; ++i) {
            vertex_t* vertex = graph->vertices[i];
            while (vertex != NULL) {
                vertex_t* tmp = vertex;
                vertex = ial_vertex_next(vertex);
                ial_vertex_free(tmp);
            }
        }
        
        ial_matrix_free(graph->matrix);
    }

    free(graph);
}

/**
 * Vrati pocet vrcholu v grafu.
 *
 * @relates _graph_st
 * @return Pocet vrcholu; vrati 0 pri chybe.
 **/
size_t ial_graph_count_vertices(const graph_t* graph)
{
    if (graph == NULL) {
        return 0;
    }

    return graph->numvertices;
}

/**
 * Vrati vazany seznam i-teho vrcholu.
 *
 * @relates _graph_st
 * @param vertex Vrchol.
 * @return Linearni seznam spojenych vrcholu v pripade uspechu; jinak NULL.
 **/
vertex_t* ial_graph_get_vertex(const graph_t* graph, size_t vertex)
{
    if (graph == NULL) {
        return NULL;
    }
    if (vertex >= graph->numvertices) {
        return NULL;
    }

    return graph->vertices[vertex];
}

/**
 * Vrati matici sousednosti spojenou s grafem.
 *
 * @relates _graph_st
 * @return Matici sousednosti v pripade uspechu; jinak NULL.
 **/
matrix_t* ial_graph_get_matrix(const graph_t* graph)
{
    if (graph == NULL) {
        return NULL;
    }

    return graph->matrix;
}

/**
 * Prevede neuplny graf na uplny spojenim nesousedicich vrcholu.
 *
 * @relates _graph_st
 * @return true pri uspechu; jinak false.
 **/
int ial_graph_complete(graph_t* graph)
{
    if (graph == NULL) {
        return false;
    }

    size_t join_1st;
    size_t join_2nd;
    while (_graph_find_non_neighbour_vertices(graph, &join_1st, &join_2nd)) {
        if (!ial_matrix_join(graph->matrix, join_1st, join_2nd)) {
            return false;
        }
        if (!ial_matrix_delete(graph->matrix, join_2nd)) {
            return false;
        }

        if (!ial_vertex_join(graph->vertices[join_2nd],
                             graph->vertices[join_1st]))
        {
            return false;
        }

        graph->vertices[join_1st] = graph->vertices[join_2nd];

        memmove(&graph->vertices[join_2nd],
                &graph->vertices[join_2nd + 1],
                (graph->numvertices - join_2nd - 1) * sizeof(vertex_t*));

        graph->vertices[--graph->numvertices] = NULL;
    }

    return true;
}

int _graph_find_non_neighbour_vertices(graph_t* graph,
        size_t* vertex_1st, size_t* vertex_2nd)
{
    if (graph == NULL) {
        return false;
    }
    if (vertex_1st == NULL) {
        return false;
    }
    if (vertex_2nd == NULL) {
        return false;
    }

    const bool* rawdata = _matrix_raw(graph->matrix);
    for (size_t row = 0; row < graph->numvertices; ++row) {
        for (size_t col = row + 1; col < graph->numvertices; ++col) {
            if (*rawdata++ == 0) {
                *vertex_1st = row;
                *vertex_2nd = col;
                return true;
            }
        }
    }

    return false;
}
