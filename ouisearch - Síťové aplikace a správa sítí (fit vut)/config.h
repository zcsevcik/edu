/*
 * config.h:       Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/14
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

/* Name of package */
#define PACKAGE                         "ouisearch"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT               "xsevci44@stud.fit.vutbr.cz"

/* Define to the full name of this package. */
#define PACKAGE_NAME                    "ouisearch"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING                  "ouisearch"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME                 "xsevci44.tar"

/* Define to the home page for this package. */
#define PACKAGE_URL                     ""

/* Define to the version of this package. */
#define PACKAGE_VERSION                 ""

#define PACKAGE_AUTHOR                  "Radek Sevcik"

#ifndef _MSC_VER
# define _countof(a)                    (sizeof(a)/sizeof((a)[0]))
#endif

/* Enable GNU extensions on systems that have them. */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE                    1
#endif

/* needed to get ansi functions definitions */
#ifndef _BSD_SOURCE
# define _BSD_SOURCE                    1
#endif

/* Define to `htonl' if <endian.h> does not define. */
/* #undef htobe32 */

/* Define to `ntohl' if <endian.h> does not define. */
/* #undef be32toh */

/* Define to `htons' if <endian.h> does not define. */
/* #undef htobe16 */

/* Define to `ntohs' if <endian.h> does not define. */
/* #undef be16toh */
