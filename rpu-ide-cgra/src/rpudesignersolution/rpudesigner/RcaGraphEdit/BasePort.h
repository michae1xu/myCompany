#ifndef BASEPORT_H
#define BASEPORT_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include "BaseItem.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class BasePort : public BaseItem
{
    Q_OBJECT
public:
    explicit BasePort(QGraphicsItem *parent,PortFromWhereAttr attr);
    virtual ~BasePort();
    PortFromWhereAttr portAttr;
protected:
//    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
//    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
//    void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;

//    QColor color;
//    bool dragOver;

};

#endif // BASEPORT_H
