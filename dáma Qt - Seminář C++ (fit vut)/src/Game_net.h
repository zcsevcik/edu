/*
 * Game_net.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAME_NET_H
# define GAME_NET_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Color.h"
#include "Move.h"

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
/*      G  A  M  E     N  E  T  W  O  R  K                              */
/* ==================================================================== */
class Game_net final
{
public:
    Game_net(Game *);
    ~Game_net();

    Game *getGame();
    int getSock() const;
    std::string errno_to_string(int) const;

    std::string getLocalName() const;
    std::string getPeerName() const;

    int listen(uint16_t port = 0);
    std::vector<std::string> getListenAddr() const;
    const std::vector<int> &getListenSock() const;
    int waitAccept();

    int beginConnect(std::string host, std::string port);
    int endConnect();

    bool dataAvailable() const;
    int waitMessage();

    bool mePulls() const;
    PlayerColor myColor() const;

    int sendGame(PlayerColor initiatorClr);
    bool makeMove(Turn const &);

    using chlng_fn = std::function<bool (PlayerColor, const std::vector<Moves> &)>;
    void setCallbackChallenge(chlng_fn);

    using rspchlng_fn = std::function<void (bool)>;
    void setCallbackChallengeResponse(rspchlng_fn);

    using move_fn = std::function<void (Turn const &, bool ok)>;
    void setOnMove(move_fn);

    using moveack_fn = std::function<void (bool ack)>;
    void setOnMoveAck(moveack_fn);

    using chat_fn = std::function<void (std::string const &)>;
    void setOnChat(chat_fn);

    using surr_fn = std::function<void ()>;
    void setOnSurrender(surr_fn);


protected:
    class impl;
    std::unique_ptr<impl> _pImpl;
};

}

#endif                          //GAME_NET_H
