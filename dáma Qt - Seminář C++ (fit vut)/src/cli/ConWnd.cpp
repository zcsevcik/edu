/*
 * ConWnd.cpp:     Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ConWnd.h"
#include "ConCtx.h"

#include <iostream>

namespace icp {
/* ==================================================================== */
static struct impl {
    ConWnd *instance;
    ConCtx *context;

    impl() : instance(nullptr), context(nullptr)
    { }

    void clear();
    void header();
} simpl;

/* ==================================================================== */
void impl::clear()
{
    std::cout << "\f";
}

/* ==================================================================== */
void impl::header()
{
    std::cout << "\tICP Dama 2013 @ fit.vutbr.cz";
    std::cout << "\t\tRadek Sevcik, xsevci44" << std::endl;
    std::cout << "========================================"
                 "========================================";

    std::cout << std::endl;
}

/* ==================================================================== */
ConWnd::ConWnd()
{
    simpl.instance = this;
}

/* ==================================================================== */
ConWnd::~ConWnd() = default;

/* ==================================================================== */
void ConWnd::render()
{
    simpl.clear();
    simpl.header();
    if (simpl.context) {
        std::cout << "\t\t\\ " << simpl.context->title() << " /"
            << std::endl;
        simpl.context->render();
        std::cout << std::endl;
    }
}

/* ==================================================================== */
int ConWnd::process()
{
    if (simpl.context) {
        render();
        return simpl.context->process();
    }
    else return 1;
}

/* ==================================================================== */
ConWnd *ConWnd::instance()
{
    return simpl.instance;
}

/* ==================================================================== */
void
ConWnd::setContext(ConCtx * const value)
{
    simpl.context = value;
}

/* ==================================================================== */
ConCtx *
ConWnd::context() const
{
    return simpl.context;
}

}  // namespace icp
