#ifndef ModuleReadImd_H
#define ModuleReadImd_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>

#include "BasePort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class ModuleReadImd: public BasePort
{
    Q_OBJECT
public:
    explicit ModuleReadImd(int index,QGraphicsItem *parent = 0);
    virtual ~ModuleReadImd();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    int getReadImdIndex() { return readImdIndex;}
    void setReadImdIndex(int index);
    bool canSetReadImdIndex(int index);
protected:
    QPainterPath paintPath;
    QRectF mapImdRect;
    QLineF mapImdLine;
    int readImdIndex;
public:

};

#endif // ModuleReadImd_H
