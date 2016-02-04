/*
 * CheckerBoard.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef CHECKERBOARD_H
# define CHECKERBOARD_H

#include <array>

namespace icp {
/* ==================================================================== */
/*      C  H  E  C  K  E  R  B  O  A  R  D                              */
/* ==================================================================== */
template<class T, std::size_t N>
using CheckerBoard = std::array<T, N * N>;

/* ==================================================================== */
template<class T, std::size_t N>
inline T &
at(CheckerBoard<T, N> &obj,
   std::size_t file,
   std::size_t rank)
{
    return obj.at(rank * N + file);
}

/* ==================================================================== */
template<class T, std::size_t N>
inline T const &
at(const CheckerBoard<T, N> &obj,
   std::size_t file,
   std::size_t rank)
{
    return obj.at(rank * N + file);
}

}

#endif                          //CHECKERBOARD_H


