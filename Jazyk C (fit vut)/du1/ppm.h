/*
 * ppm.h
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-02-26
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU1 b).
 */

#ifndef PPM_H_
#define PPM_H_

/* ==================================================================== */
struct ppm {
    unsigned xsize;
    unsigned ysize;
    unsigned char data[];    // RGB bajty, celkem 3*xsize*ysize
};

/* ==================================================================== */
extern struct ppm *
ppm_read(const char * filename);

/* ==================================================================== */
extern int
ppm_write(struct ppm *p,
          const char * filename);

/* ==================================================================== */

#endif                          //PPM_H_
