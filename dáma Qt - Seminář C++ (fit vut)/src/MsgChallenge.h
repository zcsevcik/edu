/*
 * MsgChallenge.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGCHALLENGE_H
# define MSGCHALLENGE_H

#include "MsgBase.h"
#include "Color.h"
#include "Move.h"

#include <vector>

namespace icp
{

/**
 * Typ vyzvy
 */
enum class ChallengeType
{
    New,                        /**< Nova partie */
    Saved,                      /**< Rozehrana partie */
};

constexpr MsgType MSG_CHALLENGE = 0;

class MsgChallenge : public MsgBase
{
    friend class MsgFactory;

public:
    MsgChallenge(ChallengeType type,
                 PlayerColor initiatorClr,
                 PlayerColor targetClr);
    MsgChallenge();
    ~MsgChallenge() = default;

    const MsgType &getType() const override {
        return MSG_CHALLENGE;
    };

    const ChallengeType &getChallengeType() const {
        return _challenge_type;
    }
    void setChallengeType(const ChallengeType &value) {
        _challenge_type = value;
    }

    const PlayerColor &getInitiatorColor() const {
        return _initiator_color;
    }
    void setInitiatorColor(const PlayerColor &value) {
        _initiator_color = value;
    }

    const PlayerColor &getTargetColor() const {
        return _target_color;
    }
    void setTargetColor(const PlayerColor &value) {
        _target_color = value;
    }

    const std::vector<Moves> &getGame() const;
    void setGame(const std::vector<Moves> &);

protected:
    ChallengeType _challenge_type;
    PlayerColor _initiator_color;
    PlayerColor _target_color;
    std::vector<Moves> _game;
};


inline MsgChallenge::MsgChallenge(ChallengeType type,
                                  PlayerColor initiatorClr,
                                  PlayerColor targetClr)
    : _challenge_type(type),
      _initiator_color(initiatorClr),
      _target_color(targetClr)
{ }

inline MsgChallenge::MsgChallenge()
    : MsgChallenge(ChallengeType::New,
                   PlayerColor::White,
                   PlayerColor::Black)
{ }

inline const std::vector< Moves >& MsgChallenge::getGame() const
{
    return this->_game;
}

inline void MsgChallenge::setGame(const std::vector< Moves >& value)
{
    this->_game = value;
}



}

#endif                          //MSGCHALLENGE_H
