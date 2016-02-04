/*
 * countof.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#ifndef COUNTOF_H
#define COUNTOF_H

#include <cstddef>

/* ==================================================================== */
template<class T, std::size_t N>
inline std::size_t _countof(T (&)[N])
{
    return N;
}

#endif                          //COUNTOF_H
