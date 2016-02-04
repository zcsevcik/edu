/*
 * ial.h:          Implementace interpretu imperativniho jazyka IFJ12
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

#ifndef IAL_H
# define IAL_H

# include <stdbool.h>
# include "str.h"

// -----------------------------------------------------------------------
// BINARNI VYHLEDAVACI STROM
// -----------------------------------------------------------------------
/**
 * Binarni vyhledavaci strom.
 */
typedef struct _bst_st bst_t;

/**
 * Vytvori novy BVS.
 * @return Novy BVS.
 */
bst_t *bst_new();

/**
 * Uvolni BVS, pro kazdou polozku zavola item_dispose().
 * @param item_dispose Funkce volana pro uvolneni kazde polozky ve stromu
 */
void bst_dispose(bst_t **, void (*item_dispose) (void *item));

/**
 * Vyhleda polozku v BVS.
 * @param key Klic polozky
 * @param item Nalezena polozka
 * @return Pokud nalezne polozku v BVS vrati true; jinak false
 */
bool bst_search(const bst_t *, const char *key, void **item);

/**
 * Prida do BVS novou polozku.
 * @param key Klic polozky
 * @param item Pridavana polozka
 * @return True v pripade uspechu; jinak false
 */
bool bst_insert(bst_t **, const char *key, void *item);

// -----------------------------------------------------------------------

/**
 * Funkce vyhleda podretezec s2 v retezci s1 Boyer-Mooreovym algoritmem.
 * Funkce se chova stejne jako strstr ze standardni knihovny.
 */
char *ial_strstr(const string_t * s1, const string_t * s2);

/**
 * Radici funkce implementovana algoritmem heap sort.
 * @return Vrati false v pripade neuspechu; jinak true
 */
bool ial_sort(string_t *);

#endif                          // IAL_H
