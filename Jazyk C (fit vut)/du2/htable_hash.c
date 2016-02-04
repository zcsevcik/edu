/*
 * htable_hash.c
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           2014-04-08
 * Compiler:       gcc (Debian 4.8.2-16) 4.8.2
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of IJC-DU2 2).
 */

#include "htable.h"

/* ==================================================================== */
/* prevzato ze: https://www.fit.vutbr.cz/study/courses/IJC/public/DU2.html
 *   http://www.cse.yorku.ca/~oz/hash.html varianta sdbm
 */
unsigned int hash_function(const char *str, unsigned htab_size)
{
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % htab_size;
}
