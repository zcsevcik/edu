/*
 * XmlNotation.h:  Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef XMLNOTATION_H
# define XMLNOTATION_H

#include "Notation.h"

#include <memory>
#include <string>

namespace icp {
/* ==================================================================== */
/*      X  M  L     N  O  T  A  T  I  O  N                              */
/* ==================================================================== */
class XmlNotation final: public Notation
{
public:
    XmlNotation(std::string fileName);
    ~XmlNotation();

    virtual bool write(const std::vector<Moves> &) override;
    virtual bool read(std::vector<Moves> &) override;

private:
    class impl;
    std::unique_ptr<impl> _pImpl;
};

}

#endif                          //XMLNOTATION_H
