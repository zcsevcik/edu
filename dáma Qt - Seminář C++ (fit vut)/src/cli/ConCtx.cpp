/*
 * ConContext.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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

#include "ConCtx.h"
#include "ConWnd.h"
#include "Log.h"
#include "Move.h"

#include <cstring>
#include <exception>
#include <iostream>
#include <string>

namespace icp {
/* ==================================================================== */
ConCtx::ConCtx(ConCtx *parent)
  : _parent(parent)
{ }

/* ==================================================================== */
ConCtx::~ConCtx() = default;

/* ==================================================================== */
int ConCtx::show()
{
    ConWnd::instance()->setContext(this);
    return true;
}

/* ==================================================================== */
int ConCtx::hide()
{
    ConWnd::instance()->setContext(_parent);
    return true;
}

/* ==================================================================== */
std::string const &
ConCtx::title()
{
    return _title;
}

/* ==================================================================== */
void
ConCtx::setTitle(std::string const &value)
{
    _title = value;
}

/* ==================================================================== */
int
ConCtx::choose(std::string const &msg)
{
    for (;;) {
        std::string val = prompt(msg);
        try {
            return std::stoi(val);
        }
        catch (std::exception const &e) {
            Log::Error("%s", e.what());
            continue;
        }
    }
}

/* ==================================================================== */
std::string
ConCtx::prompt(std::string const &msg)
{
    if (!msg.empty()) {
        std::cout << msg;
    }

    std::cout << ": ";

    std::string s;
    std::getline(std::cin, s);
    return s;
}

/* ==================================================================== */
command_t
ConCtx::command(std::string const &msg, void **obj)
{
    std::string line = prompt(msg);

    if (0 == line.compare(0, 1, "!")) {
        if (0 == line.compare(1, std::string::npos, "quit"))
            return command_t::Quit;
        else if (0 == line.compare(1, 5, "save ")) {
            auto idx_file = std::find_if_not(
                begin(line) + 5, end(line), ::isspace);
            std::string file(idx_file, end(line));
            *obj = strdup(file.c_str());
            return command_t::Save;
        }
    }
    else if (0 == line.compare(0, 1, "?")) {
        Turn mv = Turn::from_string(line.substr(1, std::string::npos));
        *obj = new Square(mv.position.at(0));
        return command_t::HelpMove;
    }
    else {
        *obj = new Turn(Turn::from_string(line));
        return command_t::Move;
    }

    return command_t::Undef;
}

/* ==================================================================== */
int ConCtx::default_process(command_t cmd, void *)
{
    switch (cmd) {
    default:
        return 0;

    case command_t::Quit:
        return 1;
    }
}

}  // namespace icp
