/*
 * wdggame.cpp:    Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "wdggame.h"
#include "wdgcheckerboard.h"
#include "modelmoves.h"
#include "Game.h"
#include "Game_io.h"
#include "Log.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
    
#include <QtGui>
    namespace icp {
/* ==================================================================== */
WdgGame::WdgGame(QWidget *parent, Game *g)  : QWidget(parent),
    game(g),
    prevLoc(0, 1)
{
    ui.setupUi(this);

    if (g) {
        ui.movesView->setModel(new ModelMoves(this, g));
        board = new WdgCheckerBoard(ui.frame, g);
        connect(this,                   SIGNAL(updateBoard()),
                board,                  SLOT(onGameUpdated()));
        connect(this,                   SIGNAL(updateBoard()),
                ui.movesView->model(),  SLOT(updateData()));
        connect(board,                  SIGNAL(selectedLoc(Square)),
                this,                   SLOT(onSelectedLoc(Square)));
        ui.frame->setFixedSize(board->sizeHint());
    }

    connect(this, SIGNAL(updateBoard()),
            this, SLOT(doCheckEnd()));
}

/* ==================================================================== */
bool WdgGame::save(QString filename)
{
    return Game_io::save(filename.toStdString(), game);
}

/* ==================================================================== */
void WdgGame::onSelectedLoc(Square const &loc)
{
    if (game && !game->isEnd()) {
        const Piece &prevPc = game->getPiece(prevLoc);
        const Piece &pc = game->getPiece(loc);

        bool canBeTurn = isPiecePlayers(prevPc, game->pulls())
            && isPieceEmpty(pc);

        if (canBeTurn) {
            std::vector<Turn> availMoves = game->getAvailableMoves(prevLoc);
            std::vector<Turn> setMoves;
            std::copy_if(begin(availMoves), end(availMoves),
                         std::back_inserter(setMoves),
                         [&](Turn const &t) -> bool {
                    return t.position.front() == prevLoc
                        && t.position.back() == loc;
                }
            );

            auto bestMove = std::max_element(begin(setMoves), end(setMoves),
                             [](Turn const &lhs, Turn const &rhs) -> bool {
                    return lhs.position.size() < rhs.position.size();
                }
            );

            if (bestMove != setMoves.end()) {
                emit makingMove(*bestMove);
                Log::Debug("making move: %s", bestMove->to_string().c_str());
            }
        }
    }

    prevLoc = loc;
}

/* ==================================================================== */
void WdgGame::doCheckEnd()
{
    if (game && game->isEnd())
        emit gameEnded(game->winner());
}

}  // namespace icp
