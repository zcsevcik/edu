/*
 * GameCmd.cpp:    Navrhnete a implementujte aplikaci pro hru Dama
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

#include "GameCmd.h"
#include "Game.h"
#include "Game_p.h"
#include "Move.h"

namespace icp
{
/* ==================================================================== */
/*      I  G  a  m  e  C  m  d                                          */
/* ==================================================================== */
struct IGameCmd::impl {
    Game *q_ptr;
    Game::impl *d_ptr;
};

IGameCmd::IGameCmd(Game *g)
    : pImpl(new impl())
{
    pImpl->q_ptr = g;
    pImpl->d_ptr = g->pImpl.get();
}

IGameCmd::~IGameCmd() = default;

/* ==================================================================== */
/*      P  r  o  m  o  t  e  C  m  d                                    */
/* ==================================================================== */
struct PromoteCmd::impl {
    Square loc;
};

PromoteCmd::PromoteCmd(Game *g, const Square &l)
    : IGameCmd(g),
      pImpl(new impl())
{
    pImpl->loc = l;
}

PromoteCmd::~PromoteCmd() = default;

/* ==================================================================== */
void PromoteCmd::exec()
{
    IGameCmd::pImpl->d_ptr->promote(pImpl->loc);
}

void PromoteCmd::undo()
{
    IGameCmd::pImpl->d_ptr->demote(pImpl->loc);
}

const Square &PromoteCmd::getLoc() const
{
    return pImpl->loc;
}

/* ==================================================================== */
/*      J  u  m  p  C  m  d                                             */
/* ==================================================================== */
struct JumpCmd::impl {
    Square loc_from;
    Square loc_to;
};

JumpCmd::JumpCmd(Game *g, const Square &l_f, const Square &l_t)
    : IGameCmd(g),
      pImpl(new impl())
{
    pImpl->loc_from = l_f;
    pImpl->loc_to = l_t;
}

JumpCmd::~JumpCmd() = default;

/* ==================================================================== */
void JumpCmd::exec()
{
    IGameCmd::pImpl->d_ptr->jump(pImpl->loc_from, pImpl->loc_to);
}

void JumpCmd::undo()
{
    IGameCmd::pImpl->d_ptr->jump(pImpl->loc_to, pImpl->loc_from);
}

/* ==================================================================== */
/*      C  a  p  t  u  r  e  C  m  d                                    */
/* ==================================================================== */
struct CaptureCmd::impl {
    Square loc;
    Piece piece;
};

CaptureCmd::CaptureCmd(Game *g, const Square &l)
    : IGameCmd(g),
      pImpl(new impl())
{
    pImpl->loc = l;
    pImpl->piece = Piece::Empty;
}

CaptureCmd::~CaptureCmd() = default;

/* ==================================================================== */
void CaptureCmd::exec()
{
    pImpl->piece = IGameCmd::pImpl->d_ptr->capture(pImpl->loc);
}

void CaptureCmd::undo()
{
    IGameCmd::pImpl->d_ptr->insert(pImpl->loc, pImpl->piece);
}

}
