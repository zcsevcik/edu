/*
 * AiRandom.cpp:   Navrhnete a implementujte aplikaci pro hru Dama
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

#include "AiRandom.h"
#include "Game.h"
#include "Move.h"
#include "Log.h"

#include <cassert>
#include <cstddef>
#include <random>
#include <vector>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct AiRandomCaptureHeuristic final {
    bool operator()(const Turn &lhs, const Turn &rhs) {
        return lhs.position.size() < rhs.position.size();
    }
};

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
AiRandom::AiRandom() = default;

/* ==================================================================== */
AiRandom::~AiRandom() = default;

/* ==================================================================== */
Turn AiRandom::think(Game *g)
{
    if (!g) return { };
    if (g->isEnd()) return { };

    /* do best mandatory move if available */
    const std::vector<Turn> &mandatory = g->getMandatoryMoves();
    if (!mandatory.empty()) {
        auto bestMove = std::max_element(begin(mandatory), end(mandatory),
                                         AiRandomCaptureHeuristic());

        if (bestMove == mandatory.end()) return mandatory.front();
        else return *bestMove;
    }

    /* available stones */
    std::vector<Square> myLoc;
    myLoc.reserve((((g->getBoardSize() - 2) / 2) * g->getBoardSize()) / 2);

    for (size_t file = 0, rank = 0;
         rank < g->getBoardSize();
         (++file %= g->getBoardSize()) || rank++)
    {
        const Square loc(file, rank);
        if (!isPiecePlayers(g->getPiece(loc), g->pulls())) continue;

        myLoc.push_back(loc);
    }

    /* generator */
    std::random_device gen;

    /* randomly select one piece */
    std::uniform_int_distribution<size_t> locDis(0, myLoc.size() - 1);

    for (;;) {
        /* get available moves; if none, select next piece */
        const Square &loc = myLoc.at(locDis(gen));
        std::vector<Turn> myTurns = g->getAvailableMoves(loc);
        if (myTurns.empty()) continue;

        std::uniform_int_distribution<size_t> turnDis(0, myTurns.size() - 1);
        return myTurns.at(turnDis(gen));
    }
}

}  // namespace icp
