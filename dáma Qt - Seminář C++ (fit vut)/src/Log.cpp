/*
 * Log.cpp:        Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <alloca.h>
#include <cassert>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Log.h"

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct Log::impl final {
    static level_type lvl;
    static target_type fn;
};

Log::level_type Log::impl::lvl = 0;
Log::target_type Log::impl::fn = nullptr;

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
constexpr Log::level_type Log::LEVEL_NONE;
constexpr Log::level_type Log::LEVEL_ALL;

constexpr Log::level_type Log::LEVEL_ERROR;
constexpr Log::level_type Log::LEVEL_VERBOSE;
constexpr Log::level_type Log::LEVEL_DEBUG;
constexpr Log::level_type Log::LEVEL_LOGIC;
constexpr Log::level_type Log::LEVEL_XML;
constexpr Log::level_type Log::LEVEL_SOCKET;

/* ==================================================================== */
void
Log::Target(const target_type &val)
{
    impl::fn = val;
}

/* ==================================================================== */
void
Log::Level(const level_type &val)
{
    impl::lvl = val;
}

/* ==================================================================== */
const Log::level_type &
Log::Level()
{
    return impl::lvl;
}

/* ==================================================================== */
void
Log::_Write(level_type lvl, const char_type *fmt, ...)
{
    std::va_list args;
    char *str = nullptr;
    int ret = 0;

    if (!impl::fn) return;

    va_start(args, fmt);

#ifdef _MSC_VER
    str = (char *)alloca((_vscprintf(fmt, args) + 1) * sizeof(char));
    ret = vsprintf(str, fmt, args);
#elif HAVE_VASPRINTF
    ret = vasprintf(&str, fmt, args);
#else
    str = (char *)alloca(4096 * sizeof(char));
    ret = vsnprintf(str, 4096, fmt, args);
#endif

    assert(str != nullptr);
    assert(ret > 0);

    va_end(args);

    if (ret > 0) {
        impl::fn(lvl, str);
    }

#if !defined(_MSC_VER) && defined(HAVE_VASPRINTF)
    free(str);
#endif
}

/* ==================================================================== */
void
Log::_Assert(const char *fn, const char *file, int line)
{
    Debug("ASSERT: %s at %s:%d",
          fn, file, line);
}

/* ==================================================================== */
void
Log::_AssertIf(bool cond, const char *strCond, const char *strErr,
               const char *fn, const char *file, int line)
{
    if (!cond) {
        Debug("ASSERT(%s): %s : %s at %s:%d",
              strCond, strErr, fn, file, line);
    }
}

}
