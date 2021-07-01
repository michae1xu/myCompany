#include "BaseItem.h"
#include "RcaGraphScene.h"
#include "RcaItemSetting/RcaSetting.h"
#include "ItemUndoCommands.h"

#include <QGraphicsScene>
#include <QListIterator>
#include <QPointer>
#include <QDebug>
#include <QCursor>
#include <QtMath>
#include <QApplication>
#include <QUndoCommand>
#include "citemattributesetting.h"

BaseItem::BaseItem(QGraphicsItem *parent) : QGraphicsObject(parent)
    , enabledMoveableRect(QRectF())
    , paintRect(QRectF())
    , textRect(QRectF())
    , realBoundingRect(QRectF())
    , posAtItem(PosNone)
    , isEnabledAdjustRect(true)
    , isCollidedItems(false)
    , isChangedColliedItems(true)
    , oldPos(QPointF())
    , unitId(UNIT_NONE)
    , hasRectChanged(false)
    , isPartOfChildRect(true)
    , lastCmd(NULL)
    , oldRect(QRectF())
{
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    collidedItems.clear();

//    setZValue(qreal(0));
//    QL_DEBUG << "zValue:" << zValue();
//    setToolTip("this is tip");
}

BaseItem::~BaseItem()
{
//    emit itemBeDeleted(this);
//    deleteThisFromCollidedItems();
//    QL_DEBUG<<"~BaseItem()";
}

QRectF BaseItem::getEnabledMoveableRect()
{
    return enabledMoveableRect;
}

QRectF BaseItem::getPaintRect()
{
    return paintRect;
}

QRectF BaseItem::getRealBoundingRect()
{
    return realBoundingRect;
}

QRectF BaseItem::boundingRect() const
{
    qreal adjust = DEFAULT_PEN_WIDTH/2;
    return realBoundingRect.adjusted(-adjust,-adjust,adjust,adjust);
}

void BaseItem::clearCollidedItems()
{
    collidedItems.clear();
}

void BaseItem::showItemPortWire()
{
}

void BaseItem::setChildrenSelected(bool enabled)
{
    for(int i=0;i<childItems().count();i++)
    {
        QGraphicsItem* childItem = dynamic_cast<QGraphicsItem*>(childItems().at(i));
        if(childItem)
        {
           childItem->setSelected(enabled);
        }
    }
}

QVariant BaseItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(ItemSelectedHasChanged == change)
    {
//        setChildrenSelected(value.toBool());
        showItemPortWire();
        if(value.toBool())
        {
            oldPos = this->pos();
        }
    }
    if(scene())
    {        
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if (change == ItemPositionChange && rcaScene && (rcaScene->isAutoSetItemPos == false))
        {
//            setWindowToModified();
//            int posStep = rcaScene->getScenePosStep() >= MIN_POS_STEP_COUNT ? rcaScene->getScenePosStep() : MIN_POS_STEP_COUNT;
            isChangedColliedItems = true;
            // value is the new position. pos 坐标是相对parent的坐标，就是setpos后get的pos坐标。
//            QL_DEBUG << value.toPoint();
            QPointF resultPos = value.toPointF();
            QPointF newPos = QPointF(resultPos.x() + realBoundingRect.left(),
                                     resultPos.y() + realBoundingRect.top());
//            if((qAbs(newPos.x() - this->pos().x()) < posStep)
//                    && (qAbs(newPos.y() - this->pos().y()) < posStep))
//            {
//                return this->pos();
//            }
            QRectF rect = QRectF();
            if(parentItem())
            {
                BaseItem* item = dynamic_cast<BaseItem*>(parentItem());
                if(item)
                {
                    rect = item->getEnabledMoveableRect();
                }
    //            QL_DEBUG << "parent" << rect.toRect();
            }
            else
            {
                rect = scene()->sceneRect();
    //            QL_DEBUG << "scene" << rect.toRect();
            }

            QRectF itemChangedRect = QRectF(newPos.x(),newPos.y(),realBoundingRect.width(),realBoundingRect.height());
    //        QL_DEBUG << itemChangedRect.toRect() << realBoundingRect.toRect() << rect.toRect() << objectName();
            if (!rect.contains(itemChangedRect))
            {
                // Keep the item inside the scene rect.
                // 右下边对齐优先级高
                newPos.setX(qMin(rect.right()-realBoundingRect.width(), qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom()-realBoundingRect.height(), qMax(newPos.y(), rect.top())));

                resultPos = QPointF(newPos.x() - realBoundingRect.left(),
                                    newPos.y() - realBoundingRect.top());
    //            QL_DEBUG << resultPos.toPoint() << newPos.toPoint() << itemChangedRect.toRect() << rect.toRect() << realBoundingRect.toRect();
                return resultPos;
            }
        }
        if(rcaScene && (rcaScene->isAutoSetItemPos == true))
        {
            isChangedColliedItems = true;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

bool BaseItem::checkCollidedItems(Qt::ItemSelectionMode mode)
{
    if(isChangedColliedItems)
    {
        isChangedColliedItems = false;
        QList<QGraphicsItem*> currentCollidedItems = collidingItems(mode);
    //    QL_DEBUG << listTemp.size() << objectName();

        //先检测已有的collidedItems各个item在最新的currentCollidedItems中有没有包括
        //如果最新的currentCollidedItems中包含此item，则此item不变。
        //如果最新的currentCollidedItems中不包含此item了，则此item重新isChangedColliedItems = true，update并在collidedItems中删除此item。
        QListIterator<QGraphicsItem*> itemListTemp(collidedItems);
        {
            while (itemListTemp.hasNext())
            {
                QGraphicsItem *item = itemListTemp.next();
                if(item)
                {
                    if(!currentCollidedItems.contains(item))
                    {
                        BaseItem* baseTemp = dynamic_cast<BaseItem*>(item);
                        if(baseTemp)
                        {
                            baseTemp->isChangedColliedItems = true;
                            baseTemp->update();
                        }
                        collidedItems.removeOne(item);
                    }
                }
            }
        }
        itemListTemp = currentCollidedItems;
        //再检测最新的currentCollidedItems各个item在已有的collidedItems中有没有包括
        //如果已有的collidedItems中包含此item，则此item不变。
        //如果已有的collidedItems中不包含此item了，则此item添加到collidedItems中
        //并对此item中的collidedItems添加this（item），并将isCollidedItems设置为true，update。此时不会重新checkCollidedItems。
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                BaseItem* baseTemp = dynamic_cast<BaseItem*>(item);
    //            QL_DEBUG << baseTemp->objectName() << objectName();
                if((baseTemp) && (baseTemp->objectName() == this->objectName()))
                {
    //                QL_DEBUG << baseTemp->objectName() << objectName();
                    if(!collidedItems.contains(baseTemp))
                    {
                        collidedItems.append(baseTemp);
                        baseTemp->collidedItems.append(this);
                        baseTemp->isCollidedItems = true;
                        baseTemp->update();
                    }
                }
            }
        }
        isCollidedItems = collidedItems.size()>0 ? true : false;
    }
    return isCollidedItems;
}

void BaseItem::deleteThisFromCollidedItems()
{
    //因为此item被删除，则所有的collidedItems的item中的collidedItems删除this(item).
    //并检测baseTemp->collidedItems.size()，小于等于0，则直接isCollidedItems=fasle。update。
    //这样不会重新checkCollidedItems
    QList<QGraphicsItem*>::iterator iter;
    for(iter=collidedItems.begin();iter!=collidedItems.end();iter++)
    {
        if((*iter))
        {
            BaseItem* baseTemp = dynamic_cast<BaseItem*>(*iter);
            if(baseTemp)
            {
                baseTemp->collidedItems.removeOne(this);
                if(baseTemp->collidedItems.size() <= 0)
                {
                    baseTemp->isCollidedItems = false;
                    baseTemp->update();
                }
            }
        }
    }
}

void BaseItem::setWindowToModified()
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        emit rcaScene->sendWindowToModified(true);
    }
}

void BaseItem::deleteThisItem()
{
    deleteThisFromCollidedItems();
    collidedItems.clear();
    emit itemBeDeleted(this);
    delete this;
//    this->deleteLater();
}

void BaseItem::deleteThisForUndo()
{
    deleteThisFromCollidedItems();
    collidedItems.clear();
    emit itemBeDeleted(this);
}

void BaseItem::pushStackSetPos(QPointF newPos)
{
    RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return;

    oldPos = pos();
    setPos(newPos);
    QList<BaseItem*> movedItemList;
    movedItemList.append(this);
    rcaScene->pushCmdItemMoved(movedItemList);
}

void BaseItem::recheckCollidedItems()
{
    isChangedColliedItems = true;
}

void BaseItem::pushItemToDeletedStack(bool isForced)
{
    RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return;
    if(isForced || isSelected())
    {
        DeleteItemCommand* cmdDelItem = new DeleteItemCommand(rcaScene,this);
        rcaScene->pushCmdDeleteItem(cmdDelItem);
    }
}

void BaseItem::keyPressEvent(QKeyEvent *event)
{
//    if(event->key() == Qt::Key_Delete)
//    {
//        QL_DEBUG;
//        if(isSelected())
//        {
//            deleteThisFromCollidedItems();
//            collidedItems.clear();
//            emit itemBeDeleted(this);
//            delete this;
//        }
//        return;
//    }
    QGraphicsItem::keyPressEvent(event);
}

void BaseItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    oldPos = this->pos();
//    QL_DEBUG << oldPos;
    if(posAtItem != PosNone)
    {
        oldRect = realBoundingRect;
        hasRectChanged = false;
    }
    QGraphicsItem::mousePressEvent(event);
}

void BaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(posAtItem != PosNone)
    {
        if(adjustRect(event->pos()))
        {
            hasRectChanged = true;
//            setWindowToModified();
        }
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void BaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    if(oldPos != this->pos())
//    {
//        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
//        if(rcaScene)
//        {
//            rcaScene->itemMoved(this,oldPos);
//        }
//    }
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->doubleClickedItemBak = this;
//        rcaScene->getCurItemSetting()->updateItemAttribute(this);
    }

    if(hasRectChanged)
    {
        hasRectChanged = false;
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            ItemRectChangedCommand* cmdChangeItemRect = new ItemRectChangedCommand(rcaScene,this,oldRect,realBoundingRect);
            rcaScene->pushCmdItemRectChanged(cmdChangeItemRect);
        }
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void BaseItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//    if(isEnabledAdjustRect)
//    {
//        setHoverCursor(event->pos());
//    }
    QGraphicsItem::hoverEnterEvent(event);
}

void BaseItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
//    setCursor(Qt::ArrowCursor);
//    qApp->restoreOverrideCursor();
//    posAtItem = PosNone;
    QGraphicsItem::hoverLeaveEvent(event);
}

void BaseItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
//    if(isEnabledAdjustRect)
//    {
//        setHoverCursor(event->pos());
//    }
    QGraphicsItem::hoverMoveEvent(event);
}

void BaseItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            if(rcaScene->getSceneState() != Painting_wire)
            {
                rcaScene->itemDoubleClicked(this);
            }
//            RcaSetting *rcaSetting = RcaSetting::instance(rcaScene);
//            if(rcaSetting)
//            {
//                rcaSetting->show(this);
//            }
        }
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

//更改item rect.
bool BaseItem::adjustRect(QPointF pos)
{
    QL_DEBUG;
    QRectF tempRect = realBoundingRect;
    bool isChanged = false;
    bool hasChild = false;

    QRectF minBoundingRect; //最小的边框大小。因为含有child，不能小于child的边框。
    if(childItems().count() > 0)
    {
        for(int i=0; i<childItems().size(); i++)
        {
            BaseItem *child = dynamic_cast<BaseItem*>(childItems().at(i));
            if((child) && (child->isPartOfChildRect))
            {
                hasChild = true;
                minBoundingRect |= mapRectFromItem(child,child->realBoundingRect);
            }
        }
    //    minBoundingRect = minBoundingRect.adjusted(-ENABLED_RECT_IN_SPACE,-ENABLED_RECT_IN_SPACE,ENABLED_RECT_IN_SPACE,ENABLED_RECT_IN_SPACE);
        minBoundingRect = enabledRect2BoundingRect(minBoundingRect);
    }

    qreal minWidth = MIN_MODULE_WIDTH;
    qreal minHeight = MIN_MODELE_HEIGHT;
    switch(unitId)
    {
    case BCU_ID:
        minWidth = BCU_MODULE_MIN_WIDTH;
        minHeight = BCU_MODULE_MIN_HEIGHT;
        break;
    case RCU_ID:
        minWidth = RCU_MODULE_MIN_WIDTH;
        minHeight = RCU_MODULE_MIN_HEIGHT;
        break;
    default:break;
    }

    QRectF parentRect;
    QPointF posToParent;//当与parent的rect比较时，需要将pos转换为parent相对的pos进行比较。
    if(parentItem())
    {
//            QL_DEBUG<< "parent";
        BaseItem* item = dynamic_cast<BaseItem*>(parentItem());
        if(item)
        {
            parentRect = item->getEnabledMoveableRect();
            posToParent = mapToItem(item,pos);
        }
    }
    else
    {
//            QL_DEBUG<< "scene";
        parentRect = scene()->sceneRect();
        posToParent = mapToScene(pos);
    }

//    QL_DEBUG<< minBoundingRect << childrenBoundingRect() << hasChild;
//    QL_DEBUG<< parentRect << pos << posToParent << posAtItem;
    //当设置top时，并且鼠标y坐标小于item的bottom并且不大于parent的top时，设置item的top坐标。
    if((posAtItem & PosTop) && (pos.y() < realBoundingRect.bottom()) && (posToParent.y() > parentRect.top()))
    {
        isChanged = true;
        //此item有child时，top设置为（y和最小边框中top较小的一个，top不能超过最小边框）。
        //此item没有child时，top设置为（y和bottom-min_height中较小的一个，top最小的位置）。
        tempRect.setTop(hasChild ? qMin(pos.y(),minBoundingRect.top()) : qMin(pos.y(),tempRect.bottom()-minHeight));
    }
    if((posAtItem & PosBottom) && (pos.y() > realBoundingRect.top()) && (posToParent.y() < parentRect.bottom()))
    {
        isChanged = true;
        tempRect.setBottom(hasChild ? qMax(pos.y(),minBoundingRect.bottom()) : qMax(pos.y(),tempRect.top()+minHeight));
    }
    if((posAtItem & PosLeft) && (pos.x() < realBoundingRect.right()) && (posToParent.x() > parentRect.left()))
    {
        isChanged = true;
        tempRect.setLeft(hasChild ? qMin(pos.x(),minBoundingRect.left()) : qMin(pos.x(),tempRect.right()-minWidth));
    }
    if((posAtItem & PosRight) && (pos.x() > realBoundingRect.left()) && (posToParent.x() < parentRect.right()))
    {
        isChanged = true;
        tempRect.setRight(hasChild ? qMax(pos.x(),minBoundingRect.right()) : qMax(pos.x(),tempRect.left()+minWidth));
    }
    if(isChanged)
    {
//        QL_DEBUG << tempRect << childrenBoundingRect();
        isChangedColliedItems = true;
        setRealBoundingRect(tempRect);
    }
    return isChanged;
}

//void BaseItem::setHoverCursor(QPointF pos)
//{
////    QL_DEBUG << pos << realBoundingRect;

//    if(qFabs(pos.y() - realBoundingRect.top()) < DEFAULT_COLLIDE_RANGE)
//    {
//        if(qFabs(pos.x() - realBoundingRect.left()) < DEFAULT_COLLIDE_RANGE)
//        {
//            // cursor pos at top and left
//            posAtItem = PosTopLeft;
//            setCursor(Qt::SizeFDiagCursor);
//        }
//        else if(qFabs(pos.x() - realBoundingRect.right()) < DEFAULT_COLLIDE_RANGE)
//        {
//            // cursor pos at top and right
//            posAtItem = PosTopRight;
//            setCursor(Qt::SizeBDiagCursor);
//        }
//        else
//        {
//            // cursor pos at top
//            posAtItem = PosTop;
//            setCursor(Qt::SizeVerCursor);
//        }
//    }
//    else if(qFabs(pos.y() - realBoundingRect.bottom()) < DEFAULT_COLLIDE_RANGE)
//    {
//        if(qFabs(pos.x() - realBoundingRect.left()) < DEFAULT_COLLIDE_RANGE)
//        {
//            // cursor pos at bottom and left
//            posAtItem = PosBottomLeft;
//            setCursor(Qt::SizeBDiagCursor);
//        }
//        else if(qFabs(pos.x() - realBoundingRect.right()) < DEFAULT_COLLIDE_RANGE)
//        {
//            // cursor pos at bottom and right
//            posAtItem = PosBottomRight;
//            setCursor(Qt::SizeFDiagCursor);
//        }
//        else
//        {
//            // cursor pos at bottom
//            posAtItem = PosBottom;
//            setCursor(Qt::SizeVerCursor);
//        }
//    }
//    else if(qFabs(pos.x() - realBoundingRect.left()) < DEFAULT_COLLIDE_RANGE)
//    {
//        // cursor pos at left
//        posAtItem = PosLeft;
//        setCursor(Qt::SizeHorCursor);
//    }
//    else if(qFabs(pos.x() - realBoundingRect.right()) < DEFAULT_COLLIDE_RANGE)
//    {
//        // cursor pos at right
//        posAtItem = PosRight;
//        setCursor(Qt::SizeHorCursor);
//    }
//    else
//    {
//        posAtItem = PosNone;
//        setCursor(Qt::ArrowCursor);
//        qApp->restoreOverrideCursor();
//    }
//}

void BaseItem::setRealBoundingRect(QRectF rect)
{
    Q_UNUSED(rect);
}

QRectF BaseItem::enabledRect2BoundingRect(QRectF rect)
{
    return rect;
}

//由boundingrect调整到enabledRect。
QRectF BaseItem::paintRect2EnabledRect(QRectF rect)
{
    qreal adjust = ENABLED_RECT_IN_SPACE;
    //rect.adjustd 表示四个顶点坐标分别added参数。
    return rect.adjusted(adjust,adjust,-adjust,-adjust);
}

QRectF BaseItem::enabledRect2PaintRect(QRectF rect)
{
    qreal adjust = ENABLED_RECT_IN_SPACE;
    //rect.adjustd 表示四个顶点坐标分别added参数。
    return rect.adjusted(-adjust,-adjust,adjust,adjust);
}

//paint的时候pen有粗细，设置的bounding描画时，pen以中点划线，
//所以bounding的外面还有pen/2的区域需要重新绘画，
//此函数有rect得到描画的bounding区域。
QRectF BaseItem::adjust2BoundingRect(QRectF rect)
{
    qreal adjust = DEFAULT_PEN_WIDTH/2;
    return rect.adjusted(-adjust,-adjust,adjust,adjust);
}

