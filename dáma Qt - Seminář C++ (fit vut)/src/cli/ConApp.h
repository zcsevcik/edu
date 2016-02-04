/*
 * ConApp.h:       Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef CONAPP_H
# define CONAPP_H

namespace icp {
/* ==================================================================== */
class ConApp final
{
public:
    ConApp(int argc, char **argv);
    ~ConApp();

    int exec();

public:
    static ConApp *instance();
};

}  // namespace icp

#endif                          //CONAPP_H
