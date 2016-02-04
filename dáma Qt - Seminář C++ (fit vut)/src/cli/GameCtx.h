/*
 * GameCtx.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAMECTX_H
# define GAMECTX_H

#include "ConCtx.h"
#include "Move.h"

#include <vector>

namespace icp {
/* ==================================================================== */
class Game;

/* ==================================================================== */
class GameCtx : public ConCtx
{
public:
    GameCtx(Game *, ConCtx *parent = nullptr);
    ~GameCtx();

    bool save(std::string const &);
    static Game *load(std::string const & = std::string());

    virtual int process() = 0;
    virtual void render() override;

protected:
    virtual int default_process(command_t, void *obj) override;

private:
    void render_moves(const std::vector<Moves> &) const;
    void render_moves(const std::vector<Turn> &) const;
    void render_topbottom() const;

protected:
    Game *game;

private:
    std::vector<Turn> availMoves;
};

}  // namespace icp

#endif                          //GAMECTX_H
