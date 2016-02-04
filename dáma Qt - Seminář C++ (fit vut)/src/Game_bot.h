/*
 * Game_bot.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAME_BOT_H
# define GAME_BOT_H

#include <cstddef>
#include <memory>

#include "Ai.h"
#include "Color.h"
#include "Move.h"

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
/*      G  A  M  E     B  O  T                                          */
/* ==================================================================== */
class Game_bot final
{
public:
    Game_bot(Game *, PlayerColor botClr = PlayerColor::Black);
    ~Game_bot();

    Game *getGame();
    const std::shared_ptr<Ai> &getAi();
    void setAi(const std::shared_ptr<Ai> &);

    const PlayerColor &getAiColor() const;

    bool think();
    const Turn &lastTurn() const;

protected:
    class impl;
    std::unique_ptr<impl> _pImpl;
};

}

#endif                          //GAME_BOT_H
