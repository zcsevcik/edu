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

#include "Game.h"
#include "GameCmd.h"
#include "CheckerBoard.h"
#include "Move.h"
#include "Piece.h"

#include <cstddef>
#include <deque>
#include <memory>
#include <vector>
#include <tuple>

#ifndef GAME_P_H
# define GAME_P_H

namespace icp {
/* ==================================================================== */
typedef std::deque<std::unique_ptr<IGameCmd>> CmdQueue;
typedef std::tuple<Turn, CmdQueue> TurnCmd;

/* ==================================================================== */
/*      G  A  M  E     ( p r i v a t e   m e m b e r s )                */
/* ==================================================================== */
class Game::impl
{
public:
    static constexpr std::size_t BoardSize = 8;
    typedef CheckerBoard<Piece, BoardSize> Board;

    typedef std::deque<TurnCmd> TurnDeque;

public:
    Game *_base;
    Board _board;
    std::size_t _seqnum;
    PlayerColor _pulls;
    std::vector<Turn> _mandatoryMoves;
    TurnDeque _moves;
    bool _isEnd;
    PlayerColor _looser;

public:
    impl(Game *q);

public:
    Piece &at(const std::size_t &file,
              const std::size_t &rank) {
        return icp::at<Piece, BoardSize>(_board, file, rank);
    }

    const Piece &at(const std::size_t &file,
                    const std::size_t &rank) const {
        return icp::at<Piece, BoardSize>(_board, file, rank);
    }

    Piece &at(const Square &pos) {
        return at(pos.file, BoardSize - pos.rank - 1);
    }

    const Piece &at(const Square &pos) const {
        return at(pos.file, BoardSize - pos.rank - 1);
    }

    static bool not_in_range(const Square &loc) {
        return loc.rank >= BoardSize || loc.file >= BoardSize;
    }

public:
    void promote(const Square &);
    void demote(const Square &);
    void jump(const Square &, const Square &);
    void insert(const Square &, const Piece &);
    Piece capture(const Square &);

public:
    void
    getAvailableMoves(std::vector<Turn> &, const Turn &) const;

    TurnCmd const *
    prepareTurn(const Turn &);

    TurnCmd
    popTurn();

    void
    checkEnd();

public:
    PlayerColor me() const {
        return _pulls;
    }

    PlayerColor opponent() const {
        return _pulls == PlayerColor::White
               ? PlayerColor::Black
               : PlayerColor::White;
    }

    void switchPlayer() {
        _pulls = opponent();
    }
};

}


#endif                          //GAME_P_H
