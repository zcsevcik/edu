/*
 * wdgmulti.cpp:   Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "wdgmulti.h"
#include "Game_io.h"
#include "Game_net.h"
#include "Log.h"

#include <cstddef>
#include <numeric>
#include <string>
#include <vector>

#include <QtGui>
    namespace icp {
/* ==================================================================== */
WdgMulti::WdgMulti(QWidget *parent, Game_net *g)  : WdgGame(parent, g->getGame()),
    gamenet(g)
{
    static size_t num = 1;
    setWindowTitle(QString("PvP #%1").arg(num++));

    emit updateBoard();

    connect(this, SIGNAL(makingMove(Turn)),
            this, SLOT(onMakingMove(Turn)));
    connect(this, SIGNAL(gameEnded(PlayerColor)),
            this, SLOT(onGameEnded(PlayerColor)));

    tmr = new QTimer(this);
    connect(tmr, SIGNAL(timeout()),
            this, SLOT(checkData()));
    tmr->setInterval(1000);
    emit tmr->start(200);

    using namespace std::placeholders;
    gamenet->setCallbackChallenge(std::bind(&WdgMulti::onChallenge, this, _1, _2));
}

/* ==================================================================== */
void WdgMulti::checkData()
{
    if (gamenet->dataAvailable()) {
        int res;
        if (0 != (res = gamenet->waitMessage())) {
            print_error(this, gamenet->errno_to_string(res));
            emit tmr->stop();
            close();
        }
        else {
            emit updateBoard();
        }
    }
}

/* ==================================================================== */
bool WdgMulti::onChallenge(PlayerColor, const std::vector<Moves> &mvs)
{
    auto count_strlen = [](size_t init, Moves const &mv) -> size_t {
        return init + mv.to_string().size() + 1;
    };

    size_t mvslen = std::accumulate(begin(mvs), end(mvs),
                                    0, count_strlen);

    std::string smvs;
    smvs.reserve(mvslen);

    for (auto &i : mvs) {
        smvs.append(i.to_string());
        smvs.push_back('\n');
    }

    QMessageBox::StandardButton res = QMessageBox::information(
        this, tr("Výzva"), tr("Prijmes vyzvu:\n") + QString::fromStdString(smvs),
        QMessageBox::Yes | QMessageBox::No);

    return res == QMessageBox::Yes;
}

/* ==================================================================== */
void WdgMulti::print_error(QWidget *parent, std::string const &msg)
{
    QMessageBox::warning(parent, tr("Network error"),
                         QString::fromStdString(msg));
}

/* ==================================================================== */
void WdgMulti::onGameEnded(PlayerColor winner)
{
    QString text;
    if (winner == gamenet->myColor())
        text = tr("Gratuluji, jsi vitez.");
    else
        text = tr("Lituji, prohral jsi.");

    QMessageBox::information(this, tr("Konec hry"), text);
}

/* ==================================================================== */
void WdgMulti::onMakingMove(Turn const &move)
{
    if (gamenet->mePulls() && gamenet->makeMove(move)) {
        emit updateBoard();
    }
}

/* ==================================================================== */
WdgMulti *WdgMulti::host(QWidget *parent)
{
    std::unique_ptr<Game_net> net(new Game_net(new Game));
    int res;

    if (0 != (res = net->listen())) {
        print_error(parent, net->errno_to_string(res));
        return nullptr;
    }

    auto count_strlen = [](size_t init, std::string const &str) -> size_t {
        return init + str.size() + 1;
    };

    std::vector<std::string> addrs = net->getListenAddr();
    size_t addrlen = std::accumulate(begin(addrs), end(addrs),
                                     0, count_strlen);

    std::string addr;
    addr.reserve(addrlen);

    for (auto &i : addrs) {
        addr.append(i);
        addr.push_back('\n');
    }

    QMessageBox::information(parent, tr("Cekam na vyzvu"),
                             tr("Cekam na vyzyvatele na pocitaci: ") +
                             QString::fromStdString(net->getLocalName()) +
                             tr("\n\nNa adresach:\n") +
                             QString::fromStdString(addr));

    if (0 != (res = net->waitAccept())) {
        print_error(parent, net->errno_to_string(res));
        return nullptr;
    }

    return new WdgMulti(parent, net.release());
}

/* ==================================================================== */
WdgMulti *WdgMulti::join(QWidget *parent, PlayerColor wishClr)
{
    std::unique_ptr<Game_net> net(new Game_net(new Game));
    int res;

    for (;;) {
        bool ok;
        std::string addr = QInputDialog::getText(parent,
            tr("Adresa hosta"), tr("Adresa hosta:"),
            QLineEdit::Normal, QString(), &ok).toStdString();

        if (!ok) return nullptr;

        std::string::size_type delim = addr.find_last_of(':');
        if (delim == std::string::npos) continue;

        std::string host = addr.substr(0, delim);
        std::string port = addr.substr(delim + 1);

        if (*host.begin() == '[' && *host.rbegin() == ']') {
            host.pop_back();
            host.erase(0, 1);
        }

        if (0 != (res = net->beginConnect(host, port))) {
            print_error(parent, net->errno_to_string(res));
            continue;
        }

        while (EINPROGRESS == (res = net->endConnect()));
        if (0 == res) break;

        print_error(parent, net->errno_to_string(res));
    }

    WdgMulti *wdg = new WdgMulti(parent, net.get());
    net->sendGame(wishClr);
    return net.release(), wdg;
}

/* ==================================================================== */
WdgMulti *WdgMulti::join(QWidget *parent, PlayerColor wishClr,
                         QString const &filename)
{
    Game *g = Game_io::load(filename.toStdString());
    if (!g) return nullptr;

    std::unique_ptr<Game_net> net(new Game_net(g));
    int res;

    for (;;) {
        bool ok;
        std::string addr = QInputDialog::getText(parent,
            tr("Adresa hosta"), tr("Adresa hosta:"),
            QLineEdit::Normal, QString(), &ok).toStdString();

        if (!ok) return nullptr;

        std::string::size_type delim = addr.find_last_of(':');
        if (delim == std::string::npos) continue;

        std::string host = addr.substr(0, delim);
        std::string port = addr.substr(delim + 1);

        if (*host.begin() == '[' && *host.rbegin() == ']') {
            host.pop_back();
            host.erase(0, 1);
        }

        if (0 != (res = net->beginConnect(host, port))) {
            print_error(parent, net->errno_to_string(res));
            continue;
        }

        while (EINPROGRESS == (res = net->endConnect()));
        if (0 == res) break;

        print_error(parent, net->errno_to_string(res));
    }

    WdgMulti *wdg = new WdgMulti(parent, net.get());
    net->sendGame(wishClr);
    return net.release(), wdg;
}

}  // namespace icp
