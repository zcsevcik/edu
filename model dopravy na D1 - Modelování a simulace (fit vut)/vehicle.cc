/*
 * vehicle.cc
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "vehicle.h"

#include "simlib.h"

#include "model.h"
#include "route_generator.h"

/* ==================================================================== */
Histogram highwayTime("cas vozidel pri prujezdu dalnici", 16, 0.05, 60);
std::size_t vehicles_wo_path = 0;
bool (*vehicle_created::callback)(Vehicle &) = NULL;

/* ==================================================================== */
Vehicle::Vehicle()
  : c_model(auto_incr<Vehicle>::num - 1)
{ }

/* ==================================================================== */
void Vehicle::Behavior()
{
    /* generovani najezdu a vyjezdu */
    std::size_t nStrt /* = static_cast<std::size_t>(Uniform(0, d1_exit::size())) */;
    std::size_t nEnd /* = static_cast<std::size_t>(Uniform(0, d1_exit::size())) */;

    /* najdeme najezd/sjezd podle km */
    getRoute(&nStrt, &nEnd);
    d1_exit *start = d1_exit::get_at(nStrt);
    d1_exit *end = d1_exit::get_at(nEnd);

    if (!start || !end) {
        /* najezd/sjezd tady neexistuje */
//        highwayTime(0);
        vehicles_wo_path++;
        return;
    }


    if (nStrt < nEnd) {
        /* pojedeme na Olomouc */
        drive_to<smer_olomouc>(*start, *end);
        direction = 1;
    }
    else if (nStrt > nEnd) {
        /* pojedeme na Prahu */
        drive_to<smer_praha>(*start, *end);
        direction = -1;
    }
    else {
        /* nejedeme nikam */
        direction = 0;
        vehicles_wo_path++;
//        highwayTime(0);
    }
}

/* ==================================================================== */
double Vehicle::get_alive_time() const
{
    return Time - created_time;
}

/* ==================================================================== */
template<class Smer>
void Vehicle::drive_to(d1_exit const& start, d1_exit const& end)
{
    typedef d1_usek<Smer> usek;

    c_najezd = start.c_model;
    c_sjezd = end.c_model;
    created_time = Time;
    passed_distance = 0;

    /* najdeme usek, kteremu vyhovuje najezd a sjezd */
    usek* u_strt = usek::get_by_strt_exit(start);
    usek* u_end = usek::get_by_end_exit(end);

    if (!u_strt || !u_end) {
        /* na dalnici nemame kde najet/sjet */
//        highwayTime(0);
        vehicles_wo_path++;
        return;
    }

    if (vehicle_created::callback) {
        if (!vehicle_created::callback(*this))
            return;
    }

    /* najedeme na najezdu */
    Enter(u_strt->store, 1);

    for (usek *p = u_strt; p; p = p->get_next()) {
        /* snazime se +/- dodrzovat povolenou rychlost */
        double spd = Normal(p->prumerna_rychlost, 20);
        while (spd <= 0.0f) { spd = Normal(p->prumerna_rychlost, 10); }
        double ride_time = (double)p->delka * 60.0f / spd;

        /* zapocitame nehodovost */
        for (accident<Smer> *crash = p->accidents;
            crash; crash = crash->next)
        {
            if (Time >= crash->created &&
                Time < crash->created + crash->lifetime)
            {
                double min_delay =
                    std::min(crash->lifetime, crash->delay);
                double lifetime_end =
                    (crash->created + crash->lifetime) - Time;
                double delay =
                    std::min(min_delay, lifetime_end);

                ride_time += delay;
            }
        }

        if (vehicle_in_usek<Smer>::enter)
            vehicle_in_usek<Smer>::enter(*this, *p);

        /* simulujeme dobu stravenou v useku */
        double usek_start_time = Time;
        Activate(Time + ride_time);

        /* pokud jeste nesjizdime, stoupneme si do fronty
         * pro dalsi usek
         */
        if (p != u_end && p->get_next())
            Enter(p->get_next()->store, 1);

        /* opustime usek */
        Leave(p->store, 1);

        passed_distance += p->delka;
        double usek_end_time = Time;
        double usek_dt = usek_end_time - usek_start_time;

        if (vehicle_in_usek<Smer>::leave)
            vehicle_in_usek<Smer>::leave(*this, *p, usek_dt);

        /* sjizdime */
        if (p == u_end) break;
    }

    /* zapocitame celkovy cas straveny na dalnici */
    finished_time = Time - created_time;
    highwayTime(finished_time);
}

