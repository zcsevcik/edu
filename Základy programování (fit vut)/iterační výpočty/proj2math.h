/*
 * Soubor:  proj2math.h
 * Datum:   2010/10/21
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program pro vypocet hyperbolickeho tangens, obecneho logaritmu,
 *          vazeneho aritmetickeho a kvadratickeho prumeru za pomoci zakladnich
 *          matematickych operaci a standartniho vstupu/vystupu.
 */

#ifndef PROJ2MATH_H
#define	PROJ2MATH_H

#ifdef	__cplusplus
extern "C" {
#endif

// Eulerova konstanta
#ifndef M_E
#define M_E             2.7182818284590452354
#endif

// vazena hodnota
typedef struct {
    double value;
    double weight;
} WeightedValue;

// rada pro hyperbolicke fce bartsch-matematicke_vzorce 9.1.4(str. 704)
double izp_tanh(double, double);
// rada pro logaritmickou fce bartsch-matematicke_vzorce 9.1.4(str. 703)
double izp_log(double, double, double);
double izp_ln(double, double);

// vazeny prumer bartsch-matematicke_vzorce 10.2.3(str. 745-746)
double izp_wam(WeightedValue* const, const WeightedValue* const);
double izp_wqm(WeightedValue* const, const WeightedValue* const);

#ifdef	__cplusplus
}
#endif

#endif	/* PROJ2MATH_H */

