#include "RcaGraphScene.h"
#include "ModuleBcu.h"
#include "RcaItemSetting/RcaSetting.h"

#include <QDebug>
#include <QTransform>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsSceneEvent>
#include <QtWidgets>
#include <QMessageBox>
#include <QPainter>
#include <QSvgGenerator>
#include <QPrinter>
#include "ItemUndoCommands.h"
#include "common/cappenv.hpp"
#include "fileresolver/cxmlresolver.h"
#include "citemattributesetting.h"

RcaGraphScene::RcaGraphScene(QHash<int, SCustomPEConfig> *customPEConfigHash, QObject *parent, CProjectResolver *projectResolver)
    : QGraphicsScene(parent)
      , m_customPEConfigHash(customPEConfigHash)
      , bcuMaxCount(BCU_MAX_COUNT)
      //    , bcuExistList(QList<int>())
      , isBcuFulled(false)
      , currentAddWireCmd(NULL)
      , currentPaintingWire(NULL)
      , currentMouseInPort(NULL)
      , allWireList(QList<ElementWire*>())
      , m_projectResolver(projectResolver)
      , selectedAttr(SelectedAttrNone)
      , whatItemAlign(Module_None)
      , whatItemCopy(Module_None)
      , selectedBcuList(QList<ModuleBcu*>())
      //    , selectedOneBcu(NULL)
      , selectedRcuList(QList<ModuleRcu*>())
      //    , selectedOneRcu(NULL)
      , selectedArithList(QList<BaseArithUnit*>())
      , sceneState(Normal)
      , undoStack(new QUndoStack(this))
      , funCopyPaste(ItemCopyPaste::instance(CAppEnv::m_mainWgt))
      //    , undoAction(NULL)
      //    , redoAction(NULL)
      //    , gridStepPoint(QPoint(MIN_STEP_COUNT,MIN_STEP_COUNT))
      //    , isGridHidden(false)
      //    , gridItem(NULL)
      , isDebugMode(false)
      , bcuExistList(QList<ModuleBcu*>())
      , isAutoSetItemPos(false)
      , xmlInView(NULL)
      , isAllWireShow(false)
      , doubleClickedItemBak(NULL)
      , wirePenBrush(WIRE_UNSELECTED_BRUSH)
      , wirePenWidth(WIRE_DEFAULT_PEN_WIDTH)
      , curMouseScenePos(QPointF())
      , isModified(false)
{
    //    undoStack = new QUndoStack(this);
    //    undoAction = undoStack->createUndoAction(this, tr(u8"&Undo"));
    //    undoAction->setShortcuts(QKeySequence::Undo);

    //    redoAction = undoStack->createRedoAction(this, tr(u8"&Redo"));
    //    redoAction->setShortcuts(QKeySequence::Redo);
    //    gridItem = new RcaGridItem();
    //    addItem(gridItem);
    //    setForegroundBrush(QBrush(Qt::lightGray, Qt::CrossPattern));
    clearFocus();
    //    setGridBackGround();

    curItemSetting = new CItemAttributeSetting(this);
    curPropertyBrowser = new CPropertyBrowser(this);
}

RcaGraphScene:: ~RcaGraphScene()
{
    if(undoStack)
    {
        delete undoStack;
//        undoStack->deleteLater();
        undoStack = NULL;
    }
    if(curItemSetting)
    {
        delete curItemSetting;
        curItemSetting = NULL;
    }
    QL_DEBUG<<"~RcaGraphScene";
}

void RcaGraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(Painting_wire != sceneState)
    {
//        event->setModifiers(event->modifiers() | Qt::ControlModifier);
        QGraphicsScene::mousePressEvent(event);
    }
}

void RcaGraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QL_DEBUG << event->pos().toPoint() << event->scenePos().toPoint() << sceneRect().toRect() << event->button() << sceneState;

    if(sceneRect().contains(event->scenePos()))
    {
        if(Qt::LeftButton == event->button())
        {
            //判断当前状态为add bcu 并且点击位置没有item的时候添加bcu，否则将鼠标信息传递到下一级。
            if((sceneState == Add_Bcu) && (!(itemAt(event->scenePos(),QTransform()))))
            {
                addNewBcu(event->scenePos());
            }
            //判断当前状态为add wire 并且点击位置为port的时候添加wire，否则将鼠标信息传递到下一级。
            else if(sceneState == Add_Wire)
            {
                QL_DEBUG;
                QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
//                ElementWire* eleWire = dynamic_cast<ElementWire*>(item);
//                if(eleWire)
//                {
//                    QL_DEBUG;
//                    eleWire->addSubMovePointF(event->scenePos());
//                    currentPaintingWire = eleWire;
//                    sceneState = Painting_wire;
//                }
//                else
                {
                    ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                    if((elePort) && (OutputDirection == elePort->getPortDirection()))
                    {
                        QL_DEBUG;
                        addNewWire(elePort->mapToScene(QPointF(elePort->boundingRect().center().x(),
                                                               elePort->getRealBoundingRect().bottom()))
                                   ,elePort);
                    }
                }
            }
            else if((sceneState == Painting_wire) && (currentPaintingWire))
            {
                QL_DEBUG;
                QPointF newPos = event->scenePos();
                QGraphicsItem* item = itemAt(newPos,QTransform());
                ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                if(elePort && (item != currentPaintingWire->parentItem()) && (InputDirection == elePort->getPortDirection()))
                {
//                    elePort->setColor(WIRE_DRAG_COLOR);
                    newPos = elePort->mapToScene(QPointF(elePort->boundingRect().center().x(),
                                                         elePort->getRealBoundingRect().top()));
                    elePort->update();
                    if(elePort->canConnectOutPort(currentPaintingWire->getInputSignal()))
                    {
                        if(currentPaintingWire->commitPath(elePort,newPos))
                        {
                            currentPaintingWire = NULL;
                            pushCmdAddWire(currentAddWireCmd);

                            currentAddWireCmd = NULL;

                            sceneState = Add_Wire;
                        }
                    }
                }
                else if(elePort == NULL)
                {
                    currentPaintingWire->addPointF(newPos);
                }
            }
            else if(sceneState == Add_Mem)
            {
                QL_DEBUG;
                QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
                {
                    ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                    if((elePort) && (elePort->parentItem()))
                    {
                        QL_DEBUG;
                        BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(elePort->parentItem());
                        if(baseArith)
                        {
                            if(elePort->getPortDirection() == InputDirection)
                            {
                                elePort->addNewReadMem();
                            }
                            else if(elePort->getPortDirection() == OutputDirection)
                            {
                                elePort->addNewWriteMem(baseArith);
                            }
                        }
                    }
                }
            }
            else if(sceneState == Add_Imd)
            {
                QL_DEBUG;
                QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
                {
                    ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                    if((elePort) && (elePort->parentItem()))
                    {
                        QL_DEBUG;
                        BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(elePort->parentItem());
                        if(baseArith)
                        {
                            if(elePort->getPortDirection() == InputDirection)
                            {
                                elePort->addNewReadImd();
                            }
                        }
                    }
                }
            }
            else if(sceneState == Add_Rch)
            {
                QL_DEBUG;
                QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
                {
                    ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                    if((elePort) && (elePort->parentItem()))
                    {
                        QL_DEBUG;
                        BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(elePort->parentItem());
                        if(baseArith)
                        {
                            if(elePort->getPortDirection() == InputDirection)
                            {
                                elePort->addNewReadRch();
                            }
                            else if(elePort->getPortDirection() == OutputDirection)
                            {
                                elePort->addNewWriteRch(baseArith);
//                                ElementBfu* parentBfu = dynamic_cast<ElementBfu*>(baseArith);
//                                if(parentBfu && parentBfu->outPortX == elePort)
//                                {
//                                    elePort->addNewWriteRch(baseArith);
//                                }
                            }
                        }
                    }
                }
            }
            else if(sceneState == Add_Lfsr)
            {
                QL_DEBUG;
                QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
                {
                    ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                    if((elePort) && (elePort->parentItem()))
                    {
                        QL_DEBUG;
                        BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(elePort->parentItem());
                        if(baseArith)
                        {
                            if(elePort->getPortDirection() == InputDirection)
                            {
                                elePort->addNewReadLfsr();
                            }
                            else if(elePort->getPortDirection() == OutputDirection)
                            {
                                elePort->addNewWriteLfsr(baseArith);
                            }
                        }
                    }
                }
            }

        }
        else if(Qt::RightButton == event->button())
        {
            if((sceneState == Painting_wire) && (currentAddWireCmd))
            {
                QL_DEBUG;
                removeItem(currentAddWireCmd->getElementWire());
                delete currentAddWireCmd;
                currentAddWireCmd = NULL;
                currentPaintingWire = NULL;
                sceneState = Add_Wire;
            }
            else
            {
                sceneState = Normal;
                emit statusChanged(sceneState);
            }
        }
    }

    if(Painting_wire != sceneState)
    {
        QList<QGraphicsItem *> selectedList = selectedItems();
        QList<BaseItem*> movedItemList;
        for(int i=0; i<selectedList.count(); i++)
        {
            QGraphicsItem* graphItemTemp = selectedList.at(i);
            BaseItem* baseItemTemp = dynamic_cast<BaseItem*>(graphItemTemp);
            BasePort* basePortTemp = dynamic_cast<BasePort*>(graphItemTemp);
            if(baseItemTemp && (basePortTemp == NULL))
            {
                if(baseItemTemp->pos() != baseItemTemp->getOldPos())
                {
                    movedItemList.append(baseItemTemp);
                }
            }
        }
        if(movedItemList.count() > 0)
        {
            pushCmdItemMoved(movedItemList);
        }
        QGraphicsScene::mouseReleaseEvent(event);
    }
    //必须放在QGraphicsScene::mouseReleaseEvent(event);下面
    //因为只有当mouse信号发送出去，并返回到此函数后，item才算是做了mouse的动作处理。
    resetSelectedAttr();
}

//置于底层
void RcaGraphScene::slotLayerBottom(BaseItem* itemSelected)
{
    QList<QGraphicsItem *> collideItems = collidingItems(itemSelected);
    for (int i = 0; i < collideItems.size(); ++i)
        itemSelected->stackBefore(collideItems.at(i));
    update();
}

void RcaGraphScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    BaseItem* itemSelected = NULL;
    if(!selectedItems().isEmpty())
    {
        for(int i=selectedItems().count()-1;i>=0;i--)
        {
            itemSelected = dynamic_cast<BaseItem*>(selectedItems().at(i));
            if(itemSelected)
            {
                ModuleWriteMem* itemTempMem = dynamic_cast<ModuleWriteMem*>(itemSelected);
                if(itemTempMem)
                    break;
                ModuleWriteRch* itemTempRch = dynamic_cast<ModuleWriteRch*>(itemSelected);
                if(itemTempRch)
                    break;
                ModuleWriteLfsr* itemTempLfsr = dynamic_cast<ModuleWriteLfsr*>(itemSelected);
                if(itemTempLfsr)
                    break;
                break;
            }
        }
    }
    slotLayerBottom(itemSelected);
    QGraphicsScene::mouseDoubleClickEvent(event);
}

void RcaGraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    curMouseScenePos = event->scenePos();
    if(sceneState == Add_Wire)
    {
//        QL_DEBUG;
        QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
        ElementPort* elePort = dynamic_cast<ElementPort*>(item);
        if((elePort))
        {
            if(currentMouseInPort)
            {
                if(currentMouseInPort != elePort)
                {
                    currentMouseInPort->hoverState = None_Hover;
                    currentMouseInPort->update();
                    currentMouseInPort = elePort;
                    currentMouseInPort->hoverState = (elePort->getPortDirection() == OutputDirection) ?
                                Can_Hover : Cannot_Hover;
                    currentMouseInPort->update();
                }
            }
            else
            {
                currentMouseInPort = elePort;
                currentMouseInPort->hoverState = (elePort->getPortDirection() == OutputDirection) ?
                            Can_Hover : Cannot_Hover;
                currentMouseInPort->update();
            }
        }
        else
        {
//            QL_DEBUG;
            if(currentMouseInPort)
            {
                currentMouseInPort->hoverState = None_Hover;
                currentMouseInPort->update();
                currentMouseInPort = NULL;
            }
        }
    }
    else if(sceneState == Add_Imd)
    {
//        QL_DEBUG;
        QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
        ElementPort* elePort = dynamic_cast<ElementPort*>(item);
        if((elePort))
        {
            currentMouseInPort = elePort;
            currentMouseInPort->hoverState = (elePort->getPortDirection() == InputDirection) ?
                        Can_Hover : Cannot_Hover;
            currentMouseInPort->update();
        }
        else
        {
//            QL_DEBUG;
            if(currentMouseInPort)
            {
                currentMouseInPort->hoverState = None_Hover;
                currentMouseInPort->update();
                currentMouseInPort = NULL;
            }
        }
    }
    else if(sceneState == Add_Rch || sceneState == Add_Mem)
    {
//        QL_DEBUG;
        QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
        ElementPort* elePort = dynamic_cast<ElementPort*>(item);
        if((elePort))
        {
            currentMouseInPort = elePort;
            currentMouseInPort->hoverState = Can_Hover;
            currentMouseInPort->update();
        }
        else
        {
//            QL_DEBUG;
            if(currentMouseInPort)
            {
                currentMouseInPort->hoverState = None_Hover;
                currentMouseInPort->update();
                currentMouseInPort = NULL;
            }
        }
    }
    else if ((sceneState == Painting_wire) && (currentPaintingWire))
    {

//        QL_DEBUG;
        QPointF newPos = event->scenePos();
        QGraphicsItem* item = itemAt(newPos,QTransform());
        ElementPort* elePort = dynamic_cast<ElementPort*>(item);
        if(elePort && (item != currentPaintingWire->parentItem()))
        {
            newPos = elePort->mapToScene(QPointF(elePort->boundingRect().center().x(),
                                                 elePort->getRealBoundingRect().top()));
        }
        currentPaintingWire->movedPointF(newPos);

        if((elePort))
        {
            if(currentMouseInPort)
            {
                if(currentMouseInPort != elePort)
                {
                    currentMouseInPort->hoverState = None_Hover;
                    currentMouseInPort->update();
                    currentMouseInPort = elePort;
                    currentMouseInPort->hoverState = elePort->canConnectOutPort(currentPaintingWire->getInputSignal()) ?
                                Can_Hover : Cannot_Hover;
                    currentMouseInPort->update();
                }
            }
            else
            {
                currentMouseInPort = elePort;
                currentMouseInPort->hoverState = elePort->canConnectOutPort(currentPaintingWire->getInputSignal()) ?
                            Can_Hover : Cannot_Hover;
                currentMouseInPort->update();
            }
        }
        else
        {
//            QL_DEBUG;
            if(currentMouseInPort)
            {
                currentMouseInPort->hoverState = None_Hover;
                currentMouseInPort->update();
                currentMouseInPort = NULL;
            }
        }
    }
    else
    {
//        QL_DEBUG;
        if(currentMouseInPort)
        {
            currentMouseInPort->hoverState = None_Hover;
            currentMouseInPort->update();
            currentMouseInPort = NULL;
        }
    }

    if(Painting_wire != sceneState)
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void RcaGraphScene::keyPressEvent(QKeyEvent *keyEvent)
{
    bool isDelPaintingWire = false;
    if(keyEvent->key() == Qt::Key_Escape)
    {
        if((sceneState == Painting_wire))
        {
            isDelPaintingWire = true;
            sceneState = Add_Wire;
        }
        else
        {
            sceneState = Normal;
            emit statusChanged(sceneState);
        }
    }
//    if(keyEvent->modifiers() == Qt::CTRL)
//    {
//        QL_DEBUG << Qt::Key(keyEvent->key());
//        switch(keyEvent->key())
//        {
//        case Qt::Key_Z:actionUndo();break;
//        case Qt::Key_Y:actionRedo();break;
//        default: break;
//        }
//    }
    if(isDelPaintingWire && (currentAddWireCmd))
    {
        removeItem(currentAddWireCmd->getElementWire());
        delete currentAddWireCmd;
        currentAddWireCmd = NULL;
        currentPaintingWire = NULL;
    }
    QGraphicsScene::keyPressEvent(keyEvent);
}

void RcaGraphScene::drawBackground(QPainter *painter, const QRectF &rect)
{
//    QL_DEBUG;
    painter->setRenderHints(QPainter::Antialiasing|
                            QPainter::TextAntialiasing);
    painter->setBrushOrigin(0, 0);
    painter->fillRect(rect, backgroundBrush());
}

void RcaGraphScene::drawForeground(QPainter *painter, const QRectF &rect)
{
//    QL_DEBUG;
    painter->setRenderHints(QPainter::Antialiasing|
                            QPainter::TextAntialiasing);
    painter->setBrushOrigin(0, 0);
    painter->fillRect(rect, foregroundBrush());
}
bool RcaGraphScene::addBcuFromXml(QPointF posAtParent, ModuleBcu* moduleBcu)
{
    if(moduleBcu == NULL) return false;
    int bcuIndex = moduleBcu->getIndexBcu();
    if(isBcuFulled) return false;   //add bcu failed when bcu fulled.
    if(moduleBcu == NULL) return false;

    int insertIndex = bcuExistList.size();

    //bcuItemList 需要排序
    for(int i=0;i<bcuExistList.size();i++)
    {
        if(bcuIndex < bcuExistList.at(i)->getIndexBcu())
        {
            insertIndex = i;
            break;
        }
    }
    bcuExistList.insert(insertIndex,moduleBcu);

//    for(int index=0;index<bcuItemList.size();index++)
//    {
//        if(bcuIndex == bcuItemList.at(index)->getIndexBcu())
//        {
//            return false;
//        }
//    }
//    bcuItemList.insert(bcuIndex,moduleBcu);

    this->addItem(moduleBcu);
    QL_DEBUG << posAtParent;
    moduleBcu->setPos(posAtParent);

    connect(moduleBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));

    if(bcuMaxCount <= bcuExistList.size())
    {
        isBcuFulled = true;
        emit bcuFullStatesChanged(isBcuFulled);
    }

    return true;
}

bool RcaGraphScene::addRcuFromXml(QPointF posAtParent, ModuleRcu *moduleRcu)
{
    if(moduleRcu == NULL) return false;
    ModuleBcu* moduleBcu = findBcuFromScene(moduleRcu->getIndexBcu());
    if(moduleBcu == NULL)   return false;

    return moduleBcu->addRcuFromXml(posAtParent,moduleRcu);
}

bool RcaGraphScene::addBfuFromXml(QPointF posAtParent, ElementBfu* elementBfu)
{
    if(elementBfu == NULL)  return false;
    ModuleBcu* moduleBcu = findBcuFromScene(elementBfu->getIndexBcu());
    if(moduleBcu == NULL)   return false;

    QL_DEBUG << posAtParent;

    return moduleBcu->addBfuFromXml(posAtParent,elementBfu);
}

bool RcaGraphScene::addCustomPEFromXml(QPointF posAtParent, ElementCustomPE* elementCustomPE)
{
    if(elementCustomPE == NULL)  return false;
    ModuleBcu* moduleBcu = findBcuFromScene(elementCustomPE->getIndexBcu());
    if(moduleBcu == NULL)   return false;

    QL_DEBUG << posAtParent;

    return moduleBcu->addCustomPEFromXml(posAtParent,elementCustomPE);
}

bool RcaGraphScene::addSboxFromXml(QPointF posAtParent, ElementSbox* elementSbox)
{
    if(elementSbox == NULL) return false;
    ModuleBcu* moduleBcu = findBcuFromScene(elementSbox->getIndexBcu());
    if(moduleBcu == NULL)   return false;

    QL_DEBUG << posAtParent;

    return moduleBcu->addSboxFromXml(posAtParent,elementSbox);
}

bool RcaGraphScene::addBenesFromXml(QPointF posAtParent, ElementBenes* elementBenes)
{
    if(elementBenes == NULL) return false;
    ModuleBcu* moduleBcu = findBcuFromScene(elementBenes->getIndexBcu());
    if(moduleBcu == NULL)   return false;

    QL_DEBUG << posAtParent;

    return moduleBcu->addBenesFromXml(posAtParent,elementBenes);
}

bool RcaGraphScene::addElementWireFromXml(ElementPort* inputPort, ElementPort *outputPort)
{
    if(inputPort && (inputPort->getPortDirection() == InputDirection)
            && (outputPort) && (outputPort->getPortDirection() == OutputDirection)
            && (inputPort->getInSignal() == outputPort))
    {
        QL_DEBUG;

        QPointF posAtScene = outputPort->mapToScene(QPointF(outputPort->boundingRect().center().x(),
                                                            outputPort->getRealBoundingRect().bottom()));
        AddWireCommand* cmdAddWire = new AddWireCommand(this,posAtScene,outputPort,wirePenBrush,wirePenWidth);

//        ElementWire* wireTemp = new ElementWire(outputPort->mapToScene(QPointF(outputPort->boundingRect().center().x(),
//                                                                               outputPort->getRealBoundingRect().top()))
//                                                ,outputPort,wirePenBrush,wirePenWidth);

//        this->addItem(wireTemp);
//        allWireList.append(wireTemp);

        ElementWire* wireTemp = cmdAddWire->getElementWire();
        if(wireTemp)
        {
            QPointF newPos = inputPort->mapToScene(QPointF(inputPort->boundingRect().center().x(),
                                                           inputPort->getRealBoundingRect().top()));
            wireTemp->addEndPortFromXml(inputPort,newPos);

            emit addNewWireSuccess(wireTemp);

            pushCmdAddWire(cmdAddWire);
            return true;
        }
        delete cmdAddWire;
    }
    return false;
}

bool RcaGraphScene::addElementWireBySet(ElementPort *inputPort, ElementPort *outputPort)
{
    if(inputPort && (inputPort->getPortDirection() == InputDirection)
            && (outputPort) && (outputPort->getPortDirection() == OutputDirection)
            && (inputPort->getInSignal() == outputPort))
    {
        QL_DEBUG;

        QPointF posAtScene = outputPort->mapToScene(QPointF(outputPort->boundingRect().center().x(),
                                                            outputPort->getRealBoundingRect().bottom()));
        AddWireCommand* cmdAddWire = new AddWireCommand(this,posAtScene,outputPort,wirePenBrush,wirePenWidth);

//        ElementWire* wireTemp = new ElementWire(outputPort->mapToScene(QPointF(outputPort->boundingRect().center().x(),
//                                                                               outputPort->getRealBoundingRect().top()))
//                                                ,outputPort,wirePenBrush,wirePenWidth);

//        this->addItem(wireTemp);
//        allWireList.append(wireTemp);

        ElementWire* wireTemp = cmdAddWire->getElementWire();
        if(wireTemp)
        {
            QPointF newPos = inputPort->mapToScene(QPointF(inputPort->boundingRect().center().x(),
                                                           inputPort->getRealBoundingRect().top()));
            wireTemp->addEndPortBySet(inputPort,newPos);

            emit addNewWireSuccess(wireTemp);

            pushCmdAddWire(cmdAddWire);
            return true;
        }
        delete cmdAddWire;
    }
    return false;
}

//bool RcaGraphScene::addElementWireWithoutUndo(ElementPort *inputPort, ElementPort *outputPort)
//{
//    if(inputPort && (inputPort->getPortDirection() == InputDirection)
//            && (outputPort) && (outputPort->getPortDirection() == OutputDirection)
//            && (inputPort->getInSignal() == outputPort))
//    {
//        ElementWire* wireTemp = new ElementWire(outputPort->mapToScene(QPointF(outputPort->boundingRect().center().x(),
//                                                                               outputPort->getRealBoundingRect().top()))
//                                                ,outputPort,wirePenBrush,wirePenWidth);

//        this->addItem(wireTemp);
//        allWireList.append(wireTemp);
//        QPointF newPos = inputPort->mapToScene(QPointF(inputPort->boundingRect().center().x(),
//                                                       inputPort->getRealBoundingRect().top()));
//        wireTemp->addEndPortFromXml(inputPort,newPos,inputPort->middlePosY);

//        emit addNewWireSuccess(wireTemp);
//        return true;
//    }
//    return false;
//}

ModuleBcu* RcaGraphScene::findBcuFromScene(int bcuIndex)
{
    QList<QGraphicsItem *> listItems = items();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=listItems.begin();iter!=listItems.end();iter++)
    {
        if((*iter))
        {
            ModuleBcu* moduleBcu = dynamic_cast<ModuleBcu*>(*iter);
            if(moduleBcu)
            {
                QL_DEBUG << moduleBcu->getIndexBcu() << bcuIndex;
                if(moduleBcu->getIndexBcu() == bcuIndex)
                {
                    return moduleBcu;
                }
            }
        }
    }
    QL_DEBUG;
    return NULL;
}


ModuleRcu* RcaGraphScene::findRcuFromScene(int bcuIndex,int rcuIndex)
{
    ModuleBcu* findedBcu = findBcuFromScene(bcuIndex);
    if(findedBcu == NULL) return NULL;

    return findedBcu->findRcuFromThis(rcuIndex);
}

ElementBfu* RcaGraphScene::findBfuFromScene(int bcuIndex,int rcuIndex,int indexInRcu)
{
    ModuleRcu* findedRcu = findRcuFromScene(bcuIndex,rcuIndex);
    if(findedRcu)
    {
        return findedRcu->findBfuFromRcu(indexInRcu);
    }
    return NULL;
}

ElementCustomPE* RcaGraphScene::findCustomPEFromScene(int bcuIndex,int rcuIndex,int indexInRcu)
{
    ModuleRcu* findedRcu = findRcuFromScene(bcuIndex,rcuIndex);
    if(findedRcu)
    {
        return findedRcu->findCustomPEFromRcu(indexInRcu);
    }
    return NULL;
}

ElementSbox* RcaGraphScene::findSboxFromScene(int bcuIndex,int rcuIndex,int indexInRcu)
{
    ModuleRcu* findedRcu = findRcuFromScene(bcuIndex,rcuIndex);
    if(findedRcu)
    {
        return findedRcu->findSboxFromRcu(indexInRcu);
    }
    return NULL;
}

ElementBenes* RcaGraphScene::findBenesFromScene(int bcuIndex,int rcuIndex,int indexInRcu)
{
    ModuleRcu* findedRcu = findRcuFromScene(bcuIndex,rcuIndex);
    if(findedRcu)
    {
        return findedRcu->findBenesFromRcu(indexInRcu);
    }
    return NULL;
}

void RcaGraphScene::initWireAndMemConPort()
{
    QList<QGraphicsItem*>  childItemList = items();
    QList<QGraphicsItem*>::iterator portIter;
    for(portIter=childItemList.begin();portIter!=childItemList.end();portIter++)
    {
        if((*portIter))
        {
            ElementPort* elePort = dynamic_cast<ElementPort*>(*portIter);
            if((elePort) && (elePort->parentItem()) && (elePort->getPortDirection() == InputDirection))
            {
                BaseArithUnit* portParentIsArith = dynamic_cast<BaseArithUnit*>(elePort->parentItem());
                ModuleRcu* portParentIsRcu = dynamic_cast<ModuleRcu*>(elePort->parentItem());
                InputPortType inputType = elePort->getInputType();
                int inputIndex = elePort->getInputIndex();
                int bcuIndex = 0;
                int rcuIndex = 0;
                bool isPortValid = false;
                if(portParentIsRcu)
                {
                    bcuIndex = portParentIsRcu->getIndexBcu();
//                    rcuIndex = OUT_FIFO_SIGNAL_RCU_INDEX;
                    rcuIndex = portParentIsRcu->getIndexRcu();
                    isPortValid = true;
                }
                else if(portParentIsArith)
                {
                    switch(inputType)
                    {
                        case InputPort_CurrentBfuX:
                        case InputPort_CurrentBfuY:
                        case InputPort_CurrentCustomPE:
                        case InputPort_CurrentSbox:
                        case InputPort_CurrentBenes:
                        {
                            bcuIndex = portParentIsArith->getIndexBcu();
                            rcuIndex = portParentIsArith->getIndexRcu();
                            isPortValid = true;
                            break;
                        }
                        case InputPort_LastBfuX:
                        case InputPort_LastBfuY:
                        case InputPort_LastCustomPE:
                        case InputPort_LastSbox:
                        case InputPort_LastBenes:
                        {
                            //如果行号为0
                            if(portParentIsArith->getIndexRcu() == 0)
                            {
                                ModuleBcu* bcuTemp = findBcuFromScene(portParentIsArith->getIndexBcu());
                                bcuIndex = bcuTemp->srcDataFromBcu;
                                rcuIndex = OUT_FIFO_SIGNAL_RCU_INDEX;
                                isPortValid = true;
                            }
                            else
                            {
                                bcuIndex = portParentIsArith->getIndexBcu();
                                rcuIndex = portParentIsArith->getIndexRcu() - 1;
                                isPortValid = true;
                            }
                            break;
                        }
                        case InputPort_InFifo:
                        {
                            bcuIndex = portParentIsArith->getIndexBcu();
                            rcuIndex = portParentIsArith->getIndexRcu();
                            isPortValid = true;
                            break;
                        }
                        case InputPort_Mem:
                        {
                            bcuIndex = portParentIsArith->getIndexBcu();
                            rcuIndex = portParentIsArith->getIndexRcu();
                            isPortValid = true;
                            break;
                        }
                        case InputPort_Imd:
                        {
                            bcuIndex = portParentIsArith->getIndexBcu();
                            rcuIndex = portParentIsArith->getIndexRcu();
                            isPortValid = true;
                            break;
                        }
                        case InputPort_Rch:
                        {
                            bcuIndex = portParentIsArith->getIndexBcu();
                            rcuIndex = portParentIsArith->getIndexRcu();
                            isPortValid = true;
                            break;
                        }
                        case InputPort_Lfsr:
                        {
                            bcuIndex = portParentIsArith->getIndexBcu();
                            rcuIndex = portParentIsArith->getIndexRcu();
                            isPortValid = true;
                            break;
                        }
                        default:break;
                    }
                }
                if(isPortValid)
                {
                    switch(inputType)
                    {
                        case InputPort_CurrentBfuX:
                        case InputPort_LastBfuX:
                        {
                            ElementBfu *elementBfu = findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                            if((elementBfu) && (elementBfu->outPortX))
                            {
                                elePort->connectInputSignal(elementBfu->outPortX);
                                addElementWireFromXml(elePort,elementBfu->outPortX);
                            }
                            break;
                        }
                        case InputPort_CurrentBfuY:
                        case InputPort_LastBfuY:
                        {
                            ElementBfu *elementBfu = findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                            if((elementBfu) && (elementBfu->outPortY))
                            {
                                elePort->connectInputSignal(elementBfu->outPortY);
                                addElementWireFromXml(elePort,elementBfu->outPortY);
                            }
                            break;
                        }
                        case InputPort_CurrentSbox:
                        case InputPort_LastSbox:
                        {
                            ElementSbox* elementSbox = findSboxFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
                            if(elementSbox)
                            {
                                switch(inputIndex%SBOX_OUTPUTCOUNT)
                                {
                                    case 0:elePort->connectInputSignal(elementSbox->outPort0);addElementWireFromXml(elePort,elementSbox->outPort0);break;
                                    case 1:elePort->connectInputSignal(elementSbox->outPort1);addElementWireFromXml(elePort,elementSbox->outPort1);break;
                                    case 2:elePort->connectInputSignal(elementSbox->outPort2);addElementWireFromXml(elePort,elementSbox->outPort2);break;
                                    case 3:elePort->connectInputSignal(elementSbox->outPort3);addElementWireFromXml(elePort,elementSbox->outPort3);break;
                                    default:break;
                                }
                            }
                            break;
                        }
                        case InputPort_CurrentBenes:
                        case InputPort_LastBenes:
                        {
                            ElementBenes* elementBenes = findBenesFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
                            if(elementBenes)
                            {
                                switch(inputIndex%BENES_OUTPUTCOUNT)
                                {
                                    case 0:elePort->connectInputSignal(elementBenes->outPort0);addElementWireFromXml(elePort,elementBenes->outPort0);break;
                                    case 1:elePort->connectInputSignal(elementBenes->outPort1);addElementWireFromXml(elePort,elementBenes->outPort1);break;
                                    case 2:elePort->connectInputSignal(elementBenes->outPort2);addElementWireFromXml(elePort,elementBenes->outPort2);break;
                                    case 3:elePort->connectInputSignal(elementBenes->outPort3);addElementWireFromXml(elePort,elementBenes->outPort3);break;
                                    default:break;
                                }
                            }
                            break;
                        }
                        case InputPort_InFifo:
                        {
                            ModuleRcu* rcuTemp = findRcuFromScene(bcuIndex, rcuIndex);
                            if((rcuTemp) && (rcuTemp->inFifo0) && (rcuTemp->inFifo1) && (rcuTemp->inFifo2) && (rcuTemp->inFifo3))
                            {
                                switch(inputIndex)
                                {
                                    case 0:elePort->connectInputSignal(rcuTemp->inFifo0);addElementWireFromXml(elePort,rcuTemp->inFifo0);break;
                                    case 1:elePort->connectInputSignal(rcuTemp->inFifo1);addElementWireFromXml(elePort,rcuTemp->inFifo1);break;
                                    case 2:elePort->connectInputSignal(rcuTemp->inFifo2);addElementWireFromXml(elePort,rcuTemp->inFifo2);break;
                                    case 3:elePort->connectInputSignal(rcuTemp->inFifo3);addElementWireFromXml(elePort,rcuTemp->inFifo3);break;
                                    default:break;
                                }
                            }
                            break;
                        }
                        case InputPort_Mem:
                        {
                            ModuleRcu* rcuTemp = findRcuFromScene(bcuIndex,rcuIndex);
                            if(rcuTemp)
                            {
                                elePort->addReadMemFromXml(inputIndex);
                            }
                            break;
                        }
                        case InputPort_Imd:
                        {
                            ModuleRcu* rcuTemp = findRcuFromScene(bcuIndex,rcuIndex);
                            if(rcuTemp)
                            {
                                elePort->addReadImdFromXml(inputIndex);
                            }
                            break;
                        }
                        case InputPort_Rch:
                        {
                            ModuleRcu* rcuTemp = findRcuFromScene(bcuIndex,rcuIndex);
                            if(rcuTemp)
                            {
                                elePort->addReadRchFromXml(inputIndex);
                            }
                            break;
                        }
                        case InputPort_Lfsr:
                        {
                            ModuleRcu* rcuTemp = findRcuFromScene(bcuIndex,rcuIndex);
                            if(rcuTemp)
                            {
                                elePort->addReadLfsrFromXml(inputIndex);
                            }
                            break;
                        }
                        default:break;
                    }
                }
            }
        }
    }
}

void RcaGraphScene::initRcuRectByBcu()
{
    for(int i=0;i<bcuExistList.count();i++)
    {
        ModuleBcu* bcuModule = bcuExistList.at(i);

        bcuModule->setChildrenRcuWidth(bcuModule->getEnabledMoveableRect().left(),bcuModule->getEnabledMoveableRect().width());
    }
}

//bool RcaGraphScene::resetInputPortConnectWire(ElementPort* elePort , InputPortType inputType , int inputIndex)
//{
//    if((elePort) && (elePort->parentItem()) && (elePort->getPortDirection() == InputDirection))
//    {

//        BaseArithUnit* portParentIsArith = dynamic_cast<BaseArithUnit*>(elePort->parentItem());
//        ModuleBcu* portParentIsBcu = dynamic_cast<ModuleBcu*>(elePort->parentItem());
////        InputPortType inputType = elePort->getInputType();
////        int inputIndex = elePort->getInputIndex();
//        int bcuIndex = 0;
//        int rcuIndex = 0;
//        bool isPortValid = false;
//        if(portParentIsBcu)
//        {
//            bcuIndex = portParentIsBcu->getIndexBcu();
//            rcuIndex = OUT_FIFO_SIGNAL_RCU_INDEX;
//            isPortValid = true;
//        }
//        else if(portParentIsArith)
//        {
//            switch(inputType)
//            {
//                case InputPort_NULL:isPortValid = true;break;
//                case InputPort_CurrentBfuX:
//                case InputPort_CurrentBfuY:
//                case InputPort_CurrentSbox:
//                case InputPort_CurrentBenes:
//                {
//                    bcuIndex = portParentIsArith->getIndexBcu();
//                    rcuIndex = portParentIsArith->getIndexRcu();
//                    isPortValid = true;
//                    break;
//                }
//                case InputPort_LastBfuX:
//                case InputPort_LastBfuY:
//                case InputPort_LastSbox:
//                case InputPort_LastBenes:
//                {
//                    //如果行号为0
//                    if(portParentIsArith->getIndexRcu() == 0)
//                    {
//                        ModuleBcu* bcuTemp = findBcuFromScene(portParentIsArith->getIndexBcu());
//                        bcuIndex = bcuTemp->srcDataFromBcu;
//                        rcuIndex = OUT_FIFO_SIGNAL_RCU_INDEX;
//                        isPortValid = true;
//                    }
//                    else
//                    {
//                        bcuIndex = portParentIsArith->getIndexBcu();
//                        rcuIndex = portParentIsArith->getIndexRcu() - 1;
//                        isPortValid = true;
//                    }
//                    break;
//                }
//                case InputPort_InFifo:
//                {
//                    bcuIndex = portParentIsArith->getIndexBcu();
//                    rcuIndex = portParentIsArith->getIndexRcu();
//                    isPortValid = true;
//                    break;
//                }
//                case InputPort_Mem:
//                {
//                    bcuIndex = portParentIsArith->getIndexBcu();
//                    rcuIndex = portParentIsArith->getIndexRcu();
//                    isPortValid = true;
//                    break;
//                }
//                default:break;
//            }


//        }
//        if(isPortValid)
//        {
//            QL_DEBUG << bcuIndex << rcuIndex << inputIndex;
//            switch(inputType)
//            {
//                case InputPort_NULL:
//                {
//                    QL_DEBUG;
//                    elePort->deleteAllConnectWire();
//                    qDeleteAll(elePort->childItems());
//                    elePort->clearInSignalAllAttr();
//                    return true;
//                }
//                case InputPort_CurrentBfuX:
//                case InputPort_LastBfuX:
//                {
//                    ElementBfu *elementBfu = findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
//                    if((elementBfu) && (elementBfu->outPortX))
//                    {
//                        QL_DEBUG;
//                        elePort->deleteAllConnectWire();
//                        qDeleteAll(elePort->childItems());
//                        elePort->clearInSignalAllAttr();
//                        elePort->setInSignalAllAttr(elementBfu->outPortX);
//                        addElementWireFromXml(elePort,elementBfu->outPortX);
//                        return true;
//                    }
//                    break;
//                }
//                case InputPort_CurrentBfuY:
//                case InputPort_LastBfuY:
//                {
//                    ElementBfu *elementBfu = findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
//                    if((elementBfu) && (elementBfu->outPortY))
//                    {
//                        QL_DEBUG;
//                        elePort->deleteAllConnectWire();
//                        qDeleteAll(elePort->childItems());
//                        elePort->clearInSignalAllAttr();
//                        elePort->setInSignalAllAttr(elementBfu->outPortY);
//                        addElementWireFromXml(elePort,elementBfu->outPortY);
//                        return true;
//                    }
//                    break;
//                }
//                case InputPort_CurrentSbox:
//                case InputPort_LastSbox:
//                {
//                    ElementSbox* elementSbox = findSboxFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
//                    if(elementSbox)
//                    {
//                        switch(inputIndex%SBOX_OUTPUTCOUNT)
//                        {
//                            case 0:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementSbox->outPort0);
//                                addElementWireFromXml(elePort,elementSbox->outPort0);
//                                return true;
//                            }
//                            break;
//                            case 1:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementSbox->outPort1);
//                                addElementWireFromXml(elePort,elementSbox->outPort1);
//                                return true;
//                            }
//                            break;
//                            case 2:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementSbox->outPort2);
//                                addElementWireFromXml(elePort,elementSbox->outPort2);
//                                return true;
//                            }
//                            break;
//                            case 3:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementSbox->outPort3);
//                                addElementWireFromXml(elePort,elementSbox->outPort3);
//                                return true;
//                            }
//                            break;
//                            default:break;
//                        }
//                    }
//                    break;
//                }
//                case InputPort_CurrentBenes:
//                case InputPort_LastBenes:
//                {
//                    ElementBenes* elementBenes = findBenesFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
//                    if(elementBenes)
//                    {
//                        switch(inputIndex%BENES_OUTPUTCOUNT)
//                        {
//                            case 0:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementBenes->outPort0);
//                                addElementWireFromXml(elePort,elementBenes->outPort0);
//                                return true;
//                            }
//                            break;
//                            case 1:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementBenes->outPort1);
//                                addElementWireFromXml(elePort,elementBenes->outPort1);
//                                return true;
//                            }
//                            break;
//                            case 2:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementBenes->outPort2);
//                                addElementWireFromXml(elePort,elementBenes->outPort2);
//                                return true;
//                            }
//                            break;
//                            case 3:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(elementBenes->outPort3);
//                                addElementWireFromXml(elePort,elementBenes->outPort3);
//                                return true;
//                            }
//                            break;
//                            default:break;
//                        }
//                    }
//                    break;
//                }
//                case InputPort_InFifo:
//                {
//                    ModuleBcu* bcuTemp = findBcuFromScene(bcuIndex);
//                    if((bcuTemp) && (bcuTemp->inFifo0) && (bcuTemp->inFifo1) && (bcuTemp->inFifo2) && (bcuTemp->inFifo3))
//                    {
//                        switch(inputIndex)
//                        {
//                            case 0:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(bcuTemp->inFifo0);
//                                addElementWireFromXml(elePort,bcuTemp->inFifo0);
//                                return true;
//                            }
//                            break;
//                            case 1:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(bcuTemp->inFifo1);
//                                addElementWireFromXml(elePort,bcuTemp->inFifo1);
//                                return true;
//                            }
//                            break;
//                            case 2:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(bcuTemp->inFifo2);
//                                addElementWireFromXml(elePort,bcuTemp->inFifo2);
//                                return true;
//                            }
//                            break;
//                            case 3:
//                            {
//                                elePort->deleteAllConnectWire();
//                                qDeleteAll(elePort->childItems());
//                                elePort->clearInSignalAllAttr();
//                                elePort->setInSignalAllAttr(bcuTemp->inFifo3);
//                                addElementWireFromXml(elePort,bcuTemp->inFifo3);
//                                return true;
//                            }
//                            break;
//                            default:break;
//                        }
//                    }
//                    break;
//                }
//                case InputPort_Mem:
//                {
//                    ModuleRcu* rcuTemp = findRcuFromScene(bcuIndex,rcuIndex);
//                    if(rcuTemp)
//                    {
//                        elePort->deleteAllConnectWire();
//                        qDeleteAll(elePort->childItems());
//                        elePort->clearInSignalAllAttr();
//                        elePort->addReadMemFromXml(inputIndex);
//                        return true;
//                    }
//                    break;
//                }
//                default:break;
//            }
//        }
//    }
//    return false;
//}

void RcaGraphScene::handleDeleteWire(ElementWire* deleteWire)
{
    QL_DEBUG;
    allWireList.removeOne(deleteWire);
    emit deleteWireSuccess(deleteWire);
}

void RcaGraphScene::autoAddBcu()
{
    isAutoSetItemPos = true;
    undoStack->beginMacro("auto make all item");
    for(int bcuIndex = 0;bcuIndex < bcuMaxCount; bcuIndex++)
    {
        autoMakeNewBcu(bcuIndex);
    }
    undoStack->endMacro();
    resetSelectedAttr();
    isAutoSetItemPos = false;
}

void RcaGraphScene::autoAddNewRcuCfg()
{
    isAutoSetItemPos = true;
    undoStack->beginMacro("auto make all item");
    for(int bcuIndex = 0;bcuIndex < bcuMaxCount; bcuIndex++)
    {
        autoMakeNewBcu(bcuIndex);
        addNewRcuCfg();
    }
    undoStack->endMacro();
    resetSelectedAttr();
    isAutoSetItemPos = false;
}

void RcaGraphScene::addNewRcuCfg(int rcuCfgID)
{
    auto* bcu = findBcuFromScene(0);
    if(bcu)
    {
        if(!findRcuFromScene(0,rcuCfgID))
            bcu->autoMakeNewRcu(rcuCfgID);
    }
}

void RcaGraphScene::addNewRcuCfg()
{
    auto* bcu = findBcuFromScene(0);
    if(bcu)
    {
        int iMaxIndex = 0;bool bFindRcu = false;
        QList<QGraphicsItem *> listItems = bcu->childItems();
        QList<QGraphicsItem*>::iterator iter;
        for(iter=listItems.begin();iter!=listItems.end();iter++)
        {
            if((*iter))
            {
                ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(*iter);
                if(moduleRcu)
                {
                    bFindRcu = true;
                    if(iMaxIndex < moduleRcu->getIndexRcu())
                    {
                        iMaxIndex = moduleRcu->getIndexRcu();
                    }
                }
            }
        }
        if(bFindRcu)
            addNewRcuCfg(++iMaxIndex);
        else
            addNewRcuCfg(iMaxIndex);
    }
}

void RcaGraphScene::autoSetPosOfChildBcu(EBcuComposing bcuCom)
{
    getCurPropertyBrowser()->setEnable(false);
    isAutoSetItemPos = true;
    QPointF bcuPos = BCU_POS_AT_SCENE[0];

    undoStack->beginMacro("atuo set pos of all item");
    for(int index=0;index<bcuExistList.size();index++)
    {
        ModuleBcu* moduleBcu = bcuExistList.at(index);
        moduleBcu->pushStackSetPos(bcuPos);
        moduleBcu->autoSetPosOfChildRcu();
        switch(bcuCom)
        {
        case BcuVCom : bcuPos.setY(bcuPos.y() + (BCU_AT_SCENE_YSPACE + moduleBcu->getRealBoundingRect().height()));break;
        case BcuVCom2 :
        {
            if(index%2 == 0)
            {
                bcuPos.setX(bcuPos.x() + (BCU_AT_SCENE_XSPACE + moduleBcu->getRealBoundingRect().width()));
            }
            else
            {
                bcuPos.setX(BCU_POS_AT_SCENE[0].x());
        bcuPos.setY(bcuPos.y() + (BCU_AT_SCENE_YSPACE + moduleBcu->getRealBoundingRect().height()));
            }
        }break;
        case BcuHCom : bcuPos.setX(bcuPos.x() + (BCU_AT_SCENE_XSPACE + moduleBcu->getRealBoundingRect().width()));break;
        default:break;
        }
    }
    undoStack->endMacro();
    getCurPropertyBrowser()->setEnable(true);
    resetSelectedAttr();
    isAutoSetItemPos = false;
}
void RcaGraphScene::clearAllWire()
{
    for(int i=0;i<allWireList.count();i++)
    {
        allWireList.at(i)->pressedPosList.clear();
        allWireList.at(i)->infletionPosList.clear();
    }
}

void RcaGraphScene::showAllWire(bool isShow)
{
    if(isShow)
    {
        showAllWire();
    }
    else
    {
        hideAllWire();
    }
}

QString RcaGraphScene::setBaseItemAttr(BaseItem *baseItem)
{
    ModuleBcu *moduleBcu = dynamic_cast<ModuleBcu*>(baseItem);
    if(moduleBcu) return setBcuItemAttr(moduleBcu);

    ModuleRcu *moduleRcu = dynamic_cast<ModuleRcu*>(baseItem);
    if(moduleRcu) return setRcuItemAttr(moduleRcu);

    ElementBfu *elementBfu = dynamic_cast<ElementBfu*>(baseItem);
    if(elementBfu) return setBfuItemAttr(elementBfu);

    ElementCustomPE *elementCustomPE = dynamic_cast<ElementCustomPE*>(baseItem);
    if(elementCustomPE) return setCustomPEItemAttr(elementCustomPE);

    ElementSbox *elementSbox = dynamic_cast<ElementSbox*>(baseItem);
    if(elementSbox) return setSboxItemAttr(elementSbox);

    ElementBenes *elementBenes = dynamic_cast<ElementBenes*>(baseItem);
    if(elementBenes) return setBenesItemAttr(elementBenes);

    ModuleReadMem *readMem = dynamic_cast<ModuleReadMem*>(baseItem);
    if(readMem) return setReadMemItemAttr(readMem);

    ModuleWriteMem *writeMem = dynamic_cast<ModuleWriteMem*>(baseItem);
    if(writeMem) return setWriteMemItemAttr(writeMem);

    ModuleReadImd *readImd = dynamic_cast<ModuleReadImd*>(baseItem);
    if(readImd) return setReadImdItemAttr(readImd);

    ModuleReadRch *readRch = dynamic_cast<ModuleReadRch*>(baseItem);
    if(readRch) return setReadRchItemAttr(readRch);

    ModuleWriteRch *writeRch = dynamic_cast<ModuleWriteRch*>(baseItem);
    if(writeRch) return setWriteRchItemAttr(writeRch);

    ModuleReadLfsr *readLfsr = dynamic_cast<ModuleReadLfsr*>(baseItem);
    if(readLfsr) return setReadLfsrItemAttr(readLfsr);

    ModuleWriteLfsr *writeLfsr = dynamic_cast<ModuleWriteLfsr*>(baseItem);
    if(writeLfsr) return setWriteLfsrItemAttr(writeLfsr);

    return QString(tr(u8"没有对应元素"));
}

QString RcaGraphScene::setBcuItemAttr(ModuleBcu *moduleBcu)
{
    ModuleBcu* bcuBak = dynamic_cast<ModuleBcu*>(doubleClickedItemBak);
    if(bcuBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(moduleBcu == NULL) return QString(tr(u8"Item无效"));
    int indexBcu = moduleBcu->getIndexBcu();
    if(bcuBak->getIndexBcu() != indexBcu)
    {
        if(indexBcu >= BCU_MAX_COUNT) return QString(tr(u8"BCU index超过最大值"));
        if(findBcuFromScene(indexBcu) != NULL) return QString(tr(u8"已存在此BCU"));
    }

//    const int portCount = 4;
//    ElementPort* inSignal[portCount];
//    for(int i=0;i<portCount;i++)
//    {
//        inSignal[i] = NULL;
//    }
//    InputPortType uiInputType[portCount];
//    int uiInputIndex[portCount];
//    bool isSetFifo[portCount] = {false};
//    ElementPort* outFifo[portCount] = {moduleBcu->outFifo0,
//                                       moduleBcu->outFifo1,
//                                       moduleBcu->outFifo2,
//                                       moduleBcu->outFifo3};
//    ElementPort* bakOutFifo[portCount] = {bcuBak->outFifo0,
//                                          bcuBak->outFifo1,
//                                          bcuBak->outFifo2,
//                                          bcuBak->outFifo3};
//    for(int i=0;i<portCount;i++)
//    {
//        uiInputType[i] = outFifo[i]->getInputType();
//        uiInputIndex[i] = outFifo[i]->getInputIndex();
//        if((uiInputType[i] != bakOutFifo[i]->getInputType())
//                || uiInputIndex[i] != bakOutFifo[i]->getInputIndex())
//        {
//            if(!bakOutFifo[i]->canConnectOutPort(uiInputType[i],uiInputIndex[i],&inSignal[i])) return QString(tr(u8"outFifo%1设置错误")).arg(i);
//            isSetFifo[i] = true;
//        }
//    }

    bool hasBeginPushStack = false;

    if(bcuBak->getIndexBcu() != indexBcu)
    {
        int ret = QMessageBox::information(CAppEnv::m_mainWgt, tr(u8"BCU设置id"),
                                       tr(u8"如果设置为新的id,\n则不合法的连接线将自动删除,\n是否将BCU设置为新的id?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
        {
            if(!hasBeginPushStack)
            {
                undoStack->beginMacro("set bcu");
                hasBeginPushStack = true;
            }
            SetBcuIndexCommand* cmdSetBcuId = new SetBcuIndexCommand(this,bcuBak,indexBcu);
            pushCmdSetBcuIndex(cmdSetBcuId);

//            setBcuIndex(bcuBak,indexBcu);
        }
        else if (ret == QMessageBox::Cancel)
        {
            return QString(tr(u8"请重新设置BCU"));
        }
        else if (ret == QMessageBox::No)
        {
            return QString(tr(u8"请重新设置BCU"));
        }
    }

//    ModuleBcuIndex uiBcuSrc = moduleBcu->srcDataFromBcu;
    if(bcuBak->srcDataFromBcu != moduleBcu->srcDataFromBcu ||
            bcuBak->sort != moduleBcu->sort ||
            bcuBak->burst != moduleBcu->burst ||
            bcuBak->gap != moduleBcu->gap ||
            bcuBak->infifoSrc != moduleBcu->infifoSrc ||
            bcuBak->outfifoSrc != moduleBcu->outfifoSrc ||
            bcuBak->lfsrGroup != moduleBcu->lfsrGroup ||
            bcuBak->readImdMode128_32 != moduleBcu->readImdMode128_32 ||
            bcuBak->imdSrcConfig != moduleBcu->imdSrcConfig ||
            bcuBak->memorySrcConfig != moduleBcu->memorySrcConfig ||
            bcuBak->inFifoSrcConfig != moduleBcu->inFifoSrcConfig ||
            bcuBak->writeDepth != moduleBcu->writeDepth ||
            bcuBak->writeWidth != moduleBcu->writeWidth ||
            bcuBak->addressMode != moduleBcu->addressMode)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set bcu");
            hasBeginPushStack = true;
        }

        SetBcuAttrCommand* cmdSetBcuAttr = new SetBcuAttrCommand(this, bcuBak, moduleBcu);
        pushCmdSetBcuAttr(cmdSetBcuAttr);
    }

//    for(int i=0;i<portCount;i++)
//    {
//        if(isSetFifo[i])
//        {
//            if(!hasBeginPushStack)
//            {
//                undoStack->beginMacro("set bcu");
//                hasBeginPushStack = true;
//            }
//            bakOutFifo[i]->resetConnectPort(this,uiInputType[i],uiInputIndex[i],inSignal[i]);
//        }
//    }
//    bcuBak->update();
    emit sendWindowToModified(true);

    if(hasBeginPushStack)
    {
        undoStack->endMacro();
    }
    resetSelectedAttr();
    return QString(tr(u8""));
}

QString RcaGraphScene::setRcuItemAttr(ModuleRcu *moduleRcu)
{
    ModuleRcu* rcuBak = dynamic_cast<ModuleRcu*>(doubleClickedItemBak);
    if(rcuBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(moduleRcu == NULL) return QString(tr(u8"Item无效"));

    bool hasBeginPushStack = false;
    int indexRcu = moduleRcu->getIndexRcu();
    if(rcuBak->getIndexRcu() != indexRcu)
    {
        if(indexRcu >= RCU_MAX_COUNT) return QString(tr(u8"RCU index超过最大值"));
        if(findRcuFromScene(moduleRcu->getIndexBcu(),indexRcu) != NULL) return QString(tr(u8"已存在此RCU"));

        int ret = QMessageBox::information(CAppEnv::m_mainWgt, tr(u8"RCU设置id"),
                                       tr(u8"如果设置为新的id,\n则不合法的连接线将自动删除,\n是否将RCU设置为新的id?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
        {
            if(!hasBeginPushStack)
            {
                undoStack->beginMacro("set rcu");
                hasBeginPushStack = true;
            }
            SetRcuIndexCommand* cmdSetRcuId = new SetRcuIndexCommand(this,rcuBak,indexRcu);
            pushCmdSetRcuIndex(cmdSetRcuId);
        }
        else if (ret == QMessageBox::Cancel)
        {
            return QString(tr(u8"请重新设置RCU"));
        }
        else if (ret == QMessageBox::No)
        {
            return QString(tr(u8"请重新设置RCU"));
        }
    }

    const int portCount = 4;
    ElementPort* inSignal[portCount];
    for(int i=0;i<portCount;i++)
    {
        inSignal[i] = NULL;
    }
    InputPortType uiInputType[portCount];
    int uiInputIndex[portCount];
    bool isSetFifo[portCount] = {false};
    ElementPort* outFifo[portCount] = {moduleRcu->outFifo0,
                                       moduleRcu->outFifo1,
                                       moduleRcu->outFifo2,
                                       moduleRcu->outFifo3};
    ElementPort* bakOutFifo[portCount] = {rcuBak->outFifo0,
                                          rcuBak->outFifo1,
                                          rcuBak->outFifo2,
                                          rcuBak->outFifo3};
    for(int i=0;i<portCount;i++)
    {
        uiInputType[i] = outFifo[i]->getInputType();
        uiInputIndex[i] = outFifo[i]->getInputIndex();
        if((uiInputType[i] != bakOutFifo[i]->getInputType())
            || uiInputIndex[i] != bakOutFifo[i]->getInputIndex())
        {
            if(!bakOutFifo[i]->canConnectOutPort(uiInputType[i],uiInputIndex[i],&inSignal[i])) return QString(tr(u8"outFifo%1设置错误")).arg(i);
            isSetFifo[i] = true;
        }
    }

    if(     rcuBak->loopStartEndFlag != moduleRcu->loopStartEndFlag ||
            rcuBak->loopTimes != moduleRcu->loopTimes ||
            rcuBak->loopStartEndFlag2 != moduleRcu->loopStartEndFlag2 ||
            rcuBak->loopTimes2 != moduleRcu->loopTimes2 ||

            rcuBak->readMemMode128_32 != moduleRcu->readMemMode128_32 ||
            rcuBak->readMemMode != moduleRcu->readMemMode ||
            rcuBak->readMemAddr1 != moduleRcu->readMemAddr1 ||
            rcuBak->readMemAddr2 != moduleRcu->readMemAddr2 ||
            rcuBak->readMemAddr3 != moduleRcu->readMemAddr3 ||
            rcuBak->readMemAddr4 != moduleRcu->readMemAddr4 ||
            rcuBak->readMemOffset != moduleRcu->readMemOffset ||
            rcuBak->readMemThreashold != moduleRcu->readMemThreashold ||

            rcuBak->writeMemMode != moduleRcu->writeMemMode ||
            rcuBak->writeMemAddr != moduleRcu->writeMemAddr ||
            rcuBak->writeMemMask != moduleRcu->writeMemMask ||
            rcuBak->writeMemOffset != moduleRcu->writeMemOffset ||
            rcuBak->writeMemThreashold != moduleRcu->writeMemThreashold ||

            rcuBak->rchMode != moduleRcu->rchMode

            )
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set rcu");
            hasBeginPushStack = true;
        }
        SetRcuAttrCommand* cmdSetRcuAttr = new SetRcuAttrCommand(this,rcuBak,moduleRcu);
        pushCmdSetRcuAttr(cmdSetRcuAttr);
    }


//    for(int i=0;i<portCount;i++)
//    {
//        if(isSetFifo[i])
//        {
//            if(!hasBeginPushStack)
//            {
//                undoStack->beginMacro("set bcu");
//                hasBeginPushStack = true;
//            }
//            bakOutFifo[i]->resetConnectPort(this,uiInputType[i],uiInputIndex[i],inSignal[i]);
//        }
//    }//NOTE 连线规则检查缺失看这里
    rcuBak->update();
    emit sendWindowToModified(true);

    if(hasBeginPushStack)
    {
        undoStack->endMacro();
    }
    resetSelectedAttr();
    return QString(tr(u8""));
}

QString RcaGraphScene::setBfuItemAttr(ElementBfu *elementBfu)
{
    ElementBfu* bfuBak = dynamic_cast<ElementBfu*>(doubleClickedItemBak);
    if(bfuBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(elementBfu == NULL) return QString(tr(u8"Item无效"));

    int indexInRcu = elementBfu->getIndexInRcu();
    if(bfuBak->getIndexInRcu() != indexInRcu)
    {
        if(indexInRcu >= BFU_MAX_COUNT) return QString(tr(u8"BFU index超过最大值"));
        if(findBfuFromScene(elementBfu->getIndexBcu(),elementBfu->getIndexRcu(),indexInRcu) != NULL) return QString(tr(u8"已存在此BFU"));
    }

    const int portCount = 3;
    ElementPort* inSignal[portCount];
    for(int i=0;i<portCount;i++)
    {
        inSignal[i] = NULL;
    }
    InputPortType uiInputType[portCount];
    int uiInputIndex[portCount];
    bool isSetPort[portCount] = {false};
    ElementPort* outPort[portCount] = {elementBfu->inPortA,
                                       elementBfu->inPortB,
                                       elementBfu->inPortT};
    ElementPort* bakOutPort[portCount] = {bfuBak->inPortA,
                                          bfuBak->inPortB,
                                          bfuBak->inPortT};
    for(int i=0;i<portCount;i++)
    {
        uiInputType[i] = outPort[i]->getInputType();
        uiInputIndex[i] = outPort[i]->getInputIndex();
        if((uiInputType[i] != bakOutPort[i]->getInputType())
                || uiInputIndex[i] != bakOutPort[i]->getInputIndex())
        {
            if(!bakOutPort[i]->canConnectOutPort(uiInputType[i],uiInputIndex[i],&inSignal[i]))
            {
                QString name;
                switch(i)
                {
                case 0:name=tr(u8"A");break;
                case 1:name=tr(u8"B");break;
                case 2:name=tr(u8"T");break;
                default:break;
                }
                return QString(tr(u8"port%1设置错误")).arg(name);
            }
            isSetPort[i] = true;
        }
    }

    bool hasBeginPushStack = false;

    if(bfuBak->getIndexInRcu() != indexInRcu)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set bfu");
            hasBeginPushStack = true;
        }
        SetArithIndexCommand* cmdSetArithId = new SetArithIndexCommand(this,bfuBak,indexInRcu);
        pushCmdSetArithIndex(cmdSetArithId);
    }

    for(int i=0;i<portCount;i++)
    {
        if(isSetPort[i])
        {
            if(!hasBeginPushStack)
            {
                undoStack->beginMacro("set bfu");
                hasBeginPushStack = true;
            }
            bakOutPort[i]->resetConnectPort(this,uiInputType[i],uiInputIndex[i],inSignal[i]);
        }
    }

    if(bfuBak->funcIndex != elementBfu->funcIndex ||
            bfuBak->funcExp != elementBfu->funcExp ||
            bfuBak->bypassIndex != elementBfu->bypassIndex ||
            bfuBak->funcAuModIndex != elementBfu->funcAuModIndex ||
            bfuBak->funcAuCarry != elementBfu->funcAuCarry ||
            bfuBak->funcMuModIndex != elementBfu->funcMuModIndex ||
            bfuBak->bypassExp != elementBfu->bypassExp)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set bfu");
            hasBeginPushStack = true;
        }
        SetBfuAttrCommand* cmdSetBfuAttr = new SetBfuAttrCommand(this,bfuBak,elementBfu);
        pushCmdSetBfuAttr(cmdSetBfuAttr);
    }

//    bfuBak->funcIndex = elementBfu->funcIndex;
//    bfuBak->funcExp = elementBfu->funcExp;
//    bfuBak->bypassIndex = elementBfu->bypassIndex;

//    bfuBak->funcAuModIndex = elementBfu->funcAuModIndex;

//    bfuBak->bypassExp = elementBfu->bypassExp;

//    bfuBak->update();

    emit sendWindowToModified(true); //zhangjun modified at 20210102

    if(hasBeginPushStack)
    {
        undoStack->endMacro();
    }
    resetSelectedAttr(); //zhangjun modified at 20210102

    return QString(tr(u8""));
}

QString RcaGraphScene::setCustomPEItemAttr(ElementCustomPE *elementCustomPE)
{
    ElementCustomPE* customPEBak = dynamic_cast<ElementCustomPE*>(doubleClickedItemBak);
    if(customPEBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(elementCustomPE == NULL) return QString(tr(u8"Item无效"));

    int indexInRcu = elementCustomPE->getIndexInRcu();
    if(customPEBak->getIndexInRcu() != indexInRcu)
    {
        if(indexInRcu >= BFU_MAX_COUNT) return QString(tr(u8"CustomPE index超过最大值"));
        if(findBfuFromScene(elementCustomPE->getIndexBcu(),elementCustomPE->getIndexRcu(),indexInRcu) != NULL) return QString(tr(u8"已存在此BFU"));
    }


//    const int portCount = elementCustomPE->inputNum();
//    ElementPort* inSignal[portCount];
//    for(int i=0;i<portCount;i++)
//    {
//        inSignal[i] = NULL;
//    }
//    InputPortType uiInputType[portCount];
//    int uiInputIndex[portCount];
//    bool isSetPort[portCount] = {false};
//    ElementPort* outPort[portCount];
//    for(int i = 0;i<portCount;i++)
//    {
//        outPort[i] = elementCustomPE->m_inputPortVector[i];
//    }

//    ElementPort* bakOutPort[portCount];
//    for(int i = 0;i<portCount;i++)
//    {
//        bakOutPort[i] = customPEBak->m_inputPortVector[i];
//    }

//    for(int i=0;i<portCount;i++)
//    {
//        uiInputType[i] = outPort[i]->getInputType();
//        uiInputIndex[i] = outPort[i]->getInputIndex();
//        if((uiInputType[i] != bakOutPort[i]->getInputType())
//                || uiInputIndex[i] != bakOutPort[i]->getInputIndex())
//        {
//            if(!bakOutPort[i]->canConnectOutPort(uiInputType[i],uiInputIndex[i],&inSignal[i]))
//            {
//                QString name;
//                switch(i)
//                {
//                case 0:name=tr(u8"A");break;
//                case 1:name=tr(u8"B");break;
//                case 2:name=tr(u8"T");break;
//                default:break;
//                }
//                return QString(tr(u8"port%1设置错误")).arg(name);
//            }
//            isSetPort[i] = true;
//        }
//    }

    bool hasBeginPushStack = false;

    if(customPEBak->getIndexInRcu() != indexInRcu)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set customPE");
            hasBeginPushStack = true;
        }
        SetArithIndexCommand* cmdSetArithId = new SetArithIndexCommand(this,customPEBak,indexInRcu);
        pushCmdSetArithIndex(cmdSetArithId);
    }

//    for(int i=0;i<portCount;i++)
//    {
//        if(isSetPort[i])
//        {
//            if(!hasBeginPushStack)
//            {
//                undoStack->beginMacro("set customPE");
//                hasBeginPushStack = true;
//            }
//            bakOutPort[i]->resetConnectPort(this,uiInputType[i],uiInputIndex[i],inSignal[i]);
//        }
//    }

    if(customPEBak->funcIndex != elementCustomPE->funcIndex ||
            customPEBak->inputNum() != elementCustomPE->inputNum() ||
            customPEBak->outputNum() != elementCustomPE->outputNum() ||
            customPEBak->funcExp != elementCustomPE->funcExp ||
            customPEBak->bypassIndex != elementCustomPE->bypassIndex ||
            customPEBak->funcAuModIndex != elementCustomPE->funcAuModIndex ||
//            customPEBak->funcAuCarry != elementCustomPE->funcAuCarry ||
//            customPEBak->funcMuModIndex != elementCustomPE->funcMuModIndex ||
            customPEBak->bypassExp != elementCustomPE->bypassExp)
    {
        bool bOk = false;
        if(customPEBak->inputNum() != elementCustomPE->inputNum() ||
                customPEBak->outputNum() != elementCustomPE->outputNum())
        {
            bOk = isOkChangePortNum();
        }
        if(bOk)
        {
            if(!hasBeginPushStack)
            {
                undoStack->beginMacro("set customPE");
                hasBeginPushStack = true;
            }
            SetCustomPEAttrCommand* cmdSetCustomPEAttr = new SetCustomPEAttrCommand(this,customPEBak,elementCustomPE);
            pushCmdSetCustomPEAttr(cmdSetCustomPEAttr);
        }
    }

    emit sendWindowToModified(true); //zhangjun modified at 20210102

    if(hasBeginPushStack)
    {
        undoStack->endMacro();
    }
    resetSelectedAttr(); //zhangjun modified at 20210102

    return QString(tr(u8""));
}


QString RcaGraphScene::setSboxItemAttr(ElementSbox *elementSbox)
{
    ElementSbox* sboxBak = dynamic_cast<ElementSbox*>(doubleClickedItemBak);
    if(sboxBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(elementSbox == NULL) return QString(tr(u8"Item无效"));

    int indexInRcu = elementSbox->getIndexInRcu();
    if(sboxBak->getIndexInRcu() != indexInRcu)
    {
        if(indexInRcu >= SBOX_MAX_COUNT) return QString(tr(u8"SBOX index超过最大值"));
        if(findSboxFromScene(elementSbox->getIndexBcu(),elementSbox->getIndexRcu(),indexInRcu) != NULL) return QString(tr(u8"已存在此SBOX"));
    }

    ElementPort* inSignal0;
    bool isSetPort0 = false;
    ElementPort* bakOutPort0 = sboxBak->inPort0;
    InputPortType input0Type = elementSbox->inPort0->getInputType();
    int input0Index = elementSbox->inPort0->getInputIndex();
    if((input0Type != bakOutPort0->getInputType())
            || input0Index != bakOutPort0->getInputIndex())
    {
        //重新找到port口进行连接，如果没有则提示。
        if(!bakOutPort0->canConnectOutPort(input0Type,input0Index,&inSignal0)) return QString(tr(u8"port0设置错误"));
        isSetPort0 = true;
    }

    bool hasBeginPushStack = false;
    if(sboxBak->getIndexInRcu() != indexInRcu)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set sbox");
            hasBeginPushStack = true;
        }
        SetArithIndexCommand* cmdSetArithId = new SetArithIndexCommand(this,sboxBak,indexInRcu);
        pushCmdSetArithIndex(cmdSetArithId);
    }
    if(isSetPort0)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set sbox");
            hasBeginPushStack = true;
        }
        bakOutPort0->resetConnectPort(this,input0Type,input0Index,inSignal0);
    }

    if(     sboxBak->groupCnt0 != elementSbox->groupCnt0 ||
            sboxBak->groupCnt1 != elementSbox->groupCnt1 ||
            sboxBak->groupCnt2 != elementSbox->groupCnt2 ||
            sboxBak->groupCnt3 != elementSbox->groupCnt3 ||
            sboxBak->sboxGroup != elementSbox->sboxGroup ||
            sboxBak->sboxMode != elementSbox->sboxMode ||
            sboxBak->sboxByteSel != elementSbox->sboxByteSel ||
            sboxBak->sboxIncreaseMode != elementSbox->sboxIncreaseMode ||
            sboxBak->srcConfig != elementSbox->srcConfig)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set sbox");
            hasBeginPushStack = true;
        }
        SetSboxAttrCommand* cmdSetSboxAttr = new SetSboxAttrCommand(this,sboxBak,elementSbox);
        pushCmdSetSboxAttr(cmdSetSboxAttr);
    }

//    sboxBak->groupCnt0 = elementSbox->groupCnt0;
//    sboxBak->groupCnt1 = elementSbox->groupCnt1;
//    sboxBak->groupCnt2 = elementSbox->groupCnt2;
//    sboxBak->groupCnt3 = elementSbox->groupCnt3;
//    sboxBak->sboxMode = elementSbox->sboxMode;
//    sboxBak->srcConfig = elementSbox->srcConfig;
//    sboxBak->update();

    emit sendWindowToModified(true);

    if(hasBeginPushStack)
    {
        undoStack->endMacro();
    }
    resetSelectedAttr();
    return QString(tr(u8""));
}

QString RcaGraphScene::setBenesItemAttr(ElementBenes *elementBenes)
{
    ElementBenes* benesBak = dynamic_cast<ElementBenes*>(doubleClickedItemBak);
    if(benesBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(elementBenes == NULL) return QString(tr(u8"Item无效"));
    int indexInRcu = elementBenes->getIndexInRcu();
    if(benesBak->getIndexInRcu() != indexInRcu)
    {
        if(indexInRcu >= BENES_MAX_COUNT) return QString(tr(u8"BENES index超过最大值"));
        if(findBenesFromScene(elementBenes->getIndexBcu(),elementBenes->getIndexRcu(),indexInRcu) != NULL) return QString(tr(u8"已存在此BENES"));
    }

    const int portCount = 4;
    ElementPort* inSignal[portCount];
    for(int i=0;i<portCount;i++)
    {
        inSignal[i] = NULL;
    }
    InputPortType uiInputType[portCount];
    int uiInputIndex[portCount];
    bool isSetPort[portCount] = {false};
    ElementPort* outPort[portCount] = {elementBenes->inPort0,
                                       elementBenes->inPort1,
                                       elementBenes->inPort2,
                                       elementBenes->inPort3};
    ElementPort* bakOutPort[portCount] = {benesBak->inPort0,
                                          benesBak->inPort1,
                                          benesBak->inPort2,
                                          benesBak->inPort3};
    for(int i=0;i<portCount;i++)
    {
        uiInputType[i] = outPort[i]->getInputType();
        uiInputIndex[i] = outPort[i]->getInputIndex();
        if((uiInputType[i] != bakOutPort[i]->getInputType())
                || uiInputIndex[i] != bakOutPort[i]->getInputIndex())
        {
            if(!bakOutPort[i]->canConnectOutPort(uiInputType[i],uiInputIndex[i],&inSignal[i])) return QString(tr(u8"port%1设置错误")).arg(i);

            isSetPort[i] = true;
        }
    }

    bool hasBeginPushStack = false;
    if(benesBak->getIndexInRcu() != indexInRcu)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set benes");
            hasBeginPushStack = true;
        }
        SetArithIndexCommand* cmdSetArithId = new SetArithIndexCommand(this,benesBak,indexInRcu);
        pushCmdSetArithIndex(cmdSetArithId);
    }

    for(int i=0;i<portCount;i++)
    {
        if(isSetPort[i])
        {
            if(!hasBeginPushStack)
            {
                undoStack->beginMacro("set benes");
                hasBeginPushStack = true;
            }
            bakOutPort[i]->resetConnectPort(this,uiInputType[i],uiInputIndex[i],inSignal[i]);
        }
    }

    if(benesBak->srcConfig != elementBenes->srcConfig)
    {
        if(!hasBeginPushStack)
        {
            undoStack->beginMacro("set benes");
            hasBeginPushStack = true;
        }
        SetBenesAttrCommand* cmdSetBenesAttr = new SetBenesAttrCommand(this,benesBak,elementBenes);
        pushCmdSetBenesAttr(cmdSetBenesAttr);
    }

    emit sendWindowToModified(true);

    if(hasBeginPushStack)
    {
        undoStack->endMacro();
    }
    resetSelectedAttr();
    return QString(tr(u8""));
}

QString RcaGraphScene::setReadMemItemAttr(ModuleReadMem *readMem)
{
    ModuleReadMem* readMemBak = dynamic_cast<ModuleReadMem*>(doubleClickedItemBak);
    if(readMemBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(readMem == NULL) return QString(tr(u8"Item无效"));

    int newIndex = readMem->getReadMemIndex();
    if(readMemBak->canSetReadMemIndex(newIndex))
    {
        if(newIndex != readMemBak->getReadMemIndex())
        {
            SetRMemIndexCommand* cmdSetRMemId = new SetRMemIndexCommand(this,readMemBak,newIndex);
            pushCmdSetRMemIndex(cmdSetRMemId);
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return QString(tr(u8"修改失败"));
}

QString RcaGraphScene::setWriteMemItemAttr(ModuleWriteMem *writeMem)
{
    ModuleWriteMem* writeMemBak = dynamic_cast<ModuleWriteMem*>(doubleClickedItemBak);
    if(writeMemBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(writeMem == NULL) return QString(tr(u8"Item无效"));


    int newIndex = writeMem->getWriteMemIndex();
    QString result = writeMemBak->canSetWriteMemIndex(newIndex);
    if(result.isEmpty())
    {
        if(newIndex != writeMemBak->getWriteMemIndex())
        {
            SetWMemIndexCommand* cmdSetWMemId = new SetWMemIndexCommand(this,writeMemBak,newIndex);
            pushCmdSetWMemIndex(cmdSetWMemId);
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return result;
}

QString RcaGraphScene::setReadImdItemAttr(ModuleReadImd *readImd)
{
    ModuleReadImd* readImdBak = dynamic_cast<ModuleReadImd*>(doubleClickedItemBak);
    if(readImdBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(readImd == NULL) return QString(tr(u8"Item无效"));

    int newIndex = readImd->getReadImdIndex();
    if(readImdBak->canSetReadImdIndex(newIndex))
    {
        if(newIndex != readImdBak->getReadImdIndex())
        {
            SetRImdAttrCommand* cmdSetRImdAttr = new SetRImdAttrCommand(this,readImdBak,readImd);
            pushCmdSetRImdAttr(cmdSetRImdAttr);
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return QString(tr(u8"修改失败"));
}

QString RcaGraphScene::setReadRchItemAttr(ModuleReadRch *readRch)
{
    ModuleReadRch* readRchBak = dynamic_cast<ModuleReadRch*>(doubleClickedItemBak);
    if(readRchBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(readRch == NULL) return QString(tr(u8"Item无效"));

    int newIndex = readRch->getReadRchIndex();
    if(readRchBak->canSetReadRchIndex(newIndex))
    {
        if(newIndex != readRchBak->getReadRchIndex())
        {
            if(newIndex/READ_RCH_MAX_COUNT_ONE_GROUP !=
                    readRchBak->getReadRchIndex()/READ_RCH_MAX_COUNT_ONE_GROUP)
            {
                int ret = QMessageBox::information(CAppEnv::m_mainWgt, tr(u8"RCH设置id"),
                                               tr(u8"如果设置为新的id,\n则同一行RCU内不合法的Read RCH将自动删除,\n是否将RCH设置为新的id?"),
                                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                if (ret == QMessageBox::Cancel)
                {
                    return QString(tr(u8"请重新设置RCH"));
                }
                else if (ret == QMessageBox::No)
                {
                    return QString(tr(u8"请重新设置RCH"));
                }
            }
            undoStack->beginMacro("set read rch index");
            SetRRchAttrCommand* cmdSetRRchAttr = new SetRRchAttrCommand(this,readRchBak,readRch);
            pushCmdSetRRchAttr(cmdSetRRchAttr);
            undoStack->endMacro();
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return QString(tr(u8"修改失败"));
}

QString RcaGraphScene::setWriteRchItemAttr(ModuleWriteRch *writeRch)
{
    ModuleWriteRch* writeRchBak = dynamic_cast<ModuleWriteRch*>(doubleClickedItemBak);
    if(writeRchBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(writeRch == NULL) return QString(tr(u8"Item无效"));

    int newIndex = writeRch->getWriteRchIndex();
    int newAddress = writeRch->getWriteAddress();
    QString result = writeRchBak->canSetWriteRchIndex(newIndex);
    if(result.isEmpty())
    {
        if(newIndex != writeRchBak->getWriteRchIndex() ||
           newAddress != writeRchBak->getWriteAddress())
        {
            SetWRchAttrCommand* cmdSetWRchAttr = new SetWRchAttrCommand(this,writeRchBak,newIndex,newAddress);
            pushCmdSetWRchAttr(cmdSetWRchAttr);
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return result;
}

QString RcaGraphScene::setReadLfsrItemAttr(ModuleReadLfsr *readLfsr)
{
    ModuleReadLfsr* readLfsrBak = dynamic_cast<ModuleReadLfsr*>(doubleClickedItemBak);
    if(readLfsrBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(readLfsr == NULL) return QString(tr(u8"Item无效"));

    int newIndex = readLfsr->getReadLfsrIndex();
    if(readLfsrBak->canSetReadLfsrIndex(newIndex))
    {
        if(newIndex != readLfsrBak->getReadLfsrIndex())
        {
            if(newIndex/READ_LFSR_MAX_COUNT !=
                    readLfsrBak->getReadLfsrIndex()/READ_LFSR_MAX_COUNT)
            {
                int ret = QMessageBox::information(CAppEnv::m_mainWgt, tr(u8"LFSR设置id"),
                                               tr(u8"如果设置为新的id,\n则同一行RCU内不合法的Read LFSR将自动删除,\n是否将LFSR设置为新的id?"),
                                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                if (ret == QMessageBox::Cancel)
                {
                    return QString(tr(u8"请重新设置LFSR"));
                }
                else if (ret == QMessageBox::No)
                {
                    return QString(tr(u8"请重新设置LFSR"));
                }
            }
            undoStack->beginMacro("set read rch index");
            SetRLfsrAttrCommand* cmdSetRLfsrAttr = new SetRLfsrAttrCommand(this,readLfsrBak,readLfsr);
            pushCmdSetRLfsrAttr(cmdSetRLfsrAttr);
            undoStack->endMacro();
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return QString(tr(u8"修改失败"));
}

QString RcaGraphScene::setWriteLfsrItemAttr(ModuleWriteLfsr *writeLfsr)
{
    ModuleWriteLfsr* writeLfsrBak = dynamic_cast<ModuleWriteLfsr*>(doubleClickedItemBak);
    if(writeLfsrBak == NULL)  return QString(tr(u8"Item查找失败"));
    if(writeLfsr == NULL) return QString(tr(u8"Item无效"));

    int newIndex = writeLfsr->getWriteLfsrIndex();
    QString result = writeLfsrBak->canSetWriteLfsrIndex(newIndex);
    if(result.isEmpty())
    {
        if(newIndex != writeLfsrBak->getWriteLfsrIndex())
        {
            SetWLfsrAttrCommand* cmdSetWLfsrAttr = new SetWLfsrAttrCommand(this,writeLfsrBak,newIndex);
            pushCmdSetWLfsrAttr(cmdSetWLfsrAttr);
        }
        emit sendWindowToModified(true);
        return QString(tr(u8""));
    }
    return result;
}

void RcaGraphScene::itemDoubleClicked(BaseItem *baseItem)
{
    if(baseItem == NULL) return;

    doubleClickedItemBak = baseItem;
    emit itemBeDoubleClicked(doubleClickedItemBak);
}

void RcaGraphScene::showAllWire()
{
//    if(isAllWireShow) return;

    isAllWireShow = true;
    QListIterator<ElementWire*> wireListTemp(allWireList);
    while (wireListTemp.hasNext())
    {
        QL_DEBUG;
        ElementWire *wire = wireListTemp.next();
        if(wire)
        {
            wire->show();
        }
    }
}

void RcaGraphScene::setProject(CProjectResolver *projectResolver)
{
    m_projectResolver = projectResolver;
    setResourceMap();
}

void RcaGraphScene::hideAllWire()
{
//    if(isAllWireShow)
    {
        isAllWireShow = false;
        QListIterator<ElementWire*> wireListTemp(allWireList);
        while (wireListTemp.hasNext())
        {
            QL_DEBUG;
            ElementWire *wire = wireListTemp.next();
            if(wire)
            {
                wire->isHadToShow = false;
                wire->hide();
            }
        }
    }
}

void RcaGraphScene::setResourceMap()
{
//    RcaSetting* rcaSetting = RcaSetting::instance();
//    if(rcaSetting)
//    {
//        rcaSetting->resetResourceMap(this);
//    }
}

bool RcaGraphScene::autoMakeNewBcu(int bcuIndex)
{
    if(bcuIndex >= bcuMaxCount) return false;

    ModuleBcu* moduleBcu = NULL;

    for(int index=0;index<bcuExistList.size();index++)
    {
        if(bcuIndex == bcuExistList.at(index)->getIndexBcu())
        {
            moduleBcu = bcuExistList.at(index);
        }
    }

    //bcuItemList没有bcuIndex，则需要新建一个bcu。
    if(moduleBcu == NULL)
    {
        if(isBcuFulled) return false;   //add bcu failed when bcu fulled.

        AddBcuCommand* cmdAddBcu = new AddBcuCommand(this,bcuIndex/*,BCU_POS_AT_SCENE[bcuIndex]*/);
        pushCmdAddBcu(cmdAddBcu);
        moduleBcu = cmdAddBcu->getBcuModule();

//        moduleBcu = new ModuleBcu(bcuIndex);
//        this->addItem(moduleBcu);
//        bcuItemList.insert(bcuIndex,moduleBcu);
//        connect(moduleBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));

        if(bcuMaxCount <= bcuExistList.size())
        {
            isBcuFulled = true;
            emit bcuFullStatesChanged(isBcuFulled);
        }

        QL_DEBUG << bcuExistList << bcuIndex;

        emit addNewBcuSuccess(moduleBcu);
    }
    moduleBcu->setPos(BCU_POS_AT_SCENE[bcuIndex]);
//    moduleBcu->autoMakeAllRcu(); //RCU根据重构数量决定

    return true;
}

ModuleBcu* RcaGraphScene::pasteNewBcu(ModuleBcu &copyBcu, QPointF posAtThis)
{
    if(isBcuFulled) return NULL;   //add bcu failed when bcu fulled.

    int copyIndex = copyBcu.getIndexBcu();

    ModuleBcu* moduleBcu = NULL;

    for(int index=0;index<bcuExistList.size();index++)
    {
        if(copyIndex == bcuExistList.at(index)->getIndexBcu())
        {
            moduleBcu = bcuExistList.at(index);
        }
    }

    int pasteIndex = 0;
    if(moduleBcu == NULL)
    {
        pasteIndex = copyIndex;
    }
    else
    {
        int curIndex=0;
        for(;curIndex<bcuExistList.size();curIndex++)
        {
            if(curIndex != bcuExistList.at(curIndex)->getIndexBcu()) break;
        }
        QL_DEBUG << curIndex;
        if(curIndex >= bcuMaxCount) return NULL;

        pasteIndex = curIndex;
    }

    if(pasteIndex >= bcuMaxCount) return NULL;

    beginPushMacro("paste new bcu");
    //bcuItemList没有pasteIndex，则需要新建一个bcu。
    AddBcuCommand* cmdAddBcu = new AddBcuCommand(this,pasteIndex);
    pushCmdAddBcu(cmdAddBcu);
    moduleBcu = cmdAddBcu->getBcuModule();

    if(bcuMaxCount <= bcuExistList.size())
    {
        isBcuFulled = true;
        emit bcuFullStatesChanged(isBcuFulled);
    }

    QL_DEBUG << bcuExistList << pasteIndex;
    emit addNewBcuSuccess(moduleBcu);

    moduleBcu->setRealBoundingRect(copyBcu.getRealBoundingRect());
    moduleBcu->setPos(posAtThis);

    moduleBcu->srcDataFromBcu = copyBcu.srcDataFromBcu;
    moduleBcu->sort = copyBcu.sort;
    moduleBcu->burst = copyBcu.burst;
    moduleBcu->gap = copyBcu.gap;
    moduleBcu->infifoSrc = copyBcu.infifoSrc;
    moduleBcu->lfsrGroup = copyBcu.lfsrGroup;
    moduleBcu->readImdMode128_32 = copyBcu.readImdMode128_32;
    moduleBcu->memorySrcConfig = copyBcu.memorySrcConfig;
    moduleBcu->inFifoSrcConfig = copyBcu.inFifoSrcConfig;
    moduleBcu->imdSrcConfig = copyBcu.imdSrcConfig;
    moduleBcu->pasteAllChildWithOtherBcu(copyBcu);

    endPushMacro();
    return moduleBcu;
}

void RcaGraphScene::saveCmdModified()
{
    for(int i=0;i<undoStack->count();i++)
    {
        QUndoCommand* command = (QUndoCommand*)(undoStack->command(i));
        ItemUndoCommands* itemCommand = dynamic_cast<ItemUndoCommands*>(command);
        if(itemCommand)
        {
            if(i == (undoStack->count()-1))
            {
                itemCommand->setNoModified();
            }
            else
            {
                itemCommand->setToModified();
            }
        }
    }
}

bool RcaGraphScene::isOkChangePortNum()
{
    int ret = QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                   tr(u8"正在进行自定义算子端口数修改，可能造成数据丢失，是否继续？"),
                                   QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes)
    {
        return true;
    }
    else if (ret == QMessageBox::Cancel)
    {
        return false;
    }
    return false;
}

bool RcaGraphScene::addNewBcu(QPointF posAtScene)
{
    if(isBcuFulled) return false;   //add bcu failed when bcu fulled.

//    for(int i=0;i<bcuItemList.size();i++)
//    {
//        QL_DEBUG << bcuItemList.at(i)->getIndexBcu();
//    }

    int curIndex=0;
    for(;curIndex<bcuExistList.size();curIndex++)
    {
        if(curIndex != bcuExistList.at(curIndex)->getIndexBcu()) break;
    }

    QL_DEBUG << curIndex;

    if(curIndex >= bcuMaxCount) return false;

    AddBcuCommand* cmdAddBcu = new AddBcuCommand(this,curIndex/*,posAtScene*/);
    pushCmdAddBcu(cmdAddBcu);
    ModuleBcu* moduleBcu = cmdAddBcu->getBcuModule();

//    ModuleBcu* moduleBcu = new ModuleBcu(curIndex);
//    // setpos的时候先要校验一下此pos放置item后，
//    // scene的rect是否contains这个bcu。
//    // 否，则重新调整位置。
//    this->addItem(moduleBcu);
//    bcuItemList.insert(curIndex,moduleBcu);
//    connect(moduleBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));

    // setpos的时候先要校验一下此pos放置item后，
    // scene的rect是否contains这个bcu。
    // 否，则重新调整位置。
    moduleBcu->setPos(posAtScene);
    if(bcuMaxCount <= bcuExistList.size())
    {
        isBcuFulled = true;
        emit bcuFullStatesChanged(isBcuFulled);
    }

    QL_DEBUG << bcuExistList << curIndex;

    emit addNewBcuSuccess(moduleBcu);

    return true;
}

bool RcaGraphScene::addNewWire(QPointF posAtScene, ElementPort *inSingal)
{
    currentAddWireCmd = new AddWireCommand(this,posAtScene,inSingal,wirePenBrush,wirePenWidth);

//    cmdPushAddWire(currentAddWireCmd);

    currentPaintingWire = currentAddWireCmd->getElementWire();

//    currentPaintingWire = new ElementWire(posAtScene,inSingal,wirePenBrush,wirePenWidth);
//    this->addItem(currentPaintingWire);
//    allWireList.append(currentPaintingWire);

//        currentPaintingWire = wireTemp;
    sceneState = Painting_wire;
//        emit addNewWireSuccess(currentPaintingWire);
    return true;
}

void RcaGraphScene::clearAllSelectedStatus()
{
    whatItemAlign = Module_None;
    whatItemCopy = Module_None;
    selectedBcuList.clear();
//    selectedOneBcu = NULL;
    selectedRcuList.clear();
//    selectedOneRcu = NULL;
    selectedArithList.clear();
}

void RcaGraphScene::isItemBakBeDeleted(BaseItem *baseItem)
{
    if(baseItem == NULL) return;

    if(baseItem == doubleClickedItemBak)
    {
        doubleClickedItemBak = NULL;
        emit deletedItemBakSuccess(false);
    }
}

void RcaGraphScene::selectAllItems()
{
//    for(int i=0;i<bcuItemList.count();i++)
//    {
//        ModuleBcu* moduleBcu= bcuItemList.at(i);
//        if(moduleBcu)
//        {
//            moduleBcu->setSelected(true);
//        }
//    }
    QPainterPath selectionPath;
    selectionPath.addRect(itemsBoundingRect());
    setSelectionArea(selectionPath);

    resetSelectedAttr();
}

bool RcaGraphScene::alignSelectedItems(ItemsAlignPosAttr alignAttr)
{
    if(selectedAttr & isEnabledAlign)
    {
        switch(whatItemAlign)
        {
        case Module_Bcu: return alignSelectedBcu(alignAttr);
        case Module_Rcu: return alignSelectedRcu(alignAttr);
        case Arith_Unit: return alignSelectedArith(alignAttr);
        default:break;
        }
    }
    return false;
}

bool RcaGraphScene::equiSelectedItems(bool isHorEqui)
{
    if(selectedAttr & isEnabledEquidistance)
    {
        switch(whatItemAlign)
        {
        case Module_Bcu: return equiSelectedBcu(isHorEqui);
        case Module_Rcu: return equiSelectedRcu(isHorEqui);
        case Arith_Unit: return equiSelectedArith(isHorEqui);
        default:break;
        }
    }
    return false;
}

void RcaGraphScene::setSceneState(RcaSceneStates state)
{
    if(sceneState == Painting_wire && currentAddWireCmd)
    {
        removeItem(currentAddWireCmd->getElementWire());
        delete currentAddWireCmd;
        currentAddWireCmd = NULL;
        currentPaintingWire = NULL;
    }
    sceneState = state;
    emit statusChanged(sceneState);
}

void RcaGraphScene::pushCmdItemMoved(QList<BaseItem *> movedItemList)
{
    QL_DEBUG;
    undoStack->push(new ItemMoveCommand(this,movedItemList));
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddWire(AddWireCommand *cmdAddWire)
{
    if(cmdAddWire)
    {
        emit addNewWireSuccess(cmdAddWire->getElementWire());
        undoStack->push(cmdAddWire);
        resetSelectedAttr();
        emit sendWindowToModified(true);
    }
}

void RcaGraphScene::pushCmdWireDeleted(QList<ElementWire *> delWireList)
{
    QL_DEBUG;
    undoStack->push(new DeleteWireCommand(this,delWireList));
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdWireChanged(ElementWire *changedWire)
{
    QL_DEBUG;

    undoStack->push(new WireChangeCommand(this,changedWire));
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddBcu(AddBcuCommand *cmdAddBcu)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddBcu);

//    clearSelection();
    undoStack->push(cmdAddBcu);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddRcu(AddRcuCommand *cmdAddRcu)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddRcu);

//    clearSelection();
    undoStack->push(cmdAddRcu);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddBfu(AddBfuCommand *cmdAddBfu)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddBfu);

//    clearSelection();
    undoStack->push(cmdAddBfu);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddSbox(AddSboxCommand *cmdAddSbox)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddSbox);

//    clearSelection();
    undoStack->push(cmdAddSbox);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddBenes(AddBenesCommand *cmdAddBenes)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddBenes);

//    clearSelection();
    undoStack->push(cmdAddBenes);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddRMem(AddReadMemCommand *cmdAddRMem)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddRMem);

//    clearSelection();
    undoStack->push(cmdAddRMem);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddRImd(AddReadImdCommand *cmdAddRImd)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddRImd);

//    clearSelection();
    undoStack->push(cmdAddRImd);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddRRch(AddReadRchCommand *cmdAddRRch)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddRRch);

    undoStack->push(cmdAddRRch);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddRLfsr(AddReadLfsrCommand *cmdAddRLfsr)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddRLfsr);

    undoStack->push(cmdAddRLfsr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddWMem(AddWriteMemCommand *cmdAddWMem)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddWMem);

//    clearSelection();
    undoStack->push(cmdAddWMem);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}
void RcaGraphScene::pushCmdAddCustomPE(AddCustomPECommand *cmdAddCustomPE)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddCustomPE);
    //    clearSelection();
        undoStack->push(cmdAddCustomPE);
        resetSelectedAttr();
        emit sendWindowToModified(true);
    }

void RcaGraphScene::pushCmdAddWRch(AddWriteRchCommand *cmdAddWRch)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddWRch);

//    clearSelection();
    undoStack->push(cmdAddWRch);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdAddWLfsr(AddWriteLfsrCommand *cmdAddWLfsr)
{
    QL_DEBUG;
    Q_ASSERT(cmdAddWLfsr);

//    clearSelection();
    undoStack->push(cmdAddWLfsr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdDeleteItem(DeleteItemCommand *cmdDelItem)
{
    QL_DEBUG;
    Q_ASSERT(cmdDelItem);

//    clearSelection();
    undoStack->push(cmdDelItem);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetBcuIndex(SetBcuIndexCommand *cmdSetBcuId)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetBcuId);

    undoStack->push(cmdSetBcuId);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetRcuIndex(SetRcuIndexCommand *cmdSetRcuId)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetRcuId);

    undoStack->push(cmdSetRcuId);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetArithIndex(SetArithIndexCommand *cmdSetArithId)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetArithId);

    undoStack->push(cmdSetArithId);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetRMemIndex(SetRMemIndexCommand *cmdSetRMemId)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetRMemId);

    undoStack->push(cmdSetRMemId);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetWMemIndex(SetWMemIndexCommand *cmdSetWMemId)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetWMemId);

    undoStack->push(cmdSetWMemId);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetRImdAttr(SetRImdAttrCommand *cmdSetRImdAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetRImdAttr);

    undoStack->push(cmdSetRImdAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetRRchAttr(SetRRchAttrCommand *cmdSetRRchAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetRRchAttr);

    undoStack->push(cmdSetRRchAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetWRchAttr(SetWRchAttrCommand *cmdSetWRchAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetWRchAttr);

    undoStack->push(cmdSetWRchAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetRLfsrAttr(SetRLfsrAttrCommand *cmdSetRLfsrAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetRLfsrAttr);

    undoStack->push(cmdSetRLfsrAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetWLfsrAttr(SetWLfsrAttrCommand *cmdSetWLfsrAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetWLfsrAttr);

    undoStack->push(cmdSetWLfsrAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetBcuAttr(SetBcuAttrCommand *cmdSetBcuAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetBcuAttr);

    undoStack->push(cmdSetBcuAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetRcuAttr(SetRcuAttrCommand *cmdSetRcuAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetRcuAttr);

    undoStack->push(cmdSetRcuAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetBfuAttr(SetBfuAttrCommand *cmdSetBfuAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetBfuAttr);

    undoStack->push(cmdSetBfuAttr);
    resetSelectedAttr(); //zhangjun modified at 20210102
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetCustomPEAttr(SetCustomPEAttrCommand *cmdSetCustomPEAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetCustomPEAttr);

    undoStack->push(cmdSetCustomPEAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetSboxAttr(SetSboxAttrCommand *cmdSetSboxAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetSboxAttr);

    undoStack->push(cmdSetSboxAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdSetBenesAttr(SetBenesAttrCommand *cmdSetBenesAttr)
{
    QL_DEBUG;
    Q_ASSERT(cmdSetBenesAttr);

    undoStack->push(cmdSetBenesAttr);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::pushCmdItemRectChanged(ItemRectChangedCommand *cmdChangeItemRect)
{
    QL_DEBUG;
    Q_ASSERT(cmdChangeItemRect);

    undoStack->push(cmdChangeItemRect);
    resetSelectedAttr();
    emit sendWindowToModified(true);
}

void RcaGraphScene::beginPushMacro(QString str)
{
    undoStack->beginMacro(str);
}
void RcaGraphScene::endPushMacro()
{
    undoStack->endMacro();
}
//void RcaGraphScene::pushCmdSetPortCon(SetPortConnectCommand *cmdSetPortCon)
//{
//    QL_DEBUG;
//    Q_ASSERT(cmdSetPortCon);

//    undoStack->push(cmdSetPortCon);
//    resetSelectedAttr();
//    emit sendWindowToModified(true);
//}

void RcaGraphScene::actionUndo()
{
    if(undoStack->canUndo())
    {
        clearSelection();
        undoStack->undo();
        resetSelectedAttr();
    }
}

void RcaGraphScene::actionRedo()
{
    if(undoStack->canRedo())
    {
        clearSelection();
        undoStack->redo();
        resetSelectedAttr();
    }
}

void RcaGraphScene::checkUndoItemIsShownItem(BaseItem *baseItem)
{
    if(baseItem && baseItem == doubleClickedItemBak)
    {
        emit shownItemBeUndo(doubleClickedItemBak);
    }
}

void RcaGraphScene::setBcuIndex(ModuleBcu *bcuItem, int index)
{
    if(bcuItem && index < BCU_MAX_COUNT)
    {
        bcuExistList.removeOne(bcuItem);
        bcuItem->setIndexBcu(index);

        int insertIndex = bcuExistList.size();

        //bcuItemList 需要排序
        for(int i=0;i<bcuExistList.size();i++)
        {
            if(index < bcuExistList.at(i)->getIndexBcu())
            {
                insertIndex = i;
                break;
            }
        }
        bcuExistList.insert(insertIndex,bcuItem);
    }
}

void RcaGraphScene::addWireToList(ElementWire *wire)
{
    if(wire && !allWireList.contains(wire))
    {
        allWireList.append(wire);
    }
}

void RcaGraphScene::removeWireFromList(ElementWire *wire)
{
    if(wire && allWireList.contains(wire))
    {
        allWireList.removeOne(wire);
    }
}

void RcaGraphScene::insertBcuToList(int index, ModuleBcu *bcu)
{
    if(bcu)
    {
        bcuExistList.insert(index,bcu);
    }
}

void RcaGraphScene::removeBcuFromList(ModuleBcu *bcu)
{
    if(bcu)
    {
        bcuExistList.removeOne(bcu);
    }
}

void RcaGraphScene::deleteSelectedItems()
{
    curPropertyBrowser->setEnable(false);
    undoStack->beginMacro("delete seleted items");

//    while(!bcuItemList.isEmpty())
//    {
//        bcuItemList.first()->pushItemToDeletedStack(bcuItemList.first()->isSelected());
//    }
    while(!selectedArithList.isEmpty())
    {
        selectedArithList.first()->pushItemToDeletedStack(true);
//        selectedArithList.removeFirst();
    }
    while(!selectedRcuList.isEmpty())
    {
        selectedRcuList.first()->pushItemToDeletedStack(true);
//        selectedRcuList.removeFirst();
    }
    while(!selectedBcuList.isEmpty())
    {
        selectedBcuList.first()->pushItemToDeletedStack(true);
//        selectedBcuList.removeFirst();
    }

//    QList<ModuleBcu*> bcuListTemp = bcuItemList;
//    for(int i=0;i<bcuListTemp.count();i++)
//    {
//        bcuItemList.at(i)->pushItemToDeletedStack(bcuListTemp.at(i)->isSelected());
//    }

    QList<ElementWire *> delWireList;
    delWireList.clear();
    QList<QGraphicsItem *> selectedList = selectedItems();
    for(int i=0;i<selectedList.count();i++)
    {
       QGraphicsItem* itemTemp = selectedList.at(i);
       BaseItem* baseTemp = dynamic_cast<BaseItem*>(itemTemp);
       if(baseTemp &&
               (baseTemp->getUnitId() == RMEM_ID ||
                baseTemp->getUnitId() == WMEM_ID ||
                baseTemp->getUnitId() == RIMD_ID ||
                baseTemp->getUnitId() == RRCH_ID ||
                baseTemp->getUnitId() == WRCH_ID ||
                baseTemp->getUnitId() == RLFSR_ID ||
                baseTemp->getUnitId() == WLFSR_ID))
       {
           baseTemp->pushItemToDeletedStack(true);
       }
       ElementWire* wireTemp = dynamic_cast<ElementWire*>(itemTemp);
       if(wireTemp)
       {
           wireTemp->setSelected(false);
           delWireList.append(wireTemp);
       }
    }
    if(delWireList.count() > 0)
    {
        pushCmdWireDeleted(delWireList);
    }
    undoStack->endMacro();
    curPropertyBrowser->setEnable(true);
    resetSelectedAttr();
}

void RcaGraphScene::clearUndoStack()
{
    undoStack->clear();
    resetSelectedAttr();
}

void RcaGraphScene::copySelectedItems(bool hasWire)
{
    if(funCopyPaste == NULL) return;
    if(selectedAttr & isEnabledCopy)
    {
        funCopyPaste->copyItems(selectedItems(),whatItemCopy,hasWire);
        resetSelectedAttr();
    }
}

void RcaGraphScene::cutSelectedItems()
{
    if(funCopyPaste == NULL) return;
    if(selectedAttr & isEnabledCut)
    {
        funCopyPaste->copyItems(selectedItems(),whatItemCopy,true);
        deleteSelectedItems();
        resetSelectedAttr();
    }
}

void RcaGraphScene::pasteSelectedItems()
{
    QL_DEBUG;
    if(funCopyPaste == NULL) return;
    QGraphicsItem* curItem = itemAt(curMouseScenePos,QTransform());

    clearSelection();

    if(funCopyPaste->canPaste())
    {
        undoStack->beginMacro("paste item");
    }

    if(curItem)
    {
        funCopyPaste->pasteItems(this,curItem,curItem->mapFromScene(curMouseScenePos));
    }
    else
    {
        funCopyPaste->pasteItems(this,NULL,curMouseScenePos);
    }

    undoStack->endMacro();
    resetSelectedAttr();
}

//void RcaGraphScene::setGridViewStep(QPoint step)
//{
//    int newX = step.x();
//    int newY = step.y();
//    if(newX < MIN_STEP_COUNT)
//    {
//        newX = MIN_STEP_COUNT;
//    }
//    else if(newX > MAX_STEP_COUNT)
//    {
//        newX = MAX_STEP_COUNT;
//    }

//    if(newY < MIN_STEP_COUNT)
//    {
//        newY = MIN_STEP_COUNT;
//    }
//    else if(newY > MAX_STEP_COUNT)
//    {
//        newY = MAX_STEP_COUNT;
//    }

//    gridStepPoint = QPoint(newX,newY);
//    setGridBackGround();
//    gridStepPointChanged(gridStepPoint);
//}

//void RcaGraphScene::setGridViewVisable(bool enabled)
//{
//    if(isGridHidden != !enabled)
//    {
//        isGridHidden = !enabled;
//        setGridBackGround();
//        gridHiddenChanged(isGridHidden);
//    }
//}

bool RcaGraphScene::alignSelectedBcu(ItemsAlignPosAttr alignAttr)
{
    int itemsCount = selectedBcuList.size();
    if(itemsCount >= 2)
    {
        QRectF itemsRectInScene = QRectF();
        for(int i=0;i<itemsCount;i++)
        {
            itemsRectInScene |= selectedBcuList.at(i)->mapRectToScene(selectedBcuList.at(i)->getRealBoundingRect());
        }
        for(int i=0;i<itemsCount;i++)
        {
            QRectF bcuRect = selectedBcuList.at(i)->getRealBoundingRect();
            switch(alignAttr)
            {
            case ItemsAlignTop: selectedBcuList.at(i)->setY(itemsRectInScene.top() - bcuRect.top());break;
            case ItemsAlignBottom: selectedBcuList.at(i)->setY(itemsRectInScene.bottom() - bcuRect.bottom());break;
            case ItemsAlignLeft: selectedBcuList.at(i)->setX(itemsRectInScene.left() - bcuRect.left());break;
            case ItemsAlignRight: selectedBcuList.at(i)->setX(itemsRectInScene.right() - bcuRect.right());break;
            default:break;
            }
        }
        return true;
    }
    return false;
}

bool RcaGraphScene::alignSelectedRcu(ItemsAlignPosAttr alignAttr)
{
    if(selectedRcuList.size() >= 2)
    {
        QRectF itemsRectInBcu;
        for(int i=0;i<selectedRcuList.size();i++)
        {
            itemsRectInBcu |= selectedRcuList.at(i)->mapRectToParent(selectedRcuList.at(i)->getRealBoundingRect());
        }
        for(int i=0;i<selectedRcuList.size();i++)
        {
            QRectF rcuRect = selectedRcuList.at(i)->getRealBoundingRect();
            switch(alignAttr)
            {
            case ItemsAlignTop: selectedRcuList.at(i)->setY(itemsRectInBcu.top() - rcuRect.top());break;
            case ItemsAlignBottom: selectedRcuList.at(i)->setY(itemsRectInBcu.bottom() - rcuRect.bottom());break;
            case ItemsAlignLeft: selectedRcuList.at(i)->setX(itemsRectInBcu.left() - rcuRect.left());break;
            case ItemsAlignRight: selectedRcuList.at(i)->setX(itemsRectInBcu.right() - rcuRect.right());break;
            default:break;
            }
            selectedRcuList.at(i)->update();
        }
        return true;
    }
    return false;
}

bool RcaGraphScene::alignSelectedArith(ItemsAlignPosAttr alignAttr)
{
    if(selectedArithList.size() >= 2)
    {
        QRectF itemsRectInRcu;
        for(int i=0;i<selectedArithList.size();i++)
        {
            itemsRectInRcu |= selectedArithList.at(i)->mapRectToParent(selectedArithList.at(i)->getRealBoundingRect());
        }
        for(int i=0;i<selectedArithList.size();i++)
        {
            QRectF arithRect = selectedArithList.at(i)->getRealBoundingRect();
            switch(alignAttr)
            {
            case ItemsAlignTop: selectedArithList.at(i)->setY(itemsRectInRcu.top() - arithRect.top());break;
            case ItemsAlignBottom: selectedArithList.at(i)->setY(itemsRectInRcu.bottom() - arithRect.bottom());break;
            case ItemsAlignLeft: selectedArithList.at(i)->setX(itemsRectInRcu.left() - arithRect.left());break;
            case ItemsAlignRight: selectedArithList.at(i)->setX(itemsRectInRcu.right() - arithRect.right());break;
            default:break;
            }
            selectedArithList.at(i)->update();
        }
        return true;
    }
    return false;
}

bool RcaGraphScene::equiSelectedBcu(bool isHorEqui)
{
    int itemsCount = selectedBcuList.size();
    if(itemsCount >= 3)
    {
        QRectF itemsRectInParent = QRectF();
        QList<ModuleBcu*> bcuListSort = QList<ModuleBcu*>();

        qreal allHeight = 0;
        qreal allWidth = 0;
        for(int i=0;i<itemsCount;i++)
        {

            allHeight += selectedBcuList.at(i)->getRealBoundingRect().height();
            allWidth += selectedBcuList.at(i)->getRealBoundingRect().width();
            QRectF selectedRect = selectedBcuList.at(i)->mapRectToParent(selectedBcuList.at(i)->getRealBoundingRect());
            itemsRectInParent |= selectedRect;
            int k=0;
            for(;k<bcuListSort.count();k++)
            {
                QRectF sortRect = bcuListSort.at(k)->mapRectToScene(bcuListSort.at(k)->getRealBoundingRect());
                if(isHorEqui)
                {
                    if(selectedRect.left() < sortRect.left())
                    {
                        break;
                    }
                }
                else
                {
                    if(selectedRect.top() < sortRect.top())
                    {
                        break;
                    }
                }
            }
            bcuListSort.insert(k,selectedBcuList.at(i));
        }
        qreal equiHorSpace = (itemsRectInParent.width()-allWidth)/(itemsCount-1);
        qreal equiVerSpace = (itemsRectInParent.height()-allHeight)/(itemsCount-1);
        QL_DEBUG << itemsRectInParent << equiHorSpace << equiVerSpace << isHorEqui;
        qreal equiHorWidthBak = 0;
        qreal equiHorHeightBak = 0;

        for(int i=0;i<itemsCount;i++)
        {
            QRectF bcuSortRect = bcuListSort.at(i)->getRealBoundingRect();
            if(isHorEqui)
            {
                bcuListSort.at(i)->setX(itemsRectInParent.left() + equiHorWidthBak - bcuSortRect.left());
                equiHorWidthBak += (bcuSortRect.width() + equiHorSpace);
                QL_DEBUG << bcuListSort.at(i)->getIndexBcu() << bcuListSort.at(i)->pos();
            }
            else
            {
                bcuListSort.at(i)->setY(itemsRectInParent.top() + equiHorHeightBak - bcuSortRect.top());
                equiHorHeightBak += (bcuSortRect.height() + equiVerSpace);
                QL_DEBUG << bcuListSort.at(i)->getIndexBcu() << bcuListSort.at(i)->pos();
            }
        }
        return true;
    }
    return false;
}

bool RcaGraphScene::equiSelectedRcu(bool isHorEqui)
{
    int itemsCount = selectedRcuList.size();
    if(itemsCount >= 3)
    {
        QRectF itemsRectInParent = QRectF();
        QList<ModuleRcu*> rcuListSort = QList<ModuleRcu*>();

        qreal allHeight = 0;
        qreal allWidth = 0;
        for(int i=0;i<itemsCount;i++)
        {
            allHeight += selectedRcuList.at(i)->getRealBoundingRect().height();
            allWidth += selectedRcuList.at(i)->getRealBoundingRect().width();
            QRectF selectedRect = selectedRcuList.at(i)->mapRectToParent(selectedRcuList.at(i)->getRealBoundingRect());
            itemsRectInParent |= selectedRect;
            int k=0;
            for(;k<rcuListSort.count();k++)
            {
                QRectF sortRect = rcuListSort.at(k)->mapRectToParent(rcuListSort.at(k)->getRealBoundingRect());
                if(isHorEqui)
                {
                    if(selectedRect.left() < sortRect.left())
                    {
                        break;
                    }
                }
                else
                {
                    if(selectedRect.top() < sortRect.top())
                    {
                        break;
                    }
                }
            }
            rcuListSort.insert(k,selectedRcuList.at(i));
        }
        qreal equiHorSpace = (itemsRectInParent.width()-allWidth)/(itemsCount-1);
        qreal equiVerSpace = (itemsRectInParent.height()-allHeight)/(itemsCount-1);
        QL_DEBUG << itemsRectInParent << equiHorSpace << equiVerSpace << isHorEqui;
        qreal equiHorWidthBak = 0;
        qreal equiHorHeightBak = 0;

        for(int i=0;i<itemsCount;i++)
        {
            QRectF rcuSortRect = rcuListSort.at(i)->getRealBoundingRect();
            if(isHorEqui)
            {
                rcuListSort.at(i)->setX(itemsRectInParent.left() + equiHorWidthBak - rcuSortRect.left());
                equiHorWidthBak += (rcuSortRect.width() + equiHorSpace);
                QL_DEBUG << rcuListSort.at(i)->getIndexRcu() << rcuListSort.at(i)->pos();
            }
            else
            {
                rcuListSort.at(i)->setY(itemsRectInParent.top() + equiHorHeightBak - rcuSortRect.top());
                equiHorHeightBak += (rcuSortRect.height() + equiVerSpace);
                QL_DEBUG << rcuListSort.at(i)->getIndexRcu() << rcuListSort.at(i)->pos();
            }
        }
        return true;
    }
    return false;
}

bool RcaGraphScene::equiSelectedArith(bool isHorEqui)
{
    int itemsCount = selectedArithList.size();
    if(itemsCount >= 3)
    {
        QRectF itemsRectInParent = QRectF();
        QList<BaseArithUnit*> arithListSort = QList<BaseArithUnit*>();

        qreal allHeight = 0;
        qreal allWidth = 0;
        for(int i=0;i<itemsCount;i++)
        {
            allHeight += selectedArithList.at(i)->getRealBoundingRect().height();
            allWidth += selectedArithList.at(i)->getRealBoundingRect().width();
            QRectF selectedRect = selectedArithList.at(i)->mapRectToParent(selectedArithList.at(i)->getRealBoundingRect());
            itemsRectInParent |= selectedRect;
            int k=0;
            for(;k<arithListSort.count();k++)
            {
                QRectF sortRect = arithListSort.at(k)->mapRectToParent(arithListSort.at(k)->getRealBoundingRect());
                if(isHorEqui)
                {
                    if(selectedRect.left() < sortRect.left())
                    {
                        break;
                    }
                }
                else
                {
                    if(selectedRect.top() < sortRect.top())
                    {
                        break;
                    }
                }
            }
            arithListSort.insert(k,selectedArithList.at(i));
        }
        qreal equiHorSpace = (itemsRectInParent.width()-allWidth)/(itemsCount-1);
        qreal equiVerSpace = (itemsRectInParent.height()-allHeight)/(itemsCount-1);
        QL_DEBUG << itemsRectInParent << equiHorSpace << equiVerSpace << isHorEqui;
        qreal equiHorWidthBak = 0;
        qreal equiHorHeightBak = 0;

        for(int i=0;i<itemsCount;i++)
        {
            QRectF arithSortRect = arithListSort.at(i)->getRealBoundingRect();
            if(isHorEqui)
            {
                arithListSort.at(i)->setX(itemsRectInParent.left() + equiHorWidthBak - arithSortRect.left());
                equiHorWidthBak += (arithSortRect.width() + equiHorSpace);
                QL_DEBUG << arithListSort.at(i)->getIndexInRcu() << arithListSort.at(i)->pos();

            }
            else
            {
                arithListSort.at(i)->setY(itemsRectInParent.top() + equiHorHeightBak - arithSortRect.top());
                equiHorHeightBak += (arithSortRect.height() + equiVerSpace);
                QL_DEBUG << arithListSort.at(i)->getIndexInRcu() << arithListSort.at(i)->pos();
            }
        }
        return true;
    }
    return false;
}

void RcaGraphScene::resetSelectedAttr()
{
    clearAllSelectedStatus();

    int selItemsAttrTemp = SelectedAttrNone;

    //分类获取出所有的选择的bcu，rcu，arith。
    for(int i=0;i<bcuExistList.count();i++)
    {
        ModuleBcu* bcuTemp = bcuExistList.at(i);
        if(bcuTemp->isSelected())
        {
            selectedBcuList.append(bcuTemp);
        }
        selectedRcuList.append(bcuTemp->getSelectedRcu());

        QList<ModuleRcu *> rcuListTemp = bcuTemp->rcuExistList;
        for(int j =0; j <rcuListTemp.count();j++)
        {
            ModuleRcu* rcuTemp = rcuListTemp.at(j);
            selectedArithList.append(rcuTemp->getSelectedArith());
        }
    }

    //计算当前选中的bcu，当bcu大于等于2时，对bcu对齐。
    //判断对齐。
    if(selectedBcuList.count() >= 2)
    {
        whatItemAlign = Module_Bcu;
        selItemsAttrTemp |= isEnabledAlign;
        if(selectedBcuList.count() >= 3)
        {
            selItemsAttrTemp |= isEnabledEquidistance;
        }
    }
    else if(selectedRcuList.count() >= 2)
    {
        bool isValidRcu = true;
        int indexBcu = selectedRcuList.at(0)->getIndexBcu();
        for(int i=1;i<selectedRcuList.count();i++)
        {
            if(indexBcu != selectedRcuList.at(i)->getIndexBcu())
            {
                isValidRcu = false;
            }
        }
        if(isValidRcu)
        {
            whatItemAlign = Module_Rcu;
            selItemsAttrTemp |= isEnabledAlign;
            if(selectedRcuList.size() >= 3)
            {
                selItemsAttrTemp |= isEnabledEquidistance;
            }
        }
    }
    else if(selectedArithList.count() >= 2)
    {
        bool isValidArith = true;
        int indexBcu = selectedArithList.at(0)->getIndexBcu();
        int indexRcu = selectedArithList.at(0)->getIndexRcu();
        for(int i=1;i<selectedArithList.count();i++)
        {
            if(indexBcu != selectedArithList.at(i)->getIndexBcu() ||
                    indexRcu != selectedArithList.at(i)->getIndexRcu())
            {
                isValidArith = false;
            }
        }
        if(isValidArith)
        {
            whatItemAlign = Arith_Unit;
            selItemsAttrTemp |= isEnabledAlign;
            if(selectedArithList.size() >= 3)
            {
                selItemsAttrTemp |= isEnabledEquidistance;
            }
        }

    }
    if(0)
    {
//        bool isRcuAlignEnable = true;
//        ModuleRcu* rcuBak = NULL;
//        QList<QGraphicsItem *> selectedList = selectedItems();
//        for(int i=0;i<selectedList.count();i++)
//        {
//            //计算当前选中的rcu，当rcu来自不同bcu时，不能对齐，
//            //rcu来自同一个bcu，则大于等于2时，可以对齐。
//            QGraphicsItem* itemTemp = selectedList.at(i);
//            ModuleRcu* rcuTemp = dynamic_cast<ModuleRcu*>(itemTemp);
//            if(rcuTemp)
//            {
//                if(rcuBak)
//                {
//                    if(rcuBak->getIndexBcu() == rcuTemp->getIndexBcu())
//                    {
//                        selectedRcuList.append(rcuTemp);
//                    }
//                    else
//                    {
//                        isRcuAlignEnable = false;
//                        break;// 无对齐功能
//                    }
//                }
//                else
//                {
//                    rcuBak = rcuTemp;
//                    selectedRcuList.append(rcuTemp);
////                    selectedOneBcu = findBcuFromScene(rcuTemp->getIndexBcu());
//                }
//            }
//        }

//        if((selectedRcuList.size() >= 2) && isRcuAlignEnable)
//        {
//            whatItemAlign = Module_Rcu;
//            selItemsAttrTemp |= isEnabledAlign;
//            if(selectedRcuList.size() >= 3)
//            {
//                selItemsAttrTemp |= isEnabledEquidistance;
//            }
////            if(!(selectedAttr & isEnabledAlign))
////            {
////                selectedAttr |= isEnabledAlign;
////                selectedItemsAttrChanged(selectedAttr);
////            }
////            QL_DEBUG << whatItemSelected << selectedRcuList.size();
////            return;
//        }
//        //当选中的rcu等于0,对arith对齐。
////        else if (selectedRcuList.size() == 0)
//        else if(isRcuAlignEnable)
//        {
//            bool isArithAlignEnable = true;
//            BaseArithUnit* arithBak = NULL;
//            QList<QGraphicsItem *> selectedList = selectedItems();
//            for(int i=0;i<selectedList.count();i++)
//            {
//                //计算当前选中的arith，当arith来自不同bcu或者rcu时，不能对齐，
//                //arith来自同一行，则大于等于2时，可以对齐。
//                QGraphicsItem* itemTemp = selectedList.at(i);
//                BaseArithUnit* arithTemp = dynamic_cast<BaseArithUnit*>(itemTemp);
//                if(arithTemp)
//                {
//                    if(arithBak)
//                    {
//                        if(arithBak->getIndexBcu() == arithTemp->getIndexBcu() &&
//                                arithBak->getIndexRcu() == arithTemp->getIndexRcu())
//                        {
//                            selectedArithList.append(arithTemp);
//                        }
//                        else
//                        {
//                            isArithAlignEnable = false;
//                            break;// 无对齐功能
//                        }
//                    }
//                    else
//                    {
//                        arithBak = arithTemp;
//                        selectedArithList.append(arithTemp);
////                        selectedOneBcu = findBcuFromScene(arithTemp->getIndexBcu());
////                        selectedOneRcu = selectedOneBcu->findRcuFromThis(arithTemp->getIndexRcu());
//                    }
//                }
//            }

//            if((selectedArithList.size() >= 2) && isArithAlignEnable)
//            {
//                whatItemAlign = Arith_Unit;
//                selItemsAttrTemp |= isEnabledAlign;
//                if(selectedArithList.size() >= 3)
//                {
//                    selItemsAttrTemp |= isEnabledEquidistance;
//                }
////                if(!(selectedAttr & isEnabledAlign))
////                {
////                    selectedAttr |= isEnabledAlign;
////                    selectedItemsAttrChanged(selectedAttr);
////                }
////                QL_DEBUG << whatItemSelected << selectedArithList.size();
////                return;
//            }
//        }
//        //else 无对齐功能

    }

    //判断可不可以复制
    bool isCanCopy = true;
    if((selectedBcuList.count() <= 0)
            && selectedRcuList.count() <= 0
            && selectedArithList.count() <= 0)
    {
        isCanCopy = false;
    }


    if(selectedBcuList.count() > 0)
    {
        whatItemCopy = Module_Bcu;
        for(int i=0;i<selectedArithList.count();i++)
        {
            if(selectedArithList.at(i)->parentItem() == NULL)
            {
                isCanCopy = false;
                break;
            }
            if(!selectedArithList.at(i)->parentItem()->isSelected())
            {
                isCanCopy = false;
                break;
            }
        }
        for(int i=0;i<selectedRcuList.count();i++)
        {
            if(selectedRcuList.at(i)->parentItem() == NULL)
            {
                isCanCopy = false;
                break;
            }
            if(!selectedRcuList.at(i)->parentItem()->isSelected())
            {
                isCanCopy = false;
                break;
            }
        }
    }
    else
    {
        if(selectedRcuList.count() > 0)
        {
            whatItemCopy = Module_Rcu;
            for(int i=0;i<selectedArithList.count();i++)
            {
                if(selectedArithList.at(i)->parentItem() == NULL)
                {
                    QL_DEBUG;
                    isCanCopy = false;
                    break;
                }
                if(!selectedArithList.at(i)->parentItem()->isSelected())
                {
                    QL_DEBUG;
                    isCanCopy = false;
                    break;
                }
            }
            int bcuIndex = selectedRcuList.at(0)->getIndexBcu();
            for(int i=1;i<selectedRcuList.count();i++)
            {
                if(bcuIndex != selectedRcuList.at(i)->getIndexBcu())
                {
                    QL_DEBUG;
                    isCanCopy = false;
                    break;
                }
            }
        }
        else
        {
            if(selectedArithList.count() > 0)
            {
                whatItemCopy = Arith_Unit;
                int indexBcu = selectedArithList.at(0)->getIndexBcu();
                int indexRcu = selectedArithList.at(0)->getIndexRcu();
                for(int i=1;i<selectedArithList.count();i++)
                {
                    if(indexBcu != selectedArithList.at(i)->getIndexBcu()
                            || indexRcu != selectedArithList.at(i)->getIndexRcu())
                    {
                        isCanCopy = false;
                        break;
                    }
                }
            }
        }
    }

    if(selectedItems().count() > 0)
    {
        selItemsAttrTemp |= isEnabledDel;
        if(isCanCopy)
        {
            selItemsAttrTemp |= isEnabledCopy;
            selItemsAttrTemp |= isEnabledCut;
        }
    }
    if(funCopyPaste && funCopyPaste->canPaste())
    {
        selItemsAttrTemp |= isEnabledPaste;
    }

    if(undoStack->canUndo())
    {
        selItemsAttrTemp |= isEnabledUndo;
    }
    if(undoStack->canRedo())
    {
        selItemsAttrTemp |= isEnabledRedo;
    }
    if(selectedAttr != selItemsAttrTemp)
    {
        selectedAttr = (SelectedItemsAttr)selItemsAttrTemp;
        selectedItemsAttrChanged(selectedAttr);
    }
//    if((selectedAttr & isEnabledAlign))
//    {
//        selectedAttr &= ~isEnabledAlign;
//        selectedItemsAttrChanged(selectedAttr);
//    }

    BaseItem* curSetItem = NULL;
    if(!selectedItems().isEmpty())
    {
        for(int i=selectedItems().count()-1;i>=0;i--)
        {
            curSetItem = dynamic_cast<BaseItem*>(selectedItems().at(i));
            if(curSetItem)
            {
                doubleClickedItemBak = curSetItem;
                break;
            }
        }
    }
    curPropertyBrowser->updateItemProperty(selectedItems().count() > 1 ? nullptr : curSetItem);

    QL_DEBUG << whatItemAlign;
}

void RcaGraphScene::paintScene(QPainter *painter)
{
    QList<QGraphicsItem*> items = selectedItems();
    clearSelection();
//        setBackgroundBrush(Qt::transparent);
    QRectF sceneRect = itemsBoundingRect();
    sceneRect.adjust(-20,-20,20,20);
    render(painter,QRectF(),sceneRect);
//        int posXSetp = gridStepPoint.x();
//        int posYSetp = gridStepPoint.y();
    foreach (QGraphicsItem *item, items)
//        QPixmap pixmap = QPixmap(posXSetp, posYSetp);
//        pixmap.fill(Qt::transparent);
//        QPainter painter(&pixmap);
        item->setSelected(true);
//        painter.setRenderHints(QPainter::Antialiasing|
//                               QPainter::TextAntialiasing);
    selectionChanged();
//        qreal penWidth =1;
//        QPen pen(Qt::lightGray,penWidth);
//        painter.setPen(pen);
//        painter.setBrush(Qt::lightGray);
//        QPointF points[4] = {QPointF(0,0),
//                             QPointF(posXSetp,0),
//                             QPointF(0,posYSetp),
//                             QPointF(posXSetp,posYSetp)};
//        painter.drawPoints(points,4);
//        setBackgroundBrush(pixmap);
//    }
}

void RcaGraphScene::handleBcuBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG;
    if(!deletedItem) return;
    ModuleBcu* deletedBcu = dynamic_cast<ModuleBcu*>(deletedItem);
    if(!deletedBcu) return;

    int indexBcu = deletedBcu->getIndexBcu();
    if(indexBcu >= bcuMaxCount) return;

    if(bcuExistList.contains(deletedBcu))
    {
        bcuExistList.removeOne(deletedBcu);
        disconnect(deletedBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
    }

    if(bcuMaxCount > bcuExistList.size())
    {
        isBcuFulled = false;
        emit bcuFullStatesChanged(isBcuFulled);
    }
    isItemBakBeDeleted(deletedBcu);
    emit deleteBcuSuccessed(deletedBcu);
}

bool RcaGraphScene::getIsDebugMode() const
{
    return isDebugMode;
}

void RcaGraphScene::setIsDebugMode(bool value)
{
    isDebugMode = value;
}

QHash<int, SCustomPEConfig> *RcaGraphScene::getCustomPEConfigHash() const
{
    return m_customPEConfigHash;
}

CItemAttributeSetting *RcaGraphScene::getCurItemSetting() const
{
    return curItemSetting;
}

CPropertyBrowser *RcaGraphScene::getCurPropertyBrowser() const
{
    return curPropertyBrowser;
}

void RcaGraphScene::exportSvg(const QString &fileName)
{
    QSvgGenerator svg;
    QPrinter printer(QPrinter::HighResolution);
    svg.setFileName(fileName);
    svg.setSize(printer.paperSize(QPrinter::Point).toSize());
    {
        QPainter painter(&svg);
        paintScene(&painter);
    }
}
