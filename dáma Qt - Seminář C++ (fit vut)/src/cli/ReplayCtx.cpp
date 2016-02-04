/*
 * ReplayCtx.cpp:  Navrhnete a implementujte aplikaci pro hru Dama
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

#include "ReplayCtx.h"
#include "Game.h"

#include <iostream>
#include <memory>

namespace icp {
/* ==================================================================== */
ReplayCtx::ReplayCtx(Game *g, ConCtx *parent)
  : GameCtx(g, parent),
    gamerep(new Game_replay(g))
{
    setTitle("Zaznam");
}

/* ==================================================================== */
ReplayCtx::~ReplayCtx() = default;

/* ==================================================================== */
int ReplayCtx::process()
{
    switch (choose("Volba")) {
    default:
        break;
    case 0:
        return 2;
    case 1:
        gamerep->back();
        break;
    case 2:
        gamerep->forward();
        break;
    }

    return 0;
}

/* ==================================================================== */
void ReplayCtx::render()
{
    GameCtx::render();

    std::cout << "Moznosti:" << std::endl;
    std::cout << "  (1) Tah zpet" << std::endl;
    std::cout << "  (2) Tah dopredu" << std::endl;
    std::cout << "  (0) Konec" << std::endl;
}


}  // namespace icp
