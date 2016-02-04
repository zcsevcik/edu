/*
 * wndmain.cpp:    Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "wndmain.h"
#include "wdgreplay.h"
#include "wdgsingle.h"
#include "wdglocal.h"
#include "wdgmulti.h"

#include <cstddef>

#include <QtGui>
namespace icp {
/* ==================================================================== */
enum gametype {
    single,
    multiLocal,
    multiHost,
    multiJoin,
};

/* ==================================================================== */
WndMain::WndMain(QWidget *parent, Qt::WindowFlags flags)  : QMainWindow(parent, flags),
    wishColor(PlayerColor::White){
    ui.setupUi(this);

    ui.gameTabs->setCurrentWidget(ui.mainPage);
    emit ui.saveAction->setDisabled(true);

    connect(ui.aboutQtAction, SIGNAL(triggered()),
            QApplication::instance(), SLOT(aboutQt()));
}

/* ==================================================================== */
void WndMain::onAboutTriggered()
{
    QMessageBox::about(this, tr("O aplikaci"),
        tr("Hra dama\n\n"
           "Tato aplikace vznikla jako semestralni projekt "
           "predmetu ICP na FIT VUT v Brne.\n\n"
           "Autor: Radek Sevcik, xsevci44 (at) stud.fit.vutbr.cz\n"));
}

/* ==================================================================== */
int WndMain::getNewGameTabIndex() const
{
    return ui.gameTabs->indexOf(ui.newGameTab);
}

/* ==================================================================== */
void WndMain::addNewGameTab(QWidget *wdg)
{
    emit ui.gameTabs->insertTab(getNewGameTabIndex(),
                                wdg, wdg->windowTitle());
    emit ui.gameTabs->setCurrentWidget(wdg);
}

/* ==================================================================== */
QString WndMain::openGameDlg(bool save)
{
    QString game_dir = QApplication::instance()->applicationDirPath()
                     + QString("/examples/");

    const QString filt = "ICP Dama files (*.xml *.icbf *.ictf);;"
                         "XML files (*.xml);;"
                         "Binary files (*.icbf);;"
                         "Text files (*.ictf);;"
                         "All files (*.*)";

    if (save) {
        return QFileDialog::getSaveFileName(this,
            tr("Uložit partii"), game_dir, filt);
    }
    else {
        return QFileDialog::getOpenFileName(this,
            tr("Otevøít partii"), game_dir, filt);
    }
}

/* ==================================================================== */
void WndMain::onSingleBtnPressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.selectPage);
    emit ui.colorBox->setEnabled(true);

    wishGameType = single;
}

/* ==================================================================== */
void WndMain::onMultiBtnPressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.multiGamePage);
}

/* ==================================================================== */
void WndMain::onReplayBtnPressed()
{
    WdgReplay *rep = new WdgReplay(this, openGameDlg(false));
    addNewGameTab(rep);

    emit ui.stackedWidget->setCurrentWidget(ui.mainPage);
}

/* ==================================================================== */
void WndMain::onMultiLocalPressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.selectPage);
    emit ui.colorBox->setEnabled(false);

    wishGameType = multiLocal;
}

/* ==================================================================== */
void WndMain::onMultiHostPressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.mainPage);

    wishGameType = multiHost;
    onNewGamePressed();
}

/* ==================================================================== */
void WndMain::onMultiJoinPressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.selectPage);
    emit ui.colorBox->setEnabled(true);

    wishGameType = multiJoin;
}

/* ==================================================================== */
void WndMain::onNewGamePressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.mainPage);

    QWidget *wdg = nullptr;

    switch (wishGameType) {
    default:
        return;
    case single:
        wdg = WdgSingle::create(this, wishColor);
        break;
    case multiLocal:
        wdg = WdgMultiLocal::create(this);
        break;
    case multiHost:
        wdg = WdgMulti::host(this);
        break;
    case multiJoin:
        wdg = WdgMulti::join(this, wishColor);
        break;
    }

    if (wdg) addNewGameTab(wdg);
}

/* ==================================================================== */
void WndMain::onLoadGamePressed()
{
    emit ui.stackedWidget->setCurrentWidget(ui.mainPage);

    QWidget *wdg = nullptr;

    switch (wishGameType) {
    default:
        return;
    case single:
        wdg = WdgSingle::create(this, wishColor, openGameDlg(false));
        break;
    case multiLocal:
        wdg = WdgMultiLocal::create(this, openGameDlg(false));
        break;
    case multiJoin:
        wdg = WdgMulti::join(this, wishColor, openGameDlg(false));
        break;
    }

    if (wdg) addNewGameTab(wdg);
}

/* ==================================================================== */
void WndMain::onTabClosing(int index)
{
    if (index == getNewGameTabIndex()) return;

    WdgGame *game = qobject_cast<WdgGame *>(ui.gameTabs->widget(index));
    if (game) {
        QMessageBox::StandardButton answ = QMessageBox::question(this,
            tr("Zavøít hru"), tr("Chcete partii pred zavrenim ulozit"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (answ == QMessageBox::Cancel) return;
        else if (answ == QMessageBox::Save) {
            if (!game->save(openGameDlg(true))) {
                QMessageBox::warning(this, tr("Chyba"),
                                     tr("Pri ukladani doslo k chybe"));
            }
        }
    }

    if (emit ui.gameTabs->widget(index)->close())
        ui.gameTabs->removeTab(index);
}

/* ==================================================================== */
void WndMain::onTabChanged(int index)
{
    WdgGame *game = qobject_cast<WdgGame *>(ui.gameTabs->widget(index));
    emit ui.saveAction->setEnabled(game != nullptr);
}

/* ==================================================================== */
void WndMain::onSaveTriggered()
{
    WdgGame *game = qobject_cast<WdgGame *>(ui.gameTabs->currentWidget());
    if (game && !game->save(openGameDlg(true))) {
        QMessageBox::warning(this, tr("Chyba"),
                             tr("Pri ukladani doslo k chybe"));
    }
}

/* ==================================================================== */
void WndMain::onNewTriggered()
{
    emit ui.stackedWidget->setCurrentWidget(ui.mainPage);
    emit ui.gameTabs->setCurrentWidget(ui.newGameTab);
}

/* ==================================================================== */
void WndMain::onColorWhitePressed()
{
    wishColor = PlayerColor::White;
}

/* ==================================================================== */
void WndMain::onColorBlackPressed()
{
    wishColor = PlayerColor::Black;
}

}  // namespace icp
