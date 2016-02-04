/*
 * wdgsingle.h:    Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WDGSINGLE_H
# define WDGSINGLE_H

#include "wdggame.h"
#include "Game_bot.h"
#include "Move.h"

#include <memory>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
class WdgSingle: public WdgGame
{
    Q_OBJECT

public:
    static WdgSingle *create(QWidget *parent,
                             PlayerColor botClr);

    static WdgSingle *create(QWidget *parent,
                             PlayerColor botClr,
                             QString const &filename);

private:
    WdgSingle(QWidget *parent, Game *, PlayerColor botClr);

public slots:
    void onBotThinkTimeout();
    void onMakingMove(Turn const &);
    void onGameEnded(PlayerColor winner);

private:
    std::unique_ptr<Game_bot> gamebot;
    bool botThinks;
};

}  // namespace icp

#endif                          //WDGSINGLE_H
