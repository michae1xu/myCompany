#include "RcaGridItem.h"

#include <qmath.h>

#include "RcaGraphScene.h"

RcaGridItem::RcaGridItem(QGraphicsScene *scene, QGraphicsItem *parent) : QGraphicsObject(parent)
    , rect(QRectF())
    , brush(QBrush())
{
    if(scene)
    {
        rect = scene->sceneRect();
    }
    setGridBackGround();
}

RcaGridItem::~RcaGridItem()
{

}

QRectF RcaGridItem::boundingRect() const
{
    return rect;
}

void RcaGridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setBrush(brush);
    painter->setPen(Qt::black);

    painter->drawRect(rect);

//    int posStep = MIN_POS_STEP_COUNT;
//    if(scene())
//    {
//        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
//        if(rcaScene)
//        {
//            posStep = qMax(posStep,rcaScene->getScenePosStep());
//        }
//    }
//    for(int i=0;i<rect.width();i = i+posStep)
//    {
//        for(int j=0;j<rect.height();j = j+posStep)
//        {
//            painter->drawPoint(i,j);
//        }
//    }
}

void RcaGridItem::setGridBackGround()
{
    int posXSetp = 5;
    int posYSetp = 5;


//    QPolygonF myPolygon1;
//    myPolygon1 << QPointF(0,posYSetp) << QPointF(2*posXSetp,posYSetp);
//    QPolygonF myPolygon2;
//    myPolygon2 << QPointF(posXSetp,0) << QPointF(posXSetp,2*posYSetp);
    QPixmap pixmap = QPixmap(posXSetp, posYSetp);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    painter.setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);

    qreal penWidth = 1;
    QPen pen(Qt::lightGray,penWidth);
    painter.setPen(pen);
    QPoint points[4] = {QPoint(0,0),
                        QPoint(posXSetp,0),
                        QPoint(0,posYSetp),
                        QPoint(posXSetp,posYSetp)};
    painter.drawPoints(points,4);
//    painter.drawPoint(0,0);
//    painter.drawPoint(posXSetp,0);
//    painter.drawPoint(0,posYSetp);
//    painter.drawPoint(posXSetp,posYSetp);

//    painter.drawEllipse(QPointF(0,0),penWidth/2,penWidth/2);
//    painter.drawEllipse(QPointF(posXSetp,0),penWidth/2,penWidth/2);
//    painter.drawEllipse(QPointF(0,posYSetp),penWidth/2,penWidth/2);
//    painter.drawEllipse(QPointF(posXSetp,posYSetp),penWidth/2,penWidth/2);

//    QVector<qreal> dashes;//line style--虚线
//    qreal space = 4;
//    dashes << 1 << space << 1 <<space;
//    QPen pen(Qt::lightGray,1);
//    pen.setDashPattern(dashes);
//    pen.setWidth(1);

//    painter.setPen(pen);
//    painter.translate(0, 0);
//    painter.drawPolyline(myPolygon1);
//    painter.drawPolyline(myPolygon2);

    brush = QBrush(pixmap);
//        rcaScene->setBackgroundBrush(pixmap);

//        QRadialGradient gradient(0, 0, 10);
//        gradient.setSpread(QGradient::RepeatSpread);
//        rcaScene->setBackgroundBrush(gradient);
//        rcaScene->setBackgroundBrush(Qt::blue);

//        rcaScene->update();
}
