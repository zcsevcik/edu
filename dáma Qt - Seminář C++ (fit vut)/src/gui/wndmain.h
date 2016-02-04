/*
 * wndmain.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef WNDMAIN_H
# define WNDMAIN_H

#include <QMainWindow>
#include "ui_wndmain.h"

#include "Color.h"

namespace icp {
/* ==================================================================== */
class WndMain : public QMainWindow
{
    Q_OBJECT

private:
    Ui::WndMain ui;
    
public:
    WndMain(QWidget *parent = 0, Qt::WindowFlags flags = 0);

public slots:
    void onSingleBtnPressed();
    void onMultiBtnPressed();
    void onReplayBtnPressed();
    void onMultiLocalPressed();
    void onMultiHostPressed();
    void onMultiJoinPressed();
    void onNewGamePressed();
    void onLoadGamePressed();
    void onColorWhitePressed();
    void onColorBlackPressed();
    void onAboutTriggered();
    void onNewTriggered();
    void onSaveTriggered();
    void onTabClosing(int);
    void onTabChanged(int);

private:
    PlayerColor wishColor;
    int wishGameType;

    void addNewGameTab(QWidget *);
    int getNewGameTabIndex() const;
    QString openGameDlg(bool save);
};

}  // namespace icp

#endif                          //WNDMAIN_H
