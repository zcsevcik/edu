/*
 * usek_load.cc
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "simlib.h"
#include <cstddef>
#include <cstdio>
#include <iostream>

#include "accident_generator.h"
#include "vehicle_generator.h"
#include "vehicle.h"
#include "timeconv.h"

/* ==================================================================== */
struct stats {
    std::size_t pocet_aut;
    double celkovy_cas;

    stats()
      : pocet_aut(0),
        celkovy_cas(.0f)
    { }
};

/* ==================================================================== */
template<class smer>
struct pocet_aut {
    static stats *value;
};

template<class smer>
stats* pocet_aut<smer>::value = NULL;

template struct pocet_aut<smer_olomouc>;
template struct pocet_aut<smer_praha>;

/* ==================================================================== */
template<class smer>
void usek_leave(Vehicle &, d1_usek<smer> &usek, double dt)
{
    static std::size_t celkem = 0;
    ++celkem;

    stats &stat = pocet_aut<smer>::value[usek.c_model];
    stat.pocet_aut++;
    stat.celkovy_cas += dt;
}

/* ==================================================================== */
template<class smer>
void vypis()
{
    std::cout << "#" << "od do pocet min kmh"
              << std::endl;
    std::cout << detail::smer_to_str<smer>() << std::endl;
    for (d1_usek<smer> *p = d1_usek<smer>::arr; p; p = p->get_next()) {
        stats &stat = pocet_aut<smer>::value[p->c_model];
        double t_avg = stat.celkovy_cas / stat.pocet_aut;
        double v_avg = stat.pocet_aut > 0
                     ? 60.0f * p->delka / t_avg
                     : .0f;

        std::cout << std::min(p->km_od, p->km_do)
                  << " " << std::max(p->km_od, p->km_do)
                  << " " << stat.pocet_aut
                  << " "  << stat.celkovy_cas
                  << " " << v_avg
                  << " \"" << p->popis << "\"" << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::endl;
}

/* ==================================================================== */
int main(int argc, char* argv[])
{
    double sim_start = 0;
    double sim_end = 0;

    if (argc < 3 || argc > 3) {
        std::cerr << "Pouziti: " << argv[0] << " od" << " do" << std::endl;
        std::cerr << "Zadej simulacni cas od-do [h]" << std::endl;
        return 1;
    }
    else {
        if (1 != sscanf(argv[1], "%lf", &sim_start)) {
            std::cerr << "chybny cas" << std::endl;
            return 1;
        }
        if (1 != sscanf(argv[2], "%lf", &sim_end)) {
            std::cerr << "chybny cas" << std::endl;
            return 1;
        }

        sim_start *= (double)hour_to_min<1>::value;
        sim_end *= (double)hour_to_min<1>::value;
    }

    vehicle_in_usek<smer_praha>::leave = &usek_leave;
    vehicle_in_usek<smer_olomouc>::leave = &usek_leave;

    pocet_aut<smer_praha>::value = new stats[d1_usek<smer_praha>::size];
    pocet_aut<smer_olomouc>::value = new stats[d1_usek<smer_olomouc>::size];

    Init(sim_start, sim_end);
    (new Generator)->Activate();
    (new accident_generator)->Activate();
    Run();

    vypis<smer_praha>();
    vypis<smer_olomouc>();

    delete[] pocet_aut<smer_praha>::value;
    delete[] pocet_aut<smer_olomouc>::value;

    return 0;
}
