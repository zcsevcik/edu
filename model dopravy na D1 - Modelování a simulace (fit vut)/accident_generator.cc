/*
 * accident_generator.cc
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "accident_generator.h"

#include "simlib.h"
#include <cmath>

#include "model.h"
#include "timeconv.h"

/* ==================================================================== */
template<class Smer>
void accident_generator::behavior_p()
{
    std::size_t c_useku = (std::size_t)Uniform(0, d1_usek<Smer>::size);
    d1_usek<Smer> &usek = d1_usek<Smer>::arr[c_useku];

    struct accident<Smer> crash(
            fabs(Normal(hour_to_min<3>::value, hour_to_min<2>::value)),
            fabs(Normal(hour_to_min<3>::value, hour_to_min<2>::value)),
            usek);

    usek.cause_accident(crash);
}

/* ==================================================================== */
void accident_generator::Behavior()
{
    static bool first = true;
    if (first) {
        Activate(Time + day_to_min<1>::value);
        first = false;
        return;
    }

    double smer = Uniform(0, 1);
    if (smer < 0.5f) {
        behavior_p<smer_praha>();
    }
    else {
        behavior_p<smer_olomouc>();
    }

    /* reactivate generator */
    Activate(Time + abs(Normal( 2880, 1440 )));
}

