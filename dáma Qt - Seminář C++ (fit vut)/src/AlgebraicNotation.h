/*
 * AlgebraicNotation.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef ALGEBRAICNOTATION_H
# define ALGEBRAICNOTATION_H

#include <memory>
#include <string>

#include "Notation.h"

namespace icp {
/* ==================================================================== */
/*      A  L  G  E  B  R  A  I  C     N  O  T  A  T  I  O  N            */
/* ==================================================================== */
class AlgebraicNotation final: public Notation
{
public:
    AlgebraicNotation(std::string fileName);
    ~AlgebraicNotation();

    virtual bool write(const std::vector<Moves> &) override;
    virtual bool read(std::vector<Moves> &) override;

private:
    class impl;
    std::unique_ptr<impl> _pImpl;
};

}

#endif                          //ALGEBRAICNOTATION_H
