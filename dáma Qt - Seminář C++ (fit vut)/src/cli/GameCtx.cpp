/*
 * GameCtx.cpp:    Navrhnete a implementujte aplikaci pro hru Dama
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

#include "GameCtx.h"
#include "Game.h"
#include "Game_io.h"
#include "Move.h"

#include <iostream>
#include <string>
#include <vector>

namespace icp {
/* ==================================================================== */
GameCtx::GameCtx(Game *g, ConCtx *parent)
  : ConCtx(parent),
    game(g)
{
}

/* ==================================================================== */
GameCtx::~GameCtx() = default;

/* ==================================================================== */
bool GameCtx::save(std::string const &filename)
{
    return Game_io::save(filename, game);
}

/* ==================================================================== */
Game *GameCtx::load(std::string const &filename)
{
    if (filename.empty()) return new Game;
    else return Game_io::load(filename);
}

/* ==================================================================== */
int GameCtx::default_process(command_t cmd, void *obj)
{
    switch (cmd) {
    default:
        return ConCtx::default_process(cmd, obj);

    case command_t::HelpMove: {
        availMoves = game->getAvailableMoves(* (Square *) obj);
        delete (Square *) obj;
    }   break;

    case command_t::Save: {
        save((char *) obj);
        delete (char *) obj;
    }   break;
    }

    return 0;
}

/* ==================================================================== */
void GameCtx::render()
{
    render_topbottom();
    for (size_t rank = 0; rank < game->getBoardSize(); ++rank) {
        std::cout << std::to_string(game->getBoardSize() - rank) << " ";
        for (size_t file = 0; file < game->getBoardSize(); ++file) {

            std::cout << "[";
            const Piece piece = game->getPiece(rank, file);
            if (isPiecePlayers(piece, PlayerColor::White)) {
                if (isPieceStone(piece)) {
                    std::cout << "x";
                }
                else {          //isPieceDama
                    std::cout << "X";
                }
            }
            else if (isPiecePlayers(piece, PlayerColor::Black)) {
                if (isPieceStone(piece)) {
                    std::cout << "o";
                }
                else {          //isPieceDama
                    std::cout << "O";
                }
            }
            else {              //isPieceEmpty
                std::cout << " ";
            }
            std::cout << "]";
        }

        std::cout << " " << std::to_string(game->getBoardSize() -
                                           rank) << std::endl;
    }

    render_topbottom();

    std::cout << std::endl;
    render_moves(game->getMoves());

    std::vector<Turn> mand = game->getMandatoryMoves();
    if (!mand.empty()) {
        std::cout << "! ";
        render_moves(mand);
    }

    if (!availMoves.empty()) {
        std::cout << "? ";
        render_moves(availMoves);
        availMoves.clear();
    }
}

/* ==================================================================== */
void GameCtx::render_topbottom() const
{
    std::cout << "   ";
    for (unsigned char file = 'a';
        file < 'a' + game->getBoardSize();
        ++file)
    {
        std::cout << file << "  ";
    }
    std::cout << std::endl;
}

/* ==================================================================== */
void GameCtx::render_moves(const std::vector<Moves> &mvs) const
{
    for (auto &i : mvs) {
        std::cout << i.to_string() << " | ";
    }
    std::cout << std::endl << std::endl;

}

/* ==================================================================== */
void GameCtx::render_moves(const std::vector<Turn> &turn) const
{
    for (auto &i : turn) {
        std::cout << i.to_string() << " | ";
    }
    std::cout << std::endl << std::endl;
}

}  // namespace icp
