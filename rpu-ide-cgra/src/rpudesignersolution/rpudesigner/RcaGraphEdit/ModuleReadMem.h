#ifndef MODULEREADMEM_H
#define MODULEREADMEM_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleReadMem: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleReadMem(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleReadMem();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getReadMemIndex() { return readMemIndex;}
    void setReadMemIndex(int index);
    bool canSetReadMemIndex(int index);
protected:
    QPainterPath paintPath;
    QRectF mapMemRect;
    QLineF mapMemLine;
    int readMemIndex;

public:

};

#endif // MODULEREADMEM_H
