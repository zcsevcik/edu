/*
 * model_impl.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#ifndef MODEL_IMPL_H
#define MODEL_IMPL_H

#include <cmath>
#include <cstddef>
#include <sstream>

#include "countof.h"

namespace detail {
/* ==================================================================== */
template<class Smer>
inline const char *smer_to_str();

/* ==================================================================== */
template<>
inline const char *smer_to_str<smer_praha>() {
    return "Praha";
}

/* ==================================================================== */
template<>
inline const char *smer_to_str<smer_olomouc>() {
    return "Olomouc";
}

/* ==================================================================== */
}

/* ==================================================================== */
const std::size_t autNaKilometrPruh = 250;

/* ==================================================================== */
template struct accident<smer_praha>;
template struct accident<smer_olomouc>;
template struct d1_usek<smer_praha>;
template struct d1_usek<smer_olomouc>;

/* ==================================================================== */
template<class Smer>
inline accident<Smer>::accident(double delay,
                                double lifetime,
                                d1_usek<Smer> &usek)
  : next(NULL),
    delay(delay),
    lifetime(lifetime),
    created(Time),
    usek(usek),
    c_model(auto_incr<accident<Smer> >::num - 1)
{ }

/* ==================================================================== */
template<class Smer>
inline accident<Smer>::accident(accident const& o)
  : next(NULL),
    delay(o.delay),
    lifetime(o.lifetime),
    created(o.created),
    usek(o.usek),
    c_model(o.c_model)
{ }

/* ==================================================================== */
template<class Smer>
inline bool accident<Smer>::operator ==(accident const& rhs) const
{
    return c_model == rhs.c_model;
}

/* ==================================================================== */
template<class Smer>
inline bool accident<Smer>::operator !=(accident const& rhs) const
{
    return c_model != rhs.c_model;
}

/* ==================================================================== */
inline d1_exit::d1_exit(std::size_t p_num, const char *p_desc)
  : c_model(auto_incr<d1_exit>::num - 1),
    c_real(p_num),
    popis(p_desc)
{ }

/* ==================================================================== */
inline bool d1_exit::operator ==(d1_exit const& rhs) const
{
    return c_model == rhs.c_model;
}

/* ==================================================================== */
inline bool d1_exit::operator !=(d1_exit const& rhs) const
{
    return c_model != rhs.c_model;
}

/* ==================================================================== */
template<class Smer>
inline d1_usek<Smer>::d1_usek(std::size_t p_od, std::size_t p_do,
                              std::size_t p_pruhu, std::size_t p_rychl)
  : pocet_pruhu(p_pruhu),
    prumerna_rychlost(p_rychl),
    delka(std::abs((signed)p_od - (signed)p_do)),
    km_od(p_od),
    km_do(p_do),
    kapacita(autNaKilometrPruh * delka * pocet_pruhu),
    najezd(d1_exit::get_at(p_od)),
    vyjezd(d1_exit::get_at(p_do)),
    c_model(auto_incr<d1_usek<Smer> >::num - 1),
    popis(),
    Q(),
    store(kapacita),
    accidents(NULL)
{
    std::ostringstream os;
    os << "D1 " << km_od << ".." << km_do << "km";
    os << " smer " << detail::smer_to_str<Smer>();
    if (pocet_pruhu == 1)
        os << " -- provoz 1 pruhem";

    popis = os.str();
    SetName(store, popis.c_str());
    SetName(Q, popis.c_str());

    if (!is_first()) {
        store.SetQueue(get_prev()->Q);
    }
}


/* ==================================================================== */
template<class Smer>
inline d1_usek<Smer>::~d1_usek()
{
    for (struct accident<Smer> *p = accidents, *q = NULL; p; p = q) {
        q = p->next;
        delete p;
    }
    accidents = NULL;
}

/* ==================================================================== */
template<class Smer>
inline void d1_usek<Smer>::cause_accident(accident<Smer> const& crash)
{
    struct accident<Smer> *p = new accident<Smer>(crash);
    p->next = accidents;
    accidents = p;
}

/* ==================================================================== */
template<class Smer>
inline bool d1_usek<Smer>::operator ==(d1_usek const& rhs) const
{
    return c_model == rhs.c_model;
}

/* ==================================================================== */
template<class Smer>
inline bool d1_usek<Smer>::operator !=(d1_usek const& rhs) const
{
    return c_model != rhs.c_model;
}

/* ================================================================ */
template<class Smer>
inline
d1_usek<Smer>* d1_usek<Smer>::get_by_strt_exit(d1_exit const& rhs)
{
    std::size_t count = size;
    d1_usek *p = arr;

    while (count--) {
        if (p->najezd && *p->najezd == rhs)
            return p;
        ++p;
    }

    return NULL;
}

/* ================================================================ */
template<class Smer>
inline
d1_usek<Smer>* d1_usek<Smer>::get_by_end_exit(d1_exit const& rhs)
{
    std::size_t count = size;
    d1_usek *p = arr;

    while (count--) {
        if (p->vyjezd && *p->vyjezd == rhs)
            return p;
        ++p;
    }

    return NULL;
}

/* ================================================================ */
template<class Smer>
inline bool d1_usek<Smer>::is_first()
{
    return c_model == 0;
}

/* ================================================================ */
template<class Smer>
inline bool d1_usek<Smer>::is_last()
{
    return c_model == size - 1;
}

/* ================================================================ */
template<class Smer>
inline d1_usek<Smer>* d1_usek<Smer>::get_next()
{
    return (!is_last() ? &arr[c_model + 1] : NULL);
}

/* ================================================================ */
template<class Smer>
inline d1_usek<Smer>* d1_usek<Smer>::get_prev()
{
    return (!is_first() ? &arr[c_model - 1] : NULL);
}

/* ==================================================================== */
inline d1_exit* d1_exit::arr()
{
    static d1_exit s_[] = {
        d1_exit(0, "Praha"),
        d1_exit(1, "Sporilov"),
        d1_exit(2, "Chodov"),
        d1_exit(6, "Pruhonice"),
        d1_exit(8, "Cestlice"),
        d1_exit(10, "Modletice"),
        d1_exit(12, "Modletice"),
        d1_exit(15, "Vsechromy"),
        d1_exit(21, "Mirosovice"),
        d1_exit(29, "Hvezdonice"),
        d1_exit(34, "Ostredek"),
        d1_exit(41, "Sternov"),
        d1_exit(49, "Psare"),
        d1_exit(56, "Soutice"),
        d1_exit(66, "Loket"),
        d1_exit(75, "Horice"),
        d1_exit(81, "Koberovice"),
        d1_exit(90, "Humpolec"),
        d1_exit(104, "Vetrny Jenikov"),
        d1_exit(112, "Jihlava"),
        d1_exit(119, "Velky Beranov"),
        d1_exit(134, "Merin"),
        d1_exit(141, "Velke Mezirici-zapad"),
        d1_exit(146, "Velke Mezirici-vychod"),
        d1_exit(153, "Lhotka"),
        d1_exit(162, "Velka Bites"),
        d1_exit(168, "Devet Krizu"),
        d1_exit(178, "Ostrovacice"),
        d1_exit(182, "Kyvalka"),
        d1_exit(190, "Brno-zapad"),
        d1_exit(194, "Brno-centrum"),
        d1_exit(196, "Brno-jih"),
        d1_exit(201, "Brno-Slatina"),
        d1_exit(203, "Brno-vychod"),
        d1_exit(210, "Holubice"),
        d1_exit(216, "Rousinov"),
        d1_exit(226, "Vyskov-zapad"),
        d1_exit(230, "Vyskov-vychod"),
        d1_exit(236, "Ivanovice na Hane"),
        d1_exit(244, "Morice"),
        d1_exit(253, "Kojetin"),
        d1_exit(258, "Kromeriz-zapad"),
        d1_exit(260, "Kromeriz-vychod"),
        d1_exit(265, "Hulin"),
        d1_exit(298, "Lipnik nad Becvou"),
        d1_exit(308, "Hranice"),
        d1_exit(311, "Belotin"),
        d1_exit(321, "Mankovice"),
        d1_exit(330, "Hladke Zivotice"),
        d1_exit(336, "Butovice"),
        d1_exit(342, "Bravantice"),
        d1_exit(349, "Klimkovice"),
        d1_exit(354, "Rudna"),
        d1_exit(357, "Severni spoj"),
        d1_exit(361, "Mistecka"),
        d1_exit(365, "Vrbice"),
        d1_exit(366, "Vrbice"),
        d1_exit(370, "Bohumin"),
        d1_exit(372, "Bohumin"),
        d1_exit(376, "PL"),
    };

    return s_;
}

/* ==================================================================== */
/* http://mapa.dopravniinfo.cz/ 24.11.2013 12:00 */
template<> d1_usek<smer_praha> d1_usek<smer_praha>::arr[] = {
    /* (km_od, km_do, poc_pruhu, rychl) */
    d1_usek(376, 370, 2, 120),
    d1_usek(370, 365, 2, 130),
    d1_usek(365, 361, 2, 130),
    d1_usek(361, 357, 2, 130),
    d1_usek(357, 354, 2, 130),
    d1_usek(354, 349, 2, 130),
    d1_usek(349, 342, 2, 130),
    d1_usek(342, 336, 2, 130),
    d1_usek(336, 330, 2, 130),
    d1_usek(330, 321, 2, 130),
    d1_usek(321, 311, 2, 130),
    d1_usek(311, 308, 2, 130),
    d1_usek(308, 265, 2, 130),
    d1_usek(265, 260, 2, 130),
    d1_usek(260, 258, 2, 130),
    d1_usek(258, 253, 2, 130),
    d1_usek(253, 244, 2, 130),
    d1_usek(244, 236, 2, 130),
    d1_usek(236, 230, 2, 130),
    d1_usek(230, 226, 2, 130),
    d1_usek(226, 220, 2, 120),
#if !BEZ_OMEZENI
    d1_usek(220, 216, 1, 80),
    d1_usek(216, 213, 1, 80),
#else
    d1_usek(220, 216, 2, 130),
    d1_usek(216, 213, 2, 130),
#endif
    d1_usek(213, 210, 2, 110),
    d1_usek(210, 203, 2, 120),
    d1_usek(203, 201, 2, 130),
    d1_usek(201, 196, 2, 130),
    d1_usek(196, 194, 2, 130),
    d1_usek(194, 190, 2, 130),
    d1_usek(190, 182, 2, 120),
    d1_usek(182, 178, 2, 130),
    d1_usek(178, 168, 2, 130),
    d1_usek(168, 162, 2, 130),
#if !BEZ_OMEZENI
    d1_usek(162, 153, 1, 80),
#else
    d1_usek(162, 153, 2, 130),
#endif
    d1_usek(153, 146, 2, 130),
    d1_usek(146, 141, 2, 130),
    d1_usek(141, 134, 2, 130),
    d1_usek(134, 119, 2, 120),
    d1_usek(119, 112, 2, 130),
    d1_usek(112, 104, 2, 130),
    d1_usek(104, 90, 2, 130),
    d1_usek(90, 81, 2, 130),
    d1_usek(81, 75, 2, 130),
#if !BEZ_OMEZENI
    d1_usek(75, 66, 1, 80),
#else
    d1_usek(75, 66, 2, 130),
#endif
    d1_usek(66, 56, 2, 120),
    d1_usek(56, 49, 2, 120),
#if !BEZ_OMEZENI
    d1_usek(49, 41, 1, 80),
#else
    d1_usek(49, 41, 2, 130),
#endif
    d1_usek(41, 34, 2, 130),
    d1_usek(34, 29, 2, 130),
    d1_usek(29, 21, 2, 130),
    d1_usek(21, 15, 2, 130),
    d1_usek(15, 12, 2, 130),
    d1_usek(12, 10, 2, 130),
    d1_usek(10, 8, 2, 130),
    d1_usek(8, 6, 2, 130),
    d1_usek(6, 2, 2, 130),
    d1_usek(2, 0, 2, 130),
};

/* ==================================================================== */
/* http://mapa.dopravniinfo.cz/ 23.11.2013 17:00 */
template<> d1_usek<smer_olomouc> d1_usek<smer_olomouc>::arr[] = {
    /* (km_od, km_do, poc_pruhu, rychl) */
    d1_usek(0, 2, 2, 130),
    d1_usek(2, 6, 2, 130),
    d1_usek(6, 8, 2, 130),
    d1_usek(8, 10, 2, 130),
    d1_usek(10, 12, 2, 130),
    d1_usek(12, 15, 2, 130),
    d1_usek(15, 21, 2, 130),
    d1_usek(21, 29, 2, 130),
    d1_usek(29, 34, 2, 130),
    d1_usek(34, 41, 2, 130),
#if !BEZ_OMEZENI
    d1_usek(41, 49, 1, 80),
#else
    d1_usek(41, 49, 2, 130),
#endif
    d1_usek(49, 56, 2, 120),
    d1_usek(56, 66, 2, 120),
#if !BEZ_OMEZENI
    d1_usek(66, 75, 1, 80),
#else
    d1_usek(66, 75, 2, 130),
#endif
    d1_usek(75, 81, 2, 130),
    d1_usek(81, 90, 2, 130),
    d1_usek(90, 104, 2, 130),
    d1_usek(104, 112, 2, 130),
    d1_usek(112, 119, 2, 130),
    d1_usek(119, 134, 2, 130),
    d1_usek(134, 141, 2, 130),
    d1_usek(141, 146, 2, 130),
    d1_usek(146, 153, 2, 130),
    d1_usek(153, 162, 2, 130),
    d1_usek(162, 168, 2, 130),
    d1_usek(168, 178, 2, 130),
    d1_usek(178, 182, 2, 130),
    d1_usek(182, 190, 2, 130),
    d1_usek(190, 194, 2, 130),
    d1_usek(194, 196, 2, 130),
    d1_usek(196, 201, 2, 130),
    d1_usek(201, 203, 2, 130),
    d1_usek(203, 210, 2, 120),
    d1_usek(210, 213, 2, 110),
#if !BEZ_OMEZENI
    d1_usek(213, 216, 1, 80),
    d1_usek(216, 226, 1, 80),
#else
    d1_usek(213, 216, 2, 130),
    d1_usek(216, 226, 2, 130),
#endif
    d1_usek(226, 230, 2, 120),
    d1_usek(230, 236, 2, 130),
    d1_usek(236, 244, 2, 130),
    d1_usek(244, 253, 2, 130),
    d1_usek(253, 258, 2, 130),
    d1_usek(258, 260, 2, 130),
    d1_usek(260, 265, 2, 130),
    d1_usek(265, 308, 2, 130),
    d1_usek(308, 311, 2, 130),
    d1_usek(311, 321, 2, 130),
    d1_usek(321, 330, 2, 130),
    d1_usek(330, 336, 2, 130),
    d1_usek(336, 342, 2, 130),
    d1_usek(342, 349, 2, 130),
    d1_usek(349, 354, 2, 130),
    d1_usek(354, 357, 2, 130),
    d1_usek(357, 361, 2, 130),
    d1_usek(361, 365, 2, 130),
    d1_usek(365, 370, 2, 130),
    d1_usek(370, 376, 2, 130),
};

/* ==================================================================== */
template<> const std::size_t d1_usek<smer_praha>::size = _countof(arr);

/* ==================================================================== */
template<> const std::size_t d1_usek<smer_olomouc>::size = _countof(arr);

/* ==================================================================== */
inline std::size_t d1_exit::size() {
    return auto_incr<d1_exit>::num;
}

/* ==================================================================== */
inline d1_exit* d1_exit::get_at(std::size_t p_km)
{
    std::size_t count = d1_exit::size();
    d1_exit *p = d1_exit::arr();

    while (count--) {
        if (p->c_real == p_km)
            return p;
        ++p;
    }

    return NULL;
}

#endif                          //MODEL_IMPL_H
