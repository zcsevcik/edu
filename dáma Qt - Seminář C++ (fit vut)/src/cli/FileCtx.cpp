/*
 * FileCtx.cpp:    Navrhnete a implementujte aplikaci pro hru Dama
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

#include "FileCtx.h"
#include "Log.h"

#include <cerrno>
#include <iostream>
#include <system_error>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace icp {
/* ==================================================================== */
FileCtx::FileCtx(ConCtx *parent)
  : ConCtx(parent)
{
}

/* ==================================================================== */
FileCtx::~FileCtx() = default;

/* ==================================================================== */
int FileCtx::process()
{
    std::cout << "Moznosti:" << std::endl;
    std::cout << "  (N)ova partie" << std::endl << std::endl;
    _filename = prompt("Cesta k souboru");
    return 0;
}

/* ==================================================================== */
void FileCtx::render()
{
}

/* ==================================================================== */
std::string const &
FileCtx::filename() const
{
    return _filename;
}

/* ==================================================================== */
std::string FileCtx::loadDlg(ConCtx *parent)
{
    FileCtx ctx(parent);
    ctx.setTitle("Nacist soubor");
    ctx.show();
    for (;;) {
        struct stat buf;
        ctx.process();

        if (ctx.filename() == "N" ||
            ctx.filename() == "n") return { };

        if (-1 == stat(ctx.filename().c_str(), &buf)) {
            Log::Debug("stat() failed: %s", std::system_category()
                .default_error_condition(errno).message().c_str());
            continue;
        }

        if (S_ISREG(buf.st_mode))
            return ctx.filename();
    }
}

/* ==================================================================== */
std::string FileCtx::saveDlg(ConCtx *parent)
{
    FileCtx ctx(parent);
    ctx.setTitle("Ulozit soubor");
    ctx.show();
    for (;;) {
        ctx.process();
        return ctx.filename();
    }
}


}  // namespace icp
