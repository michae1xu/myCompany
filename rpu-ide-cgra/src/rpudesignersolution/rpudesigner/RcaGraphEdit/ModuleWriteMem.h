#ifndef MODULEWRITEMEM_H
#define MODULEWRITEMEM_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleWriteMem: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleWriteMem(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleWriteMem();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getWriteMemIndex(){ return writeMemIndex;}
    bool setWriteMemIndex(int index);
    QString canSetWriteMemIndex(int index);
    InputPortType getInputType();
    void setInputType(InputPortType type);
    int getInputIndex();
    void setInputIndex(int index);
protected:
    QPainterPath paintPath;
    QRectF mapMemRect;
    QLineF mapMemLine;
    int writeMemIndex;
public:
    InputPortType inputType;
    int inputIndex;
};

#endif // MODULEWRITEMEM_H
