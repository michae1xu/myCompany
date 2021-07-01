#include "DebugLogScene.h"
#include <QDebug>
#include <QTransform>
#include "cfgdebuglog/DebugLogItem.h"

DebugLogScene::DebugLogScene(QObject *parent) : QGraphicsScene(parent)
    ,mousePressPointF(QPointF())
    ,pressedItem(NULL)
{
    clearFocus();
}

DebugLogScene:: ~DebugLogScene()
{
}

bool DebugLogScene::setItemBreakInfo(int typeId, int bcuId, int rcuId, int id, int portId, bool isIn, bool isEnable)
{
    foreach (QGraphicsItem* item, items())
    {
        DebugLogItem* res = dynamic_cast<DebugLogItem*>(item);
        if(res && res->elementItemInfo->typeId == typeId
                && res->elementItemInfo->bcu == bcuId
                && res->elementItemInfo->rcu == rcuId
                && res->elementItemInfo->id == id)
        {
            return res->setBreakInfo(isIn,portId,isEnable);
        }
    }
    return false;
}

void DebugLogScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if((pressedItem != itemAt(event->scenePos(),QTransform())))
    {
        clearFocus();
        pressedItem = itemAt(event->scenePos(),QTransform());
    }
    mousePressPointF = event->screenPos();
}

void DebugLogScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem* clickedItem = itemAt(event->scenePos(),QTransform());
    if(clickedItem != focusItem())
    {
        if((event->screenPos() ==mousePressPointF) && (pressedItem == clickedItem))
        {
            setFocusItem(clickedItem);
            emit mouseClickedItem(clickedItem);
        }
    }
    if(!hasFocus())
    {
        pressedItem = NULL;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

