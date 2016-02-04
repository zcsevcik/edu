/*
 * accident_test.cc
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
double start_time;
std::size_t num_hrs;

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
struct stats_in_hour {
    stats *value;
};

/* ==================================================================== */
template<class smer>
struct pocet_aut {
    static stats_in_hour *value;
};

template<class smer>
stats_in_hour* pocet_aut<smer>::value = NULL;

template struct pocet_aut<smer_olomouc>;
template struct pocet_aut<smer_praha>;

/* ==================================================================== */
template<class smer>
void usek_leave(Vehicle &, d1_usek<smer> &usek, double dt)
{
    static std::size_t celkem = 0;
    ++celkem;

    std::size_t hr =
            (std::size_t)((Time - start_time) / hour_to_min<1>::value);

    stats &stat = pocet_aut<smer>::value[hr].value[usek.c_model];
    stat.pocet_aut++;
    stat.celkovy_cas += dt;
}

/* ==================================================================== */
template<class smer>
void vypis()
{
    double start_hour = start_time / hour_to_min<1>::value;

    for (d1_usek<smer> *p = d1_usek<smer>::arr; p; p = p->get_next()) {
        for (std::size_t hr = 0; hr < num_hrs; ++hr) {
            stats &stat = pocet_aut<smer>::value[hr].value[p->c_model];
            double t_avg = stat.celkovy_cas / stat.pocet_aut;
            double v_avg = stat.pocet_aut > 0
                         ? 60.0f * p->delka / t_avg
                         : .0f;

            std::cout << (start_hour + hr) * hour_to_min<1>::value
                      << " " << std::min(p->km_od, p->km_do)
                      << " " << std::max(p->km_od, p->km_do)
                      << " " << stat.pocet_aut
                      << " "  << stat.celkovy_cas
                      << " " << v_avg
                      << " \"" << p->popis << "\"" << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;
}

/* ==================================================================== */
template<class Smer>
static void cause_accident(std::size_t c_useku)
{
    if (c_useku >= d1_usek<Smer>::size) return;

    d1_usek<Smer> &usek = d1_usek<Smer>::arr[c_useku];

    struct accident<Smer> crash(
            fabs(Normal(180, 120)),
            fabs(Normal(180, 120)),
            usek);
    crash.created += hour_to_min<6>::value;

    std::cerr << "crash at " << usek.popis
              << " time: " << crash.created
              << " delay: " << crash.delay
              << " lifetime: " << crash.lifetime
              << std::endl;

    usek.cause_accident(crash);
}

/* ==================================================================== */
int main(int argc, char* argv[])
{
    double sim_start = 0;
    double sim_end = 0;
    num_hrs = 0;

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

        num_hrs = sim_end - sim_start;
        sim_start *= (double)hour_to_min<1>::value;
        sim_end *= (double)hour_to_min<1>::value;
    }

    vehicle_in_usek<smer_praha>::leave = &usek_leave;
    vehicle_in_usek<smer_olomouc>::leave = &usek_leave;

    pocet_aut<smer_praha>::value = new stats_in_hour[num_hrs];
    pocet_aut<smer_olomouc>::value = new stats_in_hour[num_hrs];
    for (std::size_t i = 0; i < num_hrs; ++i) {
        pocet_aut<smer_praha>::value[i].value =
                new stats[d1_usek<smer_praha>::size];
        pocet_aut<smer_olomouc>::value[i].value =
                new stats[d1_usek<smer_olomouc>::size];
    }

    start_time = sim_start;
    Init(sim_start, sim_end);
    (new Generator)->Activate();

    cause_accident<smer_praha>(24);

    Run();

    vypis<smer_praha>();
    vypis<smer_olomouc>();

    for (std::size_t i = 0; i < num_hrs; ++i) {
        delete[] pocet_aut<smer_praha>::value[i].value;
        delete[] pocet_aut<smer_olomouc>::value[i].value;
    }
    delete[] pocet_aut<smer_praha>::value;
    delete[] pocet_aut<smer_olomouc>::value;

    return 0;
}
