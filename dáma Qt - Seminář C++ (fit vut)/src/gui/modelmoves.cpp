/*
 * modelmoves.cpp: Navrhnete a implementujte aplikaci pro hru Dama
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
    
#include "modelmoves.h"
    
#include <QtGui>

#include "Log.h"

namespace icp {
/* ==================================================================== */
ModelMoves::ModelMoves(QWidget *parent, Game *g)  : QAbstractTableModel(parent),
    game(g){
    updateData();
}

/* ==================================================================== */
int ModelMoves::rowCount(const QModelIndex &) const
{
    return moves.size();
}

/* ==================================================================== */
int ModelMoves::columnCount(const QModelIndex &) const
{
    return 2;
}

/* ==================================================================== */
QVariant ModelMoves::data(const QModelIndex &index, int role) const
{
    switch (role) {
    default:
        break;
    /* ================================================================ */
    case Qt::DisplayRole: {
        Moves const &mv = moves.at(index.row());
        Turn t;
        if (0 == index.column()) t = mv.white_move;
        else if (1 == index.column()) t = mv.black_move;
        else break;
        return QString::fromStdString(t.to_string());
    }   break;

    }

    return QVariant();
}

/* ==================================================================== */
QVariant ModelMoves::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    switch (role) {
    default:
        break;
    /* ================================================================ */
    case Qt::DisplayRole:
        switch (orientation) {
        default:
            break;
        /* ============================================================ */
        case Qt::Vertical:
            return QString("%1.").arg(moves.at(section).seqnum);
        /* ============================================================ */
        case Qt::Horizontal:
            switch (section) {
            default:
                break;
            /* ======================================================== */
            case 0:
                return QString::fromUtf8(u8"B\u00EDl\u00FD");
            /* ======================================================== */
            case 1:
                return QString::fromUtf8(u8"\u010Cern\u00FD");
            }
        }
        break;
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

/* ==================================================================== */
void ModelMoves::updateData()
{
    if (game) {
        moves = game->getMoves();

        QModelIndex topLeft = createIndex(0, 0);
        QModelIndex bottomRight = createIndex(moves.size(), 1);

        emit headerDataChanged(Qt::Vertical, 0, moves.size());
        emit dataChanged(topLeft, bottomRight);
    }
}

}  // namespace icp
