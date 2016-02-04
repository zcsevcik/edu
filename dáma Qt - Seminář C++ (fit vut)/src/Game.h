/*
 * Game.h:         Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAME_H
# define GAME_H

#include "Piece.h"
#include "Move.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace icp {
/* ==================================================================== */
/*      G  A  M  E                                                      */
/* ==================================================================== */
class Game final
{
    friend class IGameCmd;

public:
    static Game *load(const std::vector<Moves> &);

    Game();
    ~Game();

    std::size_t getBoardSize() const;
    Piece getPiece(boardlen_t row, boardlen_t col) const;
    Piece getPiece(const Square &) const;
    const std::vector<Moves> getMoves() const;
    const std::size_t &getSeqNum() const;

    const PlayerColor &pulls() const;

    bool makeMove(const Turn &);
    Turn undoMove();

    bool isEnd() const;
    PlayerColor winner() const;

    std::vector<Turn>
    getAvailableMoves(const Square &) const;

    std::vector<Turn> const &
    getMandatoryMoves() const;

protected:
    class impl;
    std::unique_ptr<impl> pImpl;
};

}

#endif                          //GAME_H
