/*
 * MultiCtx.h      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MULTICTX_H
# define MULTICTX_H

#include "GameCtx.h"
#include "Game_net.h"
#include "Color.h"

#include <memory>

namespace icp {
/* ==================================================================== */
class MultiCtx : public GameCtx
{
public:
    static MultiCtx *host(ConCtx *parent = nullptr);

    static MultiCtx *join(Game *,
                          ConCtx *parent = nullptr);

    ~MultiCtx();

    virtual int process() override;

protected:
    MultiCtx(Game_net *, ConCtx *parent = nullptr);
    virtual int default_process(command_t, void *obj) override;

    static void print_error(ConCtx *, std::string const &);

    std::unique_ptr<Game_net> gamenet;
    int started;
};

}  // namespace icp

#endif                          //MULTICTX_H
