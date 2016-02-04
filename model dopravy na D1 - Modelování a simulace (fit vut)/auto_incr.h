/*
 * auto_incr.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#ifndef AUTO_INCR_H
#define AUTO_INCR_H

#include <cstddef>

/* ==================================================================== */
template<class T>
struct auto_incr {
    auto_incr() {
        ++num;
    }

    static std::size_t num;
};

/* ==================================================================== */
template<class T>
std::size_t auto_incr<T>::num = 0;

/* ==================================================================== */

#endif                          //AUTO_INCR_H
