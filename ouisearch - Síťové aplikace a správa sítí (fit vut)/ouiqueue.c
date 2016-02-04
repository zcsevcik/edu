/*
 * ouiqueue.c:     Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/28
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "oui_internal.h"
#include "ouiqueue.h"

#define IN6_QUEUE_MAXSIZE 256

// ----------------------------------------------------------------------------
typedef struct {
    size_t size;
    size_t maxsize;
    struct in6_addr* front;
    struct in6_addr* back;
    struct in6_addr* data;
} queue_t;

// ----------------------------------------------------------------------------
static queue_t _in6_queue;
static pthread_mutex_t _queue_lock;
static pthread_cond_t _empty_cv;

// ----------------------------------------------------------------------------
static
void queue_pop(queue_t* p)
{
    assert(p != NULL);
    assert(p->size > 0);

    ptrdiff_t i = p->front - p->data;
    p->front = &p->data[(i + 1) % p->maxsize];
    p->size--;
}

static
void queue_push(queue_t* p, struct in6_addr* value)
{
    assert(p != NULL);
    assert(p->size < p->maxsize);
    assert(value != NULL);

    ptrdiff_t i = p->back - p->data;
    *p->back = *value;
    p->back = &p->data[(i + 1) % p->maxsize];
    p->size++;
}

static
void queue_init(queue_t* p)
{
    assert(p != NULL);
    _in6_queue.size = 0;
    _in6_queue.maxsize = 0;
    _in6_queue.front = NULL;
    _in6_queue.back = NULL;
    _in6_queue.data = NULL;
}

// ----------------------------------------------------------------------------
void ouiqueue_cleanup()
{
    int pth_hr;

    free(_in6_queue.data);

    pth_hr = pthread_cond_destroy(&_empty_cv);
    if (0 != pth_hr) {
        fprintf(stderr, "%s\n", strerror(pth_hr));
    }
    pth_hr = pthread_mutex_destroy(&_queue_lock);
    if (0 != pth_hr) {
        fprintf(stderr, "%s\n", strerror(pth_hr));
    }
}

bool ouiqueue_init()
{
    int pth_hr;

    queue_init(&_in6_queue);

    _in6_queue.data = (struct in6_addr *)malloc(
        IN6_QUEUE_MAXSIZE * sizeof(struct in6_addr));
    if (!_in6_queue.data) {
        fprintf(stderr, "%s\n", strerror(errno));
        return false;
    }
    _in6_queue.maxsize = IN6_QUEUE_MAXSIZE;
    _in6_queue.front = _in6_queue.data;
    _in6_queue.back = _in6_queue.data;

    pth_hr = pthread_mutex_init(&_queue_lock, NULL);
    if (0 != pth_hr) {
        free(_in6_queue.data);
        fprintf(stderr, "%s\n", strerror(pth_hr));
        return false;
    }
    pth_hr = pthread_cond_init(&_empty_cv, NULL);
    if (0 != pth_hr) {
        free(_in6_queue.data);
        pthread_mutex_destroy(&_queue_lock);
        fprintf(stderr, "%s\n", strerror(pth_hr));
        return false;
    }

    return true;
}

bool ouiqueue_push(struct in6_addr addr, struct timespec* time)
{
    int pth_hr = 0;
    bool retval = false;

    pth_hr = pthread_mutex_lock(&_queue_lock); assert(pth_hr == 0);
    while (_in6_queue.size >= IN6_QUEUE_MAXSIZE) {
        pth_hr = pthread_cond_timedwait(&_empty_cv, &_queue_lock, time);

        if (pth_hr == ETIMEDOUT)
            break;
    }

    if (pth_hr == 0) {
        queue_push(&_in6_queue, &addr);
        retval = true;
    }

    pth_hr = pthread_mutex_unlock(&_queue_lock); assert(pth_hr == 0);
    return retval;
}

bool ouiqueue_pop(struct in6_addr* addr)
{
    int pth_hr;
    pth_hr = pthread_mutex_lock(&_queue_lock); assert(pth_hr == 0);

    if (_in6_queue.size > 0) {
        *addr = *_in6_queue.front;
        queue_pop(&_in6_queue);

        pth_hr = pthread_mutex_unlock(&_queue_lock); assert(pth_hr == 0);
        return true;
    }
    else {
        pth_hr = pthread_mutex_unlock(&_queue_lock); assert(pth_hr == 0);
        pth_hr = pthread_cond_signal(&_empty_cv); assert(pth_hr == 0);
        return false;
    }
}
