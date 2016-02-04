/*
 * LocalCtx.h     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef LOCALCTX_H
# define LOCALCTX_H

#include "GameCtx.h"

namespace icp {
/* ==================================================================== */
class LocalCtx : public GameCtx
{
public:
    LocalCtx(Game *, ConCtx *parent = nullptr);
    ~LocalCtx();

    virtual int process() override;

protected:
    virtual int default_process(command_t, void *obj) override;
};

}  // namespace icp

#endif                          //LOCALCTX_H
