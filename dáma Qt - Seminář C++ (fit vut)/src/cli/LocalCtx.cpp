/*
 * LocalCtx.cpp:  Navrhnete a implementujte aplikaci pro hru Dama
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

#include "LocalCtx.h"
#include "Game.h"

#include <iostream>
#include <memory>

namespace icp {
/* ==================================================================== */
LocalCtx::LocalCtx(Game *g, ConCtx *parent)
  : GameCtx(g, parent)
{
    setTitle("PvP mistne");
}

/* ==================================================================== */
LocalCtx::~LocalCtx()
{
    delete game, game = nullptr;
}

/* ==================================================================== */
int LocalCtx::process()
{
    void *pobj = nullptr;
    command_t cmd;

    if (!game->isEnd()) {
        std::string player = (PlayerColor::White == game->pulls()
                           ? "x"
                           : "o");

        cmd = command("Tah (" + player + ")", &pobj);
    }
    else {
        std::string player = (PlayerColor::White == game->winner()
                           ? "x"
                           : "o");

        cmd = command("Konec hry, vyhral (" + player + ")", &pobj);
    }

    return default_process(cmd, pobj);
}

/* ==================================================================== */
int LocalCtx::default_process(command_t cmd, void *obj)
{
    switch (cmd) {
    default:
        return GameCtx::default_process(cmd, obj);

    case command_t::Move: {
        game->makeMove(* (Turn *) obj);
        delete (Turn *) obj, obj = nullptr;
    } break;
    }

    return 0;
}

}  // namespace icp
