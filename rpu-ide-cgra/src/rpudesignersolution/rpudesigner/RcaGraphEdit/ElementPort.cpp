#include "ElementPort.h"
#include "BasePort.h"
#include "RcaGraphDef.h"
#include "ModuleWriteLfsr.h"

#include <QGraphicsScene>
#include <QDebug>
#include "RcaGraphScene.h"
#include "ModuleReadMem.h"
#include "ModuleWriteMem.h"
#include "ElementWire.h"

using namespace rca_space;

ElementPort::ElementPort(PortDirection type, QGraphicsItem *parent, PortFromWhereAttr attr) : BasePort(parent,attr)
    , portDirection(type)
    , inSignal(NULL)
    , isBreakPoint(false)
    , portShape(ShapeTriangle)
    , paintPath(QPainterPath())
    , dragOver(false)
//    , piantColor(Qt::transparent)
    , paintBrush(PORT_NORMAL_BRUSH_FILLIN)
    , inputType(InputPort_NULL)
    , inputIndex(0)
    , wireThisPort(QList<QGraphicsItem*>())
    , hoverState(None_Hover)
    , portPressedPosList(QList<QPointF>())
    , portInfletionPosList(QList<QPointF>())
    , oldPortPressedPosList(QList<QPointF>())
    , oldPortInfletionPosList(QList<QPointF>())
    , dpInfo(new DebugPortInfo())
{
    setFlag(QGraphicsItem::ItemIsSelectable);
//    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);

    setAcceptHoverEvents(true);

    unitId = PORT_ID;

    setAcceptDrops(true);
    isPartOfChildRect = false;
    isEnabledAdjustRect = false;

    paintRect = ARITHUNIT_PORT_DEFAULT_RECT;
    realBoundingRect = ARITHUNIT_PORT_DEFAULT_RECT;

    connect(this,SIGNAL(xChanged()),this,SLOT(portXYChanged()));
    connect(this,SIGNAL(yChanged()),this,SLOT(portXYChanged()));
    if(portShape == ShapeTriangle)
    {
        if(portDirection == InputDirection)
        {
            paintPath.moveTo(paintRect.topLeft());
            paintPath.lineTo(paintRect.topRight());
            paintPath.lineTo((paintRect.left() + paintRect.right())/2,paintRect.bottom()- PORT_DEFAULT_TEXT_HEIGHT);
            paintPath.lineTo(paintRect.topLeft());
            textRect = QRectF(0,paintRect.bottom()- PORT_DEFAULT_TEXT_HEIGHT,paintRect.width(),PORT_DEFAULT_TEXT_HEIGHT);
        }
        else
        {
            paintPath.moveTo(paintRect.left(),PORT_DEFAULT_TEXT_HEIGHT);
            paintPath.lineTo(paintRect.right(),PORT_DEFAULT_TEXT_HEIGHT);
            paintPath.lineTo((paintRect.left() + paintRect.right())/2,paintRect.bottom());
            paintPath.lineTo(paintRect.left(),PORT_DEFAULT_TEXT_HEIGHT);
            textRect = QRectF(0,0,paintRect.width(),PORT_DEFAULT_TEXT_HEIGHT);
        }
    }
}

ElementPort::~ElementPort()
{
//    deleteAllConnectWire();

//    QList<QGraphicsItem *> childList = childItems();

//    QList<QGraphicsItem*>::iterator iter;
//    for(iter=childList.begin();iter!=childList.end();iter++)
//    {
//        if((*iter))
//        {
//            ModuleWriteMem* deleteWriteMem = dynamic_cast<ModuleWriteMem*>(*iter);
//            if(deleteWriteMem)
//            {
//                handleWRMemBeDeleted(deleteWriteMem);
//            }
//            else
//            {
//                ModuleReadMem* deleteReadMem = dynamic_cast<ModuleReadMem*>(*iter);
//                if(deleteReadMem)
//                {
//                    handleWRMemBeDeleted(deleteReadMem);
//                }
//            }
//        }
//    }

    emit portBeDeleted(this);
    QL_DEBUG << "~ElementPort()";
}

//QPainterPath ElementPort::shape() const
//{
//    QPainterPath path;
//    path.addRect(boundingRect());
//    return path;
//}

void ElementPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    painter->setBrush(paintBrush);
    painter->setPen(Qt::black);
    switch(portShape)
    {
        case ShapeTriangle:
        {
            QFont font;
            font.setPixelSize(10);
            painter->setFont(font);
            if(portDirection == InputDirection)
            {
                RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                if(rcaScene && rcaScene->getIsDebugMode())
                {
                    if(dpInfo->isTextChanged)
                        painter->setPen(Qt::darkGreen);
                    painter->drawText(textRect,Qt::AlignBottom | Qt::AlignHCenter,dpInfo->elementStr.right(2));

                    if(isBreakPoint)
                    {
                        //画出断点圆圈符号
                        QColor color(Qt::red); color.setAlpha(60); //其中，参数x为透明度，取值范围为0~255，数值越小越透明
                        QBrush posBrush(color);
                        painter->setBrush(posBrush);
                        painter->setPen(Qt::NoPen);
                        painter->drawEllipse(textRect.center(),textRect.height()/1.8,textRect.height()/1.8);
                    }
                }

                painter->setBrush(paintBrush);
                painter->setPen(Qt::black);
            }
            else
            {
                RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                if(rcaScene && rcaScene->getIsDebugMode())
                {
                    if(dpInfo->isTextChanged)
                        painter->setPen(Qt::darkGreen);
                    painter->drawText(textRect,Qt::AlignTop | Qt::AlignHCenter,dpInfo->elementStr.right(2));

                    if(isBreakPoint)
                    {
                        //画出断点圆圈符号
                        QColor color(Qt::red); color.setAlpha(60); //其中，参数x为透明度，取值范围为0~255，数值越小越透明
                        QBrush posBrush(color);
                        painter->setBrush(posBrush);
                        painter->setPen(Qt::NoPen);
                        painter->drawEllipse(textRect.center(),textRect.height()/1.8,textRect.height()/1.8);
                    }
                }

                painter->setBrush(paintBrush);
                painter->setPen(Qt::black);
            }

            painter->drawPath(paintPath);
            if(hoverState == Can_Hover)
            {
                painter->setPen(Qt::green);
                painter->setBrush(Qt::transparent);
                painter->drawRect(realBoundingRect);
            }
            else if(hoverState == Cannot_Hover)
            {
                painter->setPen(Qt::red);
                painter->setBrush(Qt::transparent);
                painter->drawRect(realBoundingRect);
            }
            break;
        }
        case ShapeRectangle:
        {
            painter->drawRect(paintRect);
            break;
        }
        case ShapeEllipsoid:
        {
            painter->drawEllipse(paintRect);
            break;
        }
        default:break;
    }
}

//void ElementPort::setPaintRect(QRectF rect)
//{
//     paintRect = rect;
//}

void ElementPort::showItemWire(bool isShow)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
//        QL_DEBUG;
        QListIterator<QGraphicsItem*> itemListTemp(wireThisPort);
        while (itemListTemp.hasNext())
        {
//            QL_DEBUG;
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementWire* wire = dynamic_cast<ElementWire*>(item);
                if(wire)
                {
                    if(isShow || wire->isHadToShow || wire->hasFocus() || rcaScene->isAllWireShow)
                    {
                        QL_DEBUG;
                        wire->show();
//                        wire->setSelected(true);
                        wire->update();
                    }
                    else
                    {
                        QL_DEBUG;
                        wire->hide();
//                        wire->setSelected(false);
                        wire->update();
                    }
                }
            }
        }
    }
}

QVariant ElementPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
//    QL_DEBUG << change;
    if(ItemScenePositionHasChanged == change)
    {
        emit portBeChanged(this);
    }
    return QGraphicsItem::itemChange(change, value);
}

void ElementPort::portXYChanged()
{
//    QL_DEBUG;
//    emit portBeChanged(this);
}

void ElementPort::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && rcaScene->getIsDebugMode())
    {
        setToolTip(dpInfo->elementStr);
    }
}

void ElementPort::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && rcaScene->getIsDebugMode())
    {
        setToolTip("");
    }
}

void ElementPort::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    QL_DEBUG;
    if (event->mimeData()->hasColor())
    {
        event->setAccepted(true);
        dragOver = true;
//        piantColor = qvariant_cast<QColor>(event->mimeData()->colorData());
        update();
    }
    else
    {
        event->setAccepted(false);
    }
}

void ElementPort::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    QL_DEBUG;
    Q_UNUSED(event);
    dragOver = false;
//    piantColor = Qt::transparent;
    update();
}

void ElementPort::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QL_DEBUG;
    dragOver = false;
    if (event->mimeData()->hasColor())
    {
//        piantColor = Qt::transparent;
//        color = qvariant_cast<QColor>(event->mimeData()->colorData());
    }
    update();
}

//void ElementPort::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//    QL_DEBUG;
//    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
//    if(rcaScene)
//    {
//        RcaSceneStates sceneState = rcaScene->sceneState;
//        QL_DEBUG << event->pos().toPoint() << event->scenePos().toPoint() << event->button() << sceneState;
//        if(Qt::LeftButton == event->button())
//        {
//            if(sceneState == Add_Mem)
//            {
//                QL_DEBUG;
//                //必须是arithUnit的port才可以添加memory。
//                //如果是fifo的port不可以添加memory。
//                if(parentItem())
//                {
////                    BaseItem* baseItem = dynamic_cast<BaseItem*>(parentItem());
////                    if(baseItem->objectName() == ARITH_UNIT_OBJECT_NAME)
//                    BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(parentItem());
//                    if(baseArith)
//                    {
//                        if(this->portDirection == InputDirection)
//                        {
//                            addNewReadMem(realBoundingRect.center());
//                        }
//                        else if(this->portDirection == OutputDirection)
//                        {
//                            addNewWriteMem(realBoundingRect.center(),baseArith);
//                        }
//                    }
//                }
//            }
//        }
//    }
//    QGraphicsItem::mouseReleaseEvent(event);
//}

bool ElementPort::addNewReadMem()
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadMemCommand* cmdAddRMem = new AddReadMemCommand(rcaScene,0,this);
    rcaScene->pushCmdAddRMem(cmdAddRMem);
    ModuleReadMem* readMem = cmdAddRMem->getReadMemModule();

    QPointF newPos = QPointF(realBoundingRect.center().x() - readMem->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readMem->getRealBoundingRect().height());
    readMem->setPos(newPos);
    emit addReadMemSuccessed(readMem);
    return true;
}

bool ElementPort::addNewReadImd()
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadImdCommand* cmdAddRImd = new AddReadImdCommand(rcaScene,0,this);
    rcaScene->pushCmdAddRImd(cmdAddRImd);
    ModuleReadImd* readImd = cmdAddRImd->getReadImdModule();

    QPointF newPos = QPointF(realBoundingRect.center().x() - readImd->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readImd->getRealBoundingRect().height());
    readImd->setPos(newPos);
    emit addReadImdSuccessed(readImd);
    return true;
}

bool ElementPort::addNewReadRch()
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

//    int newRchIndex = 0;
//    ElementBfu* parentBfu = dynamic_cast<ElementBfu*>(this->parentItem());
//    if(parentBfu)
//    {
//        ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(parentBfu->parentItem());
//        if(parentRcu)
//        {
//            newRchIndex = parentRcu->rchChannel*READ_RCH_MAX_COUNT_ONE_GROUP;
//        }
//    }

//    AddReadRchCommand* cmdAddRRch = new AddReadRchCommand(rcaScene,newRchIndex,this);
    AddReadRchCommand* cmdAddRRch = new AddReadRchCommand(rcaScene,0,this);
    rcaScene->pushCmdAddRRch(cmdAddRRch);
    ModuleReadRch* readRch = cmdAddRRch->getReadRchModule();

    QPointF newPos = QPointF(realBoundingRect.center().x() - readRch->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readRch->getRealBoundingRect().height());
    readRch->setPos(newPos);
    emit addReadRchSuccessed(readRch);
    return true;
}

bool ElementPort::addNewReadLfsr()
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;

    QL_DEBUG;
    AddReadLfsrCommand* cmdAddRLfsr = new AddReadLfsrCommand(rcaScene,0,this);
    rcaScene->pushCmdAddRLfsr(cmdAddRLfsr);
    ModuleReadLfsr* readLfsr = cmdAddRLfsr->getReadLfsrModule();

    QPointF newPos = QPointF(realBoundingRect.center().x() - readLfsr->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readLfsr->getRealBoundingRect().height());
    readLfsr->setPos(newPos);
    emit addReadLfsrSuccessed(readLfsr);
    return true;
}

bool ElementPort::addNewWriteMem(BaseArithUnit* parentBaseArith)
{
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWMemFulled) return false;   //add write mem failed when write mem fulled.

            int curIndex=0;
            for(;curIndex<findedRcu->wMemExistList.size();curIndex++)
            {
                if(curIndex != findedRcu->wMemExistList.at(curIndex)) break;
            }

            QL_DEBUG << curIndex;

            if(curIndex >= findedRcu->wMemMaxCount) return false;

            AddWriteMemCommand* cmdAddWMem = new AddWriteMemCommand(rcaScene,curIndex,this);
            rcaScene->pushCmdAddWMem(cmdAddWMem);
            ModuleWriteMem *writeMem = cmdAddWMem->getWriteMemModule();

//            findedRcu->wMemExistList.insert(curIndex,curIndex);
//            ModuleWriteMem *writeMem = new ModuleWriteMem(curIndex,this);
//            connect(writeMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
//            addWire(writeMem);

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeMem->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeMem->setPos(newPos);


            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            if(elementBfu)
            {
                if(elementBfu->outPortX == this)
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBfuX;
                    writeMem->inputIndex = elementBfu->getIndexInRcu();

                }
                else if(elementBfu->outPortY == this)
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBfuY;
                    writeMem->inputIndex = elementBfu->getIndexInRcu();

                }
            }
            else if(elementCustomPE)
            {
                for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                    if(elementCustomPE->m_inputPortVector.at(i) == this)
                    {
                        //匹配成功
                        writeMem->inputType = InputPort_CurrentCustomPE;
                        writeMem->inputIndex = elementCustomPE->getIndexInRcu();
                    }
                }
            }
            else if(elementSbox)
            {
                if((elementSbox->outPort0 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();

                }
                else if((elementSbox->outPort1 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;

                }
                else if((elementSbox->outPort2 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;

                }
                else if((elementSbox->outPort3 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;

                }
            }
            else if (elementBenes)
            {
                if((elementBenes->outPort0 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();

                }
                else if((elementBenes->outPort1 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;

                }
                else if((elementBenes->outPort2 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;

                }
                else if((elementBenes->outPort3 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;

                }
            }

            emit addWriteMemSuccessed(writeMem);

            return true;
        }
    }
    return false;
}

bool ElementPort::addNewWriteRch(BaseArithUnit *parentBaseArith)
{
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWRchFulled) return false;   //add write Rch failed when write Rch fulled.

            int curIndex = 0;
            for(;curIndex<findedRcu->wRchExistList.size();curIndex++)
            {
                if(curIndex != findedRcu->wRchExistList.at(curIndex)) break;
            }

            QL_DEBUG << findedRcu->wRchExistList;
            QL_DEBUG << curIndex;

            if(curIndex >= findedRcu->wRchMaxCount) return false;

            rcaScene->beginPushMacro("add new write rch");
            AddWriteRchCommand* cmdAddWRch = new AddWriteRchCommand(rcaScene, curIndex, 0,this);
            rcaScene->pushCmdAddWRch(cmdAddWRch);
            ModuleWriteRch *writeRch = cmdAddWRch->getWriteRchModule();

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeRch->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeRch->setPos(newPos);

            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            if(elementBfu)
            {
                if(elementBfu->outPortX == this)
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBfuX;
                    writeRch->inputIndex = elementBfu->getIndexInRcu();

                }
                else if(elementBfu->outPortY == this)
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBfuY;
                    writeRch->inputIndex = elementBfu->getIndexInRcu();

                }
            }
            else if(elementCustomPE)
            {
                for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                    if(elementCustomPE->m_inputPortVector.at(i) == this)
                    {
                        //匹配成功
                        writeRch->inputType = InputPort_CurrentCustomPE;
                        writeRch->inputIndex = elementCustomPE->getIndexInRcu();
                    }
                }
            }
            else if(elementSbox)
            {
                if((elementSbox->outPort0 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();

                }
                else if((elementSbox->outPort1 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;

                }
                else if((elementSbox->outPort2 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;

                }
                else if((elementSbox->outPort3 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;

                }
            }
            else if (elementBenes)
            {
                if((elementBenes->outPort0 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();

                }
                else if((elementBenes->outPort1 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;

                }
                else if((elementBenes->outPort2 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;

                }
                else if((elementBenes->outPort3 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;

                }
            }

            rcaScene->endPushMacro();
            emit addWriteRchSuccessed(writeRch);

            return true;
        }
    }
    return false;
}

bool ElementPort::addNewWriteLfsr(BaseArithUnit *parentBaseArith)
{
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWLfsrFulled) return false;   //add write Lfsr failed when write Lfsr fulled.

            int curIndex = 0;
            for(;curIndex<findedRcu->wLfsrExistList.size();curIndex++)
            {
                if(curIndex != findedRcu->wLfsrExistList.at(curIndex)) break;
            }

            QL_DEBUG << findedRcu->wLfsrExistList;
            QL_DEBUG << curIndex;

            if(curIndex >= findedRcu->wLfsrMaxCount) return false;

            rcaScene->beginPushMacro("add new write rch");
            AddWriteLfsrCommand* cmdAddWLfsr = new AddWriteLfsrCommand(rcaScene, curIndex, this);
            rcaScene->pushCmdAddWLfsr(cmdAddWLfsr);
            ModuleWriteLfsr *writeLfsr = cmdAddWLfsr->getWriteLfsrModule();

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeLfsr->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeLfsr->setPos(newPos);

            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            if(elementBfu)
            {
                if(elementBfu->outPortX == this)
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBfuX;
                    writeLfsr->inputIndex = elementBfu->getIndexInRcu();

                }
                else if(elementBfu->outPortY == this)
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBfuY;
                    writeLfsr->inputIndex = elementBfu->getIndexInRcu();

                }
            }
            else if(elementCustomPE)
            {
                for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                    if(elementCustomPE->m_inputPortVector.at(i) == this)
                    {
                        //匹配成功
                        writeLfsr->inputType = InputPort_CurrentCustomPE;
                        writeLfsr->inputIndex = elementCustomPE->getIndexInRcu();
                    }
                }
            }
            else if(elementSbox)
            {
                if((elementSbox->outPort0 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();

                }
                else if((elementSbox->outPort1 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;

                }
                else if((elementSbox->outPort2 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;

                }
                else if((elementSbox->outPort3 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;

                }
            }
            else if (elementBenes)
            {
                if((elementBenes->outPort0 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();

                }
                else if((elementBenes->outPort1 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;

                }
                else if((elementBenes->outPort2 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;

                }
                else if((elementBenes->outPort3 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;

                }
            }

            rcaScene->endPushMacro();
            emit addWriteLfsrSuccessed(writeLfsr);

            return true;
        }
    }
    return false;
}

void ElementPort::pasteOtherPort(ElementPort &copyPort)
{
    if(portAttr != copyPort.portAttr) return;

    portPressedPosList = copyPort.portPressedPosList;
    portInfletionPosList = copyPort.portInfletionPosList;

    for(int i=0;i<copyPort.wireThisPort.count();i++)
    {
        QGraphicsItem* itemTemp = copyPort.wireThisPort.at(i);
        if(itemTemp)
        {
            ModuleReadMem* copyReadMem = dynamic_cast<ModuleReadMem*>(itemTemp);
            if(copyReadMem)
            {
                pasteNewReadMem(*copyReadMem);
                continue;
            }

            ModuleWriteMem* copyWriteMem = dynamic_cast<ModuleWriteMem*>(itemTemp);
            if(copyWriteMem)
            {
                pasteNewWriteMem(*copyWriteMem);
                continue;
            }

            ModuleReadImd* copyReadImd = dynamic_cast<ModuleReadImd*>(itemTemp);
            if(copyReadImd)
            {
                pasteNewReadImd(*copyReadImd);
                continue;
            }

            ModuleReadRch* copyReadRch = dynamic_cast<ModuleReadRch*>(itemTemp);
            if(copyReadRch)
            {
                pasteNewReadRch(*copyReadRch);
                continue;
            }

            ModuleWriteRch* copyWriteRch = dynamic_cast<ModuleWriteRch*>(itemTemp);
            if(copyWriteRch)
            {
                pasteNewWriteRch(*copyWriteRch);
                continue;
            }

            ModuleReadLfsr* copyReadLfsr = dynamic_cast<ModuleReadLfsr*>(itemTemp);
            if(copyReadLfsr)
            {
                pasteNewReadLfsr(*copyReadLfsr);
                continue;
            }

            ModuleWriteLfsr* copyWriteLfsr = dynamic_cast<ModuleWriteLfsr*>(itemTemp);
            if(copyWriteLfsr)
            {
                pasteNewWriteLfsr(*copyWriteLfsr);
                continue;
            }
        }
    }
}

bool ElementPort::pasteNewReadMem(ModuleReadMem &copyReadMem)
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadMemCommand* cmdAddRMem = new AddReadMemCommand(rcaScene,copyReadMem.getReadMemIndex(),this);
    rcaScene->pushCmdAddRMem(cmdAddRMem);
    ModuleReadMem* readMem = cmdAddRMem->getReadMemModule();


    QPointF newPos = QPointF(realBoundingRect.center().x() - readMem->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readMem->getRealBoundingRect().height());
    readMem->setPos(newPos);
    emit addReadMemSuccessed(readMem);
    return true;
}

bool ElementPort::pasteNewReadImd(ModuleReadImd &copyReadImd)
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    rcaScene->beginPushMacro("paste new read imd");
    AddReadImdCommand* cmdAddRImd = new AddReadImdCommand(rcaScene,copyReadImd.getReadImdIndex(),this);
    rcaScene->pushCmdAddRImd(cmdAddRImd);
    ModuleReadImd* readImd = cmdAddRImd->getReadImdModule();


    QPointF newPos = QPointF(realBoundingRect.center().x() - readImd->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readImd->getRealBoundingRect().height());
    readImd->setPos(newPos);
    emit addReadImdSuccessed(readImd);
    rcaScene->endPushMacro();
    return true;
}

bool ElementPort::pasteNewReadRch(ModuleReadRch &copyReadRch)
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    rcaScene->beginPushMacro("paste new read rch");
    AddReadRchCommand* cmdAddRRch = new AddReadRchCommand(rcaScene,copyReadRch.getReadRchIndex(),this);
    rcaScene->pushCmdAddRRch(cmdAddRRch);
    ModuleReadRch* readRch = cmdAddRRch->getReadRchModule();


    QPointF newPos = QPointF(realBoundingRect.center().x() - readRch->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readRch->getRealBoundingRect().height());
    readRch->setPos(newPos);
    rcaScene->endPushMacro();
    emit addReadRchSuccessed(readRch);
    return true;
}

bool ElementPort::pasteNewReadLfsr(ModuleReadLfsr &copyReadLfsr)
{
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    rcaScene->beginPushMacro("paste new read rch");
    AddReadLfsrCommand* cmdAddRLfsr = new AddReadLfsrCommand(rcaScene,copyReadLfsr.getReadLfsrIndex(),this);
    rcaScene->pushCmdAddRLfsr(cmdAddRLfsr);
    ModuleReadLfsr* readLfsr = cmdAddRLfsr->getReadLfsrModule();


    QPointF newPos = QPointF(realBoundingRect.center().x() - readLfsr->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readLfsr->getRealBoundingRect().height());
    readLfsr->setPos(newPos);
    rcaScene->endPushMacro();
    emit addReadLfsrSuccessed(readLfsr);
    return true;
}

bool ElementPort::pasteNewWriteMem(ModuleWriteMem &copyWriteMem)
{
    BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(this->parentItem());
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWMemFulled) return false;   //add write mem failed when write mem fulled.

            int curIndex=0;
            int copyWriteMemIndex = copyWriteMem.getWriteMemIndex();
            for(;curIndex<findedRcu->wMemExistList.size();curIndex++)
            {
                if(copyWriteMemIndex == findedRcu->wMemExistList.at(curIndex))
                {
                    return false;   //已经存在此writeMemIndex , 不用再new。
                }
            }


            AddWriteMemCommand* cmdAddWMem = new AddWriteMemCommand(rcaScene,copyWriteMemIndex,this);
            rcaScene->pushCmdAddWMem(cmdAddWMem);
            ModuleWriteMem *writeMem = cmdAddWMem->getWriteMemModule();

//            findedRcu->wMemExistList.insert(curIndex,curIndex);
//            ModuleWriteMem *writeMem = new ModuleWriteMem(curIndex,this);
//            connect(writeMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
//            addWire(writeMem);

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeMem->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeMem->setPos(newPos);


            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            if(elementBfu)
            {
                if(elementBfu->outPortX == this)
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBfuX;
                    writeMem->inputIndex = elementBfu->getIndexInRcu();

                }
                else if(elementBfu->outPortY == this)
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBfuY;
                    writeMem->inputIndex = elementBfu->getIndexInRcu();

                }
            }
            else if(elementCustomPE)
            {
                for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                    if(elementCustomPE->m_inputPortVector.at(i) == this)
                    {
                        //匹配成功
                        writeMem->inputType = InputPort_CurrentCustomPE;
                        writeMem->inputIndex = elementCustomPE->getIndexInRcu();
                    }
                }
            }
            else if(elementSbox)
            {
                if((elementSbox->outPort0 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();

                }
                else if((elementSbox->outPort1 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;

                }
                else if((elementSbox->outPort2 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;

                }
                else if((elementSbox->outPort3 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentSbox;
                    writeMem->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;

                }
            }
            else if (elementBenes)
            {
                if((elementBenes->outPort0 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();

                }
                else if((elementBenes->outPort1 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;

                }
                else if((elementBenes->outPort2 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;

                }
                else if((elementBenes->outPort3 == this))
                {
                    //匹配成功
                    writeMem->inputType = InputPort_CurrentBenes;
                    writeMem->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;

                }
            }

            emit addWriteMemSuccessed(writeMem);

            return true;
        }
    }
    return false;
}

bool ElementPort::pasteNewWriteRch(ModuleWriteRch &copyWriteRch)
{
    Q_UNUSED(copyWriteRch)
    BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(this->parentItem());
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWRchFulled) return false;   //add write Rch failed when write Rch fulled.
            int copyWriteRchIndex = copyWriteRch.getWriteRchIndex();
            int copyAddress = copyWriteRch.getWriteAddress();
            for(int curIndex=0; curIndex<findedRcu->wRchExistList.size(); curIndex++)
            {
                if(copyWriteRchIndex == findedRcu->wRchExistList.at(curIndex))
                {
                    return false;   //已经存在此WriteRchIndex , 不用再new。
                }
            }

            rcaScene->beginPushMacro("paste new write rch");
            AddWriteRchCommand* cmdAddWRch = new AddWriteRchCommand(rcaScene, copyWriteRchIndex, copyAddress, this);
            rcaScene->pushCmdAddWRch(cmdAddWRch);
            ModuleWriteRch *writeRch = cmdAddWRch->getWriteRchModule();

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeRch->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeRch->setPos(newPos);

            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            if(elementBfu)
            {
                if(elementBfu->outPortX == this)
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBfuX;
                    writeRch->inputIndex = elementBfu->getIndexInRcu();
                }
                else if(elementBfu->outPortY == this)
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBfuY;
                    writeRch->inputIndex = elementBfu->getIndexInRcu();
                }
            }
            else if(elementCustomPE)
            {
                for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                    if(elementCustomPE->m_inputPortVector.at(i) == this)
                    {
                        //匹配成功
                        writeRch->inputType = InputPort_CurrentCustomPE;
                        writeRch->inputIndex = elementCustomPE->getIndexInRcu();
                    }
                }
            }
            else if(elementSbox)
            {
                if((elementSbox->outPort0 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();
                }
                else if((elementSbox->outPort1 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;
                }
                else if((elementSbox->outPort2 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;
                }
                else if((elementSbox->outPort3 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentSbox;
                    writeRch->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;
                }
            }
            else if (elementBenes)
            {
                if((elementBenes->outPort0 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();
                }
                else if((elementBenes->outPort1 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;
                }
                else if((elementBenes->outPort2 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;
                }
                else if((elementBenes->outPort3 == this))
                {
                    //匹配成功
                    writeRch->inputType = InputPort_CurrentBenes;
                    writeRch->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;
                }
            }

            rcaScene->endPushMacro();
            emit addWriteRchSuccessed(writeRch);

            return true;
        }
    }
    return false;
}

bool ElementPort::pasteNewWriteLfsr(ModuleWriteLfsr &copyWriteLfsr)
{
    Q_UNUSED(copyWriteLfsr)
    BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(this->parentItem());
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWLfsrFulled) return false;   //add write Lfsr failed when write Lfsr fulled.
            int copyWriteLfsrIndex = copyWriteLfsr.getWriteLfsrIndex();
            for(int curIndex=0; curIndex<findedRcu->wLfsrExistList.size(); curIndex++)
            {
                if(copyWriteLfsrIndex == findedRcu->wLfsrExistList.at(curIndex))
                {
                    return false;   //已经存在此WriteLfsrIndex , 不用再new。
                }
            }

            rcaScene->beginPushMacro("paste new write rch");
            AddWriteLfsrCommand* cmdAddWLfsr = new AddWriteLfsrCommand(rcaScene, copyWriteLfsrIndex, this);
            rcaScene->pushCmdAddWLfsr(cmdAddWLfsr);
            ModuleWriteLfsr *writeLfsr = cmdAddWLfsr->getWriteLfsrModule();

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeLfsr->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeLfsr->setPos(newPos);

            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            if(elementBfu)
            {
                if(elementBfu->outPortX == this)
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBfuX;
                    writeLfsr->inputIndex = elementBfu->getIndexInRcu();
                }
                else if(elementBfu->outPortY == this)
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBfuY;
                    writeLfsr->inputIndex = elementBfu->getIndexInRcu();
                }
            }
            else if(elementCustomPE)
            {
                for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                    if(elementCustomPE->m_inputPortVector.at(i) == this)
                    {
                        //匹配成功
                        writeLfsr->inputType = InputPort_CurrentCustomPE;
                        writeLfsr->inputIndex = elementCustomPE->getIndexInRcu();
                    }
                }
            }
            else if(elementSbox)
            {
                if((elementSbox->outPort0 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();
                }
                else if((elementSbox->outPort1 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;
                }
                else if((elementSbox->outPort2 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;
                }
                else if((elementSbox->outPort3 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentSbox;
                    writeLfsr->inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;
                }
            }
            else if (elementBenes)
            {
                if((elementBenes->outPort0 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();
                }
                else if((elementBenes->outPort1 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;
                }
                else if((elementBenes->outPort2 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;
                }
                else if((elementBenes->outPort3 == this))
                {
                    //匹配成功
                    writeLfsr->inputType = InputPort_CurrentBenes;
                    writeLfsr->inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;
                }
            }

            rcaScene->endPushMacro();
            emit addWriteLfsrSuccessed(writeLfsr);

            return true;
        }
    }
    return false;
}

void ElementPort::copyOtherPort(ElementPort &copyPort)
{
    portPressedPosList = copyPort.portPressedPosList;
    portInfletionPosList = copyPort.portInfletionPosList;

    for(int i=0;i<copyPort.wireThisPort.count();i++)
    {
        QGraphicsItem* itemTemp = copyPort.wireThisPort.at(i);
        if(itemTemp)
        {
            ModuleReadMem* copyReadMem = dynamic_cast<ModuleReadMem*>(itemTemp);
            if(copyReadMem)
            {
                copyOtherReadMem(*copyReadMem);
                continue;
            }

            ModuleWriteMem* copyWriteMem = dynamic_cast<ModuleWriteMem*>(itemTemp);
            if(copyWriteMem)
            {
                copyOtherWriteMem(*copyWriteMem);
                continue;
            }

            ModuleReadImd* copyReadImd = dynamic_cast<ModuleReadImd*>(itemTemp);
            if(copyReadImd)
            {
                copyOtherReadImd(*copyReadImd);
                continue;
            }

            ModuleReadRch* copyReadRch = dynamic_cast<ModuleReadRch*>(itemTemp);
            if(copyReadRch)
            {
                copyOtherReadRch(*copyReadRch);
                continue;
            }

            ModuleWriteRch* copyWriteRch = dynamic_cast<ModuleWriteRch*>(itemTemp);
            if(copyWriteRch)
            {
                copyOtherWriteRch(*copyWriteRch);
                continue;
            }

            ModuleReadLfsr* copyReadLfsr = dynamic_cast<ModuleReadLfsr*>(itemTemp);
            if(copyReadLfsr)
            {
                copyOtherReadLfsr(*copyReadLfsr);
                continue;
            }

            ModuleWriteLfsr* copyWriteLfsr = dynamic_cast<ModuleWriteLfsr*>(itemTemp);
            if(copyWriteLfsr)
            {
                copyOtherWriteLfsr(*copyWriteLfsr);
                continue;
            }
        }
    }
}

bool ElementPort::copyOtherReadMem(ModuleReadMem &copyReadMem)
{
    if(inSignal) return false;

    ModuleReadMem *readMem = new ModuleReadMem(copyReadMem.getReadMemIndex(),this);
    connect(readMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
    setInSignalAllAttr(readMem);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readMem->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readMem->getRealBoundingRect().height());
    readMem->setPos(newPos);
    emit addReadMemSuccessed(readMem);
    return true;
}

bool ElementPort::copyOtherReadImd(ModuleReadImd &copyReadImd)
{
    if(inSignal) return false;

    ModuleReadImd *readImd = new ModuleReadImd(copyReadImd.getReadImdIndex(),this);
    connect(readImd,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRImdBeDeleted(BaseItem*)));
    setInSignalAllAttr(readImd);
    addWireOrOtherInputPort(readImd);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readImd->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readImd->getRealBoundingRect().height());
    readImd->setPos(newPos);
    emit addReadImdSuccessed(readImd);
    return true;
}

bool ElementPort::copyOtherReadRch(ModuleReadRch &copyReadRch)
{
    if(inSignal) return false;

    ModuleReadRch *readRch = new ModuleReadRch(copyReadRch.getReadRchIndex(),this);
    connect(readRch,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRRchBeDeleted(BaseItem*)));
    setInSignalAllAttr(readRch);
    addWireOrOtherInputPort(readRch);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readRch->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readRch->getRealBoundingRect().height());
    readRch->setPos(newPos);
    emit addReadRchSuccessed(readRch);
    return true;
}

bool ElementPort::copyOtherReadLfsr(ModuleReadLfsr &copyReadLfsr)
{
    if(inSignal) return false;

    ModuleReadLfsr *readLfsr = new ModuleReadLfsr(copyReadLfsr.getReadLfsrIndex(),this);
    connect(readLfsr,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
    setInSignalAllAttr(readLfsr);
    addWireOrOtherInputPort(readLfsr);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readLfsr->getRealBoundingRect().width()/2,
                             realBoundingRect.top() - readLfsr->getRealBoundingRect().height());
    readLfsr->setPos(newPos);
    emit addReadLfsrSuccessed(readLfsr);
    return true;
}

bool ElementPort::copyOtherWriteMem(ModuleWriteMem &copyWriteMem)
{
    ModuleWriteMem *writeMem = new ModuleWriteMem(copyWriteMem.getWriteMemIndex(),this);
    connect(writeMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
    addWireOrMem(writeMem);

    return true;
}

bool ElementPort::copyOtherWriteRch(ModuleWriteRch &copyWriteRch)
{
    BaseArithUnit* arithUnitParent = dynamic_cast<BaseArithUnit*>(parentItem());
    if(arithUnitParent == NULL) return false;

    ModuleWriteRch *writeRch = new ModuleWriteRch(copyWriteRch.getWriteRchIndex(), this);
    writeRch->setWriteAddress(copyWriteRch.getWriteAddress());
    connect(writeRch,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRRchBeDeleted(BaseItem*)));
    addWireOrOtherInputPort(writeRch);

    return true;
}

bool ElementPort::copyOtherWriteLfsr(ModuleWriteLfsr &copyWriteLfsr)
{
    BaseArithUnit* arithUnitParent = dynamic_cast<BaseArithUnit*>(parentItem());
    if(arithUnitParent == NULL) return false;

    ModuleWriteLfsr *writeLfsr = new ModuleWriteLfsr(copyWriteLfsr.getWriteLfsrIndex(), this);
//    writeLfsr->setWriteAddress(copyWriteLfsr.getWriteAddress());
    connect(writeLfsr,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
    addWireOrOtherInputPort(writeLfsr);

    return true;
}

bool ElementPort::insertWMemToList(int index, int curIndex)
{
    BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if(parentBaseArith == NULL) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWMemFulled) return false;   //add write mem failed when write mem fulled.
            findedRcu->wMemExistList.insert(index,curIndex);
            return true;
        }
    }
    return false;
}

bool ElementPort::insertWRchToList(int index, int indexWRch)
{
    BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if(parentBaseArith == NULL) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWRchFulled) return false;   //add write Rch failed when write Rch fulled.
            findedRcu->wRchExistList.insert(index,indexWRch);
            return true;
        }
    }
    return false;
}

bool ElementPort::insertWLfsrToList(int index, int indexWLfsr)
{
    BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if(parentBaseArith == NULL) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWLfsrFulled) return false;   //add write Lfsr failed when write Lfsr fulled.
            findedRcu->wLfsrExistList.insert(index,indexWLfsr);
            return true;
        }
    }
    return false;
}

int ElementPort::getWireCountInPort()
{
    QL_DEBUG << wireThisPort;
    return wireThisPort.count();
}

bool ElementPort::isContainsWire(ElementWire *wire)
{
    if(wire == NULL)     return false;
    return wireThisPort.contains(wire);
}

//此函数为inport口校验outport口是否合法。
bool ElementPort::canConnectOutPort(ElementPort* outPort)
{
    QL_DEBUG;
    if(outPort == NULL)  return false;
    if(this->getPortDirection() != InputDirection) return false;
    if(outPort->portDirection != OutputDirection) return false;

    if(wireThisPort.count() > 0) return false;
    if((outPort->parentItem() == NULL) || (this->parentItem() == NULL) ) return false;

    QL_DEBUG;
    BaseArithUnit* outPortParentArith = dynamic_cast<BaseArithUnit*>(outPort->parentItem());
//    ModuleBcu* outPortParentBcu = dynamic_cast<ModuleBcu*>(outPort->parentItem());
    ModuleRcu* outPortParentRcu = dynamic_cast<ModuleRcu*>(outPort->parentItem());

    BaseArithUnit* thisParentArith = dynamic_cast<BaseArithUnit*>(this->parentItem());
//    ModuleBcu* thisParentBcu = dynamic_cast<ModuleBcu*>(this->parentItem());
    ModuleRcu* thisParentRcu = dynamic_cast<ModuleRcu*>(this->parentItem());

//    //如果两个port都来自arith
//    if(outPortParentArith && thisParentArith)
//    {
//        int outPortBcu = outPortParentArith->getIndexBcu();
//        int outPortRcu = outPortParentArith->getIndexRcu();

//        int thisBcu = thisParentArith->getIndexBcu();
//        int thisRcu = thisParentArith->getIndexRcu();

//        ModuleBcu* tempBcu = NULL;

//        if(scene())
//        {
//            RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
//            if(rcaScene)
//            {
//                tempBcu = rcaScene->findBcuFromScene(thisBcu);
//            }
//        }
//        //来自同一个bcu
//        if(outPortBcu == thisBcu && (outPortRcu == thisRcu || outPortRcu == thisRcu-1))
//        {
//            return true;
//        }
//        else
//        {
//            if(tempBcu)
//            {
//                //来自上一个bcu
//                if(outPortBcu == tempBcu->srcDataFromBcu)
//                {
//                    if(outPortRcu == 3 && thisRcu == 0)
//                    {
//                        return true;
//                    }
//                }
//            }
//        }
//    }
//    //如果outPort port来自bcu（即为bcu的in fifo），并且this来自arith
//    else if(outPortParentBcu && thisParentArith)
//    {
//        if(outPortParentBcu->getIndexBcu() == thisParentArith->getIndexBcu())
//        {
//            return true;
//        }
//    }
//    //如果outPort port来自arith，并且this来自bcu（即为bcu的out fifo）。
//    else if(outPortParentArith && thisParentBcu)
//    {
//        if((outPortParentArith->getIndexBcu() == thisParentBcu->getIndexBcu())
//                && outPortParentArith->getIndexRcu() == OUT_FIFO_SIGNAL_RCU_INDEX)
//        {
//            return true;
//        }
//    }

    //xf:如果两个port都来自arith
    if(outPortParentArith && thisParentArith)
    {
        int outPortRcu = outPortParentArith->getIndexRcu();
        int thisRcu = thisParentArith->getIndexRcu();
        //来自同一个rcu或下一个rcu
        if(outPortRcu == thisRcu || outPortRcu == thisRcu-1)
        {
            return true;
        }
    }
    //xf:如果outPort port来自rcu（即为rcu的in fifo），并且this来自arith
    else if(outPortParentRcu && thisParentArith)
    {
        if(outPortParentRcu->getIndexRcu() == thisParentArith->getIndexRcu())
        {
            return true;
        }
    }
    //xf:如果outPort port来自arith，并且this来自Rcu（即为bcu的out fifo）。
    else if(outPortParentArith && thisParentRcu)
    {
        if(outPortParentArith->getIndexRcu() == thisParentRcu->getIndexRcu())
        {
            return true;
        }
    }
    //outPort port 来自infifo，this来自out fifo，不允许连接。
    return false;
}

bool ElementPort::canConnectOutPort(InputPortType type, int index, ElementPort **resultPort)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return false;

    if(type == InputPort_NULL) return true;
    if(type == InputPort_Mem) return true;

    int bcuIndex = 0;
    int rcuIndex = 0;
    bool isPortValid = false;
    if(portAttr == PORT_FORM_OUTFIFO)
    {
        ModuleRcu* portParentIsRcu = dynamic_cast<ModuleRcu*>(parentItem());
        if(portParentIsRcu == NULL)   return false;

        bcuIndex = portParentIsRcu->getIndexBcu();
//        rcuIndex = OUT_FIFO_SIGNAL_RCU_INDEX;
        rcuIndex = portParentIsRcu->getIndexRcu();
        switch(type)
        {
        case InputPort_CurrentBfuX:
        case InputPort_CurrentBfuY:
        case InputPort_CurrentSbox:
        case InputPort_CurrentBenes:isPortValid = true;break;
        default:break;
        }
    }
    else
    {
        BaseArithUnit* portParentIsArith = dynamic_cast<BaseArithUnit*>(parentItem());
        if(portParentIsArith == NULL)   return false;
        switch(type)
        {
            case InputPort_CurrentBfuX:
            case InputPort_CurrentBfuY:
            case InputPort_CurrentSbox:
            case InputPort_CurrentBenes:
            case InputPort_InFifo:
            case InputPort_Mem:
            {
                bcuIndex = portParentIsArith->getIndexBcu();
                rcuIndex = portParentIsArith->getIndexRcu();
                isPortValid = true;
                break;
            }
            case InputPort_LastBfuX:
            case InputPort_LastBfuY:
            case InputPort_LastSbox:
            case InputPort_LastBenes:
            {
                //如果行号为0
                if(portParentIsArith->getIndexRcu() == 0)
                {
                    ModuleRcu* rcuTemp = rcaScene->findRcuFromScene(portParentIsArith->getIndexBcu(),portParentIsArith->getIndexRcu());
                    if(rcuTemp)
                    {
//                        bcuIndex = rcuTemp->srcDataFromBcu;
//                        rcuIndex = OUT_FIFO_SIGNAL_RCU_INDEX;
                        bcuIndex = rcuTemp->getIndexBcu();
                        rcuIndex = rcuTemp->getIndexRcu();
                        isPortValid = true;
                    }
                }
                else
                {
                    bcuIndex = portParentIsArith->getIndexBcu();
                    rcuIndex = portParentIsArith->getIndexRcu() - 1;
                    isPortValid = true;
                }
                break;
            }
            default:break;
        }
    }
    if(isPortValid)
    {
        switch(type)
        {
        case InputPort_CurrentBfuX:
        case InputPort_LastBfuX:
        {
            ElementBfu *elementBfu = rcaScene->findBfuFromScene(bcuIndex,rcuIndex,index);
            if((elementBfu) && (elementBfu->outPortX))
            {
                *resultPort = elementBfu->outPortX;
                return true;
            }
            break;
        }
        case InputPort_CurrentBfuY:
        case InputPort_LastBfuY:
        {
            ElementBfu *elementBfu = rcaScene->findBfuFromScene(bcuIndex,rcuIndex,index);
            if((elementBfu) && (elementBfu->outPortY))
            {
                *resultPort = elementBfu->outPortY;
                return true;
            }
            break;
        }
        case InputPort_CurrentSbox:
        case InputPort_LastSbox:
        {
            ElementSbox* elementSbox = rcaScene->findSboxFromScene(bcuIndex,rcuIndex,index/SBOX_OUTPUTCOUNT);
            if(elementSbox)
            {
                switch(index%SBOX_OUTPUTCOUNT)
                {
                case 0:
                {
                    if(elementSbox->outPort0)
                    {
                        *resultPort = elementSbox->outPort0;
                        return true;
                    }
                    break;
                }
                case 1:
                {
                    if(elementSbox->outPort1)
                    {
                        *resultPort = elementSbox->outPort1;
                        return true;
                    }
                    break;
                }
                case 2:
                {
                    if(elementSbox->outPort2)
                    {
                        *resultPort = elementSbox->outPort2;
                        return true;
                    }
                    break;
                }
                case 3:
                {
                    if(elementSbox->outPort3)
                    {
                        *resultPort = elementSbox->outPort3;
                        return true;
                    }
                    break;
                }
                default:break;
                }
            }
            break;
        }
        case InputPort_CurrentBenes:
        case InputPort_LastBenes:
        {
            ElementBenes* elementBenes = rcaScene->findBenesFromScene(bcuIndex,rcuIndex,index/SBOX_OUTPUTCOUNT);
            if(elementBenes)
            {
                switch(index%BENES_OUTPUTCOUNT)
                {
                case 0:
                {
                    if(elementBenes->outPort0)
                    {
                        *resultPort = elementBenes->outPort0;
                        return true;
                    }
                    break;
                }
                case 1:
                {
                    if(elementBenes->outPort1)
                    {
                        *resultPort = elementBenes->outPort1;
                        return true;
                    }
                    break;
                }
                case 2:
                {
                    if(elementBenes->outPort2)
                    {
                        *resultPort = elementBenes->outPort2;
                        return true;
                    }
                    break;
                }
                case 3:
                {
                    if(elementBenes->outPort3)
                    {
                        *resultPort = elementBenes->outPort3;
                        return true;
                    }
                    break;
                }
                default:break;
                }
            }
            break;
        }
        case InputPort_InFifo:
        {
//            ModuleBcu* bcuTemp = rcaScene->findBcuFromScene(bcuIndex);
            ModuleRcu* rcuTemp = rcaScene->findRcuFromScene(bcuIndex, rcuIndex);
            if((rcuTemp) && (rcuTemp->inFifo0) && (rcuTemp->inFifo1) && (rcuTemp->inFifo2) && (rcuTemp->inFifo3))
            {
                switch(index)
                {
                    case 0:*resultPort = rcuTemp->inFifo0; return true;
                    case 1:*resultPort = rcuTemp->inFifo1; return true;
                    case 2:*resultPort = rcuTemp->inFifo2; return true;
                    case 3:*resultPort = rcuTemp->inFifo3; return true;
                    default:break;
                }
            }
            break;
        }
//        case InputPort_Mem:
//        {
//            ModuleRcu* rcuTemp = rcaScene->findRcuFromScene(bcuIndex,rcuIndex);
//            if(rcuTemp)
//            {
//                return true;
//            }
//            break;
//        }
        default:break;
        }
    }
    return false;
}

int ElementPort::deletePrevLinePortIn()
{
    int result = 0;
    if(this->portDirection != InputDirection) return result;
    switch(inputType)
    {
        case InputPort_LastBfuX:
        case InputPort_LastBfuY:
        case InputPort_LastSbox:
        case InputPort_LastBenes:
        {
            result = deleteAllConnectWire();
            update();
//            clearInSignalAllAttr();
        }
        break;
        default:break;
    }
    return result;
}

int ElementPort::deleteNextLinePortOut()
{
    int result = 0;
    if(this->portDirection != OutputDirection) return result;

    QL_DEBUG;
    QList<ElementWire *> dleWireList;
    dleWireList.clear();
    QListIterator<QGraphicsItem*> itemListTemp(wireThisPort);
    while (itemListTemp.hasNext())
    {
        QL_DEBUG;
        QGraphicsItem *item = itemListTemp.next();
        if(item)
        {
            ElementWire* wire = dynamic_cast<ElementWire*>(item);
            if(wire)
            {
                ElementPort *outPort = wire->getOutPort();
                if(outPort && outPort->portDirection == InputDirection)
                {

                    switch(outPort->getInputType())
                    {
                        case InputPort_LastBfuX:
                        case InputPort_LastBfuY:
                        case InputPort_LastSbox:
                        case InputPort_LastBenes:
                        {
                            result++;
                            wireThisPort.removeOne(wire);
                            dleWireList.append(wire);
//                            delete wire;
                            update();
                        }
                        break;
                        default:break;
                    }
                }
            }
        }
    }
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && !dleWireList.isEmpty())
    {
        rcaScene->pushCmdWireDeleted(dleWireList);
    }
    return result;

}

void ElementPort::pushItemToDeletedStack(bool isForced)
{
    if(isForced)
    {
        deleteAllConnectWire();

        QList<QGraphicsItem *> childList = childItems();

        for(int i=0;i<childList.count();i++)
        {
           QGraphicsItem* itemTemp = childList.at(i);
           BaseItem* baseTemp = dynamic_cast<BaseItem*>(itemTemp);
           if(baseTemp &&
                   (baseTemp->getUnitId() == RMEM_ID ||
                    baseTemp->getUnitId() == WMEM_ID ||
                    baseTemp->getUnitId() == RRCH_ID ||
                    baseTemp->getUnitId() == WRCH_ID ||
                    baseTemp->getUnitId() == RLFSR_ID ||
                    baseTemp->getUnitId() == WLFSR_ID ||
                    baseTemp->getUnitId() == RIMD_ID))
           {
               baseTemp->pushItemToDeletedStack(true);
           }
        }
    }

    BaseItem::pushItemToDeletedStack(isForced);
}

//bool ElementPort::cleanAndCreateConnectPort(RcaGraphScene* rcaScene,InputPortType type, int index, ElementPort *inSignal)
//{
//    if(portDirection != InputDirection) return false;

//    //此时需要清空port原来的数据
//    deleteAllConnectWire();
//    clearInSignalAllAttr();
//    if(type == InputPort_NULL) return true;
//    //连接到其他的port。
//    if(type == InputPort_Mem)
//    {
//        addReadMemFromXml(index);
//        return true;
//    }
//    if(inSignal && rcaScene)
//    {
//        setInSignalAllAttr(inSignal);
//        rcaScene->addElementWireFromXml(this,inSignal);
//        return true;
//    }
//    return false;
//}

bool ElementPort::resetConnectPort(RcaGraphScene *rcaScene, InputPortType type, int index, ElementPort *inSignal)
{
    if(portDirection != InputDirection) return false;

    //此时需要清空port原来的数据
    deleteAllConnectWireAndMem();
    clearInSignalAllAttr();
    if(type == InputPort_NULL) return true;
    //连接到其他的port。
    if(type == InputPort_Mem)
    {
        addReadMemFromXml(index);
        return true;
    }
    if(inSignal && rcaScene)
    {
        setInSignalAllAttr(inSignal);
        rcaScene->addElementWireBySet(this,inSignal);
        return true;
    }
    return false;
}

QString ElementPort::getPortInflectPos()
{
    QString result = QString();
    if(OutputDirection == portDirection) return result;

//    portInfletionPosList.clear();
//    for(int k=0;k<wireThisPort.count();k++)
//    {
//        ElementWire* wire = dynamic_cast<ElementWire*>(wireThisPort.at(k));
//        if(wire != NULL)
//        {
//            resetInfletionPos(wire->infletionPosList);
//            break;
//        }
//    }

    for(int i=0;i<portInfletionPosList.count();i++)
    {
        QPointF pos = portInfletionPosList.at(i);
        result.append(QString("%1,%2;").arg(pos.x()).arg(pos.y()));
    }
    if(!result.isEmpty())
    {
        result = result.left(result.count() -1);
    }
    return result;
}

QString ElementPort::getPortPressedPos()
{
    QString result = QString();
    if(OutputDirection == portDirection) return result;

//    portPressedPosList.clear();
//    for(int k=0;k<wireThisPort.count();k++)
//    {
//        ElementWire* wire = dynamic_cast<ElementWire*>(wireThisPort.at(k));
//        if(wire != NULL)
//        {
//            resetPressedPos(wire->pressedPosList);
//            break;
//        }
//    }
    for(int i=0;i<portPressedPosList.count();i++)
    {
        QPointF pos = portPressedPosList.at(i);
        result.append(QString("%1,%2;").arg(pos.x()).arg(pos.y()));
    }
    if(!result.isEmpty())
    {
        result = result.left(result.count() -1);
    }
    return result;
}

void ElementPort::setInfletionPosByXml(QString str)
{
    if(str.isEmpty()) return;
    if(portDirection != InputDirection) return;

    QStringList strList = str.split(";");
    if(strList.isEmpty()) return;

    portInfletionPosList.clear();

    for(int i=0;i<strList.count();i++)
    {
        QStringList posStr = QString(strList.at(i)).split(",");
        if(posStr.count() == 2)
        {
            qreal posX = QString(posStr.at(0)).toDouble();
            qreal posY = QString(posStr.at(1)).toDouble();
            portInfletionPosList.append(QPointF(posX,posY));
        }
    }
//    QL_DEBUG << portInfletionPosList;
}

void ElementPort::resetInfletionPos(QList<QPointF> scenePosList)
{
    if(portDirection != InputDirection) return;

    portInfletionPosList.clear();
    for(int i=0;i<scenePosList.count();i++)
    {
        portInfletionPosList.append(mapFromScene(scenePosList.at(i)));
    }
//    QL_DEBUG << portInfletionPosList;
}

void ElementPort::setPressedPosByXml(QString str)
{
    if(portDirection != InputDirection) return;

    if(str.isEmpty()) return;
    QStringList strList = str.split(";");
    if(strList.isEmpty()) return;

    portPressedPosList.clear();

    for(int i=0;i<strList.count();i++)
    {
        QStringList posStr = QString(strList.at(i)).split(",");
        if(posStr.count() == 2)
        {
            qreal posX = QString(posStr.at(0)).toDouble();
            qreal posY = QString(posStr.at(1)).toDouble();
            portPressedPosList.append(QPointF(posX,posY));
        }
    }
//    QL_DEBUG << portPressedPosList;
}

void ElementPort::resetPressedPos(QList<QPointF> scenePosList)
{
    if(portDirection != InputDirection) return;

    portPressedPosList.clear();
    for(int i=0;i<scenePosList.count();i++)
    {
        portPressedPosList.append(mapFromScene(scenePosList.at(i)));
    }
//    QL_DEBUG << portPressedPosList;
}

void ElementPort::deleteReadRchDiffGroup(int rchGroupId)
{
    if(inSignal)
    {
        ModuleReadRch* readRch = dynamic_cast<ModuleReadRch*>(inSignal);
        if(readRch && readRch->getReadRchIndex()/READ_RCH_MAX_COUNT_ONE_GROUP != rchGroupId)
        {
            RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
            if(rcaScene == NULL) return;
            DeleteItemCommand* cmdDelItem = new DeleteItemCommand(rcaScene,readRch);
            rcaScene->pushCmdDeleteItem(cmdDelItem);
        }
    }
}

void ElementPort::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);
}

void ElementPort::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && rcaScene->getIsDebugMode())
    {
        rpu_esl_model::SBreak breakHandle;
        breakHandle.index.bcuIndex = dpInfo->bcu;
        breakHandle.index.rcuIndex = dpInfo->rcu;
        breakHandle.index.peIndex = dpInfo->id;
        if(isBreakPoint)
        {
            isBreakPoint = false;
            if(portDirection == InputDirection)
            {
                if(dpInfo->portId == 0)
                    breakHandle.inPort[0] = true;
                else if(dpInfo->portId == 1)
                    breakHandle.inPort[1] = true;
                else if(dpInfo->portId == 2)
                    breakHandle.inPort[2] = true;
                else if(dpInfo->portId == 3)
                    breakHandle.inPort[3] = true;
            }
            else
            {
                if(dpInfo->portId == 0)
                    breakHandle.outPort[0] = true;
                else if(dpInfo->portId == 1)
                    breakHandle.outPort[1] = true;
                else if(dpInfo->portId == 2)
                    breakHandle.outPort[2] = true;
                else if(dpInfo->portId == 3)
                    breakHandle.outPort[3] = true;
            }
            emit removeBreakPoint(breakHandle);
        }
        else
        {
            isBreakPoint = true;
            if(portDirection == InputDirection)
            {
                if(dpInfo->portId == 0)
                    breakHandle.inPort[0] = true;
                else if(dpInfo->portId == 1)
                    breakHandle.inPort[1] = true;
                else if(dpInfo->portId == 2)
                    breakHandle.inPort[2] = true;
                else if(dpInfo->portId == 3)
                    breakHandle.inPort[3] = true;
            }
            else
            {
                if(dpInfo->portId == 0)
                    breakHandle.outPort[0] = true;
                else if(dpInfo->portId == 1)
                    breakHandle.outPort[1] = true;
                else if(dpInfo->portId == 2)
                    breakHandle.outPort[2] = true;
                else if(dpInfo->portId == 3)
                    breakHandle.outPort[3] = true;
            }
            emit addBreakPoint(breakHandle);
        }
    }

    QGraphicsItem::mouseDoubleClickEvent(event);
}

void ElementPort::initDebugPortInfo(int typeId, int bcu, int rcu, int id, int portId, QString elementStr, bool isTextChanged)
{
    dpInfo->typeId = typeId;
    dpInfo->bcu = bcu;
    dpInfo->rcu = rcu;
    dpInfo->id = id;
    dpInfo->portId = portId;
    dpInfo->elementStr = "00000000";
    dpInfo->isTextChanged = isTextChanged;
}

bool ElementPort::isHadInSignal()
{
    return inSignal ? true : false;
}

BasePort *ElementPort::getInSignal()
{
    return inSignal ? inSignal : 0;
}


InputPortType ElementPort::getInputType()
{
    if(InputDirection == portDirection) return inputType;
    return InputPort_NULL;
}

void ElementPort::setInputType(InputPortType type)
{
    if(InputDirection == portDirection) inputType = type;
}

int ElementPort::getInputIndex()
{
     return inputIndex;
}

void ElementPort::setInputIndex(int index)
{
    inputIndex = index;
}

bool ElementPort::addWriteLfsrFromXml(InputPortType inType, int inIndex, int writeLfsrIndex, BaseArithUnit *parentBaseArith)
{
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWLfsrFulled) return false;   //add write Lfsr failed when write Lfsr fulled.

            int iExist=0;
            for(;iExist<findedRcu->wLfsrExistList.size();iExist++)
            {
                if(writeLfsrIndex == findedRcu->wLfsrExistList.at(iExist))
                {
                    return false;   //已经存在此writeLfsrIndex , 不用再new。
                }
            }

            QL_DEBUG << iExist << writeLfsrIndex;

            if(iExist >= findedRcu->wLfsrMaxCount) return false;
//            if(writeLfsrIndex >= (findedRcu->wLfsrMaxCount)*(findedRcu->getIndexBcu()+1)
//                || writeLfsrIndex < (findedRcu->wLfsrMaxCount)*(findedRcu->getIndexBcu())) return false;
            if(writeLfsrIndex >= findedRcu->wLfsrMaxCount) return false;


            AddWriteLfsrCommand* cmdAddWLfsr = new AddWriteLfsrCommand(rcaScene,writeLfsrIndex,this);
            rcaScene->pushCmdAddWLfsr(cmdAddWLfsr);
            ModuleWriteLfsr *writeLfsr = cmdAddWLfsr->getWriteLfsrModule();

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeLfsr->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeLfsr->setPos(newPos);
            writeLfsr->inputType = inType;
            writeLfsr->inputIndex = inIndex;

            emit addWriteLfsrSuccessed(writeLfsr);

            return true;
        }
    }
    return false;
}

//void ElementPort::setInputSignalFromXml()
//{

//}
bool ElementPort::addWriteRchFromXml(InputPortType inType, int inIndex, int writeRchIndex, int writeRchAddress, BaseArithUnit *parentBaseArith)
{
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWRchFulled) return false;   //add write Rch failed when write Rch fulled.

            int iExist=0;
            for(;iExist<findedRcu->wRchExistList.size();iExist++)
            {
                if(writeRchIndex == findedRcu->wRchExistList.at(iExist))
                {
                    return false;   //已经存在此writeRchIndex , 不用再new。
                }
            }

            QL_DEBUG << iExist << writeRchIndex;

            if(iExist >= findedRcu->wRchMaxCount) return false;
//            if(writeRchIndex >= (findedRcu->wRchMaxCount)*(findedRcu->getIndexBcu()+1)
//                || writeRchIndex < (findedRcu->wRchMaxCount)*(findedRcu->getIndexBcu())) return false;
            if(writeRchIndex >= findedRcu->wRchMaxCount) return false;


            AddWriteRchCommand* cmdAddWRch = new AddWriteRchCommand(rcaScene,writeRchIndex,writeRchAddress,this);
            rcaScene->pushCmdAddWRch(cmdAddWRch);
            ModuleWriteRch *writeRch = cmdAddWRch->getWriteRchModule();

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeRch->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeRch->setPos(newPos);
            writeRch->inputType = inType;
            writeRch->inputIndex = inIndex;
            writeRch->setWriteAddress(writeRchAddress);

            emit addWriteRchSuccessed(writeRch);

            return true;
        }
    }
    return false;
}

bool ElementPort::addWriteMemFromXml(InputPortType inType, int inIndex, int writeMemIndex, BaseArithUnit *parentBaseArith)
{
    if(parentBaseArith == NULL) return false;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
        if(findedRcu)
        {
            if(findedRcu->isWMemFulled) return false;   //add write mem failed when write mem fulled.

            int curIndex=0;
            for(;curIndex<findedRcu->wMemExistList.size();curIndex++)
            {
                if(writeMemIndex == findedRcu->wMemExistList.at(curIndex))
                {
                    return false;   //已经存在此writeMemIndex , 不用再new。
                }
            }

            QL_DEBUG << curIndex << writeMemIndex;

            if(writeMemIndex >= findedRcu->wMemMaxCount) return false;


            AddWriteMemCommand* cmdAddWMem = new AddWriteMemCommand(rcaScene,writeMemIndex,this);
            rcaScene->pushCmdAddWMem(cmdAddWMem);
            ModuleWriteMem *writeMem = cmdAddWMem->getWriteMemModule();

//            findedRcu->wMemExistList.insert(writeMemIndex,writeMemIndex);
//            ModuleWriteMem *writeMem = new ModuleWriteMem(writeMemIndex,this);
//            connect(writeMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
//            addWire(writeMem);

            QPointF newPos = QPointF(realBoundingRect.center().x() - writeMem->getRealBoundingRect().width()/2,
                                     realBoundingRect.bottom());
            writeMem->setPos(newPos);
            writeMem->inputType = inType;
            writeMem->inputIndex = inIndex;

            emit addWriteMemSuccessed(writeMem);

            return true;
        }
    }
    return false;
}

bool ElementPort::addReadMemFromXml(int readMemIndex)
{
    if(readMemIndex >= READ_MEM_MAX_COUNT) return false;
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadMemCommand* cmdAddRMem = new AddReadMemCommand(rcaScene,readMemIndex,this);
    rcaScene->pushCmdAddRMem(cmdAddRMem);
    ModuleReadMem* readMem = cmdAddRMem->getReadMemModule();

//    ModuleReadMem *readMem = new ModuleReadMem(readMemIndex,this);
//    connect(readMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
//    setInSignalAllAttr(readMem);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readMem->getRealBoundingRect().width()/2,
                       realBoundingRect.top() - readMem->getRealBoundingRect().height());
    readMem->setPos(newPos);
    emit addReadMemSuccessed(readMem);
    return true;
}

bool ElementPort::addReadImdFromXml(int readImdIndex)
{
    if(readImdIndex >= READ_IMD_MAX_COUNT) return false;
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadImdCommand* cmdAddRImd = new AddReadImdCommand(rcaScene,readImdIndex,this);
    rcaScene->pushCmdAddRImd(cmdAddRImd);
    ModuleReadImd* readImd = cmdAddRImd->getReadImdModule();

    QPointF newPos = QPointF(realBoundingRect.center().x() - readImd->getRealBoundingRect().width()/2,
                       realBoundingRect.top() - readImd->getRealBoundingRect().height());
    readImd->setPos(newPos);
    emit addReadImdSuccessed(readImd);
    return true;
}

bool ElementPort::addReadRchFromXml(int readRchIndex)
{
    if(readRchIndex >= READ_RCH_MAX_COUNT) return false;
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadRchCommand* cmdAddRRch = new AddReadRchCommand(rcaScene,readRchIndex,this);
    rcaScene->pushCmdAddRRch(cmdAddRRch);
    ModuleReadRch* readRch = cmdAddRRch->getReadRchModule();

//    ModuleReadRch *readRch = new ModuleReadRch(readRchIndex,this);
//    connect(readRch,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRRchBeDeleted(BaseItem*)));
//    setInSignalAllAttr(readRch);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readRch->getRealBoundingRect().width()/2,
                       realBoundingRect.top() - readRch->getRealBoundingRect().height());
    readRch->setPos(newPos);
    emit addReadRchSuccessed(readRch);
    return true;
}

bool ElementPort::addReadLfsrFromXml(int readLfsrIndex)
{
    if(readLfsrIndex >= READ_LFSR_MAX_COUNT) return false;
    if(inSignal) return false;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(!rcaScene) return false;
    QL_DEBUG;

    AddReadLfsrCommand* cmdAddRLfsr = new AddReadLfsrCommand(rcaScene,readLfsrIndex,this);
    rcaScene->pushCmdAddRLfsr(cmdAddRLfsr);
    ModuleReadLfsr* readLfsr = cmdAddRLfsr->getReadLfsrModule();

    QPointF newPos = QPointF(realBoundingRect.center().x() - readLfsr->getRealBoundingRect().width()/2,
                       realBoundingRect.top() - readLfsr->getRealBoundingRect().height());
    readLfsr->setPos(newPos);
    emit addReadLfsrSuccessed(readLfsr);
    return true;
}

bool ElementPort::addReadMemWithoutUndo(int readMemIndex)
{
    if(readMemIndex >= READ_MEM_MAX_COUNT) return false;
    if(inSignal) return false;
    QL_DEBUG;

    ModuleReadMem *readMem = new ModuleReadMem(readMemIndex,this);
    connect(readMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
    setInSignalAllAttr(readMem);

    QPointF newPos = QPointF(realBoundingRect.center().x() - readMem->getRealBoundingRect().width()/2,
                       realBoundingRect.top() - readMem->getRealBoundingRect().height());
    readMem->setPos(newPos);
    emit addReadMemSuccessed(readMem);
    return true;
}

void ElementPort::addWireOrMem(QGraphicsItem* wireOrMem)
{
    if(wireThisPort.contains(wireOrMem)) return;

    wireThisPort.append(wireOrMem);
    if(wireThisPort.size() > 0)
    {
        QL_DEBUG;
        setPortBrush(PORT_CONNECTED_BRUSH_FILLIN);
    }
    else
    {
        QL_DEBUG;
        setPortBrush(PORT_NORMAL_BRUSH_FILLIN);
    }
    update();
}

void ElementPort::addWireOrOtherInputPort(QGraphicsItem* wireOrMem)
{
    if(wireThisPort.contains(wireOrMem)) return;

    wireThisPort.append(wireOrMem);
    if(wireThisPort.size() > 0)
    {
        QL_DEBUG;
        setPortBrush(PORT_CONNECTED_BRUSH_FILLIN);
    }
    else
    {
        QL_DEBUG;
        setPortBrush(PORT_NORMAL_BRUSH_FILLIN);
    }
    update();
}

int ElementPort::deleteAllConnectWire()
{
    int result = 0;
    QL_DEBUG;
    QList<ElementWire *> dleWireList;
    dleWireList.clear();
    QListIterator<QGraphicsItem*> itemListTemp(wireThisPort);
    while (itemListTemp.hasNext())
    {
        QL_DEBUG;
        QGraphicsItem *item = itemListTemp.next();
        if(item)
        {
            ElementWire* wire = dynamic_cast<ElementWire*>(item);
            if(wire)
            {
                result++;
                wireThisPort.removeOne(item);
                dleWireList.append(wire);
//                delete wire;
            }
        }
    }
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && !dleWireList.isEmpty())
    {
        rcaScene->pushCmdWireDeleted(dleWireList);
    }
    return result;
}

void ElementPort::deleteAllConnectWireAndMem()
{
    QL_DEBUG;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene == NULL) return;

    QList<ElementWire *> dleWireList;
    dleWireList.clear();
    QListIterator<QGraphicsItem*> itemListTemp(wireThisPort);
    while (itemListTemp.hasNext())
    {
        QL_DEBUG;
        QGraphicsItem *item = itemListTemp.next();
        if(item)
        {
            ElementWire* wire = dynamic_cast<ElementWire*>(item);
            //增加删除mem的操作。
            ModuleReadMem* readMem = dynamic_cast<ModuleReadMem*>(item);
            ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(item);
            if(wire)
            {
                wireThisPort.removeOne(item);
                dleWireList.append(wire);
            }
            else if(readMem || writeMem)
            {
                BaseItem* baseItem = dynamic_cast<BaseItem*>(item);
                DeleteItemCommand* cmdDelItem = new DeleteItemCommand(rcaScene,baseItem);
                rcaScene->pushCmdDeleteItem(cmdDelItem);
            }
        }
    }

    if(!dleWireList.isEmpty())
    {
        rcaScene->pushCmdWireDeleted(dleWireList);
    }
}

int ElementPort::deleteAllConnectWireWithoutUndo()
{
    int result = 0;
    QL_DEBUG;
    QListIterator<QGraphicsItem*> itemListTemp(wireThisPort);
    while (itemListTemp.hasNext())
    {
        QL_DEBUG;
        QGraphicsItem *item = itemListTemp.next();
        if(item)
        {
            ElementWire* wire = dynamic_cast<ElementWire*>(item);
            if(wire)
            {
                result++;
                wireThisPort.removeOne(item);
                delete wire;
//                wire->deleteLater();
            }
        }
    }
    return result;
}

void ElementPort::resetConnectOutPortInputIndex(int index)
{
    if(portDirection == OutputDirection)
    {
        QListIterator<QGraphicsItem*> itemListTemp(wireThisPort);
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementWire* wire = dynamic_cast<ElementWire*>(item);
                if(wire)
                {
                    ElementPort* outPort = dynamic_cast<ElementPort*>(wire->getOutPort());
                    if(outPort)
                    {
                        outPort->setInputIndex(index);
                    }
                }
            }
        }
    }
}

void ElementPort::deleteWireOrMem(QGraphicsItem *wireOrMem)
{
    wireThisPort.removeOne(wireOrMem);
    if(wireThisPort.size() > 0)
    {
        QL_DEBUG;
        setPortBrush(PORT_CONNECTED_BRUSH_FILLIN);
    }
    else
    {
        QL_DEBUG;
        setPortBrush(PORT_NORMAL_BRUSH_FILLIN);
    }
    update();
}

void ElementPort::handleWRMemBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG << deletedItem;
    if(!deletedItem) return;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if((baseArith) && (rcaScene))
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(baseArith->getIndexBcu(),baseArith->getIndexRcu());
        if(findedRcu)
        {
            ModuleWriteMem* deletedWriteMem = dynamic_cast<ModuleWriteMem*>(deletedItem);
            if(deletedWriteMem)
            {
                int indexWriteMem = deletedWriteMem->getWriteMemIndex();
                if(indexWriteMem >= findedRcu->wMemMaxCount) return;

                QL_DEBUG << findedRcu->wMemExistList;
                if(findedRcu->wMemExistList.contains(indexWriteMem))
                {
                    findedRcu->wMemExistList.removeOne(indexWriteMem);
                    disconnect(deletedWriteMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
                    deleteWireOrMem(deletedWriteMem);
                    rcaScene->isItemBakBeDeleted(deletedWriteMem);
                    QL_DEBUG;
                    emit rcaScene->deleteWriteMemSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedWriteMem);
                }
            }
            else
            {
                ModuleReadMem* deletedReadMem = dynamic_cast<ModuleReadMem*>(deletedItem);
                if(deletedReadMem)
                {
                    disconnect(deletedReadMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRMemBeDeleted(BaseItem*)));
                    deleteWireOrMem(deletedReadMem);
                    clearInSignalAllAttr();
                    rcaScene->isItemBakBeDeleted(deletedReadMem);
                    emit rcaScene->deleteReadMemSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedReadMem);
                }
            }
        }
    }
}

void ElementPort::handleRImdBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG << deletedItem;
    if(!deletedItem) return;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if((baseArith) && (rcaScene))
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(baseArith->getIndexBcu(),baseArith->getIndexRcu());
        if(findedRcu)
        {
            ModuleReadImd* deletedReadImd = dynamic_cast<ModuleReadImd*>(deletedItem);
            if(deletedReadImd)
            {
                disconnect(deletedReadImd,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRImdBeDeleted(BaseItem*)));
                deleteWireOrMem(deletedReadImd);
                clearInSignalAllAttr();
                rcaScene->isItemBakBeDeleted(deletedReadImd);
                emit rcaScene->deleteReadImdSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedReadImd);
            }
        }
    }
}

void ElementPort::handleWRRchBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG << deletedItem;
    if(!deletedItem) return;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if((baseArith) && (rcaScene))
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(baseArith->getIndexBcu(),baseArith->getIndexRcu());
        if(findedRcu)
        {
            ModuleWriteRch* deletedWriteRch = dynamic_cast<ModuleWriteRch*>(deletedItem);
            if(deletedWriteRch)
            {
                int indexWriteRch = deletedWriteRch->getWriteRchIndex();
                if(indexWriteRch >= findedRcu->wRchMaxCount) return;

                QL_DEBUG << findedRcu->wRchExistList;
                if(findedRcu->wRchExistList.contains(indexWriteRch))
                {
                    findedRcu->wRchExistList.removeOne(indexWriteRch);
                    disconnect(deletedWriteRch,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRRchBeDeleted(BaseItem*)));
                    deleteWireOrMem(deletedWriteRch);
                    rcaScene->isItemBakBeDeleted(deletedWriteRch);
                    QL_DEBUG;
                    emit rcaScene->deleteWriteRchSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedWriteRch);
                }
            }
            else
            {
                ModuleReadRch* deletedReadRch = dynamic_cast<ModuleReadRch*>(deletedItem);
                if(deletedReadRch)
                {
                    disconnect(deletedReadRch,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRRchBeDeleted(BaseItem*)));
                    deleteWireOrMem(deletedReadRch);
                    clearInSignalAllAttr();
                    rcaScene->isItemBakBeDeleted(deletedReadRch);
                    emit rcaScene->deleteReadRchSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedReadRch);
                }
            }
        }
    }
}

void ElementPort::handleWRLfsrBeDeleted(BaseItem* deletedItem)
{
    QL_DEBUG << deletedItem;
    if(!deletedItem) return;

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    BaseArithUnit* baseArith = dynamic_cast<BaseArithUnit*>(parentItem());
    if((baseArith) && (rcaScene))
    {
        ModuleRcu* findedRcu = rcaScene->findRcuFromScene(baseArith->getIndexBcu(),baseArith->getIndexRcu());
        if(findedRcu)
        {
            ModuleWriteLfsr* deletedWriteLfsr = dynamic_cast<ModuleWriteLfsr*>(deletedItem);
            if(deletedWriteLfsr)
            {
                int indexWriteLfsr = deletedWriteLfsr->getWriteLfsrIndex();
                if(indexWriteLfsr >= findedRcu->wLfsrMaxCount) return;

                QL_DEBUG << findedRcu->wLfsrExistList;
                if(findedRcu->wLfsrExistList.contains(indexWriteLfsr))
                {
                    findedRcu->wLfsrExistList.removeOne(indexWriteLfsr);
                    disconnect(deletedWriteLfsr,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
                    deleteWireOrMem(deletedWriteLfsr);
                    rcaScene->isItemBakBeDeleted(deletedWriteLfsr);
                    QL_DEBUG;
                    emit rcaScene->deleteWriteLfsrSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedWriteLfsr);
                }
            }
            else
            {
                ModuleReadLfsr* deletedReadLfsr = dynamic_cast<ModuleReadLfsr*>(deletedItem);
                if(deletedReadLfsr)
                {
                    disconnect(deletedReadLfsr,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
                    deleteWireOrMem(deletedReadLfsr);
                    clearInSignalAllAttr();
                    rcaScene->isItemBakBeDeleted(deletedReadLfsr);
                    emit rcaScene->deleteReadLfsrSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),deletedReadLfsr);
                }
            }
        }
    }
}

//bool ElementPort::resetInputSignal(InputPortType inType, int inIndex)
//{
//    if(scene())
//    {
//        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
//        if(rcaScene)
//        {
//            QL_DEBUG << inType << inIndex;
//            return rcaScene->resetInputPortConnectWire(this,inType,inIndex);
//        }
//    }
//    return false;
//}

bool ElementPort::connectInputSignal(BasePort* inPort)
{
    QL_DEBUG;
    if((inPort) && (InputDirection == portDirection) && (inSignal == NULL))
    {
        inSignal = inPort;
        //addWire(inPort);
        return true;
    }
    return false;
}

//调用此函数时必须inSignal为NULL
bool ElementPort::setInSignalAllAttr(BasePort *inPort)
{
    QL_DEBUG;
    if((inPort) && (InputDirection == portDirection))
    {
        if((inSignal == NULL) && parentItem())
        {
            inputType = InputPort_NULL;
            inputIndex = 0;
            BaseArithUnit* thisParentArithUnit = dynamic_cast<BaseArithUnit*>(parentItem());
            ModuleRcu* thisParentRcu = dynamic_cast<ModuleRcu*>(parentItem());
            //判断this port的parent是arith还是bcu。
            if(thisParentRcu)
            {
                //如果是bcu，则inport只能来自当前行（即rcu=3）的算子。并且this只能是outfifo
                ElementPort* tempPort = dynamic_cast<ElementPort*>(inPort);
                if(tempPort && tempPort->parentItem())
                {
                    BaseArithUnit* inPortParentArith = dynamic_cast<BaseArithUnit*>(tempPort->parentItem());
                    if(inPortParentArith /*&& (inPortParentArith->getIndexRcu() == OUT_FIFO_SIGNAL_RCU_INDEX)*/)
                    {
                        ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(inPortParentArith);
                        ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(inPortParentArith);
                        ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(inPortParentArith);
                        ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(inPortParentArith);
                        if(elementBfu)
                        {
                            if(elementBfu->outPortX == tempPort)
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentBfuX;
                                inputIndex = elementBfu->getIndexInRcu();
                                //addWire(inPort);
                                return true;
                            }
                            else if(elementBfu->outPortY == tempPort)
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentBfuY;
                                inputIndex = elementBfu->getIndexInRcu();
                                //addWire(inPort);
                                return true;
                            }
                        }
                        else if(elementCustomPE)
                        {
                            for (int i = 0; i < elementCustomPE->m_inputPortVector.size(); ++i) {
                                if(elementCustomPE->m_inputPortVector.at(i) == tempPort)
                                {
                                    //匹配成功
                                    inSignal = inPort;
                                    inputType = InputPort_CurrentCustomPE;
                                    inputIndex = elementCustomPE->outputNum()*elementCustomPE->getIndexInRcu() + i;
                                    //addWire(inPort);
                                    return true;
                                }
                            }
                        }
                        else if(elementSbox)
                        {
                            if((elementSbox->outPort0 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentSbox;
                                inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();
                                //addWire(inPort);
                                return true;
                            }
                            else if((elementSbox->outPort1 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentSbox;
                                inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;
                                //addWire(inPort);
                                return true;
                            }
                            else if((elementSbox->outPort2 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentSbox;
                                inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;
                                //addWire(inPort);
                                return true;
                            }
                            else if((elementSbox->outPort3 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentSbox;
                                inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;
                                //addWire(inPort);
                                return true;
                            }
                        }
                        else if (elementBenes)
                        {
                            if((elementBenes->outPort0 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentBenes;
                                inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();
                                //addWire(inPort);
                                return true;
                            }
                            else if((elementBenes->outPort1 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentBenes;
                                inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;
                                //addWire(inPort);
                                return true;
                            }
                            else if((elementBenes->outPort2 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentBenes;
                                inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;
                                //addWire(inPort);
                                return true;
                            }
                            else if((elementBenes->outPort3 == tempPort))
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_CurrentBenes;
                                inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;
                                //addWire(inPort);
                                return true;
                            }
                        }
                    }

                }
            }
            //如果是arith，则inport只能来自当前行arith或者是上一行的arith或者是rcu的fifo或者是readmem。
            else if(thisParentArithUnit)
            {
                ModuleReadMem* inReadMem = dynamic_cast<ModuleReadMem*>(inPort);
                ModuleReadImd* inReadImd = dynamic_cast<ModuleReadImd*>(inPort);
                ModuleReadRch* inReadRch = dynamic_cast<ModuleReadRch*>(inPort);
                ModuleReadLfsr* inReadLfsr = dynamic_cast<ModuleReadLfsr*>(inPort);
                ElementPort* tempPort = dynamic_cast<ElementPort*>(inPort);

                //inport 可能来自readMem 也可能来自其他的port口。
                if(inReadMem)
                {
                    //来自readMem，则设置为readMem。
                    //匹配成功
                    inSignal = inReadMem;
                    inputType = InputPort_Mem;
                    inputIndex = inReadMem->getReadMemIndex();
                    addWireOrMem(inReadMem);
                    return true;
                }
                //inport 可能来自readImd 也可能来自其他的port口。
                else if(inReadImd)
                {
                    //来自readMem，则设置为readMem。
                    //匹配成功
                    inSignal = inReadImd;
                    inputType = InputPort_Imd;
                    inputIndex = inReadImd->getReadImdIndex();
                    addWireOrMem(inReadImd);
                    return true;
                }
                //inport 可能来自readRch 也可能来自其他的port口。
                else if(inReadRch)
                {
                    //来自readMem，则设置为readMem。
                    //匹配成功
                    inSignal = inReadRch;
                    inputType = InputPort_Rch;
                    inputIndex = inReadRch->getReadRchIndex();
                    addWireOrMem(inReadRch);
                    return true;
                }
                //inport 可能来自readLfsr 也可能来自其他的port口。
                else if(inReadLfsr)
                {
                    //来自readLfsr，则设置为readLfsr。
                    //匹配成功
                    inSignal = inReadLfsr;
                    inputType = InputPort_Lfsr;
                    inputIndex = inReadLfsr->getReadLfsrIndex();
                    addWireOrMem(inReadLfsr);
                    return true;
                }
                else if(tempPort)
                {
                    // tempPort 的parent有可能来自arithUnit也可能来自Bcu的fifo。
                    if(tempPort->parentItem())
                    {
                        BaseArithUnit* parentArithUnit = dynamic_cast<BaseArithUnit*>(tempPort->parentItem());
                        ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(tempPort->parentItem());
                        // tempPort 的parent来自arithUnit
                        if(parentArithUnit)
                        {
                            bool isFindArithSuccessed = false;  //匹配是否成功
                            bool isThisLine = false;    //是否来自当前行，ture 表示来自当前行，false表示来自上一行。
                            //来自当前行
                            if(parentArithUnit->getIndexRcu() == thisParentArithUnit->getIndexRcu())
                            {
                                isFindArithSuccessed = true;
                                isThisLine = true;
                            }
                            //来自上一行
                            //一种是rcu=0,则来自上一个bcu的最后一行rcu=3
                            //另外一种就是rcu!=0来自同一个bcu的上一行
                            else if(thisParentArithUnit->getIndexRcu() == 0)
                            {
                                //找到this bcu的srcDataFromBcu = insignal的bcu
                                RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                                if(rcaScene)
                                {
                                    ModuleBcu *bcuTemp = rcaScene->findBcuFromScene(thisParentArithUnit->getIndexBcu());
                                    if((bcuTemp) && (bcuTemp->srcDataFromBcu == parentArithUnit->getIndexBcu())
                                            &&(parentArithUnit->getIndexRcu() == (RCU_MAX_COUNT -1)))
                                    {
                                        isFindArithSuccessed = true;
                                        isThisLine = false;
                                    }
                                }
                            }
                            else if(parentArithUnit->getIndexRcu() == (thisParentArithUnit->getIndexRcu()-1))
                            {
                                isFindArithSuccessed = true;
                                isThisLine = false;
                            }
                            if(isFindArithSuccessed)
                            {
                                ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentArithUnit);
                                ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentArithUnit);
                                ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentArithUnit);
                                if(elementBfu)
                                {
                                    if(elementBfu->outPortX == tempPort)
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentBfuX : InputPort_LastBfuX;
                                        inputIndex = elementBfu->getIndexInRcu();
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if(elementBfu->outPortY == tempPort)
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentBfuY : InputPort_LastBfuY;
                                        inputIndex = elementBfu->getIndexInRcu();
                                        //addWire(inPort);
                                        return true;
                                    }
                                }
                                else if(elementSbox)
                                {
                                    if((elementSbox->outPort0 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentSbox : InputPort_LastSbox;
                                        inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu();
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if((elementSbox->outPort1 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentSbox : InputPort_LastSbox;
                                        inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 1;
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if((elementSbox->outPort2 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentSbox : InputPort_LastSbox;
                                        inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 2;
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if((elementSbox->outPort3 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentSbox : InputPort_LastSbox;
                                        inputIndex = SBOX_OUTPUTCOUNT * elementSbox->getIndexInRcu() + 3;
                                        //addWire(inPort);
                                        return true;
                                    }
                                }
                                else if (elementBenes)
                                {
                                    if((elementBenes->outPort0 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentBenes : InputPort_LastBenes;
                                        inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu();
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if((elementBenes->outPort1 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentBenes : InputPort_LastBenes;
                                        inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 1;
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if((elementBenes->outPort2 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentBenes : InputPort_LastBenes;
                                        inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 2;
                                        //addWire(inPort);
                                        return true;
                                    }
                                    else if((elementBenes->outPort3 == tempPort))
                                    {
                                        //匹配成功
                                        inSignal = inPort;
                                        inputType = isThisLine ? InputPort_CurrentBenes : InputPort_LastBenes;
                                        inputIndex = BENES_OUTPUTCOUNT * elementBenes->getIndexInRcu() + 3;
                                        //addWire(inPort);
                                        return true;
                                    }
                                }
                            }
                        }
                        // tempPort 的parent来自Bcu的fifo。
                        else if(parentRcu)
                        {
                            if(parentRcu->inFifo0 == tempPort)
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_InFifo;
                                inputIndex = 0;
                                //addWire(inPort);
                                return true;
                            }
                            else if(parentRcu->inFifo1 == tempPort)
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_InFifo;
                                inputIndex = 1;
                                //addWire(inPort);
                                return true;
                            }
                            else if(parentRcu->inFifo2 == tempPort)
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_InFifo;
                                inputIndex = 2;
                                //addWire(inPort);
                                return true;
                            }
                            else if(parentRcu->inFifo3 == tempPort)
                            {
                                //匹配成功
                                inSignal = inPort;
                                inputType = InputPort_InFifo;
                                inputIndex = 3;
                                //addWire(inPort);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool ElementPort::clearInSignalAllAttr()
{
    QL_DEBUG;
    if(InputDirection == portDirection)
    {
        inSignal = NULL;
        inputType = InputPort_NULL;
        inputIndex = 0;
        setPortBrush(PORT_NORMAL_BRUSH_FILLIN);
        wireThisPort.clear();
        portPressedPosList.clear();
        portInfletionPosList.clear();
        oldPortPressedPosList.clear();
        oldPortInfletionPosList.clear();
        update();
        return true;
    }
    return false;
}
