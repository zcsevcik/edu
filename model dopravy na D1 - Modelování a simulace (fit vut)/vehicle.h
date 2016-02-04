/*
 * vehicle.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "simlib.h"

#include "auto_incr.h"
#include "model.h"

#ifndef VEHICLE_H
#define VEHICLE_H

/* ==================================================================== */
extern Histogram highwayTime;
extern std::size_t vehicles_wo_path;

/* ==================================================================== */
class Vehicle
  : public Process,
    private auto_incr<Vehicle>
{
public:
    /* ================================================================ */
    void Behavior();

private:
    /* ================================================================ */
    template<class Smer>
    void drive_to(d1_exit const&, d1_exit const&);

public:
    /* ================================================================ */
    Vehicle();

public:
    /* ================================================================ */
    std::size_t c_model;
    std::size_t c_najezd;
    std::size_t c_sjezd;
    int direction;

    /* ================================================================ */
    double created_time;
    double finished_time;
    double get_alive_time() const;
    std::size_t passed_distance;
};

/* ==================================================================== */
template<class Smer>
struct vehicle_in_usek {
    static void (*enter)(Vehicle &, d1_usek<Smer> &);
    static void (*leave)(Vehicle &, d1_usek<Smer> &, double dt);
};

template<class Smer>
void (*vehicle_in_usek<Smer>::enter)(Vehicle &,
                                     d1_usek<Smer> &) = NULL;

template<class Smer>
void (*vehicle_in_usek<Smer>::leave)(Vehicle &,
                                     d1_usek<Smer> &,
                                     double) = NULL;

template struct vehicle_in_usek<smer_praha>;
template struct vehicle_in_usek<smer_olomouc>;

/* ==================================================================== */
struct vehicle_created {
    static bool (*callback)(Vehicle &);
};

/* ==================================================================== */

#endif                          //VEHICLE_H
