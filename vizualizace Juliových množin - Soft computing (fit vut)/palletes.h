/*
* palletes.h
*
* Author(s):   Ševčík Radek, Bc. <xsevci44@stud.fit.vutbr.cz>
* Date:        2014/10/26
*
* Soft Computing (SFC)
* Vizualizace Juliových množin
*
* Faculty of Information Technology
* Brno University of Technology
*
* This file is part of sfc2014.
*/

#ifndef PALLETES_H_
#define PALLETES_H_

#include <stddef.h>

/* ==================================================================== */
struct pallete {
    unsigned char *pixels;
    int width;
};

/* ==================================================================== */
extern const struct pallete *predefined_palletes;
extern const int predefined_palletes_size;

#endif // PALLETES_H_
