/*
 * MsgBase.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGBASE_H
# define MSGBASE_H

namespace icp {
/* ==================================================================== */
typedef char MsgType;

/* ==================================================================== */
class MsgBase
{
    friend class MsgFactory;

public:
    virtual const MsgType &getType() const = 0;
    virtual ~MsgBase();

protected:
    MsgBase();

private:
    MsgBase(const MsgBase &);
    MsgBase(MsgBase &&);
    MsgBase &operator=(const MsgBase &);
    MsgBase &operator=(MsgBase &&);
};

/* ==================================================================== */
inline MsgBase::MsgBase() = default;
inline MsgBase::~MsgBase() = default;

}  // namespace icp

#endif                          //MSGBASE_H
