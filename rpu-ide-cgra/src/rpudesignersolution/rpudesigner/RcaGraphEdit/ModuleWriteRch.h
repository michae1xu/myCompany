#ifndef MODULEWRITERCH_H
#define MODULEWRITERCH_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleWriteRch: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleWriteRch(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleWriteRch();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getWriteRchIndex(){ return writeRchIndex;}
    bool setWriteRchIndex(int index);
    QString canSetWriteRchIndex(int index);
    InputPortType getInputType();
    void setInputType(InputPortType type);
    int getInputIndex();
    void setInputIndex(int index);
    int getWriteAddress() const;
    void setWriteAddress(int value);
//    int getIndexRcu() {return indexRcu;}
//    int getRcuIndex();
protected:
    QPainterPath paintPath;
    QRectF mapRchRect;
    QLineF mapRchLine;
    int writeRchIndex;
    int writeAddress;
//    int indexRcu;
//    int indexBcu;
public:
    InputPortType inputType;
    int inputIndex;
//    int getIndexBcu() const;
//    void setIndexRcu(int value);
};

#endif // MODULEWRITERCH_H
