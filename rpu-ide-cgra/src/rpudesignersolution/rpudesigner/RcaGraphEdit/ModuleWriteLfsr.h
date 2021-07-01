#ifndef MODULEWRITELFSR_H
#define MODULEWRITELFSR_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleWriteLfsr: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleWriteLfsr(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleWriteLfsr();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getWriteLfsrIndex(){ return writeLfsrIndex;}
    bool setWriteLfsrIndex(int index);
    QString canSetWriteLfsrIndex(int index);
    InputPortType getInputType();
    void setInputType(InputPortType type);
    int getInputIndex();
    void setInputIndex(int index);
protected:
    QPainterPath paintPath;
    QRectF mapRchRect;
    QLineF mapRchLine;
    int writeLfsrIndex;
public:
    InputPortType inputType;
    int inputIndex;
};

#endif // MODULEWRITELFSR_H

