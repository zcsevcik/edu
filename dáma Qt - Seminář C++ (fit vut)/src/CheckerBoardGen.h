/*
 * Game_p.h:       Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#include "CheckerBoard.h"

#include <cstddef>

#ifndef CHECKERBOARDGEN_H
# define CHECKERBOARDGEN_H

namespace icp
{
/* ==================================================================== */
/*   C  H  E  C  K  E  R  B  O  A  R  D     G  E  N  E  R  A  T  O  R   */
/* ==================================================================== */
template<class T>
class CheckerBoardGen
{
public:
    CheckerBoardGen(const std::size_t &size)
        : _boardSize(size),
          _rank(0),
          _file(0)
    { }

    T operator()(void);

protected:
    std::size_t _boardSize;
    std::size_t _rank;
    std::size_t _file;

    T stone() const;

    std::size_t rankBorder() const {
        return _boardSize / 2 - 1;
    }

    void
    incrementPosition() {
        (++_file %= _boardSize) || _rank++;
    }

    bool
    errorRange() const {
        return _rank >= _boardSize;
    }

    bool
    isDarkSquare() const {
        return (_file + _rank) % 2;
    }

    bool
    isWhiteSquare() const {
        return !isDarkSquare();
    }

    bool
    isBlackSide() const {
        return _rank < rankBorder();
    }

    bool
    isWhiteSide() const {
        return _rank > rankBorder() + 1;
    }

    bool
    shouldPlaceBlack() const {
        return isBlackSide() && isDarkSquare();
    }

    bool
    shouldPlaceWhite() const {
        return isWhiteSide() && isDarkSquare();
    }
};

template<class T>
inline T CheckerBoardGen<T>::operator()(void)
{
    if(errorRange())
        throw std::out_of_range("");

    T t = stone();
    incrementPosition();
    return t;
}

}

#endif                          //CHECKERBOARDGEN_H
