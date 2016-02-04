/*
 * MsgMove.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGMOVE_H
# define MSGMOVE_H

#include "MsgBase.h"
#include "Move.h"

namespace icp
{

constexpr MsgType MSG_MOVE = 2;

class MsgMove : public MsgBase
{
    friend class MsgFactory;

public:
    MsgMove() = default;
    ~MsgMove() = default;

    const MsgType &getType() const override {
        return MSG_MOVE;
    };

    const Turn &getMove() const;
    void setMove(const Turn &);

protected:
    Turn _move;                 /**< Tah */
};


inline const Turn &MsgMove::getMove() const
{
    return this->_move;
}

inline void MsgMove::setMove(const Turn &value)
{
    this->_move = value;
}



}

#endif                          //MSGMOVE_H
