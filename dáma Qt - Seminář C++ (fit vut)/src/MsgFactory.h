/*
 * MsgFactory.h:   Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MSGFACTORY_H
# define MSGFACTORY_H

#include <cstddef>

namespace icp {

class MsgBase;

class MsgFactory
{
public:
    static std::size_t write(char *, std::size_t, const MsgBase &);
    static std::size_t read(const char *, std::size_t, MsgBase * &);
    static std::size_t minLength();
    static std::size_t pktLength(const char *);

protected:
    class impl;
};

}

#endif                          //MSGFACTORY_H
