/*
 * MenuCtx.cpp:    Navrhnete a implementujte aplikaci pro hru Dama
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

#include "MenuCtx.h"
#include "FileCtx.h"
#include "SingleCtx.h"
#include "LocalCtx.h"
#include "ReplayCtx.h"
#include "MultiCtx.h"

#include <iostream>

namespace icp {
/* ==================================================================== */
MenuCtx::MenuCtx(ConCtx *parent)
  : ConCtx(parent)
{
    setTitle("Hlavni menu");
}

/* ==================================================================== */
MenuCtx::~MenuCtx() = default;

/* ==================================================================== */
int MenuCtx::process()
{
    ConCtx *ctx = nullptr;

    switch (choose("Volba")) {
    default:
        return 0;
    /* ============================================================ */
    case 1: {
        std::string filename = FileCtx::loadDlg(this);
        ctx = new LocalCtx(GameCtx::load(filename), this);
    }   break;
    /* ============================================================ */
    case 2: {
        ctx = MultiCtx::host(this);
    }   break;
    /* ============================================================ */
    case 3: {
        std::string filename = FileCtx::loadDlg(this);
        ctx = MultiCtx::join(GameCtx::load(filename), this);
    }   break;
    /* ============================================================ */
    case 4: {
        std::string filename = FileCtx::loadDlg(this);
        ctx = new SingleCtx(GameCtx::load(filename), this);
    }   break;
    /* ============================================================ */
    case 5: {
        std::string filename = FileCtx::loadDlg(this);
        if (filename.empty()) return 0;

        ctx = new ReplayCtx(GameCtx::load(filename), this);
    }   break;
    }

    if (ctx) ctx->show();
    return 0;
}

/* ==================================================================== */
void MenuCtx::render()
{
    std::cout << "Vyber hry:" << std::endl;
    std::cout << "  (1) Hrac proti hraci (mistne)" << std::endl;
    std::cout << "  (2) Hrac proti hraci (host)" << std::endl;
    std::cout << "  (3) Hrac proti hraci (vyzyvatel)" << std::endl;
    std::cout << "  (4) Hrac proti pocitaci" << std::endl;
    std::cout << "  (5) Prehrani partie" << std::endl;


}

}  // namespace icp
