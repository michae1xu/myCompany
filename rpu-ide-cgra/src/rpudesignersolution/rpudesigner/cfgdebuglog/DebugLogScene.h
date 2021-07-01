#ifndef DEBUGLOGSCENE_H
#define DEBUGLOGSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QPointF>

class DebugLogScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DebugLogScene(QObject *parent = 0);
    ~DebugLogScene();

    bool setItemBreakInfo(int typeId,int bcuId,int rcuId,int id,int portId,bool isIn,bool isEnable);
signals:
    void mouseClickedItem(QGraphicsItem* clickedItem);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QPointF mousePressPointF;
    QGraphicsItem* pressedItem;




};

#endif // DEBUGLOGSCENE_H
