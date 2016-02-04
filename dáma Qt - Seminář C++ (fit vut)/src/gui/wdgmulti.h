/*
 * wdgmulti.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WDGMULTI_H
# define WDGMULTI_H

#include "wdggame.h"
#include "Game_net.h"
#include "Move.h"

#include <memory>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
class WdgMulti: public WdgGame
{
    Q_OBJECT

public:
    static WdgMulti *host(QWidget *parent);

    static WdgMulti *join(QWidget *parent,
                          PlayerColor myClr);

    static WdgMulti *join(QWidget *parent,
                          PlayerColor myClr,
                          QString const &filename);

private:
    WdgMulti(QWidget *parent, Game_net *);

    static void print_error(QWidget *parent, std::string const &msg);
    bool onChallenge(PlayerColor, const std::vector<Moves> &);

public slots:
    void onMakingMove(Turn const &);
    void onGameEnded(PlayerColor winner);

private slots:
    void checkData();

private:
    std::unique_ptr<Game_net> gamenet;
    QTimer *tmr;
};

}  // namespace icp

#endif                          //WDGMULTI_H
