/*
 * model.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include <cstddef>
#include <string>
#include "simlib.h"

#include "auto_incr.h"

#ifndef MODEL_H
#define MODEL_H

#define BEZ_OMEZENI 0

/* ==================================================================== */
struct smer_praha { };
struct smer_olomouc { };

/* ==================================================================== */
struct d1_exit;

template<class Smer>
struct d1_usek;

template<class Smer>
struct accident;

/* ==================================================================== */
struct d1_exit : private auto_incr<d1_exit>
{
    static d1_exit* arr();
    static std::size_t size();

    /* ================================================================ */
    const std::size_t c_model;
    const std::size_t c_real;
    std::string popis;

    /* ================================================================ */
    d1_exit(std::size_t p_num, const char *p_desc);

    /* ================================================================ */
    bool operator ==(d1_exit const&) const;
    bool operator !=(d1_exit const&) const;

    /* ================================================================ */
    d1_exit(const d1_exit&);
    d1_exit operator =(const d1_exit&);

    /* ================================================================ */
    static d1_exit* get_at(std::size_t p_km);
};

/* ==================================================================== */
template<class Smer>
struct d1_usek : private auto_incr<d1_usek<Smer> >
{
    static d1_usek arr[];
    static const std::size_t size;

    /* ================================================================ */
    const std::size_t pocet_pruhu;
    const std::size_t prumerna_rychlost;
    const std::size_t delka;
    const std::size_t km_od;
    const std::size_t km_do;
    const std::size_t kapacita;
    const struct d1_exit *najezd;
    const struct d1_exit *vyjezd;
    const std::size_t c_model;
    std::string popis;

    /* ================================================================ */
    Queue Q;
    Store store;

    /* ================================================================ */
    struct accident<Smer> *accidents;
    void cause_accident(accident<Smer> const&);

    /* ================================================================ */
    d1_usek(std::size_t p_od, std::size_t p_do,
            std::size_t p_pruhu, std::size_t p_rychl);

    /* ================================================================ */
    ~d1_usek();

    /* ================================================================ */
    bool operator ==(d1_usek const&) const;
    bool operator !=(d1_usek const&) const;

    /* ================================================================ */
    d1_usek(const d1_usek&);
    d1_usek& operator =(const d1_usek&);

    /* ================================================================ */
    static d1_usek* get_by_strt_exit(d1_exit const&);
    static d1_usek* get_by_end_exit(d1_exit const&);

    /* ================================================================ */
    bool is_first();
    bool is_last();

    /* ================================================================ */
    d1_usek* get_prev();
    d1_usek* get_next();
};

/* ==================================================================== */
template<class Smer>
struct accident :
        private auto_incr<accident<Smer> >
{
    struct accident *next;
    double delay;
    double lifetime;
    double created;
    d1_usek<Smer> &usek;
    const std::size_t c_model;

public:
    /* ================================================================ */
    accident(double delay, double lifetime, d1_usek<Smer> &);
    accident(accident const&);

    /* ================================================================ */
    bool operator ==(accident const&) const;
    bool operator !=(accident const&) const;

    /* ================================================================ */
    void Behavior();
};

/* ==================================================================== */
#include "model_impl.h"

#endif                          //MODEL_H
