/*
 * ConWnd.h:       Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef CONWND_H
# define CONWND_H

namespace icp {
/* ==================================================================== */
class ConCtx;

/* ==================================================================== */
class ConWnd final
{
public:
    static ConWnd *instance();

    ConWnd();
    ~ConWnd();

    int show();

    int process();
    void render();

    void setContext(ConCtx * const);
    ConCtx *context() const;
};

}  // namespace icp

#endif                          //CONWND_H
