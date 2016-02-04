/*
 * error.h
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

#ifndef ERROR_H_
#define ERROR_H_

/* ==================================================================== */
extern void
Warning(const char *fmt, ...);

extern void
FatalError(const char *fmt, ...);

#endif                          //ERROR_H_
