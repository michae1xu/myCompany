#include "BasePort.h"

#include <QDebug>
#include <QMimeData>


BasePort::BasePort(QGraphicsItem *parent, PortFromWhereAttr attr) : BaseItem (parent)
{
    setAcceptDrops(true);
    isPartOfChildRect = false;
    portAttr = attr;
}

BasePort::~BasePort()
{
//    QL_DEBUG<<"~BasePort()";
}

//void BasePort::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
//{
//    if (event->mimeData()->hasColor()) {
//        event->setAccepted(true);
//        dragOver = true;
//        update();
//    } else {
//        event->setAccepted(false);
//    }
//}

//void BasePort::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
//{
//    Q_UNUSED(event);
//    dragOver = false;
//    update();
//}

//void BasePort::dropEvent(QGraphicsSceneDragDropEvent *event)
//{
//    dragOver = false;
//    if (event->mimeData()->hasColor())
//        color = qvariant_cast<QColor>(event->mimeData()->colorData());
//    update();
//}

