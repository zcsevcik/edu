/*
 * config.h:       Implementace interpretu imperativniho jazyka IFJ12
 *
 * Team:           Daniel Berek <xberek00@stud.fit.vutbr.cz>
 *                 Dusan Fodor  <xfodor01@stud.fit.vutbr.cz>
 *                 Pavol Jurcik <xjurci06@stud.fit.vutbr.cz>
 *                 Peter Pritel <xprite01@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ifj12_b2i.
 */

#ifndef countof
#define countof(a)                     (sizeof(a)/sizeof((a)[0]))
#endif

/*
 * Enable GNU extensions on systems that have them.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE                    1
#endif

/*
 * needed to get ansi functions definitions
 */
#ifndef _BSD_SOURCE
#define _BSD_SOURCE                    1
#endif
