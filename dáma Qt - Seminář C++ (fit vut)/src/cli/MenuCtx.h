/*
 * MenuCtx.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MENUCTX_H
# define MENUCTX_H

#include "ConCtx.h"

namespace icp {
/* ==================================================================== */
class MenuCtx final : public ConCtx
{
public:
    MenuCtx(ConCtx *parent = nullptr);
    ~MenuCtx();

    virtual int process() override;
    virtual void render() override;
};

}  // namespace icp

#endif                          //MENUCTX_H
