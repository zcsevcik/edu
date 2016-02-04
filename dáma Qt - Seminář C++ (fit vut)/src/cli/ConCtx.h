/*
 * ConCtx.h:       Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef CONCTX_H
# define CONCTX_H

#include <cstddef>
#include <string>

namespace icp {
/* ==================================================================== */
enum class command_t {
    Undef,
    Move,
    HelpMove,
    Quit,
    Save,
};

/* ==================================================================== */
class ConCtx
{
public:
    virtual ~ConCtx();

    virtual int process() = 0;
    virtual void render() = 0;

    int show();
    int hide();

    std::string const &title();
    void setTitle(std::string const &);

public:
    static command_t command(std::string const & = std::string(), void **obj = nullptr);
    static int choose(std::string const & = std::string());
    static std::string prompt(std::string const & = std::string());

protected:
    ConCtx(ConCtx *parent = nullptr);

    virtual int default_process(command_t, void *obj);

private:
    ConCtx *_parent;
    std::string _title;
};

}  // namespace icp

#endif                          //CONCTX_H
