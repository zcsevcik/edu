/*
 * MyEndian.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MYENDIAN_H
# define MYENDIAN_H

#include <cstdint>
#include <type_traits>

#define _USE_BSD
#include <sys/types.h>
#include <sys/param.h>

namespace icp {
/* ==================================================================== */
/*      B  Y  T  E       S  W  A  P                                     */
/* ==================================================================== */
template<std::size_t N, class T>
using IfSize = typename std::enable_if
    <sizeof(T) == N, T>::type;

/* ==================================================================== */
template<class T>
inline IfSize<sizeof(std::uint16_t), T>
byteswap(T value)
{
#if defined(__GNUC__) && defined(__builtin_bswap16)
    return __builtin_bswap16(value);
#elif defined(_MSC_VER)
    return _byteswap_ushort(value);
#else
    return value << 0x08 |
           value >> 0x08;
#endif
}

/* ==================================================================== */
template<class T>
inline IfSize<sizeof(std::uint32_t), T>
byteswap(T value)
{
#if defined(__GNUC__)
    return __builtin_bswap32(value);
#elif defined(_MSC_VER)
    return _byteswap_ulong(value);
#else
    return value >> 0x18 |
           value << 0x08 & 0xffU << 0x10 |
           value >> 0x08 & 0xffU << 0x08 |
           value << 0x18;
#endif
}

/* ==================================================================== */
template<class T>
inline IfSize<sizeof(std::uint64_t), T>
byteswap(T value)
{
#if defined(__GNUC__)
    return __builtin_bswap64(value);
#elif defined(_MSC_VER)
    return _byteswap_uint64(value);
#else
    return value >> 0x38 |
           value << 0x28 & 0xffUL << 0x30 |
           value << 0x18 & 0xffUL << 0x28 |
           value << 0x08 & 0xffUL << 0x20 |
           value >> 0x08 & 0xffUL << 0x18 |
           value >> 0x18 & 0xffUL << 0x10 |
           value >> 0x28 & 0xffUL << 0x08 |
           value << 0x38;
#endif
}

/* ==================================================================== */
/*      B  Y  T  E     O  R  D  E  R                                    */
/* ==================================================================== */
template<typename T>
using IfBigEndian = typename std::enable_if
    <__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__, T>::type;

template<typename T>
using IfLittleEndian = typename std::enable_if
    <__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, T>::type;

/* ==================================================================== */
/*      H  O  S  T     T  O     N  E  T  W  O  R  K                     */
/* ==================================================================== */
template<class T>
inline IfLittleEndian<T>
hton(T value)
{
    return byteswap(value);
}

template<class T>
inline IfBigEndian<T>
hton(T value)
{
    return value;
}

/* ==================================================================== */
/*      N  E  T  W  O  R  K     T  O     H  O  S  T                     */
/* ==================================================================== */
template<class T>
inline IfLittleEndian<T>
ntoh(T value)
{
    return byteswap(value);
}

template<class T>
inline IfBigEndian<T>
ntoh(T value)
{
    return value;
}

/* ==================================================================== */
/*      H  O  S  T     T  O     L  I  T  T  L  E     E  N  D  I  A  N   */
/* ==================================================================== */
template<class T>
inline IfLittleEndian<T>
htole(T value)
{
    return value;
}

template<class T>
inline IfBigEndian<T>
htole(T value)
{
    return byteswap(value);
}

/* ==================================================================== */
/*      H  O  S  T     T  O     B  I  G     E  N  D  I  A  N            */
/* ==================================================================== */
template<class T>
inline IfLittleEndian<T>
htobe(T value)
{
    return byteswap(value);
}

template<class T>
inline IfBigEndian<T>
htobe(T value)
{
    return value;
}

/* ==================================================================== */
/*      L  I  T  T  L  E     E  N  D  I  A  N     T  O     H  O  S  T   */
/* ==================================================================== */
template<class T>
inline IfLittleEndian<T>
letoh(T value)
{
    return value;
}

template<class T>
inline IfBigEndian<T>
letoh(T value)
{
    return byteswap(value);
}

/* ==================================================================== */
/*      B  I  G     E  N  D  I  A  N     T  O     H  O  S  T            */
/* ==================================================================== */
template<class T>
inline IfLittleEndian<T>
betoh(T value)
{
    return byteswap(value);
}

template<class T>
inline IfBigEndian<T>
betoh(T value)
{
    return value;
}

}

#endif                          //MYENDIAN_H
