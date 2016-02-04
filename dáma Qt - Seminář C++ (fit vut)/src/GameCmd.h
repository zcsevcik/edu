/*
 * GameCmd.h       Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAMECMD_H
# define GAMECMD_H

#include "Piece.h"
#include "Move.h"

#include <memory>
#include <functional>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
/*      I  G  a  m  e  C  m  d                                          */
/* ==================================================================== */
class IGameCmd
{
public:
    IGameCmd(Game *);
    virtual ~IGameCmd();
    virtual void exec() = 0;
    virtual void undo() = 0;

protected:
    class impl;
    std::unique_ptr<impl> pImpl;
};

/* ==================================================================== */
/*      P  r  o  m  o  t  e  C  m  d                                    */
/* ==================================================================== */
class PromoteCmd : public IGameCmd
{
public:
    PromoteCmd(Game *, const Square &);
    ~PromoteCmd();
    virtual void exec() override;
    virtual void undo() override;

    const Square &getLoc() const;

protected:
    class impl;
    std::unique_ptr<impl> pImpl;
};

/* ==================================================================== */
/*      J  u  m  p  C  m  d                                             */
/* ==================================================================== */
class JumpCmd : public IGameCmd
{
public:
    JumpCmd(Game *, const Square &, const Square &);
    ~JumpCmd();
    virtual void exec() override;
    virtual void undo() override;

    const Square &getLocA() const;
    const Square &getLocB() const;

protected:
    class impl;
    std::unique_ptr<impl> pImpl;
};

/* ==================================================================== */
/*      C  a  p  t  u  r  e  C  m  d                                    */
/* ==================================================================== */
class CaptureCmd : public IGameCmd
{
public:
    CaptureCmd(Game *, const Square &);
    ~CaptureCmd();
    virtual void exec() override;
    virtual void undo() override;

    const Square &getLoc() const;
    const Piece &getPiece() const;

protected:
    class impl;
    std::unique_ptr<impl> pImpl;
};

}

#endif                          //GAMECMD_H
