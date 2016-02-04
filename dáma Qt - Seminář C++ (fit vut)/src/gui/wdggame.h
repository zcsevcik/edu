/*
 * wdggame.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WDGGAME_H
# define WDGGAME_H

#include "ui_wdggame.h"
#include "wdgcheckerboard.h"

#include "Color.h"
#include "Move.h"

#include <QWidget>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
class WdgGame : public QWidget
{
    Q_OBJECT

public:
    bool save(QString filename);

protected:
    Ui::WdgGame ui;
    WdgCheckerBoard *board;
    Game *game;
    
protected:
    WdgGame(QWidget *parent, Game *);

public slots:
    void onSelectedLoc(Square const &);

private slots:
    void doCheckEnd();

signals:
    void updateBoard();
    void makingMove(Turn);
    void gameEnded(PlayerColor winner);

private:
    Square prevLoc;
};

}  // namespace icp

#endif                          //WDGGAME_H
