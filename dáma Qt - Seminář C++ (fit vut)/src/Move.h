/*
 * Move.h:         Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MOVE_H
# define MOVE_H

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace icp
{
/* ==================================================================== */
/*      C  H  E  C  K  E  R  B  O  A  R  D     S  Q  U  A  R  E         */
/* ==================================================================== */
typedef uint8_t boardlen_t;

/* ==================================================================== */
/**
 * Policko sachovnice
 */
struct Square {
    boardlen_t rank;            /**< Radek na sachovnici */
    boardlen_t file;            /**< Sloupec na sachovnici */
    void *obj;                  /**< Co se na policku nachazi */

    Square();
    Square(boardlen_t file, boardlen_t rank);
    static Square empty();
    std::string to_string() const;
    friend int operator==(const Square &, const Square &);
    friend int operator!=(const Square &, const Square &);
};

/* ==================================================================== */
inline Square::Square()
    : rank(0),
      file(0)
{
}

inline Square::Square(boardlen_t file, boardlen_t rank)
    : rank(rank),
      file(file)
{
}

/* ==================================================================== */
inline Square Square::empty()
{
    Square l(~0, ~0);
    l.obj = nullptr;
    return l;
}

/* ==================================================================== */
inline std::string Square::to_string() const
{
    return std::string( {(char)(this->file + 'a')}) +
           std::to_string(this->rank + 1);
}

/* ==================================================================== */
inline int operator==(const Square &lhs, const Square &rhs)
{
    return lhs.file == rhs.file &&
           lhs.rank == rhs.rank;
}

inline int operator!=(const Square &lhs, const Square &rhs)
{
    return !(lhs == rhs);
}

/* ==================================================================== */
/*      C  H  E  C  K  E  R  B  O  A  R  D     T  U  R  N               */
/* ==================================================================== */
/**
 * Tah
 */
struct Turn {
    bool capture;
    std::vector<Square> position;

    Turn();
    Turn(bool, const std::vector<Square> &);
    static Turn from_string(std::string);

    std::string to_string() const;
};

/* ==================================================================== */
inline Turn::Turn()
	: capture(false),
	  position()
{ }

inline Turn::Turn(bool capt, const std::vector<Square> &loc)
	: capture(capt),
	  position(loc)
{ }

/* ==================================================================== */
inline Turn Turn::from_string(std::string s)
{
    if (s.empty()) return {};

    auto iter = begin(s);

    Turn t;
    while (iter != end(s)) {
        auto iter_file = iter;

        auto iter_rank = std::find_if_not(iter_file, end(s), ::isalpha);
        if (iter_rank == end(s)) return {}; /* unexpected eof */
        if (iter_rank == iter_file) return {}; /* unexpected char */

        auto iter_capt = std::find_if_not(iter_rank, end(s), ::isdigit);
        if (iter_capt == iter_rank) return {}; /* unexpected char */
        if (iter_capt != end(s)) { /* eof if capture is done */
            if (*iter_capt != 'x' && *iter_capt != '-')
                return {}; /* unexpected char */

            bool capture = *iter_capt++ == 'x';
            if (iter == begin(s))
                t.capture = capture; /* set capture in 1st iter */
            else if (t.capture != capture)
                return {}; /* cannot jump after capture */
        }

        t.position.emplace_back(Square(
            *iter_file - 'a',
            -1UL + stoul(std::string(iter_rank, iter_capt))
        ));

        iter = iter_capt;
    }

    return t;
}

/* ==================================================================== */
inline std::string Turn::to_string() const
{
    if(!this->position.empty()) {
        std::string s;

        for(auto &i : this->position) {
            s.append(i.to_string());
            s.push_back(this->capture ? 'x' : '-');
        }

        s.pop_back();
        return s;
    }

    return "";
}

/* ==================================================================== */
/*      S  E  R  I  E  S     O  F     M  O  V  E  S                     */
/* ==================================================================== */
typedef uint16_t seqnum_t;

/**
 * Serie tahu
 */
struct Moves {
    seqnum_t seqnum;            /**< Poradove cislo tahu */
    Turn white_move;            /**< Tah bileho */
    Turn black_move;            /**< Tah cerneho */

    Moves(const seqnum_t &, const Turn &, const Turn &);
    std::string to_string() const;
};

/* ==================================================================== */
inline Moves::Moves(const seqnum_t& seq, const Turn &wm, const Turn &bm)
    : seqnum(seq),
      white_move(wm),
      black_move(bm)
{ }

/* ==================================================================== */
inline std::string Moves::to_string() const
{
    return std::to_string(this->seqnum) +
           ". " +
           this->white_move.to_string() +
           " " +
           this->black_move.to_string();
}

}

#endif                          //MOVE_H
