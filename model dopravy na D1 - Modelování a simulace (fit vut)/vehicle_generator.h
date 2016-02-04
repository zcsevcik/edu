/*
 * vehicle_generator.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#ifndef VEHICLE_GENERATOR_H_
#define VEHICLE_GENERATOR_H_

#include "simlib.h"
#include <ostream>

/* ==================================================================== */
class Generator : public Event
{
    void Behavior();
};

/* ==================================================================== */
std::ostream& plot_vehicle_generator(std::ostream &);

#endif /* VEHICLE_GENERATOR_H_ */
