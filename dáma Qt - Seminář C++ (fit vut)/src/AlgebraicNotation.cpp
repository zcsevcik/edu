/*
 * AlgebraicNotation.cpp: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "AlgebraicNotation.h"
#include "MyEndian.h"
#include "Log.h"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <string>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct AlgebraicNotation::impl {
    std::string fileName;
};

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
AlgebraicNotation::AlgebraicNotation(std::string fileName)
    : _pImpl(new impl())
{
    _pImpl->fileName = fileName;
}

/* ==================================================================== */
AlgebraicNotation::~AlgebraicNotation() = default;

/* ==================================================================== */
bool AlgebraicNotation::write(const std::vector<Moves> &mv)
{
    std::ofstream f(_pImpl->fileName,
                    std::ios_base::trunc);

    for (auto &i : mv) {
        f << i.seqnum << ". ";
        f << i.white_move.to_string() << " ";
        f << i.black_move.to_string() << std::endl;
    }

    return true;
}

/* ==================================================================== */
bool AlgebraicNotation::read(std::vector<Moves> &mv)
{
    std::ifstream f(_pImpl->fileName);

    mv.clear();
    while (!f.eof()) {
        seqnum_t seq = 0;
        Turn white, black;

        std::string s_seq;
        std::getline(f, s_seq, ' ');
        if (s_seq.back() == '.') s_seq.pop_back();

        std::string s_white;
        std::getline(f, s_white, ' ');

        std::string s_black;
        std::getline(f, s_black, '\n');

        Log::Debug("seqnum: %s white: %s black: %s",
                   s_seq.c_str(),
                   s_white.c_str(),
                   s_black.c_str());

        try {
            seq = std::stoi(s_seq);
        }
        catch (std::exception &) {
            return !mv.empty();
        }

        white = Turn::from_string(s_white);
        black = Turn::from_string(s_black);

        mv.emplace_back(Moves(seq, white, black));
        Log::Debug("parsed as: %s", mv.back().to_string().c_str());
    }

    return true;
}

}
