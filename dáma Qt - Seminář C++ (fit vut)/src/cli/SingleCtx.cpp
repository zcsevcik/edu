/*
 * SingleCtx.cpp:  Navrhnete a implementujte aplikaci pro hru Dama
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

#include "SingleCtx.h"
#include "Game.h"
#include "AiRandom.h"
#include "Color.h"

#include <iostream>
#include <memory>

namespace icp {
/* ==================================================================== */
SingleCtx::SingleCtx(Game *g, ConCtx *parent)
  : GameCtx(g, parent),
    gamebot(new Game_bot(g, PlayerColor::Black))
{
    setTitle("PvE");

    static std::shared_ptr<Ai> ai_rand = std::make_shared<AiRandom>();
    gamebot->setAi(ai_rand);
}

/* ==================================================================== */
SingleCtx::~SingleCtx() = default;

/* ==================================================================== */
int SingleCtx::process()
{
    void *pobj = nullptr;
    command_t cmd;

    if (!game->isEnd()) {
        if (gamebot->getAiColor() == game->pulls()) {
            if (gamebot->think())
                return 0;
            else return 2;
        }
        else {
            cmd = command("Tah (x)", &pobj);
        }
    }
    else {
        std::string player = (gamebot->getAiColor() != game->winner()
                           ? "vyhral jsi."
                           : "prohral jsi.");

        cmd = command("Konec hry, " + player, &pobj);
    }

    return default_process(cmd, pobj);
}

/* ==================================================================== */
int SingleCtx::default_process(command_t cmd, void *obj)
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
