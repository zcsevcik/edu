/*
 * Ai.h:           Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef AI_H
# define AI_H

#include <cstddef>
#include <memory>

#include "Move.h"

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
/*      A  I                                                            */
/* ==================================================================== */
class Ai
{
public:
    virtual Turn think(Game *) = 0;

protected:
    Ai();
    virtual ~Ai();
};

/* ==================================================================== */
inline Ai::Ai() = default;

/* ==================================================================== */
inline Ai::~Ai() = default;

}  // namespace icp

#endif                          //AI_H
