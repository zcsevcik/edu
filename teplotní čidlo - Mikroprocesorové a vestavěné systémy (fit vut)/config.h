/*
 * config.h:       Inteligentni teplotni cidlo s mcu PIC,
 *                 kompatibilni s protokolem tqs3 Modbus RTU
 *                 IMP projekt: Vlastni tema
 *
 * Date:           Wed, 12 Dec 2012 23:46:02 +0100
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * This file is part of mytqs.
 *
 * Original.
 */

/* Pocet prvku pole */
#ifndef _countof
# define _countof(arr) (sizeof(arr)/sizeof(*(arr)))
#endif

/* Pracovni frekvence mikrokontroleru */
#define _XTAL_FREQ      4000000

/* PIC16f628a je v ramci HITECH-C prekladace little endian */
#define LITTLE_ENDIAN   1
