/*
 * MultiCtx.cpp:  Navrhnete a implementujte aplikaci pro hru Dama
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

#include "MultiCtx.h"
#include "Game.h"
#include "Color.h"
#include "Log.h"

#include <iostream>
#include <memory>

namespace icp {
/* ==================================================================== */
MultiCtx::MultiCtx(Game_net *g, ConCtx *parent)
  : GameCtx(g->getGame(), parent),
    gamenet(g),
    started(false)
{
    setTitle("PvP network");

    gamenet->setCallbackChallenge([&](PlayerColor, const std::vector<Moves> &)
                                  -> bool { return (started = true); });

    gamenet->setCallbackChallengeResponse([&](bool value) {
        started = value ? true : -1;
    });
}

/* ==================================================================== */
MultiCtx::~MultiCtx() = default;

/* ==================================================================== */
int MultiCtx::process()
{
    void *pobj = nullptr;
    command_t cmd;

    if (-1 == started)
        return 1;

    if (!game->isEnd()) {
        if (started && gamenet->mePulls()) {
            std::string player = (PlayerColor::White == gamenet->myColor()
                               ? "x"
                               : "o");
            cmd = command("Tah (" + player + ")", &pobj);
        }
        else {
            std::cout << "Cekam na tah protihrace" << std::endl;
            if (0 != gamenet->waitMessage())
                return 1;
            return 0;
        }
    }
    else {
        std::string player = (gamenet->myColor() == game->winner()
                           ? "vyhral jsi."
                           : "prohral jsi.");

        cmd = command("Konec hry, " + player, &pobj);
    }

    return default_process(cmd, pobj);
}

/* ==================================================================== */
int MultiCtx::default_process(command_t cmd, void *obj)
{
    switch (cmd) {
    default:
        return GameCtx::default_process(cmd, obj);

    case command_t::Move: {
        gamenet->makeMove(* (Turn *) obj);
        delete (Turn *) obj, obj = nullptr;
    } break;
    }

    return 0;
}

/* ==================================================================== */
MultiCtx *MultiCtx::host(ConCtx *parent)
{
    std::unique_ptr<Game_net> net(new Game_net(new Game));
    int res;

    if (0 != (res = net->listen())) {
        print_error(parent, net->errno_to_string(res));
        return nullptr;
    }

    std::cout << "Cekam na vyzyvatele na pocitaci: " << std::endl;
    std::cout << net->getLocalName() << std::endl << std::endl;
    std::cout << "Na adresach:" << std::endl;
    for (auto &i : net->getListenAddr()) {
        std::cout << i << std::endl;
    }
    std::cout << std:: endl;

    if (0 != (res = net->waitAccept())) {
        print_error(parent, net->errno_to_string(res));
        return nullptr;
    }

    return new MultiCtx(net.release(), parent);
}

/* ==================================================================== */
MultiCtx *MultiCtx::join(Game *g, ConCtx *parent)
{
    if (!g) return nullptr;

    std::unique_ptr<Game_net> net(new Game_net(g));
    int res;

    for (;;) {
        std::string addr = ConCtx::prompt("Adresa hosta");
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

    MultiCtx *wdg = new MultiCtx(net.get(), parent);
    net->sendGame(PlayerColor::White);
    return net.release(), wdg;
}

/* ==================================================================== */
void MultiCtx::print_error(ConCtx *, std::string const &msg)
{
    Log::Error("%s", msg.c_str());
}

}  // namespace icp
