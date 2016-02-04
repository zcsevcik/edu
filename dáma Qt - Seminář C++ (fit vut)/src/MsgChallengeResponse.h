/*
 * MsgChallengeResponse.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGCHALLENGERESPONSE_H
# define MSGCHALLENGERESPONSE_H

#include "MsgBase.h"

namespace icp
{

enum class ChallengeResponse
{
    Denied,
    Accepted,
};

constexpr MsgType MSG_CHALLENGE_RESPONSE = 1;

class MsgChallengeResponse : public MsgBase
{
    friend class MsgFactory;

public:
    MsgChallengeResponse();
    MsgChallengeResponse(ChallengeResponse);
    ~MsgChallengeResponse() = default;

    const MsgType &getType() const override {
        return MSG_CHALLENGE_RESPONSE;
    };

    const ChallengeResponse &getResponse() const {
        return _response;
    }
    void setResponse(const ChallengeResponse &value) {
        _response = value;
    }

protected:
    ChallengeResponse _response;
};


inline MsgChallengeResponse::MsgChallengeResponse()
    : MsgChallengeResponse(ChallengeResponse::Denied)
{ }

inline MsgChallengeResponse::MsgChallengeResponse(ChallengeResponse resp)
    : _response(resp)
{ }


}

#endif                          //MSGCHALLENGERESPONSE_H
