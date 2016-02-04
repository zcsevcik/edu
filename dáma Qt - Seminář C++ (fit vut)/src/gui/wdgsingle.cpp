/*
 * wdgsingle.cpp:  Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "wdgsingle.h"
#include "Game_io.h"
#include "AiRandom.h"
#include "Log.h"

#include <cstddef>
    
#include <QtGui>
#include <QTimer>
    namespace icp {
/* ==================================================================== */
WdgSingle::WdgSingle(QWidget *parent, Game *g, PlayerColor myClr)  : WdgGame(parent, g),
    botThinks(false)
{
    static size_t num = 1;
    static std::shared_ptr<Ai> ai_rand = std::make_shared<AiRandom>();

    setWindowTitle(QString("PvE #%1").arg(num++));

    emit updateBoard();

    PlayerColor botClr = PlayerColor::White == myClr
                       ? PlayerColor::Black
                       : PlayerColor::White;
    gamebot.reset(new Game_bot(g, botClr));
    gamebot->setAi(ai_rand);

    connect(this, SIGNAL(makingMove(Turn)),
            this, SLOT(onMakingMove(Turn)));
    connect(this, SIGNAL(gameEnded(PlayerColor)),
            this, SLOT(onGameEnded(PlayerColor)));

    if (g->pulls() == botClr) {
        botThinks = true;
        QTimer::singleShot(2000, this, SLOT(onBotThinkTimeout()));
    }
}

/* ==================================================================== */
void WdgSingle::onGameEnded(PlayerColor winner)
{
    QString text;
    if (winner == gamebot->getAiColor())
        text = tr("Lituji, prohral jsi.");
    else
        text = tr("Gratuluji, jsi vitez.");

    QMessageBox::information(this, tr("Konec hry"), text);
}

/* ==================================================================== */
void WdgSingle::onBotThinkTimeout()
{
    if (game->isEnd()) return;

    botThinks = !gamebot->think();
    if (botThinks) QMessageBox::warning(this, tr("AI Error"),
        tr("Nastala chyba, je AI na rade?"));

    emit updateBoard();
}

/* ==================================================================== */
void WdgSingle::onMakingMove(Turn const &move)
{
    bool isMyMove = game->pulls() != gamebot->getAiColor();

    if (isMyMove && game->makeMove(move)) {
        emit updateBoard();

        botThinks = true;
        QTimer::singleShot(2000, this, SLOT(onBotThinkTimeout()));
    }
}

/* ==================================================================== */
WdgSingle *WdgSingle::create(QWidget *parent, PlayerColor botClr)
{
    return new WdgSingle(parent, new Game, botClr);
}

/* ==================================================================== */
WdgSingle *WdgSingle::create(QWidget *parent, PlayerColor botClr,
                             QString const &filename)
{
    Game *g = Game_io::load(filename.toStdString());
    return g ? new WdgSingle(parent, g, botClr) : nullptr;
}

}  // namespace icp
