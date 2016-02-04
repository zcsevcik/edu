/*
 * main.cpp:       Navrhnete a implementujte aplikaci pro hru Dama
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
#include "ConWnd.h"
#include "MenuCtx.h"
#include "Log.h"

#include <cstring>
#include <iostream>
#include <locale>
#include <memory>

using namespace icp;

/* ==================================================================== */
static void print_log(Log::level_type, const Log::char_type *s)
{
    std::cerr << "## " << s << std::endl;
}

/* ==================================================================== */
int main(int argc, char *argv[])
{
    std::locale::global(std::locale(""));

    Log::Level(Log::LEVEL_ERROR);
    Log::Target(&print_log);

    ConApp app(argc, argv);
    ConWnd wnd;

    MenuCtx *ctx = new MenuCtx();
    ctx->show();

    return app.exec();
}

