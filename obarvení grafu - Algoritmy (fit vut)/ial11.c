/*
 * Projekt: Obarveni grafu, nahradni projekt c. 6 pro predmet IAL
 * $Id: ial11.c 29 2011-12-11 22:33:42Z zcsevcik $
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "graph.h"
#include "matrix.h"
#include "vertex.h"

#include <stdio.h>
#include <stdlib.h>

#define TAKEDOWN(code) do { printf("%s\n", ERRMSG[code]); return code; } while(0);

typedef vertex_t* vertex_ptr;

const char *ERRMSG[] = {
  // EOK
  "",
  // EVAL
  "false",
  // EMEM
  "Malo pameti!",
  // EFILE
  "Poskozeny nebo neexistujici soubor.",
  // EPARAM
  "Chyba v parametrech.",  
  // ERUN
  "Runtime chyba."
};

typedef enum errs 
{ EOK, EVAL, EMEM, EFILE, EPARAM, ERUN } err_t;

void print_matrix(matrix_t* matrix)
{
    size_t vertices = ial_matrix_get_vertices(matrix);

    for (size_t row = 0; row < vertices; ++row) {
        for (size_t col = 0; col < vertices; ++col) {
            printf("%d ", ial_matrix_get(matrix, row, col));
        }
        printf("\n");
    }

    printf("\n");
}

void print_graph(graph_t* graph)
{
    size_t vertices = ial_graph_count_vertices(graph);
    size_t color = 0;

    for (size_t i = 0; i < vertices; ++i) {
        printf("barva %d: ", (int)++color);

        vertex_t* vertex = ial_graph_get_vertex(graph, i);
        while (vertex != NULL) {
            printf("%lu ", ial_vertex_number(vertex));
            vertex = ial_vertex_next(vertex);
        }

        printf("\n");
    }

    printf("\n");
}

/**
 * Nacte matici sousednosti ze souboru.
 *
 * @param file Soubor, ze ktereho ma cist.
 * @param mat Ukazatel na ctvercovou matici, do ktere se ulozi jednotlive prvky.
 * @param vertices Ukazatel na pole vrcholu.
 * @param size Predava pocet vrcholu.
 * @return Pri uspechu EOK; jinak cislo chyby #err_t.
 **/
err_t load_input(const char *file, bool **mat, vertex_ptr **vertices, size_t *size)
{
  FILE *fin = fopen(file, "r");
  if (fin) {

#ifdef _WIN32
      unsigned long gsize;
      if (1 != fscanf(fin, "%lu", &gsize))
          TAKEDOWN(EFILE)
#else
      size_t gsize;
      if (1 != fscanf(fin, "%zu", &gsize))
          TAKEDOWN(EFILE);
#endif
  
      *size = gsize;

      if (gsize == 0)
          return EOK;

      *vertices = (vertex_ptr*)malloc(gsize * sizeof(vertex_ptr*));
      if (*vertices == NULL)
          TAKEDOWN(EMEM);
  
      for (size_t i = 0; i < gsize; i++)
      {
        vertex_t* vertex = ial_vertex_new(i);
        if (vertex == NULL)
            TAKEDOWN(EMEM);

        (*vertices)[i] = vertex;
      }
  
      if ((*mat = (bool*)malloc(gsize*gsize * sizeof(bool))) == NULL)
        TAKEDOWN(EMEM);

    
        for (size_t row = 0; row < gsize; ++row) {
            for (size_t col = 0; col < gsize; ++col) {
              int d;
              if (fscanf(fin, "%d", &d) != 1) { TAKEDOWN(EFILE) }
              else (*mat)[row * gsize + col] = (bool)d;
            }
        }
    
      fclose(fin);
      return EOK;
  }

  TAKEDOWN(EFILE);
}


int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 2) TAKEDOWN(EPARAM);
    err_t state = EOK;
    bool *mt = NULL;
    vertex_ptr *vertices = NULL;
    size_t vcount;
    if ((state = load_input(argv[1], &mt, &vertices, &vcount)) != EOK) return state; 

    if (vcount == 0)
        return EOK;

    matrix_t* matice = ial_matrix_new(vcount);
    if (!matice)
        TAKEDOWN(EMEM);
    
    ial_matrix_set_all(matice, vcount, (bool*)mt);
    free(mt);

    graph_t* graph = ial_graph_new(vcount, vertices, matice);
    if (!graph)
        TAKEDOWN(EMEM);

    //print_matrix(ial_graph_get_matrix(graph));
    
    ial_graph_complete(graph);
    //print_matrix(ial_graph_get_matrix(graph));
    print_graph(graph);

    ial_graph_free(graph);
    free(vertices);

    return EOK;
}
