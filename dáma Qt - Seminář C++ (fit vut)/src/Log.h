/*
 * Log.h:          Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef LOG_H
# define LOG_H

#include <functional>

namespace icp {
/* ==================================================================== */
/*      L  O  G                                                         */
/* ==================================================================== */
struct Log final {
    typedef int level_type;
    typedef char char_type;
    using target_type = std::function<void (level_type, const char_type *)>;

    static constexpr level_type LEVEL_NONE = 0;
    static constexpr level_type LEVEL_ALL = 10;

    static constexpr level_type LEVEL_ERROR = 1;
    static constexpr level_type LEVEL_VERBOSE = 2;
    static constexpr level_type LEVEL_DEBUG = 3;
    static constexpr level_type LEVEL_LOGIC = 3;
    static constexpr level_type LEVEL_XML = 4;
    static constexpr level_type LEVEL_SOCKET = 5;

    static void
    Target(const target_type &);

    static void
    Level(const level_type &);

    static const level_type &
    Level();

#define LEVEL_GE(l, ...) do { \
    if (Level() >= l || Level() >= LEVEL_ALL) \
        _Write(l, __VA_ARGS__); \
} while(0)

#define LEVEL_EQ(l, ...) do { \
    if (Level() == l || Level() >= LEVEL_ALL) \
        _Write(l, __VA_ARGS__); \
} while(0)

    template<typename... Targs>
    static void Debug(Targs... args) {
        LEVEL_GE(LEVEL_DEBUG, args...);
    }

    template<typename... Targs>
    static void Error(Targs... args) {
        LEVEL_GE(LEVEL_ERROR, args...);
    }

    template<typename... Targs>
    static void Verbose(Targs... args) {
        LEVEL_GE(LEVEL_VERBOSE, args...);
    }

    template<typename... Targs>
    static void Logic(Targs... args) {
        LEVEL_GE(LEVEL_LOGIC, args...);
    }

    template<typename... Targs>
    static void Xml(Targs... args) {
        LEVEL_GE(LEVEL_XML, args...);
    }

    template<typename... Targs>
    static void Socket(Targs... args) {
        LEVEL_GE(LEVEL_SOCKET, args...);
    }

#define Assert() \
    _Assert(__func__, __FILE__, __LINE__)

#define AssertIf(x, s) \
    _AssertIf(x, #x, s, __func__, __FILE__, __LINE__)

    static void
    _Assert(const char *fn,
            const char *file,
            int line);

    static void
    _AssertIf(bool condition,
              const char *strCondition,
              const char *strError,
              const char *fn,
              const char *file,
              int line);

private:
    struct impl;

    static void
    _Write(level_type l,
           const char_type *fmt,
           ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 2, 3)));
#else
;
#endif
};

}

#endif                          //LOG_H
