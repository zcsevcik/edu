/*
* shaders.h
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

#ifndef SHADERS_H_
#define SHADERS_H_

#include <GL/glew.h>

/* ==================================================================== */
int shaders_init(void);
void shaders_close(void);
GLuint shaders_get_program(void);
void shaders_set_iterations(int);
void shaders_set_zoom(float);
void shaders_set_center(double, double);
void shaders_set_parameter(double, double);
void shaders_set_z_plane(int);

#endif // SHADERS_H_
