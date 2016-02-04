/*
 * byteswap.h:     Inteligentni teplotni cidlo s mcu PIC,
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

#ifndef BYTESWAP_H
#define BYTESWAP_H

#include <stdint.h>

#if defined(LITTLE_ENDIAN)
/* -------------------------------------------------------------------- */
/*      L  I  T  T  L  E     E  N  D  I  A  N                           */
/* -------------------------------------------------------------------- */
# define htons _byteswap_ushort
# define ntohs _byteswap_ushort
# define htobe16 _byteswap_ushort
# define htole16
# define be16toh _byteswap_ushort
# define le16toh
#elif defined(BIG_ENDIAN)
/* -------------------------------------------------------------------- */
/*      B  I  G     E  N  D  I  A  N                                    */
/* -------------------------------------------------------------------- */
# define htons
# define ntohs
# define htobe16
# define htole16 _byteswap_ushort
# define be16toh
# define le16toh _byteswap_ushort
#else
# error endianess is not defined
#endif

/* -------------------------------------------------------------------- */
/*      B  Y  T  E     S  W  A  P                                       */
/* -------------------------------------------------------------------- */
uint16_t _byteswap_ushort(uint16_t);

#endif //BYTESWAP_H
