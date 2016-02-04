/*
 * config.h:       TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

/* Pocet prvku pole */
#ifndef _countof
# define _countof(arr) (sizeof(arr)/sizeof(*(arr)))
#endif

/* Pracovni frekvence mikrokontroleru */
#define _XTAL_FREQ      3686400

/* PIC16f690 je v ramci HITECH-C prekladace little endian */
#define LITTLE_ENDIAN   1

#define __forceinline   1
