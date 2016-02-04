/*
* params.h
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

#ifndef PARAMS_H_
#define PARAMS_H_

/* ==================================================================== */
struct {
    double cReal;
    double cImag;
} predefined_parameters[] = {
    { -0.76, 0.16 },
    { -0.73, 0.19 },
    { -0.4, 0.6 },
    { 0.285, 0.0 },
    { 0.285, 0.01 },
    { 0.45, 0.1428 },
    { -0.70176, -0.3842 },
    { -0.835, -0.2321 },
    { -0.8, 0.156 },
    { 0.0, -1.0 },
    { -2.0, 0.0 },
    { -0.77568377, 0.13646737 },
    { -1.54368901269109, 0.0 },
    { -0.1010, 0.9562 },
    { -1.0, 0.0 },
    { -0.59, 0.0 },
    { -0.74173, 0.15518 },
    { -0.74543, 0.11301 },
    { 0.29812, 0.52923 },
    { -0.55947, 0.64196 },
    { 0.23300, 0.53780 },
    { -0.62772, 0.42193 },
    { -0.62772, 0.44072 },
    { 0.41453, 0.34364 },
    { -0.74434, -0.10772 },
    { -0.67319, 0.34442 },
};

/* ==================================================================== */
#define predefined_parameters_size \
    (sizeof(predefined_parameters) / sizeof(predefined_parameters[0]))

#endif // PARAMS_H_
