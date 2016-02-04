/*
 * wdglocal.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WDGLOCAL_H
# define WDGLOCAL_H

#include "wdggame.h"
#include "Move.h"

#include <memory>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
class WdgMultiLocal: public WdgGame
{
    Q_OBJECT

public:
    static WdgMultiLocal *create(QWidget *parent = 0);

    static WdgMultiLocal *create(QWidget *parent,
                                 QString const &filename);

private:
    WdgMultiLocal(QWidget *parent, Game *);

public slots:
    void onMakingMove(Turn const &);
    void onGameEnded(PlayerColor winner);

private slots:
    void onDestroyed(QObject *);
};

}  // namespace icp

#endif                          //WDGLOCAL_H
