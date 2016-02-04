/*
 * exit_load.cc
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

#include "vehicle_generator.h"
#include "vehicle.h"
#include "timeconv.h"

/* ==================================================================== */
struct stats {
    std::size_t x_najezd;
    std::size_t x_vyjezd;
};

stats *stat = NULL;

/* ==================================================================== */
static bool on_vehicle_created(Vehicle &car)
{
    stat[car.c_najezd].x_najezd++;
    stat[car.c_sjezd].x_vyjezd++;

    /* do not simulate; kill vehicle */
    return false;
}

/* ==================================================================== */
static void vypis()
{
    std::cout << "# km popis najezdu vyjezdu" << std::endl;

    for (d1_exit *p = d1_exit::arr();
         p != &d1_exit::arr()[d1_exit::size()];
         ++p)
    {
        std::cout << p->c_real << " "
                  << "\"" << p->popis << "\" "
                  << stat[p->c_model].x_najezd << " "
                  << stat[p->c_model].x_vyjezd << " "
                  << std::endl;
    }
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

    vehicle_created::callback = &on_vehicle_created;

    stat = new stats[d1_exit::size()];

    Init(sim_start, sim_end);
    (new Generator)->Activate();
    Run();

    vypis();

    delete[] stat;

    return 0;
}
