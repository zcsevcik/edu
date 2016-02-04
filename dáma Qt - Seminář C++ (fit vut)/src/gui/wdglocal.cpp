/*
 * wdglocal.cpp:   Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "wdglocal.h"
#include "Game_io.h"
#include "Log.h"

#include <cstddef>
    
#include <QtGui>
    namespace icp {
/* ==================================================================== */
WdgMultiLocal::WdgMultiLocal(QWidget *parent, Game *g)  : WdgGame(parent, g)
{
    static size_t num = 1;

    setWindowTitle(QString("PvP local #%1").arg(num++));

    emit updateBoard();

    connect(this, SIGNAL(destroyed(QObject *)),
            this, SLOT(onDestroyed(QObject *)));
    connect(this, SIGNAL(makingMove(Turn)),
            this, SLOT(onMakingMove(Turn)));
    connect(this, SIGNAL(gameEnded(PlayerColor)),
            this, SLOT(onGameEnded(PlayerColor)));
}

/* ==================================================================== */
void WdgMultiLocal::onDestroyed(QObject *)
{
    delete game, game = nullptr;
}

/* ==================================================================== */
void WdgMultiLocal::onGameEnded(PlayerColor winner)
{
    QString text;
    if (winner == PlayerColor::White)
        text = tr("Vyhral bily hrac.");
    else
        text = tr("Vyhral cerny hrac.");

    QMessageBox::information(this, tr("Konec hry"), text);
}

/* ==================================================================== */
void WdgMultiLocal::onMakingMove(Turn const &move)
{
    if (game->makeMove(move)) {
        emit updateBoard();
    }
}

/* ==================================================================== */
WdgMultiLocal *WdgMultiLocal::create(QWidget *parent)
{
    return new WdgMultiLocal(parent, new Game);
}

/* ==================================================================== */
WdgMultiLocal *WdgMultiLocal::create(QWidget *parent,
                                     QString const &filename)
{
    Game *g = Game_io::load(filename.toStdString());
    return g ? new WdgMultiLocal(parent, g) : nullptr;
}

}  // namespace icp
