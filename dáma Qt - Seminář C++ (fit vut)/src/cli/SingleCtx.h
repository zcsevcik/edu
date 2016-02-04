/*
 * SingleCtx.h     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef SINGLECTX_H
# define SINGLECTX_H

#include "GameCtx.h"
#include "Game_bot.h"

#include <memory>

namespace icp {
/* ==================================================================== */
class SingleCtx : public GameCtx
{
public:
    SingleCtx(Game *, ConCtx *parent = nullptr);
    ~SingleCtx();

    virtual int process() override;

protected:
    virtual int default_process(command_t, void *obj) override;

    std::unique_ptr<Game_bot> gamebot;
};

}  // namespace icp

#endif                          //SINGLECTX_H
