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

#include "wndmain.h"
#include "Log.h"

#include <cstring>
#include <iostream>
#include <locale>
#include <memory>

#include <QApplication>

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

    //Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);

    WndMain wndMain;
    wndMain.show();

    return app.exec();
}

