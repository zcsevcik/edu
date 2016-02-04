/*
 * wdgreplay.cpp:  Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "wdgreplay.h"
#include "wdgcheckerboard.h"
#include "modelmoves.h"
#include "Game.h"
#include "Game_io.h"
#include "Game_replay.h"
#include "Log.h"

#include <cstddef>

#ifdef HAVE_BASENAME_GNU
# include <cstring>
#else
# include <libgen.h>
#endif

#include <QtGui>
    namespace icp {
/* ==================================================================== */
WdgReplay::WdgReplay(QWidget *parent, const QString &file)  : QWidget(parent),
    maxTurn(0)
{
    ui.setupUi(this);

#ifdef HAVE_BASENAME_GNU
    const char *basename_ = basename(file.toStdString().c_str());
#else
    char *basename_ = basename(file.toStdString().c_str());
#endif

    setWindowTitle(QString::fromUtf8(u8"Z\u00E1znam ") + basename_);

    tmr = new QTimer(this);
    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(onAutoplayTimedout()));
    connect(ui.spinBox, SIGNAL(valueChanged(int)),
            this,       SLOT(onAutoplayIntervalChanged(int)));

    Game *g = Game_io::load(file.toStdString());
    if (g) {

        ui.movesView->setModel(new ModelMoves(this, g));
        WdgCheckerBoard *board = new WdgCheckerBoard(ui.frame, g);
        board->setCheckWhoPulls(false);
        connect(this,  SIGNAL(updateBoard()),
                board, SLOT(onGameUpdated()));
        ui.frame->setFixedSize(board->sizeHint());

        maxTurn = g->getSeqNum() * 2 - 1;
        if (PlayerColor::Black == g->pulls()) maxTurn++;

        curTurn = maxTurn;

        ui.horizontalSlider->blockSignals(true);
        ui.horizontalSlider->setMaximum(maxTurn);
        ui.horizontalSlider->setSliderPosition(maxTurn);
        ui.horizontalSlider->blockSignals(false);
        emit ui.pushButton->setEnabled(false);
    }

    gamerep.reset(new Game_replay(g));
    emit updateBoard();
}

/* ==================================================================== */
void WdgReplay::onTurnSelected(const QModelIndex &index)
{
    size_t pos = 2 * index.row();
    if (index.column() == 1) pos++;
    ui.horizontalSlider->setSliderPosition(int(pos));
}

/* ==================================================================== */
void WdgReplay::onValueChanged(int index)
{
    if (!gamerep) return;

    gamerep->go(index - curTurn);

    curTurn = index;
    emit updateBoard();

    bool atEnd = index == ui.horizontalSlider->maximum();
    if (atEnd) emit ui.pushButton->setChecked(false);
    emit ui.pushButton->setDisabled(atEnd);
}

/* ==================================================================== */
void WdgReplay::onAutoplayToggled(bool autoplay)
{
    if (autoplay) {
        emit tmr->start(500);
    }
    else {
        emit tmr->stop();
    }
}

/* ==================================================================== */
void WdgReplay::onAutoplayTimedout()
{
    ui.horizontalSlider->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}

void WdgReplay::onAutoplayIntervalChanged(int msec)
{
    tmr->setInterval(msec * 100);
}

}  // namespace icp
