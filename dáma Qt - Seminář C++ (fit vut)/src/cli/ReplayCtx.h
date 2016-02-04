/*
 * ReplayCtx.h     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef REPLAYCTX_H
# define REPLAYCTX_H

#include "GameCtx.h"
#include "Game_replay.h"

#include <memory>

namespace icp {
/* ==================================================================== */
class ReplayCtx : public GameCtx
{
public:
    ReplayCtx(Game *, ConCtx *parent = nullptr);
    ~ReplayCtx();

    virtual int process() override;
    virtual void render() override;

protected:
    std::unique_ptr<Game_replay> gamerep;
};

}  // namespace icp

#endif                          //REPLAYCTX_H
