/*
 * Piece.h:        Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef PIECE_H
# define PIECE_H

#include "Color.h"
#include <cstdint>

namespace icp {

enum Piece {
    Empty = 0x00,
    WhiteStone = 0x01,
    BlackStone = 0x03,
    WhiteDama = 0x05,
    BlackDama = 0x07,
};

inline bool isPieceEmpty(Piece value)
{
    return !(value & 0x01);
}

inline bool isPieceStone(Piece value)
{
    return !(value & 0x04);
}

inline bool isPieceDama(Piece value)
{
    return value & 0x04;
}

inline bool isPiecePlayers(Piece value, PlayerColor player)
{
    if (isPieceEmpty(value))
        return false;

    switch (player) {
    default:
        return false;

    case PlayerColor::White:
        return !(value & 0x02);

    case PlayerColor::Black:
        return value & 0x02;
    }
}

}

#endif                          //PIECE_H
