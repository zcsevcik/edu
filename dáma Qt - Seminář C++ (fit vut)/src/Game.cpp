/*
 * Game.cpp:       Navrhnete a implementujte aplikaci pro hru Dama
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

#include "Game.h"
#include "Game_p.h"
#include "GameCmd.h"
#include "CheckerBoardGen.h"
#include "Move.h"
#include "Log.h"

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cstdint>
#include <iterator>
#include <stack>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>

namespace icp {
/* ==================================================================== */
/*      P I E C E   C H E C K E R B O A R D   G E N E R A T O R         */
/* ==================================================================== */
template<>
Piece CheckerBoardGen<Piece>::stone() const
{
    if (shouldPlaceBlack()) {
        return Piece::BlackStone;
    }
    else if (shouldPlaceWhite()) {
        return Piece::WhiteStone;
    }
    else {
        return Piece::Empty;
    }
}

/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
Game::impl::impl(Game *p)
    : _base(p),
      _seqnum(0),
      _pulls(PlayerColor::White),
      _isEnd(false)
{
    /* fill board with stones */
    std::generate(begin(_board), end(_board),
                  CheckerBoardGen<Piece>(impl::BoardSize));
}

/* ==================================================================== */
void Game::impl::promote(const Square &pos)
{
    using piece_type = typename std::underlying_type<Piece>::type;
    Log::Logic("promoting %s", pos.to_string().c_str());

    (piece_type &) at(pos) |= (Piece::WhiteDama ^ Piece::WhiteStone);
}

/* ==================================================================== */
void Game::impl::demote(const Square &pos)
{
    using piece_type = typename std::underlying_type<Piece>::type;
    Log::Logic("demoting %s", pos.to_string().c_str());

    (piece_type &) at(pos) &= ~(Piece::WhiteDama ^ Piece::WhiteStone);
}

/* ==================================================================== */
void Game::impl::jump(const Square &pos_f, const Square &pos_t)
{
    Log::Logic("jumping %s->%s", pos_f.to_string().c_str(),
               pos_t.to_string().c_str());

    /* swap squares */
    Piece &ref_f = at(pos_f);
    at(pos_t) = ref_f;
    ref_f = Piece::Empty;
}

/* ==================================================================== */
void Game::impl::insert(const Square &pos, const Piece &piece)
{
    Log::Logic("inserting %s", pos.to_string().c_str());
    at(pos) = piece;
}

/* ==================================================================== */
Piece Game::impl::capture(const Square &pos)
{
    Log::Logic("capturing %s", pos.to_string().c_str());

    /* return captured piece and clear square */
    Piece &ref = at(pos), t = ref;
    return ref = Piece::Empty, t;
}

/* ==================================================================== */
static bool operator==(const Turn &lhs, const Turn &rhs)
{
    return lhs.capture == rhs.capture &&
        lhs.position == rhs.position;
}

/* ==================================================================== */
const TurnCmd *
Game::impl::prepareTurn(const Turn &mv)
{
    Log::Debug("[%s - %s]", mv.to_string().c_str(),
               me() == PlayerColor::White ? "White(x)" : "Black(o)");

    /* capture >= 2 locations
      !capture == 2 location  */
    if ((!mv.capture && mv.position.size() != 2)
        || mv.position.size() < 2) {
        Log::Debug("bad position size");
        return nullptr;
    }

    /* check locations range */
    if (std::any_of(mv.position.begin(), mv.position.end(), &not_in_range)) {
        Log::Debug("position out of range");
        return nullptr;
    }

    /* check who pulls */
    Piece from = at(mv.position.front());
    if (!isPiecePlayers(from, me())) {
        Log::Debug("it's not your turn");
        return nullptr;
    }

    /* check if mandatory */
    if (!_base->getMandatoryMoves().empty() &&
        !std::any_of(_base->getMandatoryMoves().begin(),
                     _base->getMandatoryMoves().end(),
                     std::bind(std::equal_to<Turn>(),
                               std::placeholders::_1,
                               mv)))
    {
        Log::Debug("do mandatory move first");
        return nullptr;
    }

    /* ================================================================ */
    bool isDama = isPieceDama(from);
    CmdQueue cmds;

    for (std::size_t i = 0; i < mv.position.size() - 1; ++i) {
        /* jumping from pos_f -> pos_t */
        const Square &pos_f = mv.position[i];
        const Square &pos_t = mv.position[i + 1];
        Piece to = at(pos_t);

        /* pos_t is occupied */
        if (!isPieceEmpty(to)) {
            Log::Debug("you cannot jump to occupied square");
            return nullptr;
        }

        /* jump vector */
        std::ptrdiff_t dx = pos_t.file - pos_f.file;
        std::ptrdiff_t dy = pos_t.rank - pos_f.rank;

        /* ============================================================ */
        auto checkStoneMove = [&](std::ptrdiff_t offset) -> bool
        {
            /* check diagonal */
            if (dx != -offset && dx != offset)
                return false;
            /* white should move UP(+); black should move DOWN(-) */
            if (dy != ((me() == PlayerColor::White) ? offset : -offset))
                return false;
            return true;
        };

        /* ============================================================ */
        auto checkDamaMove = [&]() -> bool
        {
            /* check diagonal */
            if (dx != dy && dx != -dy)
                return false;
            return true;
        };

        /* ============================================================ */
        if (!mv.capture) {
            /* check jump */
            if (isDama ? !checkDamaMove() : !checkStoneMove(1)) {
                Log::Debug("invalid jump (%" PRIdPTR ",%" PRIdPTR ")",
                           dx, dy);
                return nullptr;
            }

            /* add command */
            cmds.emplace_back(new JumpCmd(_base, pos_f, pos_t));
        }
        else {
            /* check jump */
            if (isDama ? !checkDamaMove() : !checkStoneMove(2)) {
                Log::Debug("invalid jump (%" PRIdPTR ",%" PRIdPTR ")",
                           dx, dy);
                return nullptr;
            }

            if (isDama) {
                /* ==================================================== */
                /* make (dx,dy) a unit vector */
                auto normalize = [](std::ptrdiff_t & n)
                {
                    (n < 0 && (n = -1)) || (n > 0 && (n = 1));
                };

                normalize(dx);
                normalize(dy);

                /* add command */
                cmds.emplace_back(new JumpCmd(_base, pos_f, pos_t));

                /* for all squares along diagonal (pos_f -> pos_t) */
                size_t cEnemy = 0;
                for (Square i = pos_f; i != pos_t;) {
                    /* next square */
                    i.file += dx;
                    i.rank += dy;

                    /* check opponent */
                    const Piece &captPiece = at(i);
                    if (isPiecePlayers(captPiece, opponent())) {
                        /* add command */
                        cmds.emplace_back(new CaptureCmd(_base, i));
                        cEnemy++;
                    }
                }

                /* can capture only one enemy at a jump */
                if (cEnemy > 1) {
                    Log::Debug("cannot capture >1 stone in a jump");
                    return nullptr;
                }
                else if (cEnemy == 0) {
                    Log::Debug("invalid capture");
                    return nullptr;
                }
            }
            else {
                /* ==================================================== */
                /* opponent should be at pos_f + (dx /2, dy /2) */
                Square captPos(pos_f.file + dx / 2, pos_f.rank + dy / 2);
                const Piece &captPiece = at(captPos);

                /* check opponent */
                if (!isPiecePlayers(captPiece, opponent())) {
                    Log::Debug("invalid capture");
                    return nullptr;
                }

                /* add command */
                cmds.emplace_back(new JumpCmd(_base, pos_f, pos_t));
                cmds.emplace_back(new CaptureCmd(_base, captPos));
            }
        }

        from = to;
    }

    /* ================================================================ */
    /* top and bottom of board i.e. where stone becomes dama */
    constexpr boardlen_t blackKingsRow = 0;
    constexpr boardlen_t whiteKingsRow = BoardSize - 1;

    /* is stone at king's row ? */
    bool promoteBlack =
        mv.position.back().rank == blackKingsRow &&
        me() == PlayerColor::Black;
    bool promoteWhite =
        mv.position.back().rank == whiteKingsRow &&
        me() == PlayerColor::White;

    /* then promote; add command */
    if (promoteWhite || promoteBlack) {
        cmds.emplace_back(new PromoteCmd(_base, mv.position.back()));
    }

    /* ================================================================ */
    /* return commands and record turn */
    _moves.emplace_back(mv, std::move(cmds));
    return &_moves.back();
}

/* ==================================================================== */
TurnCmd Game::impl::popTurn()
{
    if (_moves.empty()) return { };

    TurnCmd turnCmd = std::move(_moves.back());
    _moves.pop_back();
    return turnCmd;
}

/* ==================================================================== */
void
Game::impl::getAvailableMoves(std::vector<Turn> &S, const Turn &mv) const
{
    /* TODO: should we remember captured stones with dama
     *       or is this recursive algorithm enough
     *       i.e. we cannot move back ?
     */

    const Piece &stone = at(mv.position.front());
    PlayerColor me;
    PlayerColor opponent;

    if (isPiecePlayers(stone, PlayerColor::White)) {
        me = PlayerColor::White;
        opponent = PlayerColor::Black;
    }
    else if (isPiecePlayers(stone, PlayerColor::Black)) {
        me = PlayerColor::Black;
        opponent = PlayerColor::White;
    }
    else return;

    /* list of search vectors */
    using vec = std::tuple<std::ptrdiff_t, std::ptrdiff_t>;
    std::vector<vec> dVec;

    /* board vectors */
    constexpr std::ptrdiff_t LEFT = -1;
    constexpr std::ptrdiff_t RIGHT = 1;
    constexpr std::ptrdiff_t UP = 1;
    constexpr std::ptrdiff_t DOWN = -1;

    bool isDama = isPieceDama(stone);
    if (isDama) {
        std::ptrdiff_t dx = 0, dy = 0;
        if (mv.capture) {
            /* (dx, dy) = mv.position[-1] - mv.position[-2]; */

            auto iter = mv.position.rbegin();
            dx = iter->file;
            dy = iter->rank;

            iter++;
            dx -= iter->file;
            dy -= iter->rank;
        }

        /* add all board vectors except (-dx, -dy)
         * i.e. cannot jump back
         */
        dVec.reserve(4);
        if (!(dx >= RIGHT && dy >= UP))
            dVec.emplace_back(vec { LEFT, DOWN });
        if (!(dx >= RIGHT && dy <= DOWN))
            dVec.emplace_back(vec { LEFT, UP });
        if (!(dx <= LEFT && dy >= UP))
            dVec.emplace_back(vec { RIGHT, DOWN });
        if (!(dx <= LEFT && dy <= DOWN))
            dVec.emplace_back(vec { RIGHT, UP });
    }
    else {
        /* add ([LEFT,RIGHT], UP) for white player and
         *     ([LEFT,RIGHT], DOWN) for black player)
         */
        dVec.reserve(2);
        std::ptrdiff_t dy = (me == PlayerColor::White) ? UP : DOWN;
        dVec.emplace_back(vec { LEFT, dy });
        dVec.emplace_back(vec { RIGHT, dy });
    }

    /* ================================================================ */
    bool captureNext = false;
    for (auto &i : dVec) {
        std::ptrdiff_t dx;
        std::ptrdiff_t dy;
        std::tie(dx, dy) = i;

        Square loc_t(mv.position.back().file + dx,
                     mv.position.back().rank + dy);

        /* if next square is empty */
        while (!not_in_range(loc_t) && isPieceEmpty(at(loc_t))) {
            if (!mv.capture) {
                /* if not capturing before, you can jump here */
                std::vector<Square> newloc(mv.position);
                newloc.push_back(loc_t);

                S.emplace_back(Turn { false, newloc });
            }

            /* move forward */
            loc_t.file += dx;
            loc_t.rank += dy;

            /* for stone check only one square */
            if (!isDama) goto for_next;
        }

        /* if next square is opponent's stone */
        if (!not_in_range(loc_t) && isPiecePlayers(at(loc_t), opponent)) {
            /* move forward */
            loc_t.file += dx;
            loc_t.rank += dy;

            /* if we can jump to next square */
            while (!not_in_range(loc_t) && isPieceEmpty(at(loc_t))) {
                std::vector<Square> newloc(mv.position);
                newloc.push_back(loc_t);

                /* capturing may continue, it's mandatory;
                 * if may not, recursive call will save this turn
                 */
                captureNext = true;

                /* recursive check if capturing continues */
                getAvailableMoves(S, Turn { true, newloc });

                /* move forward */
                loc_t.file += dx;
                loc_t.rank += dy;

                /* for stone check only one square */
                if (!isDama) goto for_next;
            }
        }

        for_next: continue;
    }

    /* if cannot capture next stone, push current turn */
    if (mv.capture && !captureNext) {
        S.push_back(mv);
    }
}

/* ==================================================================== */
void
Game::impl::checkEnd()
{
    constexpr size_t cPiecesMax =
        ((((BoardSize - 2) / 2) * BoardSize) / 2);

    std::vector<Square> whiteStones;
    std::vector<Square> blackStones;

    whiteStones.reserve(cPiecesMax);
    blackStones.reserve(cPiecesMax);

    for (size_t file = 0, rank = 0;
         rank < BoardSize;
         (++file %= BoardSize) || rank++)
    {
        const Square loc(file, rank);
        const Piece &piece = at(loc);

        if (isPiecePlayers(piece, PlayerColor::White))
            whiteStones.push_back(loc);
        else if (isPiecePlayers(piece, PlayerColor::Black))
            blackStones.push_back(loc);
    }

    /* ================================================================ */
    auto isLooser = [&](std::vector<Square> &S) -> bool {
        /* have stones */
        if (S.empty()) return true;

        /* is blocked */
        for (auto &loc : S) {
            if (!_base->getAvailableMoves(loc).empty())
                return false;
        }

        return true;
    };

    /* ================================================================ */
    if (isLooser(whiteStones)) {
        _isEnd = true;
        _looser = PlayerColor::White;
    }
    else if (isLooser(blackStones)) {
        _isEnd = true;
        _looser = PlayerColor::Black;
    }
    else {
        _isEnd = false;
    }

    return;
}

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
Game::Game()
    : pImpl(new impl(this))
{
}

/* ==================================================================== */
Game::~Game() = default;

/* ==================================================================== */
Game *Game::load(const std::vector<Moves> & mvs)
{
    Game *g = new Game();

    /* do moves, stop at empty string; return null at error */
    for (auto &i : mvs) {
        /* white moves */
        if (i.white_move.position.empty()) break;
        if (!g->makeMove(i.white_move))
            return delete g, g = nullptr;

        /* black moves */
        if (i.black_move.position.empty()) break;
        if (!g->makeMove(i.black_move))
            return delete g, g = nullptr;
    }

    return g;
}

/* ==================================================================== */
Piece Game::getPiece(boardlen_t row, boardlen_t col) const
{
    if (row >= impl::BoardSize) throw std::out_of_range("row");
    if (col >= impl::BoardSize) throw std::out_of_range("col");

    return pImpl->at(col, row);
}

/* ==================================================================== */
Piece Game::getPiece(const Square &loc) const
{
    if (loc.file >= impl::BoardSize) throw std::out_of_range("loc.file");
    if (loc.rank >= impl::BoardSize) throw std::out_of_range("loc.rank");

    return pImpl->at(loc);
}

/* ==================================================================== */
bool Game::makeMove(const Turn &t)
{
    if (pImpl->_isEnd) return false;

    /* get commands */
    const TurnCmd *turnCmd = pImpl->prepareTurn(t);
    if (!turnCmd) return false;

    /* do commands */
    auto &cmds = std::get < 1 > (*turnCmd);
    for (auto &cmd : cmds) {
        cmd->exec();
    }

    /* switch player and inc seqnum */
    pImpl->switchPlayer();
    if (PlayerColor::White == pImpl->me()) {
        pImpl->_seqnum++;
    }

    /* clear mandatory moves cache */
    pImpl->_mandatoryMoves.clear();

    /* check end of game */
    pImpl->checkEnd();

    return true;
}

/* ==================================================================== */
Turn Game::undoMove()
{
    /* get commands */
    TurnCmd turnCmd = pImpl->popTurn();
    CmdQueue &cmds = std::get < 1 > (turnCmd);
    if (cmds.empty()) return { };

    /* undo commands */
    std::for_each(cmds.rbegin(), cmds.rend(),
                  [] (const std::unique_ptr<IGameCmd> &cmd) {cmd->undo();});

    /* switch player and dec seqnum */
    if (PlayerColor::White == pImpl->me()) {
        pImpl->_seqnum--;
    }
    pImpl->switchPlayer();

    /* clear mandatory moves cache */
    pImpl->_mandatoryMoves.clear();
    return std::get < 0 > (turnCmd);
}

/* ==================================================================== */
std::vector<Turn>
Game::getAvailableMoves(const Square &loc) const
{
    /* return if loc is out of board's range */
    if (impl::not_in_range(loc)) return {};

    /* get moves */
    std::vector<Turn> turns;
    pImpl->getAvailableMoves(turns, Turn { false, { loc } });

    return turns;
}

/* ==================================================================== */
std::vector<Turn> const &
Game::getMandatoryMoves() const
{
    /* mandatory moves is cached */
    if (pImpl->_mandatoryMoves.empty()) {
        for (std::size_t file = 0, rank = 0;
             rank < impl::BoardSize;
             (++file %= impl::BoardSize) || rank++)
        {
            /* skip empty squares and opponent's stones */
            const Square loc(file, rank);
            if (!isPiecePlayers(pImpl->at(loc), pImpl->me())) continue;

            /* copy all capture turns to mandatory moves cache */
            std::vector<Turn> turns = getAvailableMoves(loc);
            std::copy_if(begin(turns), end(turns),
                         std::back_inserter(pImpl->_mandatoryMoves),
                         [] (const Turn &mv) -> bool {return mv.capture;});
        }

        /* check if my dama can capture */
        bool damaMoves = std::any_of(
            begin(pImpl->_mandatoryMoves), end(pImpl->_mandatoryMoves),
            [&](const Turn &mv) -> bool {
                return isPieceDama(pImpl->at(mv.position.front()));
            });

        /* dama turn has bigger priority; remove turns with stone */
        if (damaMoves) {
            std::remove_if(
                begin(pImpl->_mandatoryMoves), end(pImpl->_mandatoryMoves),
                [&](const Turn &mv) -> bool {
                    return !isPieceDama(pImpl->at(mv.position.front()));
                });
        }
    }

    return pImpl->_mandatoryMoves;
}

/* ==================================================================== */
const std::vector<Moves>
Game::getMoves() const
{
    std::vector<Moves> mvs;
    seqnum_t seqnum = 0;
    PlayerColor pulls = PlayerColor::White;

    for (auto &i : pImpl->_moves) {
        if (PlayerColor::White == pulls) {
            /* create new move for white's turn */
            mvs.emplace_back(
                Moves { ++seqnum, std::get < 0 > (i), Turn { } });
        }
        else {
            /* add black's turn to last move */
            mvs.back().black_move = std::get < 0 > (i);
        }

        /* switch players */
        pulls =
            PlayerColor::White == pulls ? PlayerColor::Black
                                        : PlayerColor::White;
    }

    return mvs;
}

/* ==================================================================== */
std::size_t Game::getBoardSize() const
{
    return impl::BoardSize;
}

/* ==================================================================== */
const std::size_t &Game::getSeqNum() const
{
    return pImpl->_seqnum;
}

/* ==================================================================== */
const PlayerColor &Game::pulls() const
{
    return pImpl->_pulls;
}

/* ==================================================================== */
bool Game::isEnd() const
{
    return pImpl->_isEnd;
}

/* ==================================================================== */
PlayerColor Game::winner() const
{
    return PlayerColor::White == pImpl->_looser ? PlayerColor::Black
                                                : PlayerColor::White;
}

}  // namespace icp
