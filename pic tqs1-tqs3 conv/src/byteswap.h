/*
 * byteswap.h:     TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

#ifndef BYTESWAP_H
#define BYTESWAP_H

#include <stdint.h>

#if defined(LITTLE_ENDIAN)
/* ==================================================================== */
/*      L  I  T  T  L  E     E  N  D  I  A  N                           */
/* ==================================================================== */
# define htons _byteswap_ushort
# define ntohs _byteswap_ushort
# define htobe16 _byteswap_ushort
# define htole16
# define be16toh _byteswap_ushort
# define le16toh

# define htobe16_c(x) ((x) << 8 | (x) >> 8)
# define htole16_c
# define be16toh_c(x) ((x) << 8 | (x) >> 8)
# define le16toh_c
#elif defined(BIG_ENDIAN)
/* ==================================================================== */
/*      B  I  G     E  N  D  I  A  N                                    */
/* ==================================================================== */
# define htons
# define ntohs
# define htobe16
# define htole16 _byteswap_ushort
# define be16toh
# define le16toh _byteswap_ushort

# define htobe16_c
# define htole16_c(x) ((x) << 8 | (x) >> 8)
# define be16toh_c
# define le16toh_c(x) ((x) << 8 | (x) >> 8)
#else
# error endianess is not defined
#endif

/* ==================================================================== */
/*      B  Y  T  E     S  W  A  P                                       */
/* ==================================================================== */
extern uint16_t _byteswap_ushort(uint16_t);

#endif //BYTESWAP_H
