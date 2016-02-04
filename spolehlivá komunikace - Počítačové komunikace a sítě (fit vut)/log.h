/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifndef IPK_LOG_H
#define IPK_LOG_H

#include <errno.h>
#include <stdarg.h>

#if defined(__GNUC__)
# define __MYFUNCTION__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
# define __MYFUNCTION__ __FUNCSIG__
#else // C99
# define __MYFUNCTION__ __func__
#endif

#define _ipk_assert() \
    _ipk_debug_log("ASSERT: %s at %s:%d", \
                  __MYFUNCTION__, __FILE__, __LINE__)

#define _ipk_assert_if(x) do { \
    if (!(x)) \
        _ipk_debug_log("ASSERT(%s): %s at %s:%d", \
                      #x, __MYFUNCTION__, __FILE__, __LINE__); \
} while(0)

#define _ipk_assert_val(x) \
    _ipk_debug_log("ASSERT(#%d): %s at %s:%d", \
                  (int)(x), __MYFUNCTION__, __FILE__, __LINE__)


#define _ipk_error_log(...) LEVEL(1, __VA_ARGS__)
#define _ipk_verbose_log(...) LEVEL(2, __VA_ARGS__)
#define _ipk_debug_log(...) LEVEL(3, __VA_ARGS__)

#define LEVEL(l, ...) do { \
    if (_ipk_log_level >= l || _ipk_log_level > 9) \
        _ipk_log(l, __VA_ARGS__); \
} while(0)

#define LEVEL_EQ(l, ...) do { \
    if (_ipk_log_level == l || _ipk_log_level > 9) \
        _ipk_log(l, __VA_ARGS__); \
} while(0)


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ipk_log_fn)(int, const char*);

extern int _ipk_log_level;
extern ipk_log_fn _ipk_log_fn;

void
_ipk_log(
        int lvl,
        const char* fmt,
        ...)
#ifdef __GNUC__
  __attribute__ ((format (printf, 2, 3)));
#else
  ;
#endif


void
ipk_set_log_level(
        int);

void
ipk_set_log_func(
        ipk_log_fn);

void
ipk_perror(
        int);

#ifdef _WIN32
# include <windows.h>
void
ipk_perror_win(
        unsigned long);
#endif

#ifdef __cplusplus
}
#endif

#endif //IPK_LOG_H
