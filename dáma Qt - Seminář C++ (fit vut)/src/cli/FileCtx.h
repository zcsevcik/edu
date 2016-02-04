/*
 * FileCtx.h:      Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef FILECTX_H
# define FILECTX_H

#include "ConCtx.h"

namespace icp {
/* ==================================================================== */
class FileCtx final : public ConCtx
{
public:
    FileCtx(ConCtx *parent = nullptr);
    ~FileCtx();

    virtual int process() override;
    virtual void render() override;

    std::string const &filename() const;

public:
    static std::string loadDlg(ConCtx *parent = nullptr);
    static std::string saveDlg(ConCtx *parent = nullptr);

private:
    std::string _filename;
};

}  // namespace icp

#endif                          //FILECTX_H
