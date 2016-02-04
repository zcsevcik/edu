/*
 * wdgreplay.h:    Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WDGREPLAY_H
# define WDGREPLAY_H

#include "Game_replay.h"

#include <memory>

#include "ui_wdgreplay.h"
#include <QWidget>

class QTimer;

namespace icp {
/* ==================================================================== */
class WdgReplay : public QWidget
{
    Q_OBJECT

private:
    Ui::WdgReplay ui;
    
public:
    WdgReplay(QWidget *parent = 0, QString const &file = QString());

public slots:
    void onTurnSelected(QModelIndex const &);
    void onValueChanged(int);
    void onAutoplayToggled(bool);
    void onAutoplayTimedout();
    void onAutoplayIntervalChanged(int);

signals:
    void updateBoard();

private:
    QTimer *tmr;
    std::unique_ptr<Game_replay> gamerep;
    int maxTurn;
    int curTurn;
};

}  // namespace icp

#endif                          //WDGREPLAY_H
