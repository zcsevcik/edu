/*
 * timeconv.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#ifndef TIMECONV_H_
#define TIMECONV_H_

/* ==================================================================== */
template<unsigned hour>
struct hour_to_min {
    enum { value = hour * 60 };
};

/* ==================================================================== */
template<unsigned day>
struct day_to_min {
    enum { value = hour_to_min<24>::value * day };
};

/* ==================================================================== */

#endif /* TIMECONV_H_ */
