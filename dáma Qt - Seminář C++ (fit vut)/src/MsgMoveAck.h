/*
 * MsgMoveAck.h:   Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGMOVEACK_H
# define MSGMOVEACK_H

#include "MsgBase.h"
#include "Move.h"

namespace icp
{

enum class MoveAck
{
    NACK,
    ACK,
};

constexpr MsgType MSG_MOVE_ACK = 3;

class MsgMoveAck : public MsgBase
{
    friend class MsgFactory;

public:
    MsgMoveAck();
    MsgMoveAck(seqnum_t, MoveAck);
    ~MsgMoveAck() = default;

    const MsgType &getType() const override {
        return MSG_MOVE_ACK;
    };

    const seqnum_t &getSeqNum() const {
        return _seqnum;
    }
    void setSeqNum(const seqnum_t &value) {
        _seqnum = value;
    }

    const MoveAck &getAck() const {
        return _ack;
    }
    void setAck(const MoveAck &value) {
        _ack = value;
    }

protected:
    seqnum_t _seqnum;
    MoveAck _ack;
};


inline MsgMoveAck::MsgMoveAck()
    : MsgMoveAck(0, MoveAck::NACK)
{ }

inline MsgMoveAck::MsgMoveAck(seqnum_t seq, MoveAck ack)
    : _seqnum(seq),
      _ack(ack)
{ }

}

#endif                          //MSGMOVEACK_H
