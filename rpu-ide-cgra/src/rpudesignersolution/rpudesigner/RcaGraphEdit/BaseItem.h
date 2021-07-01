#ifndef BASEITEM_H
#define BASEITEM_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QRectF>
#include <QVariant>
#include <QKeyEvent>
#include <QGraphicsSceneHoverEvent>
#include <QEvent>
#include <QGraphicsItemGroup>
#include "RcaGraphDef.h"

class QUndoCommand;

using namespace rca_space;

class BaseItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit BaseItem(QGraphicsItem *parent = 0);
    virtual ~BaseItem();
    virtual QRectF getEnabledMoveableRect();
    virtual QRectF getPaintRect();
    virtual QRectF getRealBoundingRect();
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void clearCollidedItems();
    void setWindowToModified();
    void deleteThisItem();
    void deleteThisForUndo();
    QPointF getOldPos(){return oldPos;}
    void pushStackSetPos(QPointF newPos);
    void recheckCollidedItems();
    ElementUnitId getUnitId() { return unitId;}
    virtual void pushItemToDeletedStack(bool isForced);
    virtual void setRealBoundingRect(QRectF rect);
protected:
    QRectF enabledMoveableRect; //child可以移动的内部区域。
    QRectF paintRect;           //绘画的边框，算子的paintrect与realBoundingRect不同，其他都相同。
    QRectF textRect;            //文字显示的区域
    QRectF realBoundingRect;    //外边框，不含pen的边框。
    QList<QGraphicsItem*> collidedItems;
    HoverPosAtItem posAtItem;
    bool isEnabledAdjustRect;
    bool isCollidedItems;
    bool isChangedColliedItems;
    QPointF oldPos;
    ElementUnitId unitId;
    bool hasRectChanged;

signals:
    void itemBeDeleted(BaseItem* deletedItem);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event)Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;


    QRectF paintRect2EnabledRect(QRectF rect);   //通过 realBounding rect 获得 enabled rect
    QRectF enabledRect2PaintRect(QRectF rect);   //通过 realBounding rect 获得 enabled rect
    virtual QRectF enabledRect2BoundingRect(QRectF rect);
    bool checkCollidedItems(Qt::ItemSelectionMode mode);
    void deleteThisFromCollidedItems();//delete this的时候从其他的colliede的item中删除thisitem。
    virtual void showItemPortWire();
    virtual void setChildrenSelected(bool enabled);
    virtual bool adjustRect(QPointF pos);
public:
    bool isPartOfChildRect;   //true 表示计算child的rect时计算在内，false则不计算。
    QUndoCommand* lastCmd;
    QRectF oldRect;
public slots:


private:
//    void setHoverCursor(QPointF pos);
    QRectF adjust2BoundingRect(QRectF rect);      //通过pen的大小调整boundingRect
};

#endif // BASEITEM_H
