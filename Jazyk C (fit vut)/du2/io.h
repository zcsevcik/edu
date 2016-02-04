/*
 * io.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-04-07
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU2 2).
 */

#ifndef IO_H_
#define IO_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
int fgetw(char *s, int max, FILE *f);

/* ==================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* IO_H_ */
