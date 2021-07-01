#ifndef ModuleReadRch_H
#define ModuleReadRch_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleReadRch: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleReadRch(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleReadRch();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getReadRchIndex() { return readRchIndex;}
    void setReadRchIndex(int index);
    bool canSetReadRchIndex(int index);
protected:
    QPainterPath paintPath;
    QRectF mapRchRect;
    QLineF mapRchLine;
    int readRchIndex;
public:

};

#endif // ModuleReadRch_H
