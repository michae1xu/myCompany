#ifndef RCAGRIDITEM_H
#define RCAGRIDITEM_H

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QBrush>

#include "RcaGraphDef.h"

using namespace rca_space;

class RcaGridItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RcaGridItem(QGraphicsScene* scene,QGraphicsItem *parent = 0);
    virtual ~RcaGridItem();
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    void setGridBackGround();
private:
    QRectF rect;
    QBrush brush;
};

#endif // RCAGRIDITEM_H
