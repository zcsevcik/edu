/*
 * Soubor:  proj1circularbuffer.h
 * Datum:   2010/10/07
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Jednoducha komprese textu, projekt c. 1 pro predmet IZP
 * Popis:   Program komprimuje nebo dekomprimuje text podle zadane delky bloku.
 */

#ifndef PROJ1CIRCULARBUFFER_H
#define PROJ1CIRCULARBUFFER_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// kruhovy buffer pro posloupnosti znaku
// pokud je start == NULL && end == NULL, tak je buffer prazdny
// pokud start == end, pak je v bufferu jedna polozka
// pokud end == start - 1, pak je buffer plny
// jinak ukazuje start na prvni zapsanou polozku
// end ukazuje na posledni zapsanou polozku
typedef struct {
    char* buf;
    char* start;
    char* end;
    size_t size_max;
} CircularBuffer;

// prototypy funkci
bool cb_init(CircularBuffer* const, size_t);
void cb_free(CircularBuffer* const);
void cb_clear(CircularBuffer* const);
size_t cb_count(const CircularBuffer* const);
void cb_push_back(CircularBuffer* const, char);
char cb_pop_front(CircularBuffer* const);
char cb_peek(const CircularBuffer* const, size_t);
bool cb_read_n(CircularBuffer* const, FILE*, size_t);
bool cb_write_n(CircularBuffer* const, FILE*, size_t);
bool cb_compare(const CircularBuffer* const, const CircularBuffer* const);
void cb_swap(CircularBuffer*, CircularBuffer*);

// zapise vsechna data z bufferu
#define cb_write(buf, out) \
    (cb_write_n((buf), (out), cb_count(buf)))

// nacte do bufferu data do zaplneni
#define cb_read(buf, in) \
    (cb_read_n((buf), (in), (buf)->size_max - cb_count(buf)))

#ifdef __cplusplus
}
#endif

#endif // PROJ1CIRCULARBUFFER_H