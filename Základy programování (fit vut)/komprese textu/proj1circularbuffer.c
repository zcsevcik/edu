/*
 * Soubor:  proj1circularbuffer.c
 * Datum:   2010/10/07
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Jednoducha komprese textu, projekt c. 1 pro predmet IZP
 * Popis:   Program komprimuje nebo dekomprimuje text podle zadane delky bloku.
 */

#include "proj1circularbuffer.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// precte ze streamu n znaku
// @return true pokud operace probehla uspesne nebo nastalo EOF; pri chybe zpracovani false
bool cb_read_n(CircularBuffer* const buf, FILE* fInput, size_t count)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(fInput != NULL);

    if (buf->size_max - cb_count(buf) < count) {
        return false;
    }

    for (; count > 0; --count) {
        int nextChar = fgetc(fInput);
        
        if (nextChar == EOF) {
            return !ferror(fInput);
        }
        else if (isdigit(nextChar)) {
            return false;
        }
        else if (isprint(nextChar) || isspace(nextChar)) {
            cb_push_back(buf, (char)nextChar);
        }
        else {
            return false;
        }
    }

    return true;
}

// zapise do streamu n znaku
// @return true pokud operace probehla uspesne; false pri chybe zapisu
bool cb_write_n(CircularBuffer* const buf, FILE* fOutput, size_t count)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(fOutput != NULL);
    
    if (cb_count(buf) < count) {
        return false;
    }

    for (; count > 0; --count) {
        char c = cb_pop_front(buf);
        if (c == '\0') {
            return false;
        }
        if (fputc((int)c, fOutput) == EOF) {
            return false;
        }
    }

    return true;
}

// prohodi buffery
void cb_swap(CircularBuffer* buf1, CircularBuffer* buf2)
{
    assert(buf1 != NULL);
    assert(buf1->buf != NULL);
    assert(buf1->size_max > 0);
    assert(buf2 != NULL);
    assert(buf2->buf != NULL);
    assert(buf2->size_max > 0);

    char* tempBuf = buf1->buf;
    size_t tempSize = buf1->size_max;
    char* tempStart = buf1->start;
    char* tempEnd = buf1->end;

    buf1->buf = buf2->buf;
    buf1->size_max = buf2->size_max;
    buf1->start = buf2->start;
    buf1->end = buf2->end;

    buf2->buf = tempBuf;
    buf2->size_max = tempSize;
    buf2->start = tempStart;
    buf2->end = tempEnd;
}

// porovna obsah dat v bufferech
// @return true je-li obsah bufferu stejny; jinak false
bool cb_compare(const CircularBuffer* const buf1, const CircularBuffer* const buf2)
{
    assert(buf1 != NULL);
    assert(buf1->buf != NULL);
    assert(buf1->size_max > 0);
    assert(buf2 != NULL);
    assert(buf2->buf != NULL);
    assert(buf2->size_max > 0);

    if (buf1 == buf2) {
        return true;
    }
    if (buf1->buf == buf2->buf) {
        return true;
    }
    if (buf1->start == NULL || buf1->end == NULL ||
        buf2->start == NULL || buf2->end == NULL)
    {
        return false;
    }
    if (cb_count(buf1) != cb_count(buf2)) {
        return false;
    }

    size_t count = cb_count(buf1);
    for (size_t i = 0; i < count; ++i) {
        if (cb_peek(buf1, i) != cb_peek(buf2, i)) {
            return false;
        }
    }

    return true;
}

// inicializuje kruhovy buffer
bool cb_init(CircularBuffer* const buf, size_t size_max)
{
    assert(buf != NULL);
    assert(size_max > 0);

    buf->buf = (char*)malloc(size_max);
    buf->size_max = buf->buf != NULL ? size_max : 0;
    buf->start = NULL;
    buf->end = NULL;
    
    return buf->buf != NULL;
}

void cb_free(CircularBuffer* const buf)
{
    assert(buf != NULL);
    free(buf->buf);

    buf->buf = NULL;
    buf->size_max = 0;
    buf->start = NULL;
    buf->end = NULL;
}

// vycisti buffer
void cb_clear(CircularBuffer* buf)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(buf->size_max > 0);

    buf->start = NULL;
    buf->end = NULL;
}

// zjisti zabranou velikost v bufferu
size_t cb_count(const CircularBuffer* const buf)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(buf->size_max > 0);

    if (buf->start == NULL || buf->end == NULL) {
        return 0;
    }
    else if (buf->start == buf->end) {
        return 1;
    }
    else {
        if (buf->end > buf->start) {
            return 1 + (buf->end - buf->start);
        }
        else {
            return 1 + buf->size_max - (buf->start - buf->end);
        }
    }
}

// precte znak z bufferu bez vymazani
// pokud index presahuje platna data, vrati se NULL
char cb_peek(const CircularBuffer* const buf, size_t index)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(buf->size_max > 0);

    if (index >= cb_count(buf)) {
        return '\0';
    }

    size_t offset = buf->start - buf->buf;
    char* position = buf->buf + ((offset + index) % buf->size_max);
    return *position;
}

// vlozi znak do bufferu
// pokud je buffer plny, znak se zahodi
// @param c znak
void cb_push_back(CircularBuffer* const buf, char c)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(buf->size_max > 0);

    if (cb_count(buf) >= buf->size_max) {
        return;
    }

    if (buf->start == NULL || buf->end == NULL) {
        buf->start = buf->buf;
        buf->end = buf->buf;
    }
    else if (buf->end >= buf->buf + buf->size_max - 1) {
        buf->end = buf->buf;
    }
    else {
        buf->end++;
    }

    *buf->end = c;
}

// precte znak z bufferu a vymaze jej
// pokud je buffer prazdny, vrati NULL
// @return precteny znak
char cb_pop_front(CircularBuffer* const buf)
{
    assert(buf != NULL);
    assert(buf->buf != NULL);
    assert(buf->size_max > 0);
    
    if (cb_count(buf) == 0) {
        return '\0';
    }

    char c = *buf->start;

    if (buf->start == buf->end) {
        buf->start = NULL;
        buf->end = NULL;
    }
    else if (buf->start >= buf->buf + buf->size_max - 1) {
        buf->start = buf->buf;
    }
    else {
        buf->start++;
    }

    return c;
}