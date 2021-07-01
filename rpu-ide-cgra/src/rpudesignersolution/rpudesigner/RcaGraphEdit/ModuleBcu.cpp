#include "ModuleBcu.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "RcaGraphScene.h"
#include "ItemUndoCommands.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QCursor>
#include <QtMath>
#include <QApplication>

ModuleBcu::ModuleBcu(int bcu,QGraphicsItem* parent) : BaseItem(parent)
    , indexBcu(bcu)
//    , outFifo0(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
//    , outFifo1(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
//    , outFifo2(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
//    , outFifo3(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
    , srcDataFromBcu(ModuleBcuIndex(bcu - 1 < 0 ? 3 : bcu - 1))
//    , inFifo0(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
//    , inFifo1(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
//    , inFifo2(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
//    , inFifo3(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
    , rcuExistList(QList<ModuleRcu*>())
//    , wRchMaxCount(RCU_WRITE_RCH_MAX_COUNT)
//    , wRchExistList(QList<int>())
//    , isWRchFulled(false)
//    , inPortRect(QRectF())
//    , outPortRect(QRectF())
    , rcuMaxCount(RCU_MAX_COUNT)
    , isRcuFulled(false)
    , rightMarkRect(QRectF())
{
    unitId = BCU_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);

    setObjectName(mapModuleUnitName[Module_Bcu]);
    setRealBoundingRect(BCU_MODULE_DEFAULT_RECT);
//    moduleBcuName = QString(mapModuleUnitName[Module_Bcu] + ":%1").arg(indexBcu);
    moduleBcuName = QString("RCU:%1").arg(sort);
    collidedItems.clear();

//    connect(inFifo0,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(inFifo1,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(inFifo2,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(inFifo3,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(outFifo0,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(outFifo1,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(outFifo2,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
//    connect(outFifo3,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
}

ModuleBcu::ModuleBcu(ModuleBcu &copyBcu, QGraphicsItem *parent) : BaseItem(parent)
  , indexBcu(copyBcu.indexBcu)
//  , outFifo0(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
//  , outFifo1(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
//  , outFifo2(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
//  , outFifo3(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , srcDataFromBcu(ModuleBcuIndex(indexBcu - 1 < 0 ? 3 : indexBcu - 1))
//  , inFifo0(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
//  , inFifo1(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
//  , inFifo2(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
//  , inFifo3(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , rcuExistList(QList<ModuleRcu*>())
//  , inPortRect(QRectF())
//  , outPortRect(QRectF())
  , rcuMaxCount(RCU_MAX_COUNT)
  , isRcuFulled(false)
  , rightMarkRect(QRectF())
{
    unitId = BCU_ID;

    setRealBoundingRect(copyBcu.realBoundingRect);
    setPos(copyBcu.pos());
//    moduleBcuName = QString(mapModuleUnitName[Module_Bcu] + ":%1").arg(indexBcu);
    moduleBcuName = QString("RCU:%1").arg(sort);
    collidedItems.clear();

    srcDataFromBcu = copyBcu.srcDataFromBcu;
    sort = copyBcu.sort;
    burst = copyBcu.burst;
    gap = copyBcu.gap;
    infifoSrc = copyBcu.infifoSrc;
    lfsrGroup = copyBcu.lfsrGroup;
    imdSrcConfig = copyBcu.imdSrcConfig;
    inFifoSrcConfig = copyBcu.inFifoSrcConfig;
    memorySrcConfig = copyBcu.memorySrcConfig;
    writeDepth = copyBcu.writeDepth;
    writeWidth = copyBcu.writeWidth;

//    inFifo0->copyOtherPort(*copyBcu.inFifo0);
//    inFifo1->copyOtherPort(*copyBcu.inFifo1);
//    inFifo2->copyOtherPort(*copyBcu.inFifo2);
//    inFifo3->copyOtherPort(*copyBcu.inFifo3);

//    outFifo0->copyOtherPort(*copyBcu.outFifo0);
//    outFifo1->copyOtherPort(*copyBcu.outFifo1);
//    outFifo2->copyOtherPort(*copyBcu.outFifo2);
//    outFifo3->copyOtherPort(*copyBcu.outFifo3);

    copyAllChildWithOtherBcu(copyBcu);
}

ModuleBcu::~ModuleBcu()
{
    QL_DEBUG << "~ModuleBcu()";
}

bool ModuleBcu::setIndexBcu(int index)
{
    if(index >= BCU_MAX_COUNT) return false;

    if(scene())
    {
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            moduleBcuName = QString("RCU:%1").arg(sort);
            if(rcaScene->findBcuFromScene(index) == NULL)
            {
                emit rcaScene->deleteBcuSuccessed(this);
                indexBcu = index;
//                moduleBcuName = QString(mapModuleUnitName[Module_Bcu] + ":%1").arg(indexBcu);
//                moduleBcuName = QString("RCU:%1").arg(sort);

                for(int i=0; i < rcuExistList.size(); i++)
                {
                    rcuExistList.at(i)->setIndexBcu(index);
                }
                return true;
            }
        }
    }
    return false;
}

void ModuleBcu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);

    painter->setBrush(BCU_BRUSH_FILLIN);
    painter->setPen(BCU_BOUNDING_PEN);
    painter->fillRect(paintRect,BCU_BRUSH_FILLIN);
    painter->drawRect(paintRect);

////    painter->setBrush(BCU_INFIFO_BRUSH_FILLIN);
//    painter->fillRect(inPortRect,BCU_BRUSH_FILLIN);
//    painter->drawRect(inPortRect);

////    painter->setBrush(BCU_OUTFIFO_BRUSH_FILLIN);
//    painter->fillRect(outPortRect,BCU_BRUSH_FILLIN);
//    painter->drawRect(outPortRect);

//    QL_DEBUG<<scene()->collidingItems(this,Qt::IntersectsItemBoundingRect).size();


    if(isSelected())
    {
        painter->setBrush(BCU_TEXT_SELECTED_BRUSH_FILLIN);
        painter->setPen(BCU_BOUNDING_PEN);
        painter->fillRect(textRect,BCU_TEXT_SELECTED_BRUSH_FILLIN);
        painter->drawRect(textRect);
        painter->drawRect(rightMarkRect);
    }
    else
    {
        painter->setBrush(BCU_TEXT_NORMAL_BRUSH_FILLIN);
        painter->setPen(BCU_BOUNDING_PEN);
        painter->fillRect(textRect,BCU_TEXT_NORMAL_BRUSH_FILLIN);
        painter->drawRect(textRect);
        painter->drawRect(rightMarkRect);
    }

    if(checkCollidedItems(Qt::IntersectsItemBoundingRect))
//    if(isCollidedItems)
    {
        painter->setBrush(BCU_COLLIDED_BRUSH_FILLIN);
        painter->setPen(BCU_COLLIDED_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,BCU_COLLIDED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }

    QFont font;
    font.setPixelSize(BCU_TEXT_PIXELSIZE);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(BCU_TEXT_PEN);    

//    painter->drawText(inPortRect, Qt::AlignCenter, BCU_IFIFO_NAME);
//    painter->drawText(outPortRect, Qt::AlignCenter, BCU_OFIFO_NAME);

    painter->rotate(90);
    painter->drawText(textRect.top() + 2,
                      (- (textRect.width() - font.pixelSize())/2 - textRect.left()),
                      moduleBcuName);
    painter->drawText(rightMarkRect.top() + 2,
                      (- (rightMarkRect.width() - font.pixelSize())/2 - rightMarkRect.left()),
                      moduleBcuName);
}
//由boundingrect调整到enabledRect。
QRectF ModuleBcu::paintRect2EnabledRect(QRectF rect)
{
    qreal adjust = ENABLED_BCU_RECT_IN_SPACE;
    //rect.adjustd 表示四个顶点坐标分别added参数。
    return rect.adjusted(adjust,adjust,-adjust,-adjust);
}

void ModuleBcu::setRealBoundingRect(QRectF rect)
{
    prepareGeometryChange();
    realBoundingRect = rect;

    textRect = QRectF(realBoundingRect.left(),realBoundingRect.top(),
                      BCU_TEXT_RECT_WIDTH,realBoundingRect.height());

//    inPortRect = QRectF(textRect.right(),realBoundingRect.top(),
//                        realBoundingRect.width() - BCU_TEXT_RECT_WIDTH - BCU_RIGHT_MARK_WIDTH,BCU_INPORT_RECT_HEIGHT);

//    outPortRect = QRectF(textRect.right(),realBoundingRect.bottom() - BCU_OUTPORT_RECT_HEIGHT,
//                        realBoundingRect.width() - BCU_TEXT_RECT_WIDTH - BCU_RIGHT_MARK_WIDTH,BCU_OUTPORT_RECT_HEIGHT);

    paintRect = QRectF(textRect.right(),textRect.top(),
                       realBoundingRect.width() - BCU_TEXT_RECT_WIDTH - BCU_RIGHT_MARK_WIDTH,
                       realBoundingRect.height());

    rightMarkRect = QRectF(paintRect.right(),textRect.top(),
                           BCU_RIGHT_MARK_WIDTH,realBoundingRect.height());
//    paintRect = QRectF(inPortRect.left(),inPortRect.bottom(),
//                       inPortRect.width(),
//                       realBoundingRect.height() - inPortRect.height() - outPortRect.height());

//    rightMarkRect = QRectF(inPortRect.right(),inPortRect.top(),
//                           BCU_RIGHT_MARK_WIDTH,realBoundingRect.height());

    enabledMoveableRect = paintRect2EnabledRect(paintRect);

//    qreal posX = inPortRect.left();
//    qreal posY = inPortRect.bottom() - inFifo0->getPaintRect().height() - DEFAULT_PEN_WIDTH;

//    qreal inPortSpace = (inPortRect.width() - inFifo0->getPaintRect().width()
//                                            - inFifo1->getPaintRect().width()
//                                            - inFifo2->getPaintRect().width()
//                                            - inFifo3->getPaintRect().width())/BCU_INPORT_SPACE_COUNT;

//    qreal outPortSpace = (outPortRect.width() - outFifo0->getPaintRect().width()
//                                              - outFifo1->getPaintRect().width()
//                                              - outFifo2->getPaintRect().width()
//                                              - outFifo3->getPaintRect().width())/BCU_OUTPORT_SPACE_COUNT;

//    posX += inPortSpace;
//    inFifo0->setPos(posX,posY);
//    posX += inFifo0->getPaintRect().width() + inPortSpace;
//    inFifo1->setPos(posX,posY);
//    posX += inFifo1->getPaintRect().width() + inPortSpace;
//    inFifo2->setPos(posX,posY);
//    posX += inFifo2->getPaintRect().width() + inPortSpace;
//    inFifo3->setPos(posX,posY);

//    posX = outPortRect.left();
//    posY = outPortRect.top() + DEFAULT_PEN_WIDTH;
//    posX += outPortSpace;
//    outFifo0->setPos(posX,posY);
//    posX += outFifo0->getPaintRect().width() + outPortSpace;
//    outFifo1->setPos(posX,posY);
//    posX += outFifo1->getPaintRect().width() + outPortSpace;
//    outFifo2->setPos(posX,posY);
//    posX += outFifo2->getPaintRect().width() + outPortSpace;
//    outFifo3->setPos(posX,posY);

    QL_DEBUG << realBoundingRect;

    update();
}


QRectF ModuleBcu::enabledRect2PaintRect(QRectF rect)
{
    qreal adjust = ENABLED_BCU_RECT_IN_SPACE;
    //rect.adjustd 表示四个顶点坐标分别added参数。
    return rect.adjusted(-adjust,-adjust,adjust,adjust);
}

QRectF ModuleBcu::enabledRect2BoundingRect(QRectF rect)
{
    QRectF paintRectTemp = enabledRect2PaintRect(rect);
    return paintRectTemp;
//    return paintRectTemp.adjusted(-textRect.width(),-DEFAULT_PEN_WIDTH,
//                                  +rightMarkRect.width(),DEFAULT_PEN_WIDTH);
}
void ModuleBcu::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(isEnabledAdjustRect)
    {
        setHoverCursor(event->pos());
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void ModuleBcu::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    qApp->restoreOverrideCursor();
    posAtItem = PosNone;
    QGraphicsItem::hoverLeaveEvent(event);
}

void ModuleBcu::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(isEnabledAdjustRect)
    {
        setHoverCursor(event->pos());
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void ModuleBcu::setSort(int value)
{
    sort = value;
    moduleBcuName = QString("RCU:%1").arg(sort);
}

bool ModuleBcu::adjustRect(QPointF pos)
{
    QL_DEBUG;
    QRectF tempRect = realBoundingRect;
    bool isChanged = false;

    //最小的边框大小。因为含有child，不能小于child的边框。
    QRectF minBoundingRect = getChildrenRcuMinRect();

//    qreal minWidth = minBoundingRect.width();
//    qreal minHeight = minBoundingRect.height();

    QRectF parentRect;
    QPointF posToParent;//当与parent的rect比较时，需要将pos转换为parent相对的pos进行比较。
    if(parentItem())
    {
        BaseItem* item = dynamic_cast<BaseItem*>(parentItem());
        if(item)
        {
            parentRect = item->getEnabledMoveableRect();
            posToParent = mapToItem(item,pos);
        }
    }
    else
    {
        parentRect = scene()->sceneRect();
        posToParent = mapToScene(pos);
    }

    //当设置top时，并且鼠标y坐标小于item的bottom并且不大于parent的top时，设置item的top坐标。
    if((posAtItem & PosTop) && (pos.y() < realBoundingRect.bottom()) && (posToParent.y() > parentRect.top()))
    {
        isChanged = true;
        //此item有child时，top设置为（y和最小边框中top较小的一个，top不能超过最小边框）。
        //此item没有child时，top设置为（y和bottom-min_height中较小的一个，top最小的位置）。
        tempRect.setTop(qMin(pos.y(),minBoundingRect.top()));
    }
    if((posAtItem & PosBottom) && (pos.y() > realBoundingRect.top()) && (posToParent.y() < parentRect.bottom()))
    {
        isChanged = true;
        tempRect.setBottom(qMax(pos.y(),minBoundingRect.bottom()));
    }
    if((posAtItem & PosLeft) && (pos.x() < realBoundingRect.right()) && (posToParent.x() > parentRect.left()))
    {
        isChanged = true;
        tempRect.setLeft(qMin(pos.x(),minBoundingRect.left()));
    }
    if((posAtItem & PosRight) && (pos.x() > realBoundingRect.left()) && (posToParent.x() < parentRect.right()))
    {
        isChanged = true;
        tempRect.setRight(qMax(pos.x(),minBoundingRect.right()));
    }
    if(isChanged)
    {
        isChangedColliedItems = true;
        setRealBoundingRect(tempRect);
        setChildrenRcuWidth(enabledMoveableRect.left(),enabledMoveableRect.width());
    }
    return isChanged;
}

//void ModuleBcu::showItemPortWire()
//{
//    QL_DEBUG;
//    if(inFifo0 && inFifo1 && inFifo2 && inFifo3
//            && outFifo0 && outFifo1 && outFifo2 && outFifo3)
//    {
//        inFifo0->showItemWire(this->isSelected());
//        inFifo1->showItemWire(this->isSelected());
//        inFifo2->showItemWire(this->isSelected());
//        inFifo3->showItemWire(this->isSelected());

//        outFifo0->showItemWire(this->isSelected());
//        outFifo1->showItemWire(this->isSelected());
//        outFifo2->showItemWire(this->isSelected());
//        outFifo3->showItemWire(this->isSelected());
//    }
//}

//void ModuleBcu::handleFifoDeleted(QObject *delFifo)
//{
//    if(inFifo0 == delFifo) inFifo0 = NULL;
//    if(inFifo1 == delFifo) inFifo1 = NULL;
//    if(inFifo2 == delFifo) inFifo2 = NULL;
//    if(inFifo3 == delFifo) inFifo3 = NULL;
//    if(outFifo0 == delFifo) outFifo0 = NULL;
//    if(outFifo1 == delFifo) outFifo1 = NULL;
//    if(outFifo2 == delFifo) outFifo2 = NULL;
//    if(outFifo3 == delFifo) outFifo3 = NULL;
//}

void ModuleBcu::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        RcaSceneStates sceneState = rcaScene->getSceneState();
        QL_DEBUG << event->pos().toPoint() << event->scenePos().toPoint() << event->button() << sceneState;
        if(Qt::LeftButton == event->button())
        {
            if(sceneState == Add_Rcu && enabledMoveableRect.contains(event->pos()))
            {
                addNewRcu(event->pos());
    //            return;//如果此时return，则拖动后的releaseEvent没有发送出来，造成下次拖动的时候，item复位先。
            }
        }
    }
    QL_DEBUG << event->modifiers();
    BaseItem::mouseReleaseEvent(event);
}

bool ModuleBcu::addRcuFromXml(QPointF posAtParent, ModuleRcu* moduleRcu)
{
    if(moduleRcu == NULL) return false;
    int rcuIndex = moduleRcu->getIndexRcu();
    QL_DEBUG << indexBcu << rcuIndex;
    if(isRcuFulled) return false;   //add rcu failed when rcu fulled.

    int insertIndex = rcuExistList.size();

    //rcuExistList 需要排序
    for(int i=0;i<rcuExistList.size();i++)
    {
        if(rcuIndex < rcuExistList.at(i)->getIndexRcu())
        {
            insertIndex = i;
            break;
        }
    }
    rcuExistList.insert(insertIndex,moduleRcu);

    moduleRcu->setParentItem(this);
    moduleRcu->setPos(posAtParent);

    connect(moduleRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));

    if(rcuMaxCount <= rcuExistList.size())
    {
        isRcuFulled = true;
        emit rcuFullStatesChanged(isRcuFulled);
    }
    return true;
}

bool ModuleBcu::addBfuFromXml(QPointF posAtParent, ElementBfu* elementBfu)
{
    if(elementBfu == NULL)  return false;
    ModuleRcu* moduleRcu = findRcuFromThis(elementBfu->getIndexRcu());
    if(moduleRcu == NULL) return false;

    QL_DEBUG << posAtParent;

    return moduleRcu->addBfuFromXml(posAtParent,elementBfu);
}

bool ModuleBcu::addCustomPEFromXml(QPointF posAtParent, ElementCustomPE* elementCustomPE)
{
    if(elementCustomPE == NULL)  return false;
    ModuleRcu* moduleRcu = findRcuFromThis(elementCustomPE->getIndexRcu());
    if(moduleRcu == NULL) return false;

    QL_DEBUG << posAtParent;

    return moduleRcu->addCustomPEFromXml(posAtParent,elementCustomPE);
}

bool ModuleBcu::addSboxFromXml(QPointF posAtParent, ElementSbox* elementSbox)
{
    if(elementSbox == NULL)  return false;
    ModuleRcu* moduleRcu = findRcuFromThis(elementSbox->getIndexRcu());
    if(moduleRcu == NULL) return false;

    QL_DEBUG << posAtParent;

    return moduleRcu->addSboxFromXml(posAtParent,elementSbox);
}

bool ModuleBcu::addBenesFromXml(QPointF posAtParent, ElementBenes* elementBenes)
{
    if(elementBenes == NULL)  return false;
    ModuleRcu* moduleRcu = findRcuFromThis(elementBenes->getIndexRcu());
    if(moduleRcu == NULL) return false;

    QL_DEBUG << posAtParent;

    return moduleRcu->addBenesFromXml(posAtParent,elementBenes);
}

ModuleRcu* ModuleBcu::findRcuFromThis(int rcuIndex)
{
    QList<QGraphicsItem *> listItems = this->childItems();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=listItems.begin();iter!=listItems.end();iter++)
    {
        if((*iter))
        {
            ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(*iter);
            if(moduleRcu)
            {
                QL_DEBUG << moduleRcu->getIndexRcu() << rcuIndex;
                if(moduleRcu->getIndexRcu() == rcuIndex)
                {
                    return moduleRcu;
                }
            }
        }
    }
    QL_DEBUG;
    return NULL;
}

void ModuleBcu::autoMakeAllRcu()
{
    for(int rcuIndex = 0;rcuIndex < rcuMaxCount; rcuIndex++)
    {
        autoMakeNewRcu(rcuIndex);
    }
    resetBoundingRectByChildrenBound();
}

void ModuleBcu::resetBoundingRectByChildrenBound()
{
    QRectF newRect = QRectF();
    for(int i=0; i<rcuExistList.size(); i++)
    {
        newRect |= mapRectFromItem(rcuExistList.at(i),rcuExistList.at(i)->getRealBoundingRect());
    }

    QL_DEBUG << newRect;
    if(newRect.isEmpty())
    {
        newRect = QRectF(BCU_MODULE_DEFAULT_RECT.left(),
                         BCU_MODULE_DEFAULT_RECT.top(),
                         BCU_MODULE_MIN_WIDTH,
                         BCU_MODULE_MIN_HEIGHT);
        QL_DEBUG << newRect;
    }
    else
    {
        //放大
        newRect = enabledRect2BoundingRect(newRect);

        newRect.setBottom(qMax(newRect.bottom(),newRect.top()+BCU_MODULE_MIN_HEIGHT));
        newRect.setRight(qMax(newRect.right(),newRect.left()+BCU_MODULE_MIN_WIDTH));
//        qreal adjust = DEFAULT_PEN_WIDTH/2;
//        newRect = newRect.adjusted(adjust,adjust,-adjust,-adjust);
//        newRect = newRect.adjusted(-ENABLED_RECT_IN_SPACE,-RCU_AT_BCU_YSPACE,ENABLED_RECT_IN_SPACE,RCU_AT_BCU_YSPACE);
        QL_DEBUG << newRect;
    }
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ItemRectChangedCommand* cmdChangeItemRect = new ItemRectChangedCommand(rcaScene,this,this->getRealBoundingRect(),newRect);
        rcaScene->pushCmdItemRectChanged(cmdChangeItemRect);
    }
//    setRealBoundingRect(newRect);
}

void ModuleBcu::setHoverCursor(QPointF pos)
{
    if(qFabs(pos.y() - realBoundingRect.top()) < DEFAULT_COLLIDE_RANGE)
    {
        if(qFabs(pos.x() - realBoundingRect.left()) < DEFAULT_COLLIDE_RANGE)
        {
            // cursor pos at top and left
            posAtItem = PosTopLeft;
            setCursor(Qt::SizeFDiagCursor);
        }
        else if(qFabs(pos.x() - realBoundingRect.right()) < DEFAULT_COLLIDE_RANGE)
        {
            // cursor pos at top and right
            posAtItem = PosTopRight;
            setCursor(Qt::SizeBDiagCursor);
        }
        else
        {
            // cursor pos at top
            posAtItem = PosTop;
            setCursor(Qt::SizeVerCursor);
        }
    }
    else if(qFabs(pos.y() - realBoundingRect.bottom()) < DEFAULT_COLLIDE_RANGE)
    {
        if(qFabs(pos.x() - realBoundingRect.left()) < DEFAULT_COLLIDE_RANGE)
        {
            // cursor pos at bottom and left
            posAtItem = PosBottomLeft;
            setCursor(Qt::SizeBDiagCursor);
        }
        else if(qFabs(pos.x() - realBoundingRect.right()) < DEFAULT_COLLIDE_RANGE)
        {
            // cursor pos at bottom and right
            posAtItem = PosBottomRight;
            setCursor(Qt::SizeFDiagCursor);
        }
        else
        {
            // cursor pos at bottom
            posAtItem = PosBottom;
            setCursor(Qt::SizeVerCursor);
        }
    }
    else if(qFabs(pos.x() - realBoundingRect.left()) < DEFAULT_COLLIDE_RANGE)
    {
        // cursor pos at left
        posAtItem = PosLeft;
        setCursor(Qt::SizeHorCursor);
    }
    else if(qFabs(pos.x() - realBoundingRect.right()) < DEFAULT_COLLIDE_RANGE)
    {
        // cursor pos at right
        posAtItem = PosRight;
        setCursor(Qt::SizeHorCursor);
    }
    else
    {
        posAtItem = PosNone;
        setCursor(Qt::ArrowCursor);
        qApp->restoreOverrideCursor();
    }
}

void ModuleBcu::autoSetPosOfChildRcu()
{
    QPointF rcuPos = RCU_POS_AT_BCU[0];

    for(int index=0;index<rcuExistList.size();index++)
    {
        ModuleRcu* moduleRcu = rcuExistList.at(index);
        moduleRcu->pushStackSetPos(rcuPos);
        moduleRcu->autoSetPosOfChildArithUnit();
        rcuPos.setY(rcuPos.y() + (RCU_AT_BCU_YSPACE + moduleRcu->getRealBoundingRect().height()));
    }
    resetBoundingRectByChildrenBound();
}

bool ModuleBcu::replaceRcuIndex(int indexSrc, int indexDst)
{
    if(findRcuFromThis(indexDst) != NULL) return false; //已存在indexDst。

    ModuleRcu* rcuSrc = findRcuFromThis(indexSrc);
    if(rcuSrc == NULL) return false;                    //不存在indexSrc。

    rcuExistList.removeOne(rcuSrc);

    int insertIndex = rcuExistList.size();

    //rcuExistList 需要排序
    for(int i=0;i<rcuExistList.size();i++)
    {
        if(indexDst < rcuExistList.at(i)->getIndexRcu())
        {
            insertIndex = i;
            break;
        }
    }
    rcuExistList.insert(insertIndex,rcuSrc);
    return true;
}

void ModuleBcu::setChildrenRcuWidth(qreal parentLeft, qreal width)
{
    for(int i=0;i<rcuExistList.count();i++)
    {
        QRectF rect = rcuExistList.at(i)->getRealBoundingRect();
        QPointF pos = mapToItem(rcuExistList.at(i),parentLeft,parentLeft);
        QL_DEBUG << pos << parentLeft << width;
        rect.setLeft(pos.x());
        rect.setWidth(width);
        rcuExistList.at(i)->setRealBoundingRect(rect);
    }
}

QRectF ModuleBcu::getChildrenRcuMinRect()
{
    QRectF result;
    if(rcuExistList.count() > 0)
    {
        for(int i=0;i<rcuExistList.count();i++)
        {
            ModuleRcu* moduleRcu = rcuExistList.at(i);
            if(moduleRcu && moduleRcu->isPartOfChildRect)
            {
                result |= mapRectFromItem(moduleRcu,moduleRcu->getChildrenMinRect());
//                QL_DEBUG << result;
            }
        }
//        QL_DEBUG <<result;
        result = enabledRect2BoundingRect(result);
        result = result.adjusted(0,0, +rightMarkRect.width(),0);
//        QL_DEBUG <<result;
    }
    else
    {
        result = QRectF(0,0,BCU_MODULE_MIN_WIDTH,BCU_MODULE_MIN_HEIGHT);
//        QL_DEBUG <<result;
    }
    QL_DEBUG << result;
    return result;
}

void ModuleBcu::insertRcuToList(int rcuIndex, ModuleRcu *rcuModule)
{
    if(rcuModule)
    {
        rcuExistList.insert(rcuIndex,rcuModule);
    }
}

void ModuleBcu::deleteRcuList(bool isForced)
{
//    while(!rcuExistList.isEmpty())
//    {
//        rcuExistList.first()->pushItemToDeletedStack(isForced);
//    }
    QList<ModuleRcu*> rcuListTemp = rcuExistList;
    for(int i=0;i<rcuListTemp.count();i++)
    {
        ModuleRcu* delRcu = rcuListTemp.at(i);
        delRcu->pushItemToDeletedStack(isForced);
    }
}

void ModuleBcu::undoRectToRealBoundingRect(QRectF rect)
{
    isChangedColliedItems = true;
    setRealBoundingRect(rect);
    setChildrenRcuWidth(enabledMoveableRect.left(),enabledMoveableRect.width());
}

void ModuleBcu::pushItemToDeletedStack(bool isForced)
{
    isForced |= isSelected();
    deleteRcuList(isForced);
    BaseItem::pushItemToDeletedStack(isForced);
}

bool ModuleBcu::autoMakeNewRcu(int rcuIndex)
{
    if(rcuIndex >= rcuMaxCount) return false;

    ModuleRcu* moduleRcu = NULL;

    for(int index=0;index<rcuExistList.size();index++)
    {
        if(rcuIndex == rcuExistList.at(index)->getIndexRcu())
        {
            moduleRcu = rcuExistList.at(index);
        }
    }

    if(moduleRcu == NULL)
    {
        if(isRcuFulled) return false;   //add rcu failed when rcu fulled.
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(!rcaScene) return false;

        AddRcuCommand* cmdAddRcu = new AddRcuCommand(rcaScene,indexBcu,rcuIndex,this);
        rcaScene->pushCmdAddRcu(cmdAddRcu);
        moduleRcu = cmdAddRcu->getRcuModule();
//        moduleRcu = new ModuleRcu(indexBcu,rcuIndex,this);
//        connect(moduleRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));
//        rcuExistList.insert(rcuIndex,moduleRcu);

        if(rcuMaxCount <= rcuExistList.size())
        {
            isRcuFulled = true;
            emit rcuFullStatesChanged(isRcuFulled);
        }

        QL_DEBUG << rcuExistList << rcuIndex;
        emit rcaScene->addNewRcuSuccess(moduleRcu);
    }
    moduleRcu->setPos(RCU_POS_AT_BCU[rcuIndex]);
    moduleRcu->autoMakeAllArithUnit();
    return true;
}

ModuleRcu* ModuleBcu::pasteNewRcu(ModuleRcu &copyRcu, QPointF posAtThis)
{
    //xf 调整粘贴后rcu的 x坐标=BCU_TEXT_RECT_WIDTH + ENABLED_BCU_RECT_IN_SPACE
    posAtThis.setX(BCU_TEXT_RECT_WIDTH + ENABLED_BCU_RECT_IN_SPACE);

    if(isRcuFulled) return NULL;   //add rcu failed when rcu fulled.
    int copyIndex = copyRcu.getIndexRcu();

    ModuleRcu* moduleRcu = NULL;

    for(int index=0;index<rcuExistList.size();index++)
    {
        if(copyIndex == rcuExistList.at(index)->getIndexRcu())
        {
            moduleRcu = rcuExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(moduleRcu == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<rcuExistList.size();curIndex++)
        {
            if(curIndex != rcuExistList.at(curIndex)->getIndexRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= rcuMaxCount) return NULL;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= rcuMaxCount) return NULL;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return NULL;

    AddRcuCommand* cmdAddRcu = new AddRcuCommand(rcaScene,indexBcu,pasteIndex,this);
    rcaScene->pushCmdAddRcu(cmdAddRcu);
    moduleRcu = cmdAddRcu->getRcuModule();

    if(rcuMaxCount <= rcuExistList.size())
    {
        isRcuFulled = true;
        emit rcuFullStatesChanged(isRcuFulled);
    }

    QL_DEBUG << rcuExistList << pasteIndex;
    emit rcaScene->addNewRcuSuccess(moduleRcu);

    moduleRcu->setRealBoundingRect(copyRcu.getRealBoundingRect());
    moduleRcu->setPos(posAtThis);

    moduleRcu->loopStartEndFlag = copyRcu.loopStartEndFlag;
    moduleRcu->loopTimes = copyRcu.loopTimes;
    moduleRcu->loopStartEndFlag2 = copyRcu.loopStartEndFlag2;
    moduleRcu->loopTimes2 = copyRcu.loopTimes2;
    moduleRcu->writeMemOffset = copyRcu.writeMemOffset;
    moduleRcu->writeMemMask = copyRcu.writeMemMask;
    moduleRcu->writeMemMode = copyRcu.writeMemMode;
    moduleRcu->writeMemThreashold = copyRcu.writeMemThreashold;
    moduleRcu->writeMemAddr = copyRcu.writeMemAddr;
    moduleRcu->readMemOffset = copyRcu.readMemOffset;
    moduleRcu->readMemMode = copyRcu.readMemMode;
    moduleRcu->readMemMode128_32 = copyRcu.readMemMode128_32;
    moduleRcu->readMemThreashold = copyRcu.readMemThreashold;
    moduleRcu->readMemAddr1 = copyRcu.readMemAddr1;
    moduleRcu->readMemAddr2 = copyRcu.readMemAddr2;
    moduleRcu->readMemAddr3 = copyRcu.readMemAddr3;
    moduleRcu->readMemAddr4 = copyRcu.readMemAddr4;
    moduleRcu->rchMode = copyRcu.rchMode;
    moduleRcu->writeRchAddr1 = copyRcu.writeRchAddr1;
    moduleRcu->writeRchAddr2 = copyRcu.writeRchAddr2;
    moduleRcu->writeRchAddr3 = copyRcu.writeRchAddr3;
    moduleRcu->writeRchAddr4 = copyRcu.writeRchAddr4;

    moduleRcu->pasteAllChildWithOtherRcu(copyRcu);
    return moduleRcu;
}

void ModuleBcu::pasteAllChildWithOtherBcu(ModuleBcu &otherBcu)
{
    for(int indexId = 0;indexId < otherBcu.rcuExistList.count(); indexId++)
    {
        ModuleRcu* copyRcu = otherBcu.rcuExistList.at(indexId);
        pasteNewRcu(*copyRcu,copyRcu->pos());
    }
//    resetBoundingRectByChildrenBound();
}

void ModuleBcu::copyAllChildWithOtherBcu(ModuleBcu &otherBcu)
{
    for(int indexId = 0;indexId < otherBcu.rcuExistList.count(); indexId++)
    {
        ModuleRcu* copyRcu = otherBcu.rcuExistList.at(indexId);
        copyOtherRcu(*copyRcu);
    }
}

bool ModuleBcu::copyOtherRcu(ModuleRcu &copyRcu)
{
    if(isRcuFulled) return false;   //add rcu failed when rcu fulled.
    int copyIndex = copyRcu.getIndexRcu();

    ModuleRcu* moduleRcu = NULL;

    for(int index=0;index<rcuExistList.size();index++)
    {
        if(copyIndex == rcuExistList.at(index)->getIndexRcu())
        {
            moduleRcu = rcuExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(moduleRcu == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<rcuExistList.size();curIndex++)
        {
            if(curIndex != rcuExistList.at(curIndex)->getIndexRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= rcuMaxCount) return false;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= rcuMaxCount) return false;

    moduleRcu = new ModuleRcu(copyRcu,this);
    rcuExistList.insert(pasteIndex,moduleRcu);

    if(rcuMaxCount <= rcuExistList.size())
    {
        isRcuFulled = true;
        emit rcuFullStatesChanged(isRcuFulled);
    }

    return true;
}

QList<ModuleRcu *> ModuleBcu::getSelectedRcu()
{
    QList<ModuleRcu *> resultList;
    resultList.clear();
    for(int i=0;i<rcuExistList.count();i++)
    {
        if(rcuExistList.at(i)->isSelected())
        {
            resultList.append(rcuExistList.at(i));
        }
    }
    return resultList;
}

void ModuleBcu::resetBoundingRectByPaste(QRectF newRect)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ItemRectChangedCommand* cmdChangeItemRect = new ItemRectChangedCommand(rcaScene,this,this->getRealBoundingRect(),newRect);
        rcaScene->pushCmdItemRectChanged(cmdChangeItemRect);
    }
}

bool ModuleBcu::addNewRcu(QPointF posAtThis)
{
    if(isRcuFulled) return false;   //add rcu failed when rcu fulled.

    int curIndex=0;
    for(;curIndex<rcuExistList.size();curIndex++)
    {
        if(curIndex != rcuExistList.at(curIndex)->getIndexRcu()) break;
    }

    QL_DEBUG << curIndex;

    if(curIndex >= rcuMaxCount) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;

    AddRcuCommand* cmdAddRcu = new AddRcuCommand(rcaScene,indexBcu,curIndex,this);
    rcaScene->pushCmdAddRcu(cmdAddRcu);
    ModuleRcu* moduleRcu = cmdAddRcu->getRcuModule();

//    ModuleRcu* moduleRcu = new ModuleRcu(indexBcu,curIndex,this);
//    rcuExistList.insert(curIndex,moduleRcu);
//    connect(moduleRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));

    // setpos的时候先要校验一下此pos放置item后，
    // scene的rect是否contains这个rcu。
    // 否，则重新调整位置。
    moduleRcu->setPos(posAtThis);

    if(rcuMaxCount <= rcuExistList.size())
    {
        isRcuFulled = true;
        emit rcuFullStatesChanged(isRcuFulled);
    }

    QL_DEBUG << rcuExistList << curIndex;
    emit rcaScene->addNewRcuSuccess(moduleRcu);

    return true;
}

void ModuleBcu::handleRcuBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG;
    if(!deletedItem) return;
    ModuleRcu* deletedRcu = dynamic_cast<ModuleRcu*>(deletedItem);
    if(!deletedRcu) return;

    int indexRcu = deletedRcu->getIndexRcu();
    if(indexRcu >= rcuMaxCount) return;

    if(rcuExistList.contains(deletedRcu))
    {
        rcuExistList.removeOne(deletedRcu);
        disconnect(deletedRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));
    }

    if(rcuMaxCount > rcuExistList.size())
    {
        isRcuFulled = false;
        emit rcuFullStatesChanged(isRcuFulled);
    }

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->isItemBakBeDeleted(deletedRcu);
        emit rcaScene->deleteRcuSuccess(deletedRcu);
    }

}
