/*
 * slotforwarder.h: Navrhnete a implementujte aplikaci pro hru Dama
 *
 * Team:           Zbynek Malinkovic <xmalin22@stud.fit.vutbr.cz>
 *                 Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 *
 * Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
 * Date:           $Format:%aD$
 *
 * This file is part of ICP-Dama.
 */

#ifndef SLOTFORWARDER_H
# define SLOTFORWARDER_H

#include <cstddef>
#include <functional>
#include <memory>

#include <QObject>

#include "Log.h"

namespace icp {
/* ==================================================================== */
class SlotForwarder : public QObject
{
    Q_OBJECT
    
    typedef std::function<void(void *)> Func;
    
public:
    SlotForwarder(Func fn, void *obj = nullptr, QObject *parent = 0)
      : QObject(parent),
        m_Ptr(fn),
        m_Obj(obj)
    { }
    
    ~SlotForwarder() = default;

public slots:
    void forward() {
        m_Ptr(m_Obj.get());
    }

    void forward(bool) {
        m_Ptr(m_Obj.get());
    }
    
private:
    Func m_Ptr;    
    std::unique_ptr<void> m_Obj;
};

}  // namespace icp

#endif                          //SLOTFORWARDER_H
