#include "ModuleRcu.h"
#include "RcaGraphScene.h"
#include "BaseArithUnit.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "elementcustompe.h"
#include "ItemUndoCommands.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QCursor>
#include <QtMath>

ModuleRcu::ModuleRcu(int bcu, int rcu, QGraphicsItem* parent) : BaseItem(parent)
  , indexBcu(bcu)
  , indexRcu(rcu)
  , bfuMaxCount(BFU_MAX_COUNT)
  , bfuExistList(QList<ElementBfu*>())
  , isBfuFulled(false)
  , customPEExistList(QList<ElementCustomPE*>())
  , sboxMaxCount(SBOX_MAX_COUNT)
  , sboxExistList(QList<ElementSbox*>())
  , isSboxFulled(false)
  , benesMaxCount(BENES_MAX_COUNT)
  , benesExistList(QList<ElementBenes*>())
  , isBenesFulled(false)
  , inPortRect(QRectF())
  , outPortRect(QRectF())
  , inFifo0(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , inFifo1(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , inFifo2(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , inFifo3(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , outFifo0(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , outFifo1(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , outFifo2(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , outFifo3(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , wMemMaxCount(WRITE_MEM_MAX_COUNT)
  , wMemExistList(QList<int>())
  , isWMemFulled(false)
  , wRchMaxCount(WRITE_RCH_MAX_COUNT)
  , wRchExistList(QList<int>())
  , isWRchFulled(false)
  , wLfsrMaxCount(WRITE_LFSR_MAX_COUNT)
  , wLfsrExistList(QList<int>())
  , isWLfsrFulled(false)
  , writeMemOffset(0)
  , writeMemMask(0)
  , writeMemMode(0)
  , writeMemThreashold(0)
  , writeMemAddr(0)
  , readMemOffset(0)
  , readMemMode(0)
  , readMemMode128_32(0)
  , readMemThreashold(0)
  , readMemAddr1(0)
  , readMemAddr2(0)
  , readMemAddr3(0)
  , readMemAddr4(0)
  , rchMode(0)
  , writeRchAddr1(0)
  , writeRchAddr2(0)
  , writeRchAddr3(0)
  , writeRchAddr4(0)
{
    unitId = RCU_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);

    setObjectName(mapModuleUnitName[Module_Rcu]);
    QRectF rect = RCU_MODULE_DEFAULT_RECT;
    ModuleBcu* bcuModule = dynamic_cast<ModuleBcu*>(parentItem());
    if(bcuModule)
    {
        QRectF bcuRect = bcuModule->getRealBoundingRect();
        rect = QRectF(bcuRect.left(),bcuRect.top(),
                             bcuRect.width()-2*ENABLED_BCU_RECT_IN_SPACE - BCU_RIGHT_MARK_WIDTH - BCU_TEXT_RECT_WIDTH,
                             qMin(bcuRect.height() - 2*ENABLED_BCU_RECT_IN_SPACE - BCU_INPORT_RECT_HEIGHT - BCU_OUTPORT_RECT_HEIGHT,
                                  RCU_MODULE_MIN_HEIGHT));
    }
    setRealBoundingRect(rect);
    moduleRcuName = QString(mapModuleUnitName[Module_Rcu] + " - CFG:%1").arg(indexRcu);
    collidedItems.clear();

    connect(inFifo0,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(inFifo1,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(inFifo2,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(inFifo3,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(outFifo0,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(outFifo1,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(outFifo2,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
    connect(outFifo3,SIGNAL(destroyed(QObject*)),this,SLOT(handleFifoDeleted(QObject*)));
}

ModuleRcu::ModuleRcu(ModuleRcu &copyRcu, QGraphicsItem *parent) : BaseItem(parent)
  , indexBcu(copyRcu.indexBcu)
  , indexRcu(copyRcu.indexRcu)
  , bfuMaxCount(BFU_MAX_COUNT)
  , bfuExistList(QList<ElementBfu*>())
  , isBfuFulled(false)
  , customPEExistList(QList<ElementCustomPE*>())
  , sboxMaxCount(SBOX_MAX_COUNT)
  , sboxExistList(QList<ElementSbox*>())
  , isSboxFulled(false)
  , benesMaxCount(BENES_MAX_COUNT)
  , benesExistList(QList<ElementBenes*>())
  , isBenesFulled(false)
  , inPortRect(QRectF())
  , outPortRect(QRectF())
  , inFifo0(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , inFifo1(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , inFifo2(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , inFifo3(new ElementPort(OutputDirection,this,PORT_FROM_INFIFO))
  , outFifo0(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , outFifo1(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , outFifo2(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , outFifo3(new ElementPort(InputDirection,this,PORT_FORM_OUTFIFO))
  , wMemMaxCount(WRITE_MEM_MAX_COUNT)
  , wMemExistList(QList<int>())
  , isWMemFulled(false)
  , wRchMaxCount(WRITE_RCH_MAX_COUNT)
  , wRchExistList(QList<int>())
  , isWRchFulled(false)
  , writeMemOffset(0)
  , writeMemMask(0)
  , writeMemMode(0)
  , writeMemThreashold(0)
  , writeMemAddr(0)
  , readMemOffset(0)
  , readMemMode(0)
  , readMemMode128_32(0)
  , readMemThreashold(0)
  , readMemAddr1(0)
  , readMemAddr2(0)
  , readMemAddr3(0)
  , readMemAddr4(0)
  , rchMode(0)
{
    unitId = RCU_ID;

    setRealBoundingRect(copyRcu.realBoundingRect);
    setPos(copyRcu.pos());

    inFifo0->copyOtherPort(*copyRcu.inFifo0);
    inFifo1->copyOtherPort(*copyRcu.inFifo1);
    inFifo2->copyOtherPort(*copyRcu.inFifo2);
    inFifo3->copyOtherPort(*copyRcu.inFifo3);
    outFifo0->copyOtherPort(*copyRcu.outFifo0);
    outFifo1->copyOtherPort(*copyRcu.outFifo1);
    outFifo2->copyOtherPort(*copyRcu.outFifo2);
    outFifo3->copyOtherPort(*copyRcu.outFifo3);

    loopStartEndFlag = copyRcu.loopStartEndFlag;
    loopTimes = copyRcu.loopTimes;
    loopStartEndFlag2 = copyRcu.loopStartEndFlag2;
    loopTimes2 = copyRcu.loopTimes2;
    writeMemOffset = copyRcu.writeMemOffset;
    writeMemMask = copyRcu.writeMemMask;
    writeMemMode = copyRcu.writeMemMode;
    writeMemThreashold = copyRcu.writeMemThreashold;
    writeMemAddr = copyRcu.writeMemAddr;
    readMemOffset = copyRcu.readMemOffset;
    readMemMode = copyRcu.readMemMode;
    readMemMode128_32 = copyRcu.readMemMode128_32;
    readMemThreashold = copyRcu.readMemThreashold;
    readMemAddr1 = copyRcu.readMemAddr1;
    readMemAddr2 = copyRcu.readMemAddr2;
    readMemAddr3 = copyRcu.readMemAddr3;
    readMemAddr4 = copyRcu.readMemAddr4;
    rchMode = copyRcu.rchMode;
    writeRchAddr1 = copyRcu.writeRchAddr1;
    writeRchAddr2 = copyRcu.writeRchAddr2;
    writeRchAddr3 = copyRcu.writeRchAddr3;
    writeRchAddr4 = copyRcu.writeRchAddr4;
    copyAllChildWithOtherRcu(copyRcu);
}

ModuleRcu::~ModuleRcu()
{
    QL_DEBUG << "~ModuleRcu()";
}

void ModuleRcu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);

    painter->setBrush(RCU_PAINT_BRUSH_FILLIN);
    painter->setPen(RCU_BOUNDING_PEN);

    QRectF tempBoundingRect(inPortRect.left(),inPortRect.top(),inPortRect.width(),
                           inPortRect.height()+paintRect.height()+outPortRect.height());
    painter->drawRect(tempBoundingRect);
    painter->drawLine(inPortRect.bottomLeft(),inPortRect.bottomRight());
    painter->drawLine(outPortRect.topLeft(),outPortRect.topRight());
//    painter->fillRect(paintRect,RCU_PAINT_BRUSH_FILLIN);
//    painter->drawRect(paintRect);

//    painter->fillRect(inPortRect,RCU_PAINT_BRUSH_FILLIN);
//    painter->drawRect(inPortRect);

//    painter->fillRect(outPortRect,RCU_PAINT_BRUSH_FILLIN);
//    painter->drawRect(outPortRect);

    if(isSelected())
    {
        painter->setBrush(RCU_TEXT_SELECTED_BRUSH_FILLIN);
        painter->setPen(RCU_BOUNDING_PEN);
        painter->fillRect(textRect,RCU_TEXT_SELECTED_BRUSH_FILLIN);
        painter->drawRect(textRect);
    }
    else
    {
        painter->setBrush(RCU_TEXT_NORMAL_BRUSH_FILLIN);
        painter->setPen(RCU_BOUNDING_PEN);
        painter->fillRect(textRect,RCU_TEXT_NORMAL_BRUSH_FILLIN);
        painter->drawRect(textRect);
    }
//    QL_DEBUG<<scene()->collidingItems(this,Qt::IntersectsItemBoundingRect).size();

    if(checkCollidedItems(Qt::IntersectsItemBoundingRect))
//    if(isCollidedItems)
    {
        painter->setBrush(RCU_COLLIDED_BRUSH_FILLIN);
        painter->setPen(RCU_COLLIDED_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,RCU_COLLIDED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }

    QFont font;
    font.setPixelSize(RCU_TEXT_PIXELSIZE);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(RCU_TEXT_PEN);

    painter->drawText(inPortRect, Qt::AlignCenter, RCU_IFIFO_NAME);
    painter->drawText(outPortRect, Qt::AlignCenter, RCU_OFIFO_NAME);

    painter->rotate(90);
    painter->drawText(textRect.top() + 2,
                      (- (textRect.width() - font.pixelSize())/2 - textRect.left()),
                      moduleRcuName);
}

bool ModuleRcu::setIndexBcu(int index)
{
    if(index >= BCU_MAX_COUNT) return false;
    indexBcu = index;

    for(int i=0;i<bfuExistList.size();i++)
    {
        bfuExistList.at(i)->setIndexBcu(indexBcu);
    }

    for(int i=0;i<customPEExistList.size();i++)
    {
        customPEExistList.at(i)->setIndexBcu(indexBcu);
    }

    for(int i=0;i<sboxExistList.size();i++)
    {
        sboxExistList.at(i)->setIndexBcu(indexBcu);
    }

    for(int i=0;i<benesExistList.size();i++)
    {
        benesExistList.at(i)->setIndexBcu(indexBcu);
    }

    return true;
}

bool ModuleRcu::setIndexRcu(int index)
{
    if(index >= RCU_MAX_COUNT) return false;
    if(parentItem())
    {
        ModuleBcu* parentBcu = dynamic_cast<ModuleBcu*>(parentItem());
        if(parentBcu)
        {
            if(parentBcu->replaceRcuIndex(indexRcu,index))
            {
                RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                if(rcaScene)
                {
                    emit rcaScene->deleteRcuSuccess(this);
                }
                indexRcu = index;
                moduleRcuName = QString(mapModuleUnitName[Module_Rcu] + " - CFG:%1").arg(indexRcu);

                for(int i=0;i<bfuExistList.size();i++)
                {
                    bfuExistList.at(i)->setIndexRcu(indexRcu);
                }

                for(int i=0;i<customPEExistList.size();i++)
                {
                    customPEExistList.at(i)->setIndexRcu(indexRcu);
                }

                for(int i=0;i<sboxExistList.size();i++)
                {
                    sboxExistList.at(i)->setIndexRcu(indexRcu);
                }

                for(int i=0;i<benesExistList.size();i++)
                {
                    benesExistList.at(i)->setIndexRcu(indexRcu);
                }

                return true;
            }
        }
    }

    return false;
}

//由boundingrect调整到enabledRect。
QRectF ModuleRcu::paintRect2EnabledRect(QRectF rect)
{
    qreal adjust = ENABLED_RCU_RECT_IN_SPACE;
    //rect.adjustd 表示四个顶点坐标分别added参数。
    //缩小
    return rect.adjusted(adjust,adjust,-adjust,-adjust);
}

void ModuleRcu::setRealBoundingRect(QRectF rect)
{
    QL_DEBUG;
    prepareGeometryChange();
    realBoundingRect = rect;
    textRect = QRectF(realBoundingRect.left(),realBoundingRect.top(),
                      RCU_TEXT_RECT_WIDTH,realBoundingRect.height());
    inPortRect = QRectF(textRect.right(),realBoundingRect.top(),
                        realBoundingRect.width() - RCU_TEXT_RECT_WIDTH,RCU_INPUT_RECT_HEIGHT);

    outPortRect = QRectF(textRect.right(),realBoundingRect.bottom() - RCU_OUTPORT_RECT_HEIGHT,
                        realBoundingRect.width() - RCU_TEXT_RECT_WIDTH,RCU_OUTPORT_RECT_HEIGHT);

    paintRect = QRectF(textRect.right(),inPortRect.bottom(),
                       realBoundingRect.width() - RCU_TEXT_RECT_WIDTH,
                       realBoundingRect.height() - RCU_INPUT_RECT_HEIGHT - RCU_OUTPORT_RECT_HEIGHT);

    enabledMoveableRect = paintRect2EnabledRect(paintRect);

    qreal posX = inPortRect.left();
    qreal posY = inPortRect.bottom() - inFifo0->getPaintRect().height() - DEFAULT_PEN_WIDTH;

    qreal inPortSpace = (inPortRect.width() - inFifo0->getPaintRect().width()
                                            - inFifo1->getPaintRect().width()
                                            - inFifo2->getPaintRect().width()
                                            - inFifo3->getPaintRect().width())/BCU_INPORT_SPACE_COUNT;

    qreal outPortSpace = (outPortRect.width() - outFifo0->getPaintRect().width()
                                              - outFifo1->getPaintRect().width()
                                              - outFifo2->getPaintRect().width()
                                              - outFifo3->getPaintRect().width())/BCU_OUTPORT_SPACE_COUNT;

    posX += inPortSpace;
    inFifo0->setPos(posX,posY);
    posX += inFifo0->getPaintRect().width() + inPortSpace;
    inFifo1->setPos(posX,posY);
    posX += inFifo1->getPaintRect().width() + inPortSpace;
    inFifo2->setPos(posX,posY);
    posX += inFifo2->getPaintRect().width() + inPortSpace;
    inFifo3->setPos(posX,posY);

    posX = outPortRect.left();
    posY = outPortRect.top() + DEFAULT_PEN_WIDTH;
    posX += outPortSpace;
    outFifo0->setPos(posX,posY);
    posX += outFifo0->getPaintRect().width() + outPortSpace;
    outFifo1->setPos(posX,posY);
    posX += outFifo1->getPaintRect().width() + outPortSpace;
    outFifo2->setPos(posX,posY);
    posX += outFifo2->getPaintRect().width() + outPortSpace;
    outFifo3->setPos(posX,posY);

    QL_DEBUG << realBoundingRect;

    update();
}

void ModuleRcu::showItemPortWire()
{
    QL_DEBUG;
    if(inFifo0 && inFifo1 && inFifo2 && inFifo3
            && outFifo0 && outFifo1 && outFifo2 && outFifo3)
    {
        inFifo0->showItemWire(this->isSelected());
        inFifo1->showItemWire(this->isSelected());
        inFifo2->showItemWire(this->isSelected());
        inFifo3->showItemWire(this->isSelected());

        outFifo0->showItemWire(this->isSelected());
        outFifo1->showItemWire(this->isSelected());
        outFifo2->showItemWire(this->isSelected());
        outFifo3->showItemWire(this->isSelected());
    }
}

void ModuleRcu::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QL_DEBUG << event->pos().toPoint() << event->scenePos().toPoint() << event->button();
    BaseItem::mousePressEvent(event);
}

void ModuleRcu::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        RcaSceneStates sceneState = rcaScene->getSceneState();
        QL_DEBUG << event->pos().toPoint() << event->scenePos().toPoint() << event->button() << sceneState;

        if(Qt::LeftButton == event->button() && enabledMoveableRect.contains(event->pos()))
        {
            switch(sceneState)
            {
            case Add_Bfu:addNewBfu(event->pos());break;
            case Add_CustomPE: addNewCustomPE(event->pos());break;
            case Add_Sbox: addNewSbox(event->pos());break;
            case Add_Benes: addNewBenes(event->pos());break;
            default: break;
            }
        }
    }
    BaseItem::mouseReleaseEvent(event);
}

QRectF ModuleRcu::enabledRect2PaintRect(QRectF rect)
{
    qreal adjust = ENABLED_RCU_RECT_IN_SPACE;
    //rect.adjustd 表示四个顶点坐标分别added参数。
    //放大
    return rect.adjusted(-adjust,-adjust,adjust,adjust);
}

QRectF ModuleRcu::enabledRect2BoundingRect(QRectF rect)
{
    QRectF paintRectTemp = enabledRect2PaintRect(rect);
    return paintRectTemp.adjusted(-textRect.width(),0,0,0);
}

bool ModuleRcu::addBfuFromXml(QPointF posAtParent , ElementBfu* elementBfu)
{
    if(isBfuFulled) return false;   //add bfu failed when bfu fulled.
    if(elementBfu == NULL) return false;

//    for(int index=0;index<bfuExistList.size();index++)
//    {
//        if(elementBfu->getIndexInRcu() == bfuExistList.at(index)->getIndexInRcu())
//        {
//            return false;
//        }
//    }
//    bfuExistList.insert(elementBfu->getIndexInRcu(),elementBfu);

    int curIndex = elementBfu->getIndexInRcu();
    int insertIndex = bfuExistList.size();

    //bfuExistList 需要排序
    for(int i=0;i<bfuExistList.size();i++)
    {
        if(curIndex < bfuExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    bfuExistList.insert(insertIndex,elementBfu);


    elementBfu->setParentItem(this);
    elementBfu->setPos(posAtParent);

    connect(elementBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
    connect(elementBfu,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(elementBfu,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    if(bfuMaxCount <= bfuExistList.size())
    {
        isBfuFulled = true;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    return true;
}
bool ModuleRcu::addCustomPEFromXml(QPointF posAtParent , ElementCustomPE* elementCustomPE)
{
    if(isBfuFulled) return false;   //add bfu failed when bfu fulled.
    if(elementCustomPE == NULL) return false;

    int curIndex = elementCustomPE->getIndexInRcu();
    int insertIndex = customPEExistList.size();

    //bfuExistList 需要排序
    for(int i=0;i<customPEExistList.size();i++)
    {
        if(curIndex < customPEExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    customPEExistList.insert(insertIndex,elementCustomPE);


    elementCustomPE->setParentItem(this);
    elementCustomPE->setPos(posAtParent);

    connect(elementCustomPE,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleCustomPEBeDeleted(BaseItem*)));
    connect(elementCustomPE,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(elementCustomPE,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    if(bfuMaxCount <= bfuExistList.size() + customPEExistList.size())
    {
        isBfuFulled = true;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    return true;
}

bool ModuleRcu::addSboxFromXml(QPointF posAtParent , ElementSbox* elementSbox)
{
    if(isSboxFulled) return false;   //add sbox failed when sbox fulled.
    if(elementSbox == NULL) return false;

//    for(int index=0;index<sboxExistList.size();index++)
//    {
//        if(elementSbox->getIndexInRcu() == sboxExistList.at(index)->getIndexInRcu())
//        {
//            return false;
//        }
//    }

//    sboxExistList.insert(elementSbox->getIndexInRcu(),elementSbox);

    int curIndex = elementSbox->getIndexInRcu();
    int insertIndex = sboxExistList.size();

    //sboxExistList 需要排序
    for(int i=0;i<sboxExistList.size();i++)
    {
        if(curIndex < sboxExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    sboxExistList.insert(insertIndex,elementSbox);

    elementSbox->setParentItem(this);
    elementSbox->setPos(posAtParent);

    connect(elementSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
    connect(elementSbox,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(elementSbox,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    if(sboxMaxCount <= sboxExistList.size())
    {
        isSboxFulled = true;
        emit sboxFullStatesChanged(isSboxFulled);
    }

    return true;
}

bool ModuleRcu::addBenesFromXml(QPointF posAtParent , ElementBenes* elementBenes)
{
    if(isBenesFulled) return false;   //add benes failed when benes fulled.
    if(elementBenes == NULL) return false;

//    for(int index=0;index<benesExistList.size();index++)
//    {
//        if(elementBenes->getIndexInRcu() == benesExistList.at(index)->getIndexInRcu())
//        {
//            return false;
//        }
//    }
//    benesExistList.insert(elementBenes->getIndexInRcu(),elementBenes);

    int curIndex = elementBenes->getIndexInRcu();
    int insertIndex = benesExistList.size();
    //benesExistList 需要排序
    for(int i=0;i<benesExistList.size();i++)
    {
        if(curIndex < benesExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    benesExistList.insert(insertIndex,elementBenes);

    elementBenes->setParentItem(this);
    elementBenes->setPos(posAtParent);

    connect(elementBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
    connect(elementBenes,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(elementBenes,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    if(benesMaxCount <= benesExistList.size())
    {
        isBenesFulled = true;
        emit benesFullStatesChanged(isBenesFulled);
    }

    return true;
}

ElementBfu* ModuleRcu::findBfuFromRcu(int indexInRcu)
{
    QList<QGraphicsItem *> listItems = this->childItems();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=listItems.begin();iter!=listItems.end();iter++)
    {
        if((*iter))
        {
            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(*iter);
            if(elementBfu)
            {
                QL_DEBUG << elementBfu->getIndexInRcu() << indexInRcu;
                if(elementBfu->getIndexInRcu() == indexInRcu)
                {
                    return elementBfu;
                }
            }
        }
    }
    QL_DEBUG;
    return NULL;
}

ElementCustomPE* ModuleRcu::findCustomPEFromRcu(int indexInRcu)
{
    QList<QGraphicsItem *> listItems = this->childItems();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=listItems.begin();iter!=listItems.end();iter++)
    {
        if((*iter))
        {
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(*iter);
            if(elementCustomPE)
            {
                QL_DEBUG << elementCustomPE->getIndexInRcu() << indexInRcu;
                if(elementCustomPE->getIndexInRcu() == indexInRcu)
                {
                    return elementCustomPE;
                }
            }
        }
    }
    QL_DEBUG;
    return NULL;
}

ElementSbox* ModuleRcu::findSboxFromRcu(int indexInRcu)
{
    QList<QGraphicsItem *> listItems = this->childItems();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=listItems.begin();iter!=listItems.end();iter++)
    {
        if((*iter))
        {
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(*iter);
            if(elementSbox)
            {
                QL_DEBUG << elementSbox->getIndexInRcu() << indexInRcu;
                if(elementSbox->getIndexInRcu() == indexInRcu)
                {
                    return elementSbox;
                }
            }
        }
    }
    QL_DEBUG;
    return NULL;
}

ElementBenes* ModuleRcu::findBenesFromRcu(int indexInRcu)
{
    QList<QGraphicsItem *> listItems = this->childItems();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=listItems.begin();iter!=listItems.end();iter++)
    {
        if((*iter))
        {
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(*iter);
            if(elementBenes)
            {
                QL_DEBUG << elementBenes->getIndexInRcu() << indexInRcu;
                if(elementBenes->getIndexInRcu() == indexInRcu)
                {
                    return elementBenes;
                }
            }
        }
    }
    QL_DEBUG;
    return NULL;
}

void ModuleRcu::autoMakeAllArithUnit()
{
    for(int indexId = 0;indexId < bfuMaxCount; indexId++)
    {
        autoMakeNewBfu(indexId);
    }
    for(int indexId = 0;indexId < sboxMaxCount; indexId++)
    {
        autoMakeNewSbox(indexId);
    }
    for(int indexId = 0;indexId < benesMaxCount; indexId++)
    {
        autoMakeNewBenes(indexId);
    }
    resetBoundingRectByChildrenBound();
}

void ModuleRcu::autoSetPosOfChildArithUnit()
{
    QPointF arithUnitPos = ARITHUNIT_POS_AT_RCU[0];

    for (int index = 0; index < bfuMaxCount; ++index)
    {
        for(int i=0;i<bfuExistList.size();i++)
        {
            ElementBfu* elementBfu = bfuExistList.at(i);
            if(elementBfu->getIndexInRcu() == index)
            {
                elementBfu->pushStackSetPos(arithUnitPos);
                arithUnitPos.setX(arithUnitPos.x() + (ARITHUNIT_AT_RCU_XSPACE + elementBfu->getRealBoundingRect().width()));
                break;
            }
        }

        for(int i=0;i<customPEExistList.size();i++)
        {
            ElementCustomPE* elementCustomPE = customPEExistList.at(i);
            if(elementCustomPE->getIndexInRcu() == index)
            {
                elementCustomPE->pushStackSetPos(arithUnitPos);
                arithUnitPos.setX(arithUnitPos.x() + (ARITHUNIT_AT_RCU_XSPACE + elementCustomPE->getRealBoundingRect().width()));
                break;
            }
        }
    }

    for(int index=0;index<sboxExistList.size();index++)
    {
        ElementSbox* elementSbox = sboxExistList.at(index);
        elementSbox->pushStackSetPos(arithUnitPos);
        arithUnitPos.setX(arithUnitPos.x() + (ARITHUNIT_AT_RCU_XSPACE + elementSbox->getRealBoundingRect().width()));
    }

    for(int index=0;index<benesExistList.size();index++)
    {
        ElementBenes* elementBenes = benesExistList.at(index);
        elementBenes->pushStackSetPos(arithUnitPos);
        arithUnitPos.setX(arithUnitPos.x() + (ARITHUNIT_AT_RCU_XSPACE + elementBenes->getRealBoundingRect().width()));
    }
    resetBoundingRectByChildrenBound();
}

bool ModuleRcu::replaceBfuIndex(int indexSrc, int indexDst)
{
    if(findBfuFromRcu(indexDst) != NULL) return false; //已存在indexDst。

    ElementBfu* bfuSrc = findBfuFromRcu(indexSrc);
    if(bfuSrc == NULL) return false;                    //不存在indexSrc。

    bfuExistList.removeOne(bfuSrc);

    int insertIndex = bfuExistList.size();

    //bfuExistList 需要排序
    for(int i=0;i<bfuExistList.size();i++)
    {
        if(indexDst < bfuExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    bfuExistList.insert(insertIndex,bfuSrc);
    return true;
}

bool ModuleRcu::replaceSboxIndex(int indexSrc, int indexDst)
{
    if(findSboxFromRcu(indexDst) != NULL) return false; //已存在indexDst。

    ElementSbox* sboxSrc = findSboxFromRcu(indexSrc);
    if(sboxSrc == NULL) return false;                    //不存在indexSrc。

    sboxExistList.removeOne(sboxSrc);

    int insertIndex = sboxExistList.size();

    //sboxExistList 需要排序
    for(int i=0;i<sboxExistList.size();i++)
    {
        if(indexDst < sboxExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    sboxExistList.insert(insertIndex,sboxSrc);
    return true;
}

bool ModuleRcu::replaceBenesIndex(int indexSrc, int indexDst)
{
    if(findBenesFromRcu(indexDst) != NULL) return false; //已存在indexDst。

    ElementBenes* benesSrc = findBenesFromRcu(indexSrc);
    if(benesSrc == NULL) return false;                    //不存在indexSrc。

    benesExistList.removeOne(benesSrc);

    int insertIndex = benesExistList.size();

    //benesExistList 需要排序
    for(int i=0;i<benesExistList.size();i++)
    {
        if(indexDst < benesExistList.at(i)->getIndexInRcu())
        {
            insertIndex = i;
            break;
        }
    }
    benesExistList.insert(insertIndex,benesSrc);
    return true;
}

QRectF ModuleRcu::getChildrenMinRect()
{
    QRectF result;
    if(childItems().count() > 0)
    {
        for(int i=0; i<childItems().size(); i++)
        {
            BaseItem *child = dynamic_cast<BaseItem*>(childItems().at(i));
            if((child) && (child->isPartOfChildRect))
            {
                result |= mapRectFromItem(child,child->getRealBoundingRect());
//                QL_DEBUG << result;
            }
        }
//        QL_DEBUG <<result;
        result = enabledRect2BoundingRect(result);
//        QL_DEBUG <<result;
    }
    else
    {
        result = QRectF(0,0,RCU_MODULE_MIN_WIDTH,RCU_MODULE_MIN_HEIGHT);
//        QL_DEBUG <<result;
    }
    QL_DEBUG << result;
    return result;
}

void ModuleRcu::insertArithToList(int index, BaseArithUnit *arith)
{
    if(arith)
    {
        switch(arith->getUnitId())
        {
        case BFU_ID:
        {
            ElementBfu* bfuElement = dynamic_cast<ElementBfu*>(arith);
            if(bfuElement)
            {
                insertBfuToList(index,bfuElement);
            }
            break;
        }
        case SBOX_ID:
        {
            ElementSbox* sboxElement = dynamic_cast<ElementSbox*>(arith);
            if(sboxElement)
            {
                insertSboxToList(index,sboxElement);
            }
            break;
        }
        case BENES_ID:
        {
            ElementBenes* benesElement = dynamic_cast<ElementBenes*>(arith);
            if(benesElement)
            {
                insertBenesToList(index,benesElement);
            }
            break;
        }
        default:break;
        }
    }
}

void ModuleRcu::insertBfuToList(int index, ElementBfu *bfuElement)
{
    if(bfuElement)
    {
        bfuExistList.insert(index,bfuElement);
    }
}

void ModuleRcu::insertSboxToList(int index, ElementSbox *sboxElement)
{
    if(sboxElement)
    {
        sboxExistList.insert(index,sboxElement);
    }
}

void ModuleRcu::insertBenesToList(int index, ElementBenes *benesElement)
{
    if(benesElement)
    {
        benesExistList.insert(index,benesElement);
    }
}

void ModuleRcu::insertCustomPEToList(int index, ElementCustomPE *customPEElement)
{
    if(customPEElement)
    {
        customPEExistList.insert(index,customPEElement);
    }
}

void ModuleRcu::deleteArithList(bool isForced)
{
    QList<ElementBfu*> bfuListTemp = bfuExistList;
    for(int index=0;index<bfuListTemp.size();index++)
    {
        ElementBfu* elementBfu = bfuListTemp.at(index);
        elementBfu->pushItemToDeletedStack(isForced);
    }

    QList<ElementSbox*> sboxListTemp = sboxExistList;
    for(int index=0;index<sboxListTemp.size();index++)
    {
        ElementSbox* elementSbox = sboxListTemp.at(index);
        elementSbox->pushItemToDeletedStack(isForced);
    }

    QList<ElementBenes*> benesListTemp = benesExistList;
    for(int index=0;index<benesListTemp.size();index++)
    {
        ElementBenes* elementBenes = benesListTemp.at(index);
        elementBenes->pushItemToDeletedStack(isForced);
    }
}

void ModuleRcu::pushItemToDeletedStack(bool isForced)
{
    isForced |= isSelected();
    deleteArithList(isForced);
    BaseItem::pushItemToDeletedStack(isForced);
}

void ModuleRcu::resetBoundingRectByChildrenBound()
{
    QRectF newRect = QRectF();
    for(int index=0;index<bfuExistList.size();index++)
    {
        newRect |= mapRectFromItem(bfuExistList.at(index),bfuExistList.at(index)->getRealBoundingRect());
    }

    for(int index=0;index<customPEExistList.size();index++)
    {
        newRect |= mapRectFromItem(customPEExistList.at(index),customPEExistList.at(index)->getRealBoundingRect());
    }

    for(int index=0;index<sboxExistList.size();index++)
    {
        newRect |= mapRectFromItem(sboxExistList.at(index),sboxExistList.at(index)->getRealBoundingRect());
    }

    for(int index=0;index<benesExistList.size();index++)
    {
        newRect |= mapRectFromItem(benesExistList.at(index),benesExistList.at(index)->getRealBoundingRect());
    }

    if(newRect.isEmpty())
    {
        newRect = QRectF(RCU_MODULE_DEFAULT_RECT.left(),
                         RCU_MODULE_DEFAULT_RECT.top(),
                         RCU_MODULE_MIN_WIDTH,
                         RCU_MODULE_MIN_HEIGHT);
        QL_DEBUG << newRect;
    }
    else
    {
        //放大
        newRect = enabledRect2BoundingRect(newRect);
        newRect = newRect.adjusted(-30,-30, 20,0);//TODO XUFEI 有bug 看这里

        newRect.setBottom(qMax(newRect.bottom(),newRect.top()+RCU_MODULE_MIN_HEIGHT));
        newRect.setRight(qMax(newRect.right(),newRect.left()+RCU_MODULE_MIN_WIDTH));
//        qreal adjust = DEFAULT_PEN_WIDTH/2;
//        newRect = newRect.adjusted(adjust,adjust,-adjust,-adjust);
//        adjust = ENABLED_RECT_IN_SPACE;
//        newRect = newRect.adjusted(-adjust,-adjust,adjust,adjust);
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

bool ModuleRcu::autoMakeNewBfu(int indexId)
{
    if(indexId >= bfuMaxCount) return false;

    ElementBfu* elementBfu = NULL;

    for(int index=0;index<bfuExistList.size();index++)
    {
        if(indexId == bfuExistList.at(index)->getIndexInRcu())
        {
            elementBfu = bfuExistList.at(index);
        }
    }
    if(elementBfu == NULL)
    {
        if(isBfuFulled) return false;   //add bfu failed when bfu fulled.
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene == NULL) return false;

        AddBfuCommand* cmdAddBfu = new AddBfuCommand(rcaScene,indexBcu,indexRcu,indexId,this);
        rcaScene->pushCmdAddBfu(cmdAddBfu);
        elementBfu = cmdAddBfu->getBfuElement();

//        elementBfu = new ElementBfu(indexBcu,indexRcu,indexId,this);
//        bfuExistList.insert(indexId,elementBfu);
//        connect(elementBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
//        connect(elementBfu,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
//        connect(elementBfu,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

        if(bfuMaxCount <= bfuExistList.size())
        {
            isBfuFulled = true;
            emit bfuFullStatesChanged(isBfuFulled);
        }

        emit rcaScene->addNewBfuSuccess(elementBfu);

        QL_DEBUG << bfuExistList << indexId;
    }
    elementBfu->setPos(ARITHUNIT_POS_AT_RCU[indexId]);
    return true;
}

ElementBfu *ModuleRcu::pasteNewBfu(ElementBfu &copyBfu, QPointF posAtThis)
{
    if(isBfuFulled) return NULL;   //add bfu failed when bfu fulled.
    int copyIndex = copyBfu.getIndexInRcu();

    ElementBfu* elementBfu = NULL;
    for(int index=0;index<bfuExistList.size();index++)
    {
        if(copyIndex == bfuExistList.at(index)->getIndexInRcu())
        {
            elementBfu = bfuExistList.at(index);
        }
    }
    int pasteIndex = 0;
    if(elementBfu == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<bfuExistList.size();curIndex++)
        {
            if(curIndex != bfuExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= bfuMaxCount) return NULL;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= bfuMaxCount) return NULL;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return NULL;

    AddBfuCommand* cmdAddBfu = new AddBfuCommand(rcaScene,indexBcu,indexRcu,pasteIndex,this);
    rcaScene->pushCmdAddBfu(cmdAddBfu);
    elementBfu = cmdAddBfu->getBfuElement();

    if(bfuMaxCount <= bfuExistList.size())
    {
        isBfuFulled = true;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    emit rcaScene->addNewBfuSuccess(elementBfu);

    QL_DEBUG << bfuExistList << pasteIndex;
    elementBfu->setPos(posAtThis);

    elementBfu->funcIndex = copyBfu.funcIndex;
    elementBfu->funcExp = copyBfu.funcExp;
    elementBfu->funcAuModIndex = copyBfu.funcAuModIndex;
    elementBfu->funcAuCarry = copyBfu.funcAuCarry;
    elementBfu->funcMuModIndex = copyBfu.funcMuModIndex;
    elementBfu->bypassIndex = copyBfu.bypassIndex;
    elementBfu->bypassExp = copyBfu.bypassExp;

    elementBfu->inPortA->pasteOtherPort(*copyBfu.inPortA);
    elementBfu->inPortB->pasteOtherPort(*copyBfu.inPortB);
    elementBfu->inPortT->pasteOtherPort(*copyBfu.inPortT);

    elementBfu->outPortX->pasteOtherPort(*copyBfu.outPortX);
    elementBfu->outPortY->pasteOtherPort(*copyBfu.outPortY);


    return elementBfu;
}
bool ModuleRcu::autoMakeNewCustomPE(int indexId)
{
    if(indexId >= bfuMaxCount) return false;

    ElementCustomPE* elementCustomPE = NULL;

    for(int index=0;index<customPEExistList.size();index++)
    {
        if(indexId == customPEExistList.at(index)->getIndexInRcu())
        {
            elementCustomPE = customPEExistList.at(index);
        }
    }
    if(elementCustomPE == NULL)
    {
        if(isBfuFulled) return false;   //add bfu failed when bfu fulled.
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene == NULL) return false;

        AddCustomPECommand* cmdAddCustomPE = new AddCustomPECommand(rcaScene,indexBcu,indexRcu,indexId,this);
        rcaScene->pushCmdAddCustomPE(cmdAddCustomPE);
        elementCustomPE = cmdAddCustomPE->getCustomPEElement();

        if(bfuMaxCount <= customPEExistList.size())
        {
            isBfuFulled = true;
            emit bfuFullStatesChanged(isBfuFulled);
        }

        emit rcaScene->addNewCustomPESuccess(elementCustomPE);

        QL_DEBUG << customPEExistList << indexId;
    }
    elementCustomPE->setPos(ARITHUNIT_POS_AT_RCU[indexId]);
    return true;
}

ElementCustomPE *ModuleRcu::pasteNewCustomPE(ElementCustomPE &copyCustomPE, QPointF posAtThis)
{
    if(isBfuFulled) return NULL;   //add bfu failed when bfu fulled.
    int copyIndex = copyCustomPE.getIndexInRcu();

    ElementCustomPE* elememntCustomPE = NULL;
    for(int index=0;index<customPEExistList.size();index++)
    {
        if(copyIndex == customPEExistList.at(index)->getIndexInRcu())
        {
            elememntCustomPE = customPEExistList.at(index);
        }
    }
    int pasteIndex = 0;
    if(elememntCustomPE == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<customPEExistList.size();curIndex++)
        {
            if(curIndex != customPEExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= bfuMaxCount) return NULL;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= bfuMaxCount) return NULL;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return NULL;

    AddCustomPECommand* cmdAddBfu = new AddCustomPECommand(rcaScene,indexBcu,indexRcu,pasteIndex,this);
    rcaScene->pushCmdAddCustomPE(cmdAddBfu);
    elememntCustomPE = cmdAddBfu->getCustomPEElement();

    if(bfuMaxCount <= customPEExistList.size())
    {
        isBfuFulled = true;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    emit rcaScene->addNewCustomPESuccess(elememntCustomPE);

    QL_DEBUG << customPEExistList << pasteIndex;
    elememntCustomPE->setPos(posAtThis);

    elememntCustomPE->funcIndex = copyCustomPE.funcIndex;
    elememntCustomPE->setInputNum(copyCustomPE.inputNum());
    elememntCustomPE->setOutputNum(copyCustomPE.outputNum());
    elememntCustomPE->funcExp = copyCustomPE.funcExp;
    elememntCustomPE->funcAuModIndex = copyCustomPE.funcAuModIndex;
//    elememntCustomPE->funcAuCarry = copyCustomPE.funcAuCarry;
//    elememntCustomPE->funcMuModIndex = copyCustomPE.funcMuModIndex;
    elememntCustomPE->bypassIndex = copyCustomPE.bypassIndex;
    elememntCustomPE->bypassExp = copyCustomPE.bypassExp;

    for (int i = 0; i < elememntCustomPE->inputNum(); ++i)
    {
        elememntCustomPE->m_inputPortVector.at(i)->pasteOtherPort(*copyCustomPE.m_inputPortVector.at(i));
    }

    for (int i = 0; i < elememntCustomPE->outputNum(); ++i)
    {
        elememntCustomPE->m_outputPortVector.at(i)->pasteOtherPort(*copyCustomPE.m_outputPortVector.at(i));
    }

    return elememntCustomPE;
}

bool ModuleRcu::autoMakeNewSbox(int indexId)
{
    if(indexId >= sboxMaxCount) return false;

    ElementSbox* elementSbox = NULL;

    for(int index=0;index<sboxExistList.size();index++)
    {
        if(indexId == sboxExistList.at(index)->getIndexInRcu())
        {
            elementSbox = sboxExistList.at(index);
        }
    }
    if(elementSbox == NULL)
    {
        if(isSboxFulled) return false;   //add sbox failed when sbox fulled.
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene == NULL) return false;

        AddSboxCommand* cmdAddSbox = new AddSboxCommand(rcaScene,indexBcu,indexRcu,indexId,this);
        rcaScene->pushCmdAddSbox(cmdAddSbox);
        elementSbox = cmdAddSbox->getSboxElement();

//        elementSbox = new ElementSbox(indexBcu,indexRcu,indexId,this);
//        sboxExistList.insert(indexId,elementSbox);

//        connect(elementSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
//        connect(elementSbox,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
//        connect(elementSbox,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

        if(sboxMaxCount <= sboxExistList.size())
        {
            isSboxFulled = true;
            emit sboxFullStatesChanged(isSboxFulled);
        }

        emit rcaScene->addNewSboxSuccess(elementSbox);

        QL_DEBUG << sboxExistList << indexId;
    }
    elementSbox->setPos(ARITHUNIT_POS_AT_RCU[BFU_MAX_COUNT + indexId]);
    return true;
}

ElementSbox *ModuleRcu::pasteNewSbox(ElementSbox &copySbox, QPointF posAtThis)
{
    QL_DEBUG;
    if(isSboxFulled) return NULL;   //add sbox failed when sbox fulled.
    int copyIndex = copySbox.getIndexInRcu();

    ElementSbox* elementSbox = NULL;

    for(int index=0;index<sboxExistList.size();index++)
    {
        if(copyIndex == sboxExistList.at(index)->getIndexInRcu())
        {
            elementSbox = sboxExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(elementSbox == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<sboxExistList.size();curIndex++)
        {
            if(curIndex != sboxExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= sboxMaxCount) return NULL;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= sboxMaxCount) return NULL;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return NULL;

    AddSboxCommand* cmdAddSbox = new AddSboxCommand(rcaScene,indexBcu,indexRcu,pasteIndex,this);
    rcaScene->pushCmdAddSbox(cmdAddSbox);
    elementSbox = cmdAddSbox->getSboxElement();

    if(sboxMaxCount <= sboxExistList.size())
    {
        isSboxFulled = true;
        emit sboxFullStatesChanged(isSboxFulled);
    }

    emit rcaScene->addNewSboxSuccess(elementSbox);
    QL_DEBUG << sboxExistList << pasteIndex;

    elementSbox->setPos(posAtThis);
    elementSbox->srcConfig = copySbox.srcConfig;
    elementSbox->sboxMode = copySbox.sboxMode;
    elementSbox->sboxIncreaseMode = copySbox.sboxIncreaseMode;
    elementSbox->groupCnt0 = copySbox.groupCnt0;
    elementSbox->groupCnt1 = copySbox.groupCnt1;
    elementSbox->groupCnt2 = copySbox.groupCnt2;
    elementSbox->groupCnt3 = copySbox.groupCnt3;

    elementSbox->inPort0->pasteOtherPort(*(copySbox.inPort0));
    elementSbox->outPort0->pasteOtherPort(*(copySbox.outPort0));
    elementSbox->outPort1->pasteOtherPort(*(copySbox.outPort1));
    elementSbox->outPort2->pasteOtherPort(*(copySbox.outPort2));
    elementSbox->outPort3->pasteOtherPort(*(copySbox.outPort3));

    return elementSbox;
}

bool ModuleRcu::copyOtherSbox(ElementSbox &copySbox)
{
    QL_DEBUG;
    if(isSboxFulled) return false;   //add sbox failed when sbox fulled.
    int copyIndex = copySbox.getIndexInRcu();

    ElementSbox* elementSbox = NULL;

    for(int index=0;index<sboxExistList.size();index++)
    {
        if(copyIndex == sboxExistList.at(index)->getIndexInRcu())
        {
            elementSbox = sboxExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(elementSbox == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<sboxExistList.size();curIndex++)
        {
            if(curIndex != sboxExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= sboxMaxCount) return false;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= sboxMaxCount) return false;

    elementSbox = new ElementSbox(copySbox,this);
    sboxExistList.insert(pasteIndex,elementSbox);

    if(sboxMaxCount <= sboxExistList.size())
    {
        isSboxFulled = true;
    }

    return true;
}

bool ModuleRcu::autoMakeNewBenes(int indexId)
{

    if(indexId >= benesMaxCount) return false;

    ElementBenes* elementBenes = NULL;

    for(int index=0;index<benesExistList.size();index++)
    {
        if(indexId == benesExistList.at(index)->getIndexInRcu())
        {
            elementBenes = benesExistList.at(index);
        }
    }
    if(elementBenes == NULL)
    {
        if(isBenesFulled) return false;   //add benes failed when benes fulled.
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene == NULL) return false;

        AddBenesCommand* cmdAddBenes = new AddBenesCommand(rcaScene,indexBcu,indexRcu,indexId,this);
        rcaScene->pushCmdAddBenes(cmdAddBenes);
        elementBenes = cmdAddBenes->getBenesElement();

//        elementBenes = new ElementBenes(indexBcu,indexRcu,indexId,this);
//        benesExistList.insert(indexId,elementBenes);

//        connect(elementBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
//        connect(elementBenes,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
//        connect(elementBenes,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

        if(benesMaxCount <= benesExistList.size())
        {
            isBenesFulled = true;
            emit benesFullStatesChanged(isBenesFulled);
        }

        emit rcaScene->addNewBenesSuccess(elementBenes);

        QL_DEBUG << benesExistList << indexId;
    }
    elementBenes->setPos(ARITHUNIT_POS_AT_RCU[BFU_MAX_COUNT + SBOX_MAX_COUNT + indexId]);
    return true;
}

ElementBenes *ModuleRcu::pasteNewBenes(ElementBenes &copyBenes, QPointF posAtThis)
{
    if(isBenesFulled) return NULL;   //add benes failed when benes fulled.
    int copyIndex = copyBenes.getIndexInRcu();

    ElementBenes* elementBenes = NULL;

    for(int index=0;index<benesExistList.size();index++)
    {
        if(copyIndex == benesExistList.at(index)->getIndexInRcu())
        {
            elementBenes = benesExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(elementBenes == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<benesExistList.size();curIndex++)
        {
            if(curIndex != benesExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= benesMaxCount) return NULL;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= benesMaxCount) return NULL;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return NULL;

    AddBenesCommand* cmdAddBenes = new AddBenesCommand(rcaScene,indexBcu,indexRcu,pasteIndex,this);
    rcaScene->pushCmdAddBenes(cmdAddBenes);
    elementBenes = cmdAddBenes->getBenesElement();

    if(benesMaxCount <= benesExistList.size())
    {
        isBenesFulled = true;
        emit benesFullStatesChanged(isBenesFulled);
    }

    emit rcaScene->addNewBenesSuccess(elementBenes);
    QL_DEBUG << benesExistList << pasteIndex;

    elementBenes->setPos(posAtThis);
    elementBenes->srcConfig = copyBenes.srcConfig;

    elementBenes->inPort0->pasteOtherPort(*(copyBenes.inPort0));
    elementBenes->inPort1->pasteOtherPort(*copyBenes.inPort1);
    elementBenes->inPort2->pasteOtherPort(*copyBenes.inPort2);
    elementBenes->inPort3->pasteOtherPort(*copyBenes.inPort3);
    elementBenes->outPort0->pasteOtherPort(*copyBenes.outPort0);
    elementBenes->outPort1->pasteOtherPort(*copyBenes.outPort1);
    elementBenes->outPort2->pasteOtherPort(*copyBenes.outPort2);
    elementBenes->outPort3->pasteOtherPort(*copyBenes.outPort3);

    return elementBenes;
}

bool ModuleRcu::copyOtherBenes(ElementBenes &copyBenes)
{
    if(isBenesFulled) return false;   //add benes failed when benes fulled.
    int copyIndex = copyBenes.getIndexInRcu();

    ElementBenes* elementBenes = NULL;

    for(int index=0;index<benesExistList.size();index++)
    {
        if(copyIndex == benesExistList.at(index)->getIndexInRcu())
        {
            elementBenes = benesExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(elementBenes == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<benesExistList.size();curIndex++)
        {
            if(curIndex != benesExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= benesMaxCount) return false;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= benesMaxCount) return false;

    elementBenes = new ElementBenes(copyBenes,this);
    benesExistList.insert(pasteIndex,elementBenes);

    if(benesMaxCount <= benesExistList.size())
    {
        isBenesFulled = true;
    }

    return true;
}

void ModuleRcu::pasteAllChildWithOtherRcu(ModuleRcu &otherRcu)
{
    for(int indexId = 0;indexId < otherRcu.bfuExistList.count(); indexId++)
    {
        ElementBfu* copyBfu = otherRcu.bfuExistList.at(indexId);
        pasteNewBfu(*copyBfu,copyBfu->pos());
    }
    for(int indexId = 0;indexId < otherRcu.customPEExistList.count(); indexId++)
    {
        ElementCustomPE* copyCustomPE = otherRcu.customPEExistList.at(indexId);
        pasteNewCustomPE(*copyCustomPE,copyCustomPE->pos());
    }
    for(int indexId = 0;indexId < otherRcu.sboxExistList.count(); indexId++)
    {
        ElementSbox* copySbox = otherRcu.sboxExistList.at(indexId);
        pasteNewSbox(*copySbox,copySbox->pos());
    }
    for(int indexId = 0;indexId < otherRcu.benesExistList.count(); indexId++)
    {
        ElementBenes* copyBenes = otherRcu.benesExistList.at(indexId);
        pasteNewBenes(*copyBenes,copyBenes->pos());
    }
//    resetBoundingRectByChildrenBound();
}

void ModuleRcu::copyAllChildWithOtherRcu(ModuleRcu &otherRcu)
{
    for(int indexId = 0;indexId < otherRcu.bfuExistList.count(); indexId++)
    {
        ElementBfu* copyBfu = otherRcu.bfuExistList.at(indexId);
        if(copyBfu)
        {
            copyOtherBfu(*copyBfu);
        }
    }
    for(int indexId = 0;indexId < otherRcu.customPEExistList.count(); indexId++)
    {
        ElementCustomPE* copyCustomPE = otherRcu.customPEExistList.at(indexId);
        if(copyCustomPE)
        {
            copyOtherCustomPE(*copyCustomPE);
        }
    }
    for(int indexId = 0;indexId < otherRcu.sboxExistList.count(); indexId++)
    {
        ElementSbox* copySbox = otherRcu.sboxExistList.at(indexId);
        if(copySbox)
        {
            copyOtherSbox(*copySbox);
        }
    }
    for(int indexId = 0;indexId < otherRcu.benesExistList.count(); indexId++)
    {
        ElementBenes* copyBenes = otherRcu.benesExistList.at(indexId);
        if(copyBenes)
        {
            copyOtherBenes(*copyBenes);
        }
    }
}

QList<BaseArithUnit *> ModuleRcu::getSelectedArith()
{
    QList<BaseArithUnit *> resultList;
    resultList.clear();
    for(int indexId = 0;indexId < bfuExistList.count(); indexId++)
    {
        if(bfuExistList.at(indexId)->isSelected())
        {
            resultList.append(bfuExistList.at(indexId));
        }
    }
    for(int indexId = 0;indexId < customPEExistList.count(); indexId++)
    {
        if(customPEExistList.at(indexId)->isSelected())
        {
            resultList.append(customPEExistList.at(indexId));
        }
    }
    for(int indexId = 0;indexId < sboxExistList.count(); indexId++)
    {
        if(sboxExistList.at(indexId)->isSelected())
        {
            resultList.append(sboxExistList.at(indexId));
        }
    }
    for(int indexId = 0;indexId < benesExistList.count(); indexId++)
    {
        if(benesExistList.at(indexId)->isSelected())
        {
            resultList.append(benesExistList.at(indexId));
        }
    }
    return resultList;
}

void ModuleRcu::resetBoundingRectByPaste(QRectF newRect)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ItemRectChangedCommand* cmdChangeItemRect = new ItemRectChangedCommand(rcaScene,this,this->getRealBoundingRect(),newRect);
        rcaScene->pushCmdItemRectChanged(cmdChangeItemRect);
    }
    ModuleBcu* parentBcu = dynamic_cast<ModuleBcu*>(parentItem());
    if(parentBcu)
    {
        QRectF transRect = mapRectToParent(realBoundingRect);
        transRect = parentBcu->enabledRect2BoundingRect(transRect);
        QRectF newRect = parentBcu->getRealBoundingRect();
        newRect.setRight(qMax(newRect.right(),transRect.right()));
        newRect.setBottom(qMax(newRect.bottom(),transRect.bottom()));
        newRect.setLeft(qMin(newRect.left(),transRect.left()));
        newRect.setTop(qMin(newRect.top(),transRect.top()));
        parentBcu->resetBoundingRectByPaste(newRect);
    }
}

void ModuleRcu::reaetRchChannelId(int rchGroupId)
{
//    rchChannel = rchGroupId;
    for(int curIndex=0;curIndex<bfuExistList.size();curIndex++)
    {
        ElementBfu* bfu = bfuExistList.at(curIndex);
        if(bfu && bfu->inPortA && bfu->inPortB && bfu->inPortT)
        {
            bfu->inPortA->deleteReadRchDiffGroup(rchGroupId);
            bfu->inPortB->deleteReadRchDiffGroup(rchGroupId);
            bfu->inPortT->deleteReadRchDiffGroup(rchGroupId);
        }
    }
}

bool ModuleRcu::addNewBfu(QPointF posAtThis)
{
    if(isBfuFulled) return false;   //add bfu failed when bfu fulled.

    int curIndex=0;
    for(;curIndex<bfuExistList.size()+customPEExistList.size();curIndex++)
    {
        bool bBreak = false;
        for (int i = 0; i < bfuExistList.size(); ++i)
        {
            if(curIndex == bfuExistList.at(i)->getIndexInRcu())
            {
                bBreak = true;
                break;
            }
        }

        if(!bBreak)
        {
            for (int i = 0; i < customPEExistList.size(); ++i)
            {
                if(curIndex == customPEExistList.at(i)->getIndexInRcu())
                {
                    bBreak = true;
                    break;
                }
            }
        }
        if(!bBreak)
            break;
    }

    QL_DEBUG << curIndex;

    if(curIndex >= bfuMaxCount) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return false;

    AddBfuCommand* cmdAddBfu = new AddBfuCommand(rcaScene,indexBcu,indexRcu,curIndex,this);
    rcaScene->pushCmdAddBfu(cmdAddBfu);
    ElementBfu* elementBfu = cmdAddBfu->getBfuElement();

//    ElementBfu* elementBfu = new ElementBfu(indexBcu,indexRcu,curIndex,this);
//    bfuExistList.insert(curIndex,elementBfu);
//    connect(elementBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
//    connect(elementBfu,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
//    connect(elementBfu,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    // setpos的时候先要校验一下此pos放置item后，
    // scene的rect是否contains这个bfu。
    // 否，则重新调整位置。
    elementBfu->setPos(posAtThis);

    if(bfuMaxCount <= bfuExistList.size() + customPEExistList.size())
    {
        isBfuFulled = true;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    emit rcaScene->addNewBfuSuccess(elementBfu);

    QL_DEBUG << bfuExistList << curIndex;

    return true;
}

bool ModuleRcu::copyOtherBfu(ElementBfu &copyBfu)
{
    if(isBfuFulled) return false;   //add bfu failed when bfu fulled.
    int copyIndex = copyBfu.getIndexInRcu();

    ElementBfu* elementBfu = NULL;
    for(int index=0;index<bfuExistList.size();index++)
    {
        if(copyIndex == bfuExistList.at(index)->getIndexInRcu())
        {
            elementBfu = bfuExistList.at(index);
        }
    }
    int pasteIndex = 0;
    if(elementBfu == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<bfuExistList.size();curIndex++)
        {
            if(curIndex != bfuExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= bfuMaxCount) return false;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= bfuMaxCount) return false;

    elementBfu = new ElementBfu(copyBfu,this);
    bfuExistList.insert(pasteIndex,elementBfu);

    if(bfuMaxCount <= bfuExistList.size())
    {
        isBfuFulled = true;
    }

    return true;
}

bool ModuleRcu::addNewCustomPE(QPointF posAtThis)
{
    if(isBfuFulled) return false;   //add bfu failed when bfu fulled.

    int curIndex=0;
    for(;curIndex<bfuExistList.size()+customPEExistList.size();curIndex++)
    {
        bool bBreak = false;
        for (int i = 0; i < bfuExistList.size(); ++i)
        {
            if(curIndex == bfuExistList.at(i)->getIndexInRcu())
            {
                bBreak = true;
                break;
            }
        }

        if(!bBreak)
        {
            for (int i = 0; i < customPEExistList.size(); ++i)
            {
                if(curIndex == customPEExistList.at(i)->getIndexInRcu())
                {
                    bBreak = true;
                    break;
                }
            }
        }
        if(!bBreak)
            break;
    }

    QL_DEBUG << curIndex;

    if(curIndex >= bfuMaxCount) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return false;

    AddCustomPECommand* cmdAddCustomPE = new AddCustomPECommand(rcaScene,indexBcu,indexRcu,curIndex,this);
    rcaScene->pushCmdAddCustomPE(cmdAddCustomPE);
    ElementCustomPE* elementCustomPE = cmdAddCustomPE->getCustomPEElement();

    // setpos的时候先要校验一下此pos放置item后，
    // scene的rect是否contains这个bfu。
    // 否，则重新调整位置。
    elementCustomPE->setPos(posAtThis);

    if(bfuMaxCount <= customPEExistList.size() + bfuExistList.size())
    {
        isBfuFulled = true;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    emit rcaScene->addNewCustomPESuccess(elementCustomPE);

    QL_DEBUG << customPEExistList << curIndex;

    return true;
}

bool ModuleRcu::copyOtherCustomPE(ElementCustomPE &copyCustomPE)
{
    if(isBfuFulled) return false;   //add bfu failed when bfu fulled.
    int copyIndex = copyCustomPE.getIndexInRcu();

    ElementCustomPE* elementCustomPE = NULL;
    for(int index=0;index<customPEExistList.size();index++)
    {
        if(copyIndex == customPEExistList.at(index)->getIndexInRcu())
        {
            elementCustomPE = customPEExistList.at(index);
        }
    }
    int pasteIndex = 0;
    if(elementCustomPE == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<customPEExistList.size();curIndex++)
        {
            if(curIndex != customPEExistList.at(curIndex)->getIndexInRcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= bfuMaxCount) return false;

        pasteIndex = curIndex;
    }
    if(pasteIndex >= bfuMaxCount) return false;

    elementCustomPE = new ElementCustomPE(copyCustomPE,this);
    customPEExistList.insert(pasteIndex,elementCustomPE);

    if(bfuMaxCount <= customPEExistList.size())
    {
        isBfuFulled = true;
    }

    return true;
}

bool ModuleRcu::addNewSbox(QPointF posAtThis)
{
    if(isSboxFulled) return false;   //add sbox failed when sbox fulled.

    int curIndex=0;
    for(;curIndex<sboxExistList.size();curIndex++)
    {
        if(curIndex != sboxExistList.at(curIndex)->getIndexInRcu()) break;
    }

    QL_DEBUG << curIndex;

    if(curIndex >= sboxMaxCount) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return false;

    AddSboxCommand* cmdAddSbox = new AddSboxCommand(rcaScene,indexBcu,indexRcu,curIndex,this);
    rcaScene->pushCmdAddSbox(cmdAddSbox);
    ElementSbox* elementSbox = cmdAddSbox->getSboxElement();

//    ElementSbox* elementSbox = new ElementSbox(indexBcu,indexRcu,curIndex,this);
//    sboxExistList.insert(curIndex,elementSbox);
//    connect(elementSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
//    connect(elementSbox,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
//    connect(elementSbox,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    // setpos的时候先要校验一下此pos放置item后，
    // scene的rect是否contains这个sbox。
    // 否，则重新调整位置。
    elementSbox->setPos(posAtThis);

    if(sboxMaxCount <= sboxExistList.size())
    {
        isSboxFulled = true;
        emit sboxFullStatesChanged(isSboxFulled);
    }

    emit rcaScene->addNewSboxSuccess(elementSbox);

    QL_DEBUG << sboxExistList << curIndex;

    return true;
}

bool ModuleRcu::addNewBenes(QPointF posAtThis)
{
    if(isBenesFulled) return false;   //add benes failed when benes fulled.

    int curIndex=0;
    for(;curIndex<benesExistList.size();curIndex++)
    {
        if(curIndex != benesExistList.at(curIndex)->getIndexInRcu()) break;
    }

    QL_DEBUG << curIndex;

    if(curIndex >= benesMaxCount) return false;

    if(isBenesFulled) return false;   //add benes failed when benes fulled.
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return false;

    AddBenesCommand* cmdAddBenes = new AddBenesCommand(rcaScene,indexBcu,indexRcu,curIndex,this);
    rcaScene->pushCmdAddBenes(cmdAddBenes);
    ElementBenes* elementBenes = cmdAddBenes->getBenesElement();

//    ElementBenes* elementBenes = new ElementBenes(indexBcu,indexRcu,curIndex,this);
//    benesExistList.insert(curIndex,elementBenes);
//    connect(elementBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
//    connect(elementBenes,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
//    connect(elementBenes,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));

    // setpos的时候先要校验一下此pos放置item后，
    // scene的rect是否contains这个benes。
    // 否，则重新调整位置。
    elementBenes->setPos(posAtThis);
//    elementBenes->update();


    if(benesMaxCount <= benesExistList.size())
    {
        isBenesFulled = true;
        emit benesFullStatesChanged(isBenesFulled);
    }

    emit rcaScene->addNewBenesSuccess(elementBenes);

    QL_DEBUG << benesExistList << curIndex;

    return true;
}

//主要是child被删除后，isBfuFulled 需要重新计算，
//将bfu的index删除，rcu将可以重新新建此index的bfu。
void ModuleRcu::handleBfuBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG << deletedItem << sender();
    if(!deletedItem) return;
    ElementBfu* deletedBfu = dynamic_cast<ElementBfu*>(deletedItem);
    if(!deletedBfu) return;


    QL_DEBUG;
    int indexBfu = deletedBfu->getIndexInRcu();
    if(indexBfu >= bfuMaxCount) return;

    if(bfuExistList.contains(deletedBfu))
    {
        bfuExistList.removeOne(deletedBfu);
        disconnect(deletedBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
        disconnect(deletedBfu,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
        disconnect(deletedBfu,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    }

    if(bfuMaxCount > bfuExistList.size() + customPEExistList.size())
    {
        isBfuFulled = false;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->isItemBakBeDeleted(deletedBfu);
        emit rcaScene->deleteBfuSuccess(deletedBfu);
    }
}

void ModuleRcu::handleCustomPEBeDeleted(BaseItem *deletedItem)
{
    QL_DEBUG << deletedItem << sender();
    if(!deletedItem) return;
    ElementCustomPE* deletedCustomPE = dynamic_cast<ElementCustomPE*>(deletedItem);
    if(!deletedCustomPE) return;


    QL_DEBUG;
    int indexBfu = deletedCustomPE->getIndexInRcu();
    if(indexBfu >= bfuMaxCount) return;

    if(customPEExistList.contains(deletedCustomPE))
    {
        customPEExistList.removeOne(deletedCustomPE);
        disconnect(deletedCustomPE,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleCustomPEBeDeleted(BaseItem*)));
        disconnect(deletedCustomPE,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
        disconnect(deletedCustomPE,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    }

    if(bfuMaxCount > customPEExistList.size() + bfuExistList.size())
    {
        isBfuFulled = false;
        emit bfuFullStatesChanged(isBfuFulled);
    }

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->isItemBakBeDeleted(deletedCustomPE);
        emit rcaScene->deleteCustomPESuccess(deletedCustomPE);
    }
}

void ModuleRcu::handleSboxBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG;
    if(!deletedItem) return;
    ElementSbox* deletedSbox = dynamic_cast<ElementSbox*>(deletedItem);
    if(!deletedSbox) return;

    QL_DEBUG;
    int indexSbox = deletedSbox->getIndexInRcu();
    if(indexSbox >= sboxMaxCount) return;

    if(sboxExistList.contains(deletedSbox))
    {
        sboxExistList.removeOne(deletedSbox);
        disconnect(deletedSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
        disconnect(deletedSbox,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
        disconnect(deletedSbox,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    }

    if(sboxMaxCount > sboxExistList.size())
    {
        isSboxFulled = false;
        emit sboxFullStatesChanged(isSboxFulled);
    }

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->isItemBakBeDeleted(deletedSbox);
        emit rcaScene->deleteSboxSuccess(deletedSbox);
    }
}

void ModuleRcu::handleBenesBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG;
    if(!deletedItem) return;
    ElementBenes* deletedBenes = dynamic_cast<ElementBenes*>(deletedItem);
    if(!deletedBenes) return;

    int indexBenes = deletedBenes->getIndexInRcu();
    if(indexBenes >= benesMaxCount) return;

    if(benesExistList.contains(deletedBenes))
    {
        QL_DEBUG;
        benesExistList.removeOne(deletedBenes);
        disconnect(deletedBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
        disconnect(deletedBenes,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
        disconnect(deletedBenes,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    }

    if(benesMaxCount > benesExistList.size())
    {
        isBenesFulled = false;
        emit benesFullStatesChanged(isBenesFulled);
    }

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->isItemBakBeDeleted(deletedBenes);
        emit rcaScene->deleteBenesSuccess(deletedBenes);
    }
}

void ModuleRcu::handleAddWriteMemSuccess(ModuleWriteMem* writeMem)
{
    QL_DEBUG;
    if(writeMem == NULL) return;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        emit rcaScene->addNewWriteMemSuccess(indexBcu,indexRcu,writeMem);
    }
}

void ModuleRcu::handleAddReadMemSuccess(ModuleReadMem* readMem)
{
    QL_DEBUG;
    if(readMem == NULL) return;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        emit rcaScene->addNewReadMemSuccess(indexBcu,indexRcu,readMem);
    }
}

void ModuleRcu::handleFifoDeleted(QObject *delFifo)
{
    if(inFifo0 == delFifo) inFifo0 = NULL;
    if(inFifo1 == delFifo) inFifo1 = NULL;
    if(inFifo2 == delFifo) inFifo2 = NULL;
    if(inFifo3 == delFifo) inFifo3 = NULL;
    if(outFifo0 == delFifo) outFifo0 = NULL;
    if(outFifo1 == delFifo) outFifo1 = NULL;
    if(outFifo2 == delFifo) outFifo2 = NULL;
    if(outFifo3 == delFifo) outFifo3 = NULL;
}
