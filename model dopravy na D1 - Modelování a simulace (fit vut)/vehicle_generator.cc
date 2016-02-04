/*
 * vehicle_generator.cc
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *                 Marek Kidon <xkidon00@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ims13.
 */

#include "vehicle_generator.h"

#include "simlib.h"
#include <algorithm>
#include <cmath>
#include <ostream>

#include "timeconv.h"
#include "vehicle.h"

/* ==================================================================== */
template<unsigned wakes_per_minute>
struct generator_params {
    enum { frequency = wakes_per_minute };
    static double period() { return 1.0f / frequency; }
};

/* ==================================================================== */
typedef generator_params<5> generator_t;

/* ==================================================================== */
template<unsigned num>
struct vehicle_per_freq {
    enum { value = num / generator_t::frequency / 60 };
};

/* ==================================================================== */
template<unsigned hour>
struct vehicle_in_hour;

template<>
struct vehicle_in_hour<0> {
    enum { value = 6000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<1> {
    enum { value = 3000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<2> {
    enum { value = 3000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<3> {
    enum { value = 3000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<4> {
    enum { value = 4000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<5> {
    enum { value = 7000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<6> {
    enum { value = 15000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<7> {
    enum { value = 20000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<8> {
    enum { value = 22000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<9> {
    enum { value = 21000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<10> {
    enum { value = 19000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<11> {
    enum { value = 19000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<12> {
    enum { value = 18000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<13> {
    enum { value = 21000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<14> {
    enum { value = 24000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<15> {
    enum { value = 27000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<16> {
    enum { value = 33000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<17> {
    enum { value = 33000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<18> {
    enum { value = 25000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<19> {
    enum { value = 18000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<20> {
    enum { value = 12000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<21> {
    enum { value = 6000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<22> {
    enum { value = 9000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

template<>
struct vehicle_in_hour<23> {
    enum { value = 8000 };
    enum { per_tick = vehicle_per_freq<value>::value };
};

/* ==================================================================== */
template<unsigned hour>
struct generator_helper {
    typedef hour_to_min<hour + 1> time;
    typedef vehicle_in_hour<hour> count;

    typedef generator_helper<hour + 1> next;

    static std::size_t get_count(double t) {
        if (t < time::value) return count::per_tick;
        else return next::get_count(t);
    }
};

template<>
struct generator_helper<24> {
    static std::size_t get_count(double) { return 0; }
};

/* ==================================================================== */
// returns a number of cars which should generator put on highway in
// particular time
static std::size_t generateCars()
{
    double currentDayTime = fmod(Time, day_to_min<1>::value);
    return generator_helper<0>::get_count(currentDayTime);
}

/* ==================================================================== */
struct activate_functor {
    void operator ()(Vehicle &p) {
        p.Activate();
    }
};

/* ==================================================================== */
void Generator::Behavior()
{
    /* generate vehicles by daytime */
    std::size_t count = generateCars();

    /* allocate and activate vehicles */
    Vehicle *vehicles = new Vehicle[count];
    std::for_each(vehicles, vehicles + count, activate_functor());

    /* reactivate generator */
    Activate(Time + generator_t::period());
}

/* ==================================================================== */
template<unsigned hour>
struct plot_helper {
    typedef hour_to_min<hour> time;
    typedef vehicle_in_hour<hour> count;

    typedef plot_helper<hour + 1> next;

    static std::ostream& output(std::ostream &os) {
        os << time::value << " " << count::value << std::endl;
        next::output(os);
        return os;
    }
};

/* ==================================================================== */
template<>
struct plot_helper<24> {
    static std::ostream& output(std::ostream &os) {
        return os << std::endl;
    }
};

/* ==================================================================== */
std::ostream& plot_vehicle_generator(std::ostream &os)
{
    return plot_helper<0>::output(os);
}

