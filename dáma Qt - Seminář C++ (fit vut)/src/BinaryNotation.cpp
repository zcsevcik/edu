/*
 * BinaryNotation.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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

#include "BinaryNotation.h"
#include "MyEndian.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

namespace icp {
/* ==================================================================== */
/*      P  R  I  V  A  T  E     I  N  T  E  R  F  A  C  E               */
/* ==================================================================== */
struct BinaryNotation::impl {
    std::string fileName;

    static std::string Header;
    static std::uint16_t Revision;
};

std::string BinaryNotation::impl::Header = u8"ICP-Dama 2013";
std::uint16_t BinaryNotation::impl::Revision = 0;

/* ==================================================================== */
/*      S  T  R  E  A  M     O  P  E  R  A  T  I  O  N  S               */
/* ==================================================================== */

/* ==== I N T E G E R ================================================= */
template<class T>
static std::ofstream &
mywrite(std::ofstream &f, const T &val)
{
    const T val_le = htole(val);
    f.write((typename std::ofstream::char_type *) &val_le, sizeof(T));
    return f;
}

template<class T>
static std::ifstream &
myread(std::ifstream &f, T &val)
{
    f.read((typename std::ofstream::char_type *) &val, sizeof(T));
    val = letoh(val);
    return f;
}

/* ==== C H A R ======================================================= */
template<>
std::ofstream &
mywrite(std::ofstream &f, const char &val)
{
    f.put(val);
    return f;
}

template<>
std::ifstream &
myread(std::ifstream &f, char &val)
{
    val = f.get();
    return f;
}

/* ==== T U R N ======================================================= */
template<>
std::ofstream &
mywrite(std::ofstream &f, const Turn &val)
{
    mywrite<std::uint16_t>(f,
        (std::uint16_t) val.capture << 15 |
        (std::uint16_t) val.position.size());

    for (auto &i : val.position) {
        mywrite<std::uint8_t>(f, i.file);
        mywrite<std::uint8_t>(f, i.rank);
    }

    return f;
}

template<>
std::ifstream &
myread(std::ifstream &f, Turn &val)
{
    std::uint16_t cpos;
    myread(f, cpos);

    val.capture = cpos & (1U << 15);
    std::size_t count = cpos & ~(1U << 15);

    val.position.clear();
    val.position.reserve(count);
    while (count--) {
        std::uint8_t file, rank;
        myread(f, file);
        myread(f, rank);

        val.position.emplace_back(Square(file, rank));
    }

    return f;
}

/* ==== S T R I N G =================================================== */
template<>
std::ofstream &
mywrite(std::ofstream &f, const std::string &val)
{
    f.write(val.c_str(), val.size()), f.put('\0');
    return f;
}

template<>
std::ifstream &
myread(std::ifstream &f, std::string &val)
{
    std::getline(f, val, '\0');
    return f;
}

/* ==================================================================== */
/*      P  U  B  L  I  C     I  N  T  E  R  F  A  C  E                  */
/* ==================================================================== */
BinaryNotation::BinaryNotation(std::string fileName)
    : _pImpl(new impl())
{
    _pImpl->fileName = fileName;
}

/* ==================================================================== */
BinaryNotation::~BinaryNotation() = default;

/* ==================================================================== */
bool BinaryNotation::write(const std::vector<Moves> &mv)
{
    std::ofstream f(_pImpl->fileName,
                    std::ios_base::binary |
                    std::ios_base::trunc);

    mywrite(f, impl::Header);
    mywrite(f, impl::Revision);

    mywrite<std::uint16_t>(f, mv.size());
    for (auto &i : mv) {
        mywrite(f, i.seqnum);
        mywrite(f, i.white_move);
        mywrite(f, i.black_move);
    }

    return true;
}

/* ==================================================================== */
bool BinaryNotation::read(std::vector<Moves> &mv)
{
    std::ifstream f(_pImpl->fileName, std::ios_base::binary);

    decltype(impl::Header) header;
    myread(f, header);
    if (0 != impl::Header.compare(header))
        return false;

    decltype(impl::Revision) revision;
    myread(f, revision);
    if (impl::Revision != revision)
        return false;

    std::uint16_t count;
    myread(f, count);

    mv.clear();
    mv.reserve(count);
    while (count--) {
        seqnum_t seq;
        Turn white, black;

        myread(f, seq);
        myread(f, white);
        myread(f, black);

        mv.emplace_back(Moves(seq, white, black));
    }

    return true;
}

}
