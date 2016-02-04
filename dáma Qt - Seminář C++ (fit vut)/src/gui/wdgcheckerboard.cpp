/*
 * wdgcheckerboard.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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

#include "wdgcheckerboard.h"
#include "slotforwarder.h"

#include <QtGui>

#include "Game.h"
#include "Log.h"
#include "Piece.h"

namespace icp {
/* ==================================================================== */
QString WdgCheckerBoard::btnStyle =
    "QPushButton { border: none; }"
    "QPushButton[myprop=\"unplayable\"] { background-color: rgb(255, 255, 255); }"
    "QPushButton[myprop=\"empty\"] { background-color: rgb(0, 0, 0); }"
    "QPushButton[myprop=\"jump\"] { background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(0, 255, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"capture\"] { background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(255, 0, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"capturethrough\"] { background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(164, 0, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"white\"] { color: black; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.0113636 rgba(255, 255, 255, 255), stop:0.479904 rgba(174, 174, 174, 255), stop:0.522727 rgba(0, 0, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"black\"] { color: gold; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.00568182 rgba(255, 55, 55, 255), stop:0.479904 rgba(174, 29, 29, 255), stop:0.522727 rgba(0, 0, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"whiteselected\"] { color: black; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.0113636 rgba(255, 255, 255, 255), stop:0.479904 rgba(174, 174, 174, 255), stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(255, 162, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"blackselected\"] { color: gold; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.00568182 rgba(255, 55, 55, 255), stop:0.479904 rgba(174, 29, 29, 255), stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(255, 162, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"whitemandatory\"] { color: black; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.0113636 rgba(255, 255, 255, 255), stop:0.479904 rgba(174, 174, 174, 255), stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(255, 255, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "QPushButton[myprop=\"blackmandatory\"] { color: gold; background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.00568182 rgba(255, 55, 55, 255), stop:0.479904 rgba(174, 29, 29, 255), stop:0.522727 rgba(0, 0, 0, 255), stop:0.778409 rgba(255, 255, 0, 255), stop:1 rgba(0, 0, 0, 255)); }"
    "";

/* ==================================================================== */
WdgCheckerBoard::WdgCheckerBoard(QWidget *parent, Game *g)
  : QWidget(parent),
    game(g),
    selectedPiece(0, 1),
    checkWhoPulls(true)
{
    QGridLayout *grid = new QGridLayout(this);
    setLayout(grid);

    grid->setSpacing(0);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    grid->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    emit setStyleSheet(btnStyle);

    for (size_t row = 0; row < g->getBoardSize(); ++row) {
        for (size_t col = 0; col < g->getBoardSize(); ++col) {
            QPushButton *btn = new QPushButton(this);
            //emit btn->setDisabled(true);
            if (!((row + col) % 2)) {
                btn->setProperty("myprop", QString("unplayable"));
            }
            else {
                btn->setProperty("myprop", QString("empty"));
            }

            btn->setFixedSize(32, 32);

            Square *loc = new Square(col, g->getBoardSize() - row - 1);
            btn->setToolTip(QString::fromStdString(loc->to_string()));

            auto emitor = [&](void *obj) -> void {
                selectedPiece = *(Square *) obj;
                emit selectedLoc(selectedPiece);
                emit onGameUpdated();
            };

            SlotForwarder *fwd = new SlotForwarder(emitor, loc, this);

            connect(btn, SIGNAL(clicked(bool)),
                    fwd, SLOT(forward(bool)));

            grid->addWidget(btn, row, col);

            grid->setColumnMinimumWidth(col, 32);
        }
        grid->setRowMinimumHeight(row, 32);
    }

    for (size_t i = 0; i < g->getBoardSize(); ++i) {
        QLabel *lbl_rank = new QLabel(QString::number(i + 1), this);
        grid->addWidget(lbl_rank, g->getBoardSize() - i - 1, g->getBoardSize(), Qt::AlignRight | Qt::AlignVCenter);

        QLabel *lbl_file = new QLabel(QChar((char)(i + 'a')), this);
        grid->addWidget(lbl_file, g->getBoardSize(), i, Qt::AlignHCenter | Qt::AlignBottom);
    }
}

/* ==================================================================== */
void WdgCheckerBoard::onGameUpdated()
{
    if (game != nullptr) {
        Q_ASSERT(layout()->count() ==
            game->getBoardSize() * game->getBoardSize());

        for (size_t row = 0; row < game->getBoardSize(); ++row) {
            for (size_t col = 0; col < game->getBoardSize(); ++col) {
                if (!((row + col) % 2)) continue;
                QLayoutItem * const item = qobject_cast<QGridLayout *>(layout())
                    ->itemAtPosition(row, col);

                QPushButton *btn = qobject_cast<QPushButton *>(item->widget());

                switch (game->getPiece(row, col)) {
                default:
                case Piece::Empty:
                    btn->setProperty("myprop", QString("empty"));
                    btn->setText("");
                    //btn->setEnabled(false);
                    break;

                case Piece::WhiteStone:
                    btn->setProperty("myprop", QString("white"));
                    btn->setText("");
                    //btn->setEnabled(true);
                    break;

                case Piece::BlackStone:
                    btn->setProperty("myprop", QString("black"));
                    btn->setText("");
                    //btn->setEnabled(true);
                    break;

                case Piece::WhiteDama:
                    btn->setProperty("myprop", QString("white"));
                    btn->setText("Q");
                    //btn->setEnabled(true);
                    break;

                case Piece::BlackDama:
                    btn->setProperty("myprop", QString("black"));
                    btn->setText("Q");
                    //btn->setEnabled(true);
                    break;
                }
            }
        }

        auto findWdg = [&](Square const& loc, Piece &pc, QWidget *&wdg) {
            QLayoutItem * const item = qobject_cast<QGridLayout *>(layout())
                ->itemAtPosition(game->getBoardSize() - loc.rank - 1,
                                 loc.file);

            pc = game->getPiece(loc);
            wdg = item->widget();
        };

        std::vector<Turn> mandatory = game->getMandatoryMoves();
        for (Turn &i : mandatory) {
            Square const &loc = i.position.front();
            QWidget *wdg = nullptr;
            Piece piece;
            findWdg(loc, piece, wdg);

            if (isPiecePlayers(piece, PlayerColor::White))
                wdg->setProperty("myprop", QString("whitemandatory"));
            else if (isPiecePlayers(piece, PlayerColor::Black))
                wdg->setProperty("myprop", QString("blackmandatory"));
        }

        {
            QWidget *wdg = nullptr;
            Piece piece;
            findWdg(selectedPiece, piece, wdg);

            bool isPlayers;
            if (isPiecePlayers(piece, PlayerColor::White))
                isPlayers = PlayerColor::White == game->pulls();
            else if (isPiecePlayers(piece, PlayerColor::Black))
                isPlayers = PlayerColor::Black == game->pulls();
            else
                isPlayers = false;

            if (checkWhoPulls ? isPlayers : 1) {
                if (!isPieceEmpty(piece)) {
                    if (isPiecePlayers(piece, PlayerColor::White))
                        wdg->setProperty("myprop", QString("whiteselected"));
                    else if (isPiecePlayers(piece, PlayerColor::Black))
                        wdg->setProperty("myprop", QString("blackselected"));

                    std::vector<Turn> avail = game->getAvailableMoves(selectedPiece);
                    for (Turn &i : avail) {
                        Square const &loc = i.position.back();
                        QWidget *wdg = nullptr;
                        Piece piece;
                        findWdg(loc, piece, wdg);

                        if (i.capture) {
                            wdg->setProperty("myprop", QString("capture"));
                            for (auto &th_i : i.position) {
                                if (th_i == i.position.front()) continue;
                                if (th_i == i.position.back()) continue;
                                findWdg(th_i, piece, wdg);
                                wdg->setProperty("myprop", QString("capturethrough"));
                            }
                        }
                        else
                            wdg->setProperty("myprop", QString("jump"));
                    }
                }
            }
        }
    }

    emit setStyleSheet(btnStyle);
}

/* ==================================================================== */
void WdgCheckerBoard::setCheckWhoPulls(bool value)
{
    this->checkWhoPulls = value;
}

}  // namespace icp
