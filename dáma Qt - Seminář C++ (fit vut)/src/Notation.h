/*
 * Notation.h:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef NOTATION_H
# define NOTATION_H

#include "Move.h"

#include <vector>

namespace icp {
/* ==================================================================== */
/*      G  A  M  E     N  O  T  A  T  I  O  N                           */
/* ==================================================================== */
class Notation
{
public:
    virtual ~Notation();

    virtual bool write(const std::vector<Moves> &) = 0;
    virtual bool read(std::vector<Moves> &) = 0;

protected:
    Notation();
};

inline Notation::Notation() = default;
inline Notation::~Notation() = default;

}

#endif                          //NOTATION_H
