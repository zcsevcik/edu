/*
 * Game_bot.cpp:   Navrhnete a implementujte aplikaci pro hru Dama
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

#include "Game_bot.h"
#include "Ai.h"
#include "Game.h"
#include "Move.h"
#include "Log.h"

#include <cassert>
#include <cstddef>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct Game_bot::impl {
    std::unique_ptr<Game> game;
    std::shared_ptr<Ai> ai;
    Turn lastTurn;
    PlayerColor botClr;
};

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
Game_bot::Game_bot(Game *g, PlayerColor botClr)
    : _pImpl(new impl)
{
    _pImpl->game.reset(g);
    _pImpl->botClr = botClr;
}

/* ==================================================================== */
Game_bot::~Game_bot() = default;

/* ==================================================================== */
Game *Game_bot::getGame()
{
    return _pImpl->game.get();
}

/* ==================================================================== */
const std::shared_ptr<Ai> &Game_bot::getAi()
{
    return _pImpl->ai;
}

/* ==================================================================== */
void Game_bot::setAi(const std::shared_ptr<Ai> &value)
{
    _pImpl->ai = value;
}

/* ==================================================================== */
bool Game_bot::think()
{
    if (_pImpl->game->pulls() != _pImpl->botClr) return false;
    if (!_pImpl->ai) return false;
    _pImpl->lastTurn = _pImpl->ai->think(_pImpl->game.get());

    if (!_pImpl->lastTurn.position.empty()) {
        return _pImpl->game->makeMove(_pImpl->lastTurn);
    }

    return false;
}

/* ==================================================================== */
const Turn &Game_bot::lastTurn() const
{
    return _pImpl->lastTurn;
}

/* ==================================================================== */
const PlayerColor &Game_bot::getAiColor() const
{
    return _pImpl->botClr;
}

}  // namespace icp
