/*
 * Game_replay.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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

#include "Game_replay.h"
#include "Game.h"
#include "Move.h"
#include "Log.h"

#include <cstddef>
#include <stack>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct Game_replay::impl {
    std::unique_ptr<Game> game;
    std::stack<Turn> fwdMoves;
};

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
Game_replay::Game_replay(Game *g)
    : _pImpl(new impl)
{
    _pImpl->game.reset(g);
}

/* ==================================================================== */
Game_replay::~Game_replay() = default;

/* ==================================================================== */
Game *Game_replay::getGame()
{
    return _pImpl->game.get();
}

/* ==================================================================== */
bool Game_replay::back()
{
    Turn mv = _pImpl->game->undoMove();
    if (!mv.position.empty()) {
        _pImpl->fwdMoves.push(mv);
        return true;
    }

    return false;
}

/* ==================================================================== */
bool Game_replay::forward()
{
    if (!_pImpl->fwdMoves.empty()) {
        _pImpl->game->makeMove(_pImpl->fwdMoves.top());
        _pImpl->fwdMoves.pop();
        return true;
    }

    return false;
}

/* ==================================================================== */
bool Game_replay::go(std::ptrdiff_t dx)
{
    const std::ptrdiff_t tmp = dx;

    while (dx != 0) {
        if (dx > 0) {
            if (--dx, !forward()) {
                /* error -> reverse */
                while (tmp > ++dx) back();
                return false;
            }
        }
        else {
            if (++dx, !back()) {
                /* error -> reverse */
                while (tmp < --dx) forward();
                return false;
            }
        }
    }

    return true;
}

}
