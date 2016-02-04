/*
 * accident_generator.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#ifndef ACCIDENT_GENERATOR_H_
#define ACCIDENT_GENERATOR_H_

#include "simlib.h"

/* ==================================================================== */
class accident_generator : public Event
{
    void Behavior();

    template<class Smer>
    void behavior_p();
};

#endif /* ACCIDENT_GENERATOR_H_ */
