/*
 * BinaryNotation.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef BINARYNOTATION_H
# define BINARYNOTATION_H

#include <memory>
#include <string>

#include "Notation.h"

namespace icp {
/* ==================================================================== */
/*      B  I  N  A  R  Y     N  O  T  A  T  I  O  N                     */
/* ==================================================================== */
class BinaryNotation final: public Notation
{
public:
    BinaryNotation(std::string fileName);
    ~BinaryNotation();

    virtual bool write(const std::vector<Moves> &) override;
    virtual bool read(std::vector<Moves> &) override;

private:
    class impl;
    std::unique_ptr<impl> _pImpl;
};

}

#endif                          //BINARYNOTATION_H
