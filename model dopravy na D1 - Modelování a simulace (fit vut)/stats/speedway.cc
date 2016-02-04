/*
 * speedway.cc
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "simlib.h"
#include <cstdio>
#include <iostream>

#include "accident_generator.h"
#include "vehicle_generator.h"
#include "vehicle.h"
#include "timeconv.h"

/* ==================================================================== */
template<class smer>
void vypis()
{
    for (d1_usek<smer> *p = d1_usek<smer>::arr; p; p = p->get_next()) {
        p->store.Output();
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

    Init(sim_start, sim_end);
    (new Generator)->Activate();
    (new accident_generator)->Activate();
    Run();

    vypis<smer_praha>();
    vypis<smer_olomouc>();

    highwayTime.Output();

    return 0;
}
