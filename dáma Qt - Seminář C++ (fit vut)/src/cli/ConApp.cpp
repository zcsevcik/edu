/*
 * ConApp.cpp:     Navrhnete a implementujte aplikaci pro hru Dama
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

#include "ConApp.h"
#include "ConCtx.h"
#include "ConWnd.h"

namespace icp {
/* ==================================================================== */
static struct impl {
    ConApp *instance;

    int argc;
    char **argv;

    impl() : instance(nullptr), argc(0), argv(nullptr)
    { }

} simpl;

/* ==================================================================== */
ConApp::ConApp(int argc, char **argv)
{
    simpl.argc = argc;
    simpl.argv = argv;
    simpl.instance = this;
}

/* ==================================================================== */
ConApp::~ConApp() = default;

/* ==================================================================== */
ConApp *ConApp::instance()
{
    return simpl.instance;
}

/* ==================================================================== */
int ConApp::exec()
{
    for (;;) {
        int hr = ConWnd::instance()->process();
        switch (hr) {
        default:
            return hr;
        case 0:
            continue;
        case 1: {
            ConCtx *ctx = ConWnd::instance()->context();
            if (ctx) {
                ctx->hide();
                delete ctx;
            }
        }   continue;
        case 2:
            ConCtx *ctx;
            while ((ctx = ConWnd::instance()->context())) {
                ctx->hide();
                delete ctx;
            }

            return 0;
        }
    }
}

}  // namespace icp
