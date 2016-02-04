/*
 * MsgSurrender.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGSURRENDER_H
# define MSGSURRENDER_H

#include "MsgBase.h"

namespace icp
{

constexpr MsgType MSG_SURRENDER = 5;

class MsgSurrender : public MsgBase
{
    friend class MsgFactory;

public:
    MsgSurrender() = default;
    MsgSurrender(const std::string &);
    ~MsgSurrender() = default;

    const MsgType &getType() const override {
        return MSG_SURRENDER;
    };

};


}

#endif                          //MSGSURRENDER_H
