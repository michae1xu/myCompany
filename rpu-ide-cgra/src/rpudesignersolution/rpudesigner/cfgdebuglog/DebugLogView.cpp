#include "DebugLogView.h"
#include <QDebug>
#include <QScrollBar>
#include <cmath>

#define SCALE_BASE_NUM  1.1

DebugLogView::DebugLogView(QWidget *parent) : QGraphicsView(parent)
{

}

DebugLogView::~DebugLogView()
{
}

void DebugLogView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::CTRL)
    {
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        int pointY = 0;

        if (!numPixels.isNull()) {
            pointY = numPixels.y();
        } else if (!numDegrees.isNull()) {
            QPoint numSteps = numDegrees / 15;
            pointY = numSteps.y();
        }

        qreal factor = pow(SCALE_BASE_NUM, pointY);
        const QPointF posToScene = mapToScene(event->pos());
        scale(factor,factor);

        const QPointF posFromScenc = mapFromScene(posToScene);
        const QPointF posMove = posFromScenc - event->pos();
        horizontalScrollBar()->setValue(posMove.x() + horizontalScrollBar()->value());
        verticalScrollBar()->setValue(posMove.y() + verticalScrollBar()->value());

        event->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void DebugLogView::closeEvent(QCloseEvent *)
{
    emit debugLogViewClose();
}
