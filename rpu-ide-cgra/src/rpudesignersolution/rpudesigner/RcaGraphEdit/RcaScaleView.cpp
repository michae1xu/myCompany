#include "RcaScaleView.h"

#include <QDebug>

RcaScaleView::RcaScaleView(QWidget *parent) : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

RcaScaleView::~RcaScaleView()
{
    //    QL_DEBUG<<"~RcaScaleView";
}

void RcaScaleView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mouseMoveEvent(event);
}

void RcaScaleView::mouseReleaseEvent(QMouseEvent *event)
{
    emit sendMousePosChanged(mapToScene(event->pos()));
    QAbstractScrollArea::mouseReleaseEvent(event);
}
