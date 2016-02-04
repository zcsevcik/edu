/*
 * Game_replay.h   Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAME_REPLAY_H
# define GAME_REPLAY_H

#include <cstddef>
#include <memory>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
/*      G  A  M  E     R  E  P  L  A  Y                                 */
/* ==================================================================== */
class Game_replay final
{
public:
    Game_replay(Game *);
    ~Game_replay();

    Game *getGame();

    bool back();
    bool forward();
    bool go(std::ptrdiff_t);

protected:
    class impl;
    std::unique_ptr<impl> _pImpl;
};

}

#endif                          //GAME_REPLAY_H
