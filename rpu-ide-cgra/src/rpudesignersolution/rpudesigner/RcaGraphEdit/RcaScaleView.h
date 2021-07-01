#ifndef RCASCALEVIEW_H
#define RCASCALEVIEW_H

#include <QGraphicsView>
#include <QPointF>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>

class RcaScaleView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RcaScaleView(QWidget *parent = 0);
    ~RcaScaleView();

signals:
    void sendMousePosChanged(QPointF posOnScene);

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE {QAbstractScrollArea::keyPressEvent(event);}
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE{QAbstractScrollArea::keyReleaseEvent(event);}
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE{QAbstractScrollArea::mouseDoubleClickEvent(event);}
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE{QAbstractScrollArea::mouseMoveEvent(event);}
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE{QAbstractScrollArea::wheelEvent(event);}
private:

};


#endif // RCASCALEVIEW_H
