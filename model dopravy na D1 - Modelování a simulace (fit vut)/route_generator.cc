/*
 * route_generator.cc
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "route_generator.h"

#include "simlib.h"
#include <cmath>

#include "timeconv.h"

/* ==================================================================== */
//Praha = 1300000;
//Benesov = 20000;
//HumpolecHavlbrodPelhrimov = 10000 + 24000 + 16000;
//Jihlava = 50000;
//ValasseMezirici 28000;
//Brno = 371000;
//Vyskov 21000;
//Prostejov = 46000;
//Olomouc = 100000;
//Hranice = 20000;
//Ostrava = 303000;

/* ==================================================================== */
inline bool is_morning()
{
    return fmod(Time, day_to_min<1>::value) < hour_to_min<12>::value;
}

/* ==================================================================== */
inline bool is_weekend()
{
    return fmod(Time, day_to_min<7>::value) >= day_to_min<5>::value;
}

/* ==================================================================== */
inline std::size_t getPrahaOkoli()
{
    double rand = Uniform(0, 1);

    if (rand < 0.33) {
        return 0; // praha
    }
    else if (rand < 0.66) {
        return 1; // sporilov
    }
    else {
        return 2; //chodov
    }
}

/* ==================================================================== */
inline std::size_t getPraha()
{
    double rand = Uniform(0, 1);

    if (rand < 0.25) {
        return 6; // "Pruonice"),
    }
    else if (rand < 0.25 * 2) {
        return 8; // "Cestlice"),
    }
    else if (rand < 0.25 * 3) {
        return 10; // "Modletice"),
    }
    else {
        return 12; // "Modletice"),
    }
}

/* ==================================================================== */
inline std::size_t getBeneOkoli()
{
    double rand = Uniform(0, 1);
    if (rand < 0.25) {
        return 21; //Mirosovice"),
    }
    else if (rand < 0.25 * 2) {
        return 29; //Hvezdonice"),
    }
    else if (rand < 0.25 * 3) {
        return 34; //Ostredek"),
    }
    else {
        return 41; //Sternov"),
    }
}

/* ==================================================================== */
inline std::size_t getBene()
{
    return 21; // "Mirosovice"),
}

/* ==================================================================== */
inline std::size_t getHump()
{
    return 90; // "Humpolec"),
}

/* ==================================================================== */
inline std::size_t getHumpOkoli()
{
    double rand = Uniform(0, 1);

    if (rand < 0.1666) {
        return 49; //Psare"),
    }
    else if (rand < 0.1666 * 2) {
        return 56; //Soutice"),
    }
    else if (rand < 0.1666 * 3) {
        return 66; //Loket"),
    }
    else if (rand < 0.1666 * 4) {
        return 75; //Horice"),
    }
    else if (rand < 0.1666 * 5) {
        return 81; //Koberovice"),
    }
    else
        return 90; //Humpolec"),
}

/* ==================================================================== */
inline std::size_t getJihlOkoli()
{

    double rand = Uniform(0, 1);
    if (rand < 0.5) {
        return 104; // "Vetrny Jenikov"),
    }
    else {
        return 119; // "Velky Beranov"),
    }
}

/* ==================================================================== */
inline std::size_t getJihl()
{
    return 112;
}

/* ==================================================================== */
inline std::size_t getValMezOkoli()
{
    double rand = Uniform(0, 1);
    if (rand < 0.5) {
        return 134; // "Merin"),
    }
    else {
        return 153; // "Lhotka"),
    }

}

/* ==================================================================== */
inline std::size_t getValMez()
{
    double rand = Uniform(0, 1);
    if (rand < 0.5) {
        return 141; // "Merin"),
    }
    else {
        return 146; // "Lhotka"),
    }
}

/* ==================================================================== */
inline std::size_t getBrnoOkoli()
{
    double rand = Uniform(0, 1);
    if (rand < 0.1666) {
        return 162; //Psare"),
    }
    else if (rand < 0.1666 * 2) {
        return 168; //Soutice"),
    }
    else if (rand < 0.1666 * 3) {
        return 178; //Loket"),
    }
    else if (rand < 0.1666 * 4) {
        return 182; //Horice"),
    }
    else if (rand < 0.1666 * 5) {
        return 210; //Koberovice"),
    }
    else
        return 216; //Humpolec"),
}

/* ==================================================================== */
inline std::size_t getBrno()
{
    double rand = Uniform(0, 1);
    if (rand < 0.2) {

        return 190; // "Brno-zapad"),
    }
    else if (rand < 0.4) {

        return 194; //"Brno-centrum"),
    }
    else if (rand < 0.6) {

        return 196; //"Brno-jih"),
    }
    else if (rand < 0.8) {

        return 201; //"Brno-Slatina"),
    }
    else {

        return 203; //"Brno-vychod"),
    }
}

/* ==================================================================== */
inline std::size_t getVyskovOkoli()
{
    double rand = Uniform(0, 1);
    if (rand < 0.5) {
        return 236; // "Merin"),
    }
    else {
        return 244; // "Lhotka"),
    }

}

/* ==================================================================== */
inline std::size_t getVyskov()
{
    double rand = Uniform(0, 1);
    if (rand < 0.5) {
        return 226; // "Merin"),
    }
    else {
        return 230; // "Lhotka"),
    }
}

/* ==================================================================== */
inline std::size_t getProstOkoli()
{
    double rand = Uniform(0, 1);

    if (rand < 0.25) {
        return 236; //"Ivanovice na Hane"),
    }
    else if (rand < 0.25 * 2) {
        return 244; //"Morice"),
    }
    else if (rand < 0.25 * 3) {
        return 258; //"Kromeriz-zapad"),
    }
    else {
        return 260; //"Kromeriz-vychod"),
    }
}

/* ==================================================================== */
inline std::size_t getProst()
{
    return 253; //"Kojetin"),
}

/* ==================================================================== */
inline std::size_t getOloOkoli()
{
    double rand = Uniform(0, 1);
    if (rand < 0.2) {
        return 265; //"Hulin"),
    }
    else if (rand < 0.4) {
        return 298; //"Lipnik nad Becvou"),
    }
    else if (rand < 0.6) {
        return 308; //"Hranice"),
    }
    else if (rand < 0.6) {
        return 321; //"Mankovice"),
    }
    else {
        return 330; //"Hladke Zivotice"),
    }
}

/* ==================================================================== */
inline std::size_t getOlo()
{
    return 311; //"Belotin"),
}

/* ==================================================================== */
inline std::size_t getOstOkoli()
{
    double rand = Uniform(0, 1);
    if (rand < 0.1) {
        return 330; //"Hladke Zivotice"),
    }
    else if (rand < 0.2) {
        return 336; //"Butovice"),
    }
    else if (rand < 0.3) {
        return 342; //"Bravantice"),
    }
    else if (rand < 0.4) {
        return 349; //"Klimkovice"),
    }
    else if (rand < 0.5) {
        return 361; //"Mistecka"),
    }
    else if (rand < 0.6) {
        return 365; //"Vrbice"),
    }
    else if (rand < 0.7) {
        return 366; //"Vrbice"),
    }
    else if (rand < 0.8) {
        return 370; //"Bohumin"),
    }
    else if (rand < 0.9) {
        return 372; //"Bohumin"),
    }
    else {
        return 376; //"PL"),
    }
}

/* ==================================================================== */
inline std::size_t getMestNajezd()
{
    return 311; //"Belotin"),
}

/* ==================================================================== */
inline std::size_t getOst()
{
    double rand = Uniform(0, 1);
    if (rand < 0.5) {
        return 354; //"Rudna");
    }
    else {
        return 357; //Severni spoj"),
    }
}

/* ==================================================================== */
inline void getWeekRoute(std::size_t *najezd, std::size_t *vyjezd)
{
    double rand = Uniform(0, 1);
    if (is_morning()) {
        if (rand < 0.4977) { // najezd praha
            *najezd = getPrahaOkoli();
            *vyjezd = getPraha();
        }
        else if (rand < 0.50669) { //najezd benesov
            *najezd = getBeneOkoli();
            *vyjezd = getBene();
        }
        else if (rand < 0.5317) { //najezd humpolec havlbrod pelhrimov
            *najezd = getHumpOkoli();
            *vyjezd = getHump();
        }
        else if (rand < 0.5567) { // jihlava
            *najezd = getJihlOkoli();
            *vyjezd = getJihl();
        }
        else if (rand < 0.5697) { // valmez
            *najezd = getValMezOkoli();
            *vyjezd = getPraha();
        }
        else if (rand < 0.7877) { //brno
            *najezd = getBrnoOkoli();
            *vyjezd = getBrno();
        }
        else if (rand < 0.7977) { //vyskov
            *najezd = getVyskovOkoli();
            *vyjezd = getVyskov();
        }
        else if (rand < 0.8157) { // prostejov
            *najezd = getProstOkoli();
            *vyjezd = getProst();
        }
        else if (rand < 0.8647) { // olomouc
            *najezd = getOloOkoli();
            *vyjezd = getOlo();
        }
        else { // ostrava
            *najezd = getOstOkoli();
            *vyjezd = getOst();
        }

    }
    else {
        if (rand < 0.4977) { // najezd praha
            *vyjezd = getPrahaOkoli();
            *najezd = getPraha();
        }
        else if (rand < 0.50669) { //vyjezd benesov
            *vyjezd = getBeneOkoli();
            *najezd = getBene();
        }
        else if (rand < 0.5317) { //vyjezd humpolec havlbrod pelhrimov
            *vyjezd = getHumpOkoli();
            *najezd = getHump();
        }
        else if (rand < 0.5567) { // jihlava
            *vyjezd = getJihlOkoli();
            *najezd = getJihl();
        }
        else if (rand < 0.5697) { // valmez
            *vyjezd = getValMezOkoli();
            *najezd = getPraha();
        }
        else if (rand < 0.7877) { //brno
            *vyjezd = getBrnoOkoli();
            *najezd = getBrno();
        }
        else if (rand < 0.7977) { //vyskov
            *vyjezd = getVyskovOkoli();
            *najezd = getVyskov();
        }
        else if (rand < 0.8157) { // prostejov
            *vyjezd = getProstOkoli();
            *najezd = getProst();
        }
        else if (rand < 0.8647) { // olomouc
            *vyjezd = getOloOkoli();
            *najezd = getOlo();
        }
        else { // ostrava
            *vyjezd = getOstOkoli();
            *najezd = getOst();
        }
    }
}

/* ==================================================================== */
inline std::size_t getCity()
{
    double rand = Uniform(0, 1);

    if (rand < 0.5) {
        return 311; //"Belotin"),
    }
    else if (rand < 0.10) {
        return 253; //"Kojetin"),
    }
    else if (rand < 0.15) {
        return 226; // "Merin"),
    }
    else if (rand < 0.20) {
        return 230; // "Lhotka"),
    }
    else if (rand < 0.25) {
        return 354; //"Rudna");
    }
    else if (rand < 0.30) {
        return 357; //Severni spoj"),
    }
    else if (rand < 0.35) {
        return 6; // "Pruonice"),
    }
    else if (rand < 0.40) {
        return 8; // "Cestlice"),
    }
    else if (rand < 0.45) {
        return 10; // "Modletice"),
    }
    else if (rand < 0.50) {
        return 12; // "Modletice"),
    }
    else if (rand < 0.55) {
        return 21; // "Mirosovice"),
    }
    else if (rand < 0.60) {
        return 90; // "Humpolec"),
    }
    else if (rand < 0.65) {
        return 112;
    }
    else if (rand < 0.70) {
        return 141; // "Merin"),
    }
    else if (rand < 0.75) {
        return 146; // "Lhotka"),
    }
    else if (rand < 0.80) {
        return 190; // "Brno-zapad"),
    }
    else if (rand < 0.85) {
        return 194; //"Brno-centrum"),
    }
    else if (rand < 0.90) {
        return 196; //"Brno-jih"),
    }
    else if (rand < 0.95) {
        return 201; //"Brno-Slatina"),
    }
    else {
        return 203; //"Brno-vychod"),
    }
}

/* ==================================================================== */
inline void getCities(std::size_t *najezd, std::size_t *vyjezd)
{
    *najezd = getCity();
    while ((*vyjezd = getCity()) == *najezd) ;
}

/* ==================================================================== */
inline void getWeekEndRoute(std::size_t *najezd, std::size_t *vyjezd)
{
    double rand = Uniform(0, 1);
    if (rand < 0.9) { // 90 /% of all cars will be traveling between distant cities
        getCities(najezd, vyjezd);
    }
    else { // 10% looser group have to work on sundays ^^
        getWeekRoute(najezd, vyjezd);
    }
}

inline std::size_t getDistantCity()
{
    double rand = Uniform(0, 1);

	if (  rand < 0.1 ) {
        return 6; // "Pruonice"),
	} else if ( rand < 0.2 ) {
        return 8; // "Cestlice"),
	} else if ( rand < 0.3 ) {
        return 10; // "Modletice"),
	} else if ( rand < 0.4 ) {
        return 354; //"Rudna");
	} else if ( rand < 0.5 ) {
        return 376; //"PL"),
	} else if ( rand < 0.6 ) {
        return 190; // "Brno-zapad"),
	} else if ( rand < 0.7 ) {
        return 194; //"Brno-centrum"),
	} else if ( rand < 0.8 ) {
        return 196; //"Brno-jih"),
	} else if ( rand < 0.9 ) {
        return 201; //"Brno-Slatina"),
	} else {
        return 203; //"Brno-vychod"),
	}
}

inline void getDistantRoute(std::size_t *najezd, std::size_t *vyjezd) 
{
    *najezd = getDistantCity();
    while ((*vyjezd = getDistantCity()) == *najezd) ;
}

/* ==================================================================== */
void getRoute(std::size_t *najezd, std::size_t *vyjezd)
{
	double rand;

    if (!is_weekend()) { // Week
		rand = Uniform(0,1);
		if ( rand < 0.7 ) {
        	getWeekRoute(najezd, vyjezd);
		} else { 
			getDistantRoute(najezd, vyjezd);
		}
    }
    else { // WeekEnd
        getWeekEndRoute(najezd, vyjezd);
    }
}
