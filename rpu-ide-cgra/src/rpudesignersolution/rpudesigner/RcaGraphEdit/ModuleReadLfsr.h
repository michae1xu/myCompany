#ifndef MODULEREADLFSR_H
#define MODULEREADLFSR_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleReadLfsr: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleReadLfsr(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleReadLfsr();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getReadLfsrIndex() { return readLfsrIndex;}
    void setReadLfsrIndex(int index);
    bool canSetReadLfsrIndex(int index);
protected:
    QPainterPath paintPath;
    QRectF mapRchRect;
    QLineF mapRchLine;
    int readLfsrIndex;
public:

};

#endif // MODULEREADLFSR_H

//class ModuleReadLfsr : public BasePort
//{
//    Q_OBJECT
//public:
//    ModuleReadLfsr();
//};

