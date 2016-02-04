/*
 * Soubor:  proj2math.c
 * Datum:   2010/10/21
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program pro vypocet hyperbolickeho tangens, obecneho logaritmu,
 *          vazeneho aritmetickeho a kvadratickeho prumeru za pomoci zakladnich
 *          matematickych operaci a standartniho vstupu/vystupu.
 */

#include "proj2math.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

// vypocita vazeny aritmeticky prumer hodnot, tak ze z parametru valueToAdd
// pricte citatel a jmenovatel a vrati sumu v parametru sum
// sum nesmi byt NULL
// pokud je valueToAdd = NULL a hodnoty v sum je 0, vrati NAN
// pokud valueToAdd != NULL a vaha prvku tohoto parametru je zaporna, vrati NAN
// jinak vrati vazeny prumer posloupnosti v sum
double izp_wam(WeightedValue* const sum, const WeightedValue* const valueToAdd)
{
    assert(sum != NULL);

    if (sum->weight < 0 || (valueToAdd != NULL && valueToAdd->weight < 0)) {
        return NAN;
    }

    if (valueToAdd != NULL) {
        sum->value += valueToAdd->value * valueToAdd->weight;
        sum->weight += valueToAdd->weight;
    }

    return sum->value / sum->weight;
}

// vypocita vazeny kvadraticky prumer hodnot, tak ze z parametru valueToAdd
// pricte citatel a jmenovatel a vrati sumu v parametru sum
// sum nesmi byt NULL
// pokud je valueToAdd = NULL a hodnoty v sum je 0, vrati NAN
// pokud valueToAdd != NULL a vaha prvku tohoto parametru je zaporna, vrati NAN
// jinak vrati vazeny prumer posloupnosti v sum
double izp_wqm(WeightedValue* const sum, const WeightedValue* const valueToAdd)
{
    assert(sum != NULL);

    if (sum->weight < 0 || (valueToAdd != NULL && valueToAdd->weight < 0)) {
        return NAN;
    }

    if (valueToAdd != NULL) {
        sum->value += valueToAdd->value * valueToAdd->value * valueToAdd->weight;
        sum->weight += valueToAdd->weight;
    }

    return sqrt(sum->value / sum->weight);
}

// vypocita hyperbolicky tangens hodnoty x s presnosti eps taylorovou radou pro
// exponencialni funkci e^2x, protoze tanh = sinh/cosh a ta je pro oba vypocty
// spolecna. exponencialni funkce ma obor konvergence pro cela realna cisla,
// ale nejrychleji konverguje u hodnot < 1, proto se rozdeli cela a desetinna cast
// a taylorovou radou se spocita pouze desetinna cast a v cyklu celou casti
// vynasobime vysledek eulerovou konstantou
// vraci NAN pro x=NAN nebo eps<=0.0
// vraci INFINITY pro x=INFINITY
// jinak vrati tanh
double izp_tanh(double x, double eps)
{
    if (isnan(x)) {
        return NAN;
    }
    if (isinf(x)) {
        return INFINITY;
    }
    if (eps <= .0f) {
        return NAN;
    }
    if (x == .0f) {
        return .0f;
    }

    double two_x_int_temp;
    double two_x_fract = modf(2 * x, &two_x_int_temp);
    long int two_x_int = (long int)fabs(two_x_int_temp);

    if (two_x_int == INT_MIN) {
        return x < 0.0f ? -1.0f : 1.0f;
    }

    double iter_x = 1;
    double iter_fakt = 1;
    double iter_prirustek = 1;
    double iter_predchozi = 0;

    double e_2x = 1;

    for (register int i = 1; fabs(iter_prirustek - iter_predchozi) >= eps; ++i) {
        iter_predchozi = iter_prirustek;
        iter_x *= two_x_fract;
        iter_fakt *= i;
        iter_prirustek = iter_x / iter_fakt;
        e_2x += iter_prirustek;
#if DEBUG
//        fprintf(stdout, "\n(%d) %+g[%g]\t%.10e", i, iter_prirustek,
//                fabs(iter_prirustek - iter_predchozi), e_2x);
#endif
    }

    for (register long int i = 0; i < two_x_int; ++i) {
        e_2x *= M_E;
    }

#if DEBUG
//    fprintf(stdout, "\ne^2x = {%.10e}\n", e_2x);
#endif

    if (isinf(e_2x)) {
        return x < 0.0f ? -1.0f : 1.0f;
    }
    else {
    //fprintf(stdout, "(%d)", i);
    //fprintf(stdout, "{%g ; %g}", exp(two_x), e_2x);
    //return (e_x - e_min_x) / (e_x + e_min_x) ;
        return (e_2x - 1) / (e_2x + 1) * (x < 0.0f ? -1 : 1);
    }
}

// vypocita prirozeny logaritmus hodnoty x s presnosti eps taylorovou radou
// tato rada konverguje pro vsechna realna cisla, ale pro male hodnoty konverguje
// rychleji, proto vyuzijeme vety o logaritmech a upravime hodnotu na male cislo
// kde o kolikrat sme ji vydelili eulerovym cislem, tolik pricteme k vysledku
// vraci NAN pro x=NAN nebo x<0.0 nebo eps<=0.0
// vraci INFINITY pro x=INFINITY nebo -INFINITY pro x=-INFINITY
// jinak vrati prirozeny logaritmus
double izp_ln(double x, double eps)
{
    if (isnan(x)) {
        return NAN;
    }
    if (isinf(x)) {
        return INFINITY;
    }
    if (x < .0f) {
        return NAN;
    }
    if (x == .0f) {
        return -INFINITY;
    }
    if (x == 1.0f) {
        return 0;
    }
    if (eps <= .0f) {
        return NAN;
    }

    // zrychleni vypoctu presunutim x do oboru rychlejsi konvergence
    // ln(123,456) = ln(1,23456) + ln(10^2)
    long editTimes = 0;
    while (x < M_E) {
        x *= M_E;
        --editTimes;
    }
    while (x > M_E) {
        x /= M_E;
        ++editTimes;
    }

    const double subst = (x - 1) / (x + 1);
    const double subst_2 = subst * subst;
    double iter = 1;
    double sum = iter;
    register int i = 1;

    // ln(x) = 2y(1 + (y^2)/3 + (y^4)/5 + ...); y = (x - 1) / (x + 1); x > 0
    while (iter >= eps) {
        i += 2;
        iter *= subst_2 * (i - 2) / i;
        sum += iter;
    }

    return 2 * subst * sum + editTimes;
}

// vypocita logaritmus cisla x o zakladu a s presnoti eps
// pokud je zakladem a eulerovo cislo, spocita pouze ln(x)
// pomoci vety o zakladu logaritmu -- log_a(x) = ln(x)/ln(a)
// vraci NAN pro a<=0.0 nebo a=1
// vraci NAN pro x=NAN nebo x<0.0 nebo eps<=0.0
// vraci INFINITY pro x=INFINITY nebo -INFINITY pro x=-INFINITY
// jinak vrati prirozeny logaritmus
double izp_log(double x, double a, double eps)
{
    // zaklad logaritmu a lezi v R+ - {1}
    if (a <= .0f || a == 1.0f) {
        return NAN;
    }
    if (x < .0f) {
        return NAN;
    }
    if (x == .0f) {
        return -INFINITY;
    }
    if (eps <= .0f) {
        return NAN;
    }

    if (a == M_E) {
        return izp_ln(x, eps);
    }
    else {
        return izp_ln(x, eps) / izp_ln(a, eps);
    }
}
