/*
 * AiRandom.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef AIRANDOM_H
# define AIRANDOM_H

#include <cstddef>
#include <memory>

#include "Ai.h"
#include "Move.h"

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
/*      A  I     R  A  N  D  O  M                                       */
/* ==================================================================== */
class AiRandom : public Ai
{
public:
    AiRandom();
    ~AiRandom();

public:
    virtual Turn think(Game *) override;
};

}  // namespace icp

#endif                          //AIRANDOM_H
