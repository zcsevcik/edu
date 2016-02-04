/*
 * MsgChat.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGCHAT_H
# define MSGCHAT_H

#include "MsgBase.h"

#include <string>

namespace icp
{

constexpr MsgType MSG_CHAT = 4;

class MsgChat : public MsgBase
{
    friend class MsgFactory;

public:
    MsgChat() = default;
    MsgChat(const std::string &);
    ~MsgChat() = default;

    const MsgType &getType() const override {
        return MSG_CHAT;
    };

    const std::string &getText() const {
        return _text;
    }
    void setText(const std::string &value) {
        _text = value;
    }

protected:
    std::string _text;
};


inline MsgChat::MsgChat(const std::string &text)
    : _text(text)
{ }


}

#endif                          //MSGCHAT_H
