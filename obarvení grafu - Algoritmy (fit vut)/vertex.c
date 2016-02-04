/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: vertex.c 25 2011-12-11 20:52:44Z zcsevcik $
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "vertex.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * Struktura vrcholu.
 **/
struct _vertex_st {
    /**
     * Poradove cislo vrcholu.
     * Slouzi k identifikaci vrcholu v linearnim seznamu.
     */
    unsigned long order;

    /**
     * Ukazatel na dalsi vrchol v linearnim seznamu.
     */
    struct _vertex_st* next;
};

/**
 * Vytvori novy vrchol.
 *
 * @relates _vertex_st
 * @param order Poradove cislo vrcholu.
 * @return Vrchol nebo NULL.
 **/
vertex_t* ial_vertex_new(unsigned long order)
{
    vertex_t* vertex = (vertex_t*)malloc(sizeof(vertex_t));
    if (vertex != NULL) {
        vertex->order = order;
        vertex->next = NULL;
        return vertex;
    }

    return NULL;
}

/**
 * Odstrani vrchol.
 *
 * @relates _vertex_st
 **/
void ial_vertex_free(vertex_t* vertex)
{
    free(vertex);
}

/**
 * Spoji dva vrcholy v linearnim seznamu.
 *
 * @relates _vertex_st
 * @param to Vrchol, kam se pripoji druhy vrchol.
 * @param from Vrchol, ktery se bude pripojovat.
 * @return true v pripade uspechu; jinak false.
 **/
int ial_vertex_join(vertex_t* to, vertex_t* from)
{
    if (to == NULL) {
        return false;
    }
    if (from == NULL) {
        return false;
    }

    to->next = from;
    return true;
}

/**
 * Vrati dalsi vrchol v linearnim seznamu.
 *
 * @relates _vertex_st
 * @return Nasledujici vrchol nebo NULL.
 **/
vertex_t* ial_vertex_next(vertex_t* vertex)
{
    if (vertex == NULL) {
        return NULL;
    }

    return vertex->next;
}

/**
 * Vrati poradove cislo vrcholu.
 *
 * @relates _vertex_st
 * @return Poradove cislo.
 **/
unsigned long ial_vertex_number(vertex_t* vertex)
{
    if (vertex == NULL) {
        return false;
    }

    return vertex->order;
}
