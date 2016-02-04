/*
 * main.cpp
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2014/01/06
 *
 * This file is part of bf.
 */

#include <iostream>
#include <cstring>

/* ==================================================================== */
struct bf_machine {
    char cells[255];
    char *dp;
    
    bf_machine() {
        dp = cells;
        std::memset(cells, 0, sizeof cells);
    }
};

/* ==================================================================== */
struct bf_op_inc {
    static constexpr char opcode = '+';
    
    static void execute(bf_machine &m) {
        ++(*m.dp);
    }
};

/* ==================================================================== */
struct bf_op_dec {
    static constexpr char opcode = '-';

    static void execute(bf_machine &m) {
        --(*m.dp);
    }
};

/* ==================================================================== */
struct bf_op_incp {
    static constexpr char opcode = '>';

    static void execute(bf_machine &m) {
        ++m.dp;
    }
};

/* ==================================================================== */
struct bf_op_decp {
    static constexpr char opcode = '<';

    static void execute(bf_machine &m) {
        --m.dp;
    }
};

/* ==================================================================== */
struct bf_op_putc {
    static constexpr char opcode = '.';

    static void execute(bf_machine &m) {
        std::cout << *m.dp;
    }
};

/* ==================================================================== */
struct bf_op_getc {
    static constexpr char opcode = ',';

    static void execute(bf_machine &m) {
        std::cin >> *m.dp;
    }
};

/* ==================================================================== */
template<typename...>
struct bf_program;

template<typename X, typename... Xs>
struct bf_program<X, Xs...> {
    static void run(bf_machine &m) {
        X::execute(m);
        bf_program<Xs...>::run(m);
    }
    
    static void dump(std::ostream& os) {
        os << X::opcode;
        bf_program<Xs...>::dump(os);
    }
};

template<>
struct bf_program<> {
    static void run(bf_machine &) { }
    
    static void dump(std::ostream& os) {
        os << std::endl;
    }
};

/* ==================================================================== */
namespace detail {
template<typename...>
struct push_front;

template<template<typename...> class Tpl, typename... Xs1, typename... Xs2>
struct push_front<Tpl<Xs1...>, Tpl<Xs2...>> {
    typedef Tpl<Xs1..., Xs2...> type;
};
}

/* ==================================================================== */
template<char...>
struct bf_parse;

template<char _, char... Xs>
struct bf_parse<_, Xs...> {
    typedef typename bf_parse<Xs...>::compile compile;
};

template<>
struct bf_parse<> {
    typedef bf_program<> compile;
};

template<char... Xs>
struct bf_parse<bf_op_inc::opcode, Xs...> {
    typedef typename detail::push_front<bf_program<bf_op_inc>,
            typename bf_parse<Xs...>::compile>::type compile;
};

template<char... Xs>
struct bf_parse<bf_op_dec::opcode, Xs...> {
    typedef typename detail::push_front<bf_program<bf_op_dec>,
            typename bf_parse<Xs...>::compile>::type compile;
};

template<char... Xs>
struct bf_parse<bf_op_incp::opcode, Xs...> {
    typedef typename detail::push_front<bf_program<bf_op_incp>,
            typename bf_parse<Xs...>::compile>::type compile;
};

template<char... Xs>
struct bf_parse<bf_op_decp::opcode, Xs...> {
    typedef typename detail::push_front<bf_program<bf_op_decp>,
            typename bf_parse<Xs...>::compile>::type compile;
};

template<char... Xs>
struct bf_parse<bf_op_putc::opcode, Xs...> {
    typedef typename detail::push_front<bf_program<bf_op_putc>,
            typename bf_parse<Xs...>::compile>::type compile;
};

template<char... Xs>
struct bf_parse<bf_op_getc::opcode, Xs...> {
    typedef typename detail::push_front<bf_program<bf_op_getc>,
            typename bf_parse<Xs...>::compile>::type compile;
};

/* ==================================================================== */
int main()
{
    typedef bf_parse<'[', '-', ']'>::compile program;
    
    
    bf_machine m;
    program::dump(std::cerr);
    program::run(m);
    
    return 0;
}
