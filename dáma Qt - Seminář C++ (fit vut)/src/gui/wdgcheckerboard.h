/*
 * wdgcheckerboard.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WDGCHECKERBOARD_H
# define WDGCHECKERBOARD_H

#include <QWidget>

#include "Move.h"

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
class WdgCheckerBoard : public QWidget
{
    Q_OBJECT

public:
    WdgCheckerBoard(QWidget *parent = 0, Game * = nullptr);

    void setCheckWhoPulls(bool);

public slots:
    void onGameUpdated();

signals:
    void selectedLoc(Square);

private:
    Game *game;
    static QString btnStyle;
    Square selectedPiece;
    bool checkWhoPulls;
};

}  // namespace icp

#endif                          //WDGCHECKERBOARD_H
