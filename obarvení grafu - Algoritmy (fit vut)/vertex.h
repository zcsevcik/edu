/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: vertex.h 10 2011-12-11 13:57:50Z cloq $
 */

#ifndef VERTEX_H
#define VERTEX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _vertex_st vertex_t;

vertex_t*
ial_vertex_new(unsigned long i);

void
ial_vertex_free(
        vertex_t*);

int
ial_vertex_join(
        vertex_t*,
        vertex_t*);

vertex_t*
ial_vertex_next(
        vertex_t*);

unsigned long
ial_vertex_number(
        vertex_t*);

#ifdef __cplusplus
}
#endif

#endif //VERTEX_H
