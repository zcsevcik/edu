/*
 * modelmoves.h:   Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef MODELMOVES_H
# define MODELMOVES_H

#include "Game.h"
#include "Move.h"

#include <cstddef>

#include <QAbstractTableModel>

namespace icp {
/* ==================================================================== */
class ModelMoves : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    ModelMoves(QWidget *parent = 0,
               Game * = nullptr);

    virtual int
    rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual int
    columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant
    data(const QModelIndex &index,
         int role = Qt::DisplayRole) const override;

    virtual QVariant
    headerData(int section,
               Qt::Orientation orientation,
               int role = Qt::DisplayRole) const override;

public slots:
    void updateData();

private:
    std::vector<Moves> moves;
    Game *game;
};

}  // namespace icp

#endif                          //MODELMOVES_H
