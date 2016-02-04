/*
 * Game_io.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef GAME_IO_H
# define GAME_IO_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "AlgebraicNotation.h"
#include "BinaryNotation.h"
#include "XmlNotation.h"
#include "Game.h"
#include "Move.h"

namespace icp {
/* ==================================================================== */
struct Game_io final {
    static Game *load(std::string const &file);
    static bool save(std::string const &file, Game const *);
};

/* ==================================================================== */
inline Game *Game_io::load(const std::string &file)
{
    bool isXml = false;
    bool isBin = false;
    bool isText = false;
    std::string::size_type idx_ext = file.find_last_of('.');

    if (idx_ext != std::string::npos) {
        isXml = 0 == file.compare(idx_ext, std::string::npos, ".xml");
        isBin = 0 == file.compare(idx_ext, std::string::npos, ".icbf");
        isText = 0 == file.compare(idx_ext, std::string::npos, ".ictf");
    }

    std::vector<Moves> mvs;
    if (isXml) {
        XmlNotation rdr(file);
        if (!rdr.read(mvs)) return nullptr;
    }
    else if (isText) {
        AlgebraicNotation rdr(file);
        if (!rdr.read(mvs)) return nullptr;
    }
    else if (isBin) {
        BinaryNotation rdr(file);
        if (!rdr.read(mvs)) return nullptr;
    }
    else {
        return nullptr;
    }

    return Game::load(mvs);
}

/* ==================================================================== */
inline bool Game_io::save(const std::string &file, const Game *g)
{
    if (!g) return false;

    bool isXml = false;
    bool isBin = false;
    bool isText = false;
    std::string::size_type idx_ext = file.find_last_of('.');

    if (idx_ext != std::string::npos) {
        isXml = 0 == file.compare(idx_ext, std::string::npos, ".xml");
        isBin = 0 == file.compare(idx_ext, std::string::npos, ".icbf");
        isText = 0 == file.compare(idx_ext, std::string::npos, ".ictf");
    }

    std::vector<Moves> mvs = g->getMoves();
    if (isXml) {
        XmlNotation rdr(file);
        if (!rdr.write(mvs)) return false;
    }
    else if (isText) {
        AlgebraicNotation rdr(file);
        if (!rdr.write(mvs)) return false;
    }
    else if (isBin) {
        BinaryNotation rdr(file);
        if (!rdr.write(mvs)) return false;
    }
    else {
        return false;
    }

    return true;
}

}

#endif                          //GAME_IO_H
