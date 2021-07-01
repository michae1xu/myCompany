#include "ElementWire.h"

#include <QDebug>
#include <QMimeData>
#include <QtMath>
#include <QCursor>
#include <QGraphicsScene>
#include <QApplication>
#include "RcaGraphScene.h"

ElementWire::ElementWire(QPointF posAtScene, ElementPort *inSignal, QBrush penBrush,qreal penWidth,QGraphicsItem *parent)
    : QGraphicsPathItem (parent)
    , shapePathList(QList<QPainterPath>())
    , wirePath(QPainterPath())
    , wirePathPainting(QPainterPath())
    , pressedPonitF(QPointF())
    , isCollidedPorts(false)
//    , isChangedColliedPorts(true)
    , hadConnectPorts(QList<ElementPort*>())
    , posAtItem(PosNone)
    , indexHoverAtPathLine(-1)
    , inputSignal(NULL)
    , outPort(NULL)
    , collidedOutPortList(QList<ElementPort*>())
    , isWireValid(false)
    , wireUnselBrush(penBrush)
    , wirePen(QPen(penBrush,penWidth,Qt::SolidLine))
    , isWirePathChanged(false)
    , posIn2Out(QPointF())
    , isHadToShow(true)
    , lastCmd(NULL)
    , pressedPosList(QList<QPointF>())
    , infletionPosList(QList<QPointF>())
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptDrops(true);
    QPointF newPos = posAtScene;
    if(inSignal)
    {
        QL_DEBUG ;
        connectWithPort(inSignal);
    }
    QL_DEBUG << newPos << posAtScene;
    pressedPonitF = newPos;
    wirePathPainting.moveTo(newPos);
    wirePath.moveTo(newPos);

//    setZValue(qreal(10000));
    //    QL_DEBUG << "zValue:" << zValue();
}

ElementWire::~ElementWire()
{
    deleteCollidedPortsConnect();
    hadConnectPorts.clear();
    inputSignal = NULL;
    outPort = NULL;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        rcaScene->handleDeleteWire(this);
    }

    QL_DEBUG<<"~ElementWire()";
}

////添加新的sub的moveto的pos。
//void ElementWire::addSubMovePointF(QPointF posAtScene)
//{
//    QL_DEBUG;
//    QPointF newPos = posAtScene;
//    if(parentItem())
//    {
//        newPos = parentItem()->mapFromScene(posAtScene);
//    }
//    pressedPonitF = newPos;
//    wirePathPainting.moveTo(newPos);
//    wirePath.moveTo(newPos);
//    QL_DEBUG << newPos << posAtScene << wirePath;
//}

void ElementWire::addPointF(QPointF posAtScene)
{
    QL_DEBUG;
    QPointF newPos = posAtScene;
    if(parentItem())
    {
        newPos = parentItem()->mapFromScene(posAtScene);
    }

    wirePath = wirePathPainting;
    pressedPonitF = newPos;
    pressedPosList.append(pressedPonitF);
//    resetConnectCollidedPorts();
}
void ElementWire::movedPointF(QPointF posAtScene)
{
    QL_DEBUG;
    QPointF newPos = posAtScene;
    if(parentItem())
    {
        newPos = parentItem()->mapFromScene(posAtScene);
    }
    QPainterPath pathTemp;
    pathTemp = wirePath;
    isWireValid = false;


    pathTemp = autoPath(newPos,pathTemp);
//    if(judgePathIsValid(pathTemp))
    {
        QL_DEBUG;
        isWireValid =  true;
        wirePathPainting = pathTemp;
        shapePathList = translateToLineList(wirePathPainting);
        setPath(wirePathPainting);
//        resetConnectCollidedPorts();
    }
}
#define PressedWireWidth 4
QPainterPath ElementWire::shape() const
{
    QPainterPath path = QPainterPath();
    for(int i=0;i<shapePathList.count();i++)
    {
        if(shapePathList.at(i).elementCount() == 2)
        {
            if(shapePathList.at(i).elementAt(0).x == shapePathList.at(i).elementAt(1).x)
            {
                path.addRect(shapePathList.at(i).elementAt(0).x-PressedWireWidth,
                             qMin(shapePathList.at(i).elementAt(0).y,shapePathList.at(i).elementAt(1).y),
                             2*PressedWireWidth,
                             qAbs(shapePathList.at(i).elementAt(1).y - shapePathList.at(i).elementAt(0).y));
            }
            else
            {
                path.addRect(qMin(shapePathList.at(i).elementAt(0).x,shapePathList.at(i).elementAt(1).x),
                             shapePathList.at(i).elementAt(0).y - PressedWireWidth,
                             qAbs(shapePathList.at(i).elementAt(1).x - shapePathList.at(i).elementAt(0).x),
                             2*PressedWireWidth);
            }
        }
    }
    return path;
}

//void ElementWire::commitPath()
//{
//    if(judgePathIsValid(wirePath))
//    {
//        setPath(wirePath);
//        resetConnectCollidedPorts();
//    }
//    if(wirePath.elementCount() <= 1)
//    {
//        delete this;
//    }
////    isChangedColliedPorts = true;
//}

bool ElementWire::commitPath(ElementPort *endPort, QPointF posAtScene)
{
    QL_DEBUG << isWireValid << endPort << outPort;
    if(endPort == NULL) return false;
//    if(outPort) return false;
    if(!isWireValid) return false;


    QL_DEBUG;
    QPointF newPos = posAtScene;
    if(parentItem())
    {
        newPos = parentItem()->mapFromScene(posAtScene);
    }

    wirePath = wirePathPainting;
    pressedPonitF = newPos;
//    if(judgePathIsValid(wirePath))
    {
        shapePathList = translateToLineList(wirePath);
        setPath(wirePath);
        connectWithPort(endPort);
        resetInfletionPosByPath(wirePath);
        endPort->resetInfletionPos(infletionPosList);
        endPort->resetPressedPos(pressedPosList);
        setPosIn2Out();
//        resetConnectCollidedPorts();
        return true;
    }
    return false;
}

void ElementWire::addEndPortFromXml(ElementPort *endPort, QPointF endPosAtScene)
{
    QL_DEBUG;
    if(endPort == NULL) return;
    if(outPort) return;

    isHadToShow = false;    //从xml读出的wire不必要一直显示。

    QPointF endPos = endPosAtScene;
    if(parentItem())
    {
        endPos = parentItem()->mapFromScene(endPosAtScene);
    }
    QPainterPath pathTemp;
    pathTemp = wirePath;
    if(endPort->portInfletionPosList.isEmpty())
    {
        pathTemp = autoPath(endPos,pathTemp);
    }
    else
    {
//        pressedPosList.clear();
//        for(int i=0;i<endPort->portPressedPosList.count();i++)
//        {
//            pressedPosList.append(endPort->mapToScene(endPort->portPressedPosList.at(i)));
//        }

//        infletionPosList.clear();
//        for(int i=0;i<endPort->portInfletionPosList.count();i++)
//        {
//            infletionPosList.append(endPort->mapToScene(endPort->portInfletionPosList.at(i)));
//        }

        pressedPosList = mapPosListFromPort(endPort,endPort->portPressedPosList);
        infletionPosList = mapPosListFromPort(endPort,endPort->portInfletionPosList);

        pathTemp.moveTo(pressedPonitF);
        for(int i=0;i<infletionPosList.count();i++)
        {
            pathTemp.lineTo(infletionPosList.at(i));
            pathTemp.moveTo(infletionPosList.at(i));
        }
        pathTemp.lineTo(endPos);
    }

    pressedPonitF = endPos;
    wirePathPainting = pathTemp;
    wirePath = wirePathPainting;
    shapePathList = translateToLineList(wirePath);
    setPath(wirePath);
    connectWithPort(endPort);
}

void ElementWire::addEndPortBySet(ElementPort *endPort, QPointF endPosAtScene)
{
    QL_DEBUG;
    if(endPort == NULL) return;
    if(outPort) return;

    isHadToShow = false;    //从xml读出的wire不必要一直显示。

    QPointF endPos = endPosAtScene;
    if(parentItem())
    {
        endPos = parentItem()->mapFromScene(endPosAtScene);
    }
    QPainterPath pathTemp;
    pathTemp = wirePath;

    pathTemp = autoPath(endPos,pathTemp);
    pressedPonitF = endPos;
    wirePathPainting = pathTemp;
    wirePath = wirePathPainting;
    shapePathList = translateToLineList(wirePath);
    setPath(wirePath);
    connectWithPort(endPort);
}

void ElementWire::paintWireByUndoCmd(QList<QPointF> pressedList,QList<QPointF> infelctList)
{
    QL_DEBUG;
    if((inputSignal == NULL) || (outPort == NULL)) return;

    QPointF startPos = inputSignal->mapToScene(QPointF(inputSignal->boundingRect().center().x(),
                                                       inputSignal->getRealBoundingRect().bottom()));
    QPointF endPos = outPort->mapToScene(QPointF(outPort->boundingRect().center().x(),
                                                 outPort->getRealBoundingRect().top()));

    pressedPonitF = startPos;

    outPort->portPressedPosList = pressedList;
    outPort->portInfletionPosList = infelctList;
    QPainterPath pathTemp;
    if(outPort->portInfletionPosList.isEmpty())
    {
        pathTemp = autoPath(endPos,pathTemp);
    }
    else
    {
        pressedPosList.clear();
        for(int i=0;i<outPort->portPressedPosList.count();i++)
        {
            pressedPosList.append(outPort->mapToScene(outPort->portPressedPosList.at(i)));
        }

        infletionPosList.clear();
        for(int i=0;i<outPort->portInfletionPosList.count();i++)
        {
            infletionPosList.append(outPort->mapToScene(outPort->portInfletionPosList.at(i)));
        }

        pathTemp.moveTo(pressedPonitF);
        for(int i=0;i<infletionPosList.count();i++)
        {
            pathTemp.lineTo(infletionPosList.at(i));
            pathTemp.moveTo(infletionPosList.at(i));
        }
        pathTemp.lineTo(endPos);
    }
    pressedPonitF = endPos;
    wirePathPainting = pathTemp;
    wirePath = wirePathPainting;
    shapePathList = translateToLineList(wirePath);
    setPath(wirePath);
    update();
}

QPointF ElementWire::translateToHVLine(QPointF posTrs,QPointF posSrc)
{
    qreal xLength = qFabs(posTrs.x() - posSrc.x());
    qreal yLength = qFabs(posTrs.y() - posSrc.y());
    if(xLength > yLength)
    {
        return QPointF(posTrs.x(),posSrc.y());
    }
    return QPointF(posSrc.x(),posTrs.y());
}

#define START_PATH_SPACE  ENABLED_RCU_RECT_IN_SPACE/2-2
#define BETWEEN_PATH_SPACE  4
QPainterPath ElementWire::autoPath(QPointF newPos,QPainterPath srcPath)
{
//    QL_DEBUG;
    QPainterPath resultPath = srcPath;
    if(resultPath.elementCount()>0)
    {
        pressedPonitF = QPointF(resultPath.elementAt(resultPath.elementCount()-1).x,
                                resultPath.elementAt(resultPath.elementCount()-1).y);
        if(!resultPath.elementAt(resultPath.elementCount()-1).isMoveTo())
        {
            resultPath.moveTo(pressedPonitF);
        }
    }
    //如果path还没开始画线

    if(inputSignal == NULL) return resultPath;

    int startPathYSpace = START_PATH_SPACE;

    if(inputSignal->parentItem())
    {
        ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(inputSignal->parentItem());
        BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(inputSignal->parentItem());
        if(parentRcu)
        {
            if(parentRcu->inFifo0 == inputSignal) startPathYSpace += 0;
            else if(parentRcu->inFifo1 == inputSignal) startPathYSpace += BETWEEN_PATH_SPACE;
            else if(parentRcu->inFifo2 == inputSignal) startPathYSpace += 2*BETWEEN_PATH_SPACE;
            else if(parentRcu->inFifo3 == inputSignal) startPathYSpace += 3*BETWEEN_PATH_SPACE;
        }
        else if(parentBaseArith)
        {
            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(parentBaseArith);
            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(parentBaseArith);
            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(parentBaseArith);
            startPathYSpace = startPathYSpace/2;
            if(elementBfu)
            {
                if(elementBfu->outPortX == inputSignal)  startPathYSpace = startPathYSpace+ elementBfu->getIndexInRcu()%4*2*BETWEEN_PATH_SPACE;
                else if(elementBfu->outPortY == inputSignal) startPathYSpace = startPathYSpace + elementBfu->getIndexInRcu()%4*2*BETWEEN_PATH_SPACE + BETWEEN_PATH_SPACE;
            }
            if(elementSbox)
            {
                if(elementSbox->outPort0 == inputSignal)  startPathYSpace = startPathYSpace + elementSbox->getIndexInRcu()*BETWEEN_PATH_SPACE + BETWEEN_PATH_SPACE;
            }
            if(elementBenes)
            {
                if(elementBenes->outPort0 == inputSignal)  startPathYSpace = startPathYSpace + BETWEEN_PATH_SPACE;
                else if(elementBenes->outPort1 == inputSignal)  startPathYSpace = startPathYSpace + 2*BETWEEN_PATH_SPACE;
                else if(elementBenes->outPort2 == inputSignal)  startPathYSpace = startPathYSpace + 3*BETWEEN_PATH_SPACE;
                else if(elementBenes->outPort3 == inputSignal)  startPathYSpace = startPathYSpace + 4*BETWEEN_PATH_SPACE;
            }
        }
    }
    bool isNewPathInvalid = true;  //newPath 是否是无效的，如果无效继续循环直到找到有效的path。
    QPainterPath newPath = srcPath;
    while(isNewPathInvalid)
    {
        newPath = QPainterPath();
        newPath.moveTo(pressedPonitF);
        isNewPathInvalid = false;
        if(resultPath.elementCount() == 1)
        {
            //如果newpos的距离小于初始距离，则做两条折线
            if(qFabs(newPos.y() - pressedPonitF.y()) < startPathYSpace)
            {
//                QL_DEBUG;
                newPath.lineTo(pressedPonitF.x(),newPos.y());
                newPath.moveTo(pressedPonitF.x(),newPos.y());
                if(newPos.x() != pressedPonitF.x())
                {
                    newPath.lineTo(newPos.x(),newPos.y());
                    newPath.moveTo(newPos.x(),newPos.y());
                }
            }
            //如果newpos的距离大于默认的初始距离，则做三条折线
            else
            {
                if(newPos.x() != pressedPonitF.x())
                {
//                    QL_DEBUG << newPos;
                    int startY = startPathYSpace;
                    if(newPos.y() - pressedPonitF.y() < 0)
                    {
                        startY = -startPathYSpace;
                    }

                    newPath.lineTo(pressedPonitF.x(),pressedPonitF.y()+startY);
                    newPath.moveTo(pressedPonitF.x(),pressedPonitF.y()+startY);

                    newPath.lineTo(newPos.x(),pressedPonitF.y()+startY);
                    newPath.moveTo(newPos.x(),pressedPonitF.y()+startY);

                    newPath.lineTo(newPos.x(),newPos.y());
                    newPath.moveTo(newPos.x(),newPos.y());
                }
                else
                {
//                    QL_DEBUG;
                    newPath.lineTo(pressedPonitF.x(),newPos.y());
                    newPath.moveTo(pressedPonitF.x(),newPos.y());
                }
            }
        }
        //如果path已经有其他线段了
        else
        {
            //如果y比较长
            if(qFabs(newPos.y() - pressedPonitF.y()) > qFabs(newPos.x() - pressedPonitF.x()))
            {
                newPath.lineTo(pressedPonitF.x(),newPos.y());
                newPath.moveTo(pressedPonitF.x(),newPos.y());
                if(newPos.x() != pressedPonitF.x())
                {
                    newPath.lineTo(newPos.x(),newPos.y());
                    newPath.moveTo(newPos.x(),newPos.y());
                }
            }
            //x比较长
            else
            {
                newPath.lineTo(newPos.x(),pressedPonitF.y());
                newPath.moveTo(newPos.x(),pressedPonitF.y());
                if(newPos.y() != pressedPonitF.y())
                {
                    newPath.lineTo(newPos.x(),newPos.y());
                    newPath.moveTo(newPos.x(),newPos.y());
                }
            }
        }
    }
    resultPath.addPath(newPath);
//    QL_DEBUG;
    return resultPath;
}

bool ElementWire::isCoincide(const ElementWire* eleWire, const QPainterPath &line)
{
    QPainterPath getFileName = getIntersect(eleWire,line);
    QL_DEBUG << getFileName << getFileName.length();
    if(getFileName.length() > 1) return true;
    return false;
}

void ElementWire::setWirePenWidth(qreal width)
{
    if(width > 0)
    {
        wirePen.setWidthF(width);
        update();
    }
}

void ElementWire::setWirePenBrush(QBrush brush)
{
    wireUnselBrush = brush;
    update();
}

void ElementWire::setWireVisibleForUndo(bool visable)
{
    Q_ASSERT(inputSignal);
    Q_ASSERT(outPort);

//    if(inputSignal && inputSignal->getPortDirection() == OutputDirection)
    {
//        if(visable && (!inputSignal->isContainsWire(this)))
        if(visable && (!hadConnectPorts.contains(inputSignal)))
        {
            QL_DEBUG;
            appendPortToConnectList(inputSignal);
            inputSignal->addWireOrMem(this);
            inputSignal->update();
            connect(inputSignal,SIGNAL(portBeDeleted(ElementPort*)),this,SLOT(disconnectWithPort(ElementPort*)));
            connect(inputSignal,SIGNAL(portBeChanged(ElementPort*)),this,SLOT(handlePortBeChanged(ElementPort*)));
            connect(this,SIGNAL(clearConnectPortInSignal()),inputSignal,SLOT(clearInSignalAllAttr()));
            connect(this,SIGNAL(setConnectPortInSignal(BasePort*)),inputSignal,SLOT(setInSignalAllAttr(BasePort*)));
        }
//        else if(!visable && (inputSignal->isContainsWire(this)))
        else if(!visable && (hadConnectPorts.contains(inputSignal)))
        {
            QL_DEBUG;
            removePortFromConnectedList(inputSignal);
            disconnect(inputSignal,SIGNAL(portBeDeleted(ElementPort*)),this,SLOT(disconnectWithPort(ElementPort*)));
            disconnect(inputSignal,SIGNAL(portBeChanged(ElementPort*)),this,SLOT(handlePortBeChanged(ElementPort*)));
            disconnect(this,SIGNAL(clearConnectPortInSignal()),inputSignal,SLOT(clearInSignalAllAttr()));
            disconnect(this,SIGNAL(setConnectPortInSignal(BasePort*)),inputSignal,SLOT(setInSignalAllAttr(BasePort*)));
            inputSignal->deleteWireOrMem(this);
            inputSignal->update();
        }
//        if(outPort && outPort->getPortDirection() == InputDirection)
        {
//            if(visable && (!outPort->isContainsWire(this)))
            if(visable && (!hadConnectPorts.contains(outPort)))
            {
                QL_DEBUG;
                appendPortToConnectList(outPort);
                outPort->addWireOrMem(this);
                connect(outPort,SIGNAL(portBeDeleted(ElementPort*)),this,SLOT(disconnectWithPort(ElementPort*)));
                connect(outPort,SIGNAL(portBeChanged(ElementPort*)),this,SLOT(handlePortBeChanged(ElementPort*)));
                connect(this,SIGNAL(clearConnectPortInSignal()),outPort,SLOT(clearInSignalAllAttr()));
                connect(this,SIGNAL(setConnectPortInSignal(BasePort*)),outPort,SLOT(setInSignalAllAttr(BasePort*)));
                outPort->setInSignalAllAttr(inputSignal);
                outPort->update();
            }
//            else if(!visable && (outPort->isContainsWire(this)))
            else if(!visable && (hadConnectPorts.contains(outPort)))
            {
                QL_DEBUG;
                removePortFromConnectedList(outPort);
                disconnect(outPort,SIGNAL(portBeDeleted(ElementPort*)),this,SLOT(disconnectWithPort(ElementPort*)));
                disconnect(outPort,SIGNAL(portBeChanged(ElementPort*)),this,SLOT(handlePortBeChanged(ElementPort*)));
                disconnect(this,SIGNAL(clearConnectPortInSignal()),outPort,SLOT(clearInSignalAllAttr()));
                disconnect(this,SIGNAL(setConnectPortInSignal(BasePort*)),outPort,SLOT(setInSignalAllAttr(BasePort*)));
                outPort->deleteWireOrMem(this);
                outPort->clearInSignalAllAttr();
                outPort->update();
            }
        }
    }
}

void ElementWire::setWindowToModified()
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        emit rcaScene->sendWindowToModified(true);
    }
}

void ElementWire::appendPortToConnectList(ElementPort *portItem)
{
    if(portItem && !hadConnectPorts.contains(portItem))
    {
        hadConnectPorts.append(portItem);
    }
}

void ElementWire::removePortFromConnectedList(ElementPort *portItem)
{
    if(portItem && hadConnectPorts.contains(portItem))
    {
        hadConnectPorts.removeOne(portItem);
    }
}

QPainterPath ElementWire::getIntersect(const ElementWire *eleWire, const QPainterPath &line)
{
    qreal len = line.length();
    QPointF p = line.pointAtPercent(0);
    bool intersected = false;

    QPainterPath interPath;

    for(int k = 1; k <= len; ++k)
    {
        if(eleWire->contains(p) && !intersected)
        {
            intersected = true;
            interPath.moveTo(p);
        }
        else if(!eleWire->contains(p) && intersected)
        {
            interPath.lineTo(p);
            return interPath;
        }

        qreal per = line.percentAtLength(k);
        p = line.pointAtPercent(per);
    }
    return interPath;
}

void ElementWire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
//    resetCollidedPorts(path());
    if(isSelected())
    {
        wirePen.setStyle(Qt::DashLine);
        wirePen.setBrush(WIRE_SELECTED_BRUSH);
    }
    else
    {
        wirePen.setStyle(Qt::SolidLine);
        wirePen.setBrush(wireUnselBrush);
    }
    painter->setPen(wirePen);
    painter->drawPath(path());
}
void ElementWire::deleteCollidedPortsConnect()
{
    //因为此item被删除，则所有的collidedItems的item中的collidedItems删除this(item).
    //并检测baseTemp->collidedItems.size()，小于等于0，则直接isCollidedItems=fasle。update。
    //这样不会重新checkCollidedItems

    for(int i=0;i<hadConnectPorts.count();i++)
    {
        ElementPort* elePort = hadConnectPorts.at(i);
        if(elePort)
        {
            disconnectWithPort(elePort);
        }
    }
}


#define INTER_POINT_ARC_SPACE 4
bool ElementWire::setCollidedOtherWire()
{
    QPainterPath newPath = path();
    if(parentItem())
    {
        newPath = mapToScene(newPath);
    }

    QList<QGraphicsItem*> currentCollidedWires = scene()->items(newPath);
//    QL_DEBUG << currentCollidedWires << newPath;

    QListIterator<QGraphicsItem*> itemListTemp(currentCollidedWires);
    {
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementWire* otherWire = dynamic_cast<ElementWire*>(item);
                if((otherWire) && (otherWire != this))
                {
                    QPainterPath otherWirePath = otherWire->path();
                    otherWirePath = mapFromItem(item,otherWirePath);
                    QList<QPainterPath> thisLineList = translateToLineList(path());
                    QList<QPainterPath> otherLineList = translateToLineList(otherWirePath);
                    QPainterPath newSetPath;
                    for(int i=0;i<thisLineList.size();i++)
                    {
                        QPainterPath thisLine = thisLineList.at(i);
                        if(otherWirePath.intersects(thisLine))
                        {
                            for(int i=0;i<otherLineList.size();i++)
                            {
                                QPainterPath otherLine = otherLineList.at(i);
                                if(thisLine.intersects(otherLine))
                                {
                                    if((thisLine.elementCount() == 2) &&
                                            (otherLine.elementCount() == 2))
                                    {
                                        QPointF interPoint;
                                        QPointF point1;
                                        QPointF point2;
                                        qreal xArcSpace = 0;
                                        qreal yArcSpace = 0;
                                        qreal arcRadius = INTER_POINT_ARC_SPACE;
//                                        qreal startAngleBase = 0;
                                        QRectF arcRect;
                                        bool isHline = false;
                                        bool isBackWard = false;
                                        if(thisLine.elementAt(0).x == thisLine.elementAt(1).x)
                                        {

                                            yArcSpace = thisLine.elementAt(0).y > thisLine.elementAt(1).y ? -INTER_POINT_ARC_SPACE : INTER_POINT_ARC_SPACE;
//                                            startAngleBase = thisLine.elementAt(0).y > thisLine.elementAt(1).y ? 0 : 180;
                                            isBackWard = thisLine.elementAt(0).y > thisLine.elementAt(1).y ? true : false;

                                            interPoint.setX(thisLine.elementAt(0).x);
                                            interPoint.setY(otherLine.elementAt(0).y);
                                            point1 = QPointF(interPoint.x(),interPoint.y()-yArcSpace);
                                            point2 = QPointF(interPoint.x(),interPoint.y()+yArcSpace);
                                            isHline = false;
                                        }
                                        else
                                        {
                                            xArcSpace = thisLine.elementAt(0).x > thisLine.elementAt(1).x ? -INTER_POINT_ARC_SPACE : INTER_POINT_ARC_SPACE;;
//                                            startAngleBase = thisLine.elementAt(0).x > thisLine.elementAt(1).x ? 180 : 0;
                                            isBackWard = thisLine.elementAt(0).x > thisLine.elementAt(1).x ? true : false;
                                            interPoint.setX(otherLine.elementAt(0).x);
                                            interPoint.setY(thisLine.elementAt(0).y);
                                            point1 = QPointF(interPoint.x()-xArcSpace,interPoint.y());
                                            point2 = QPointF(interPoint.x()+xArcSpace,interPoint.y());
                                            isHline = true;
                                        }

                                        arcRect = QRectF(interPoint.x()-arcRadius,
                                                         interPoint.y()-arcRadius,
                                                         2*arcRadius,
                                                         2*arcRadius);

                                        QL_DEBUG << thisLine << interPoint << xArcSpace << yArcSpace << isHline;
                                        newSetPath.moveTo(thisLine.elementAt(0).x,thisLine.elementAt(0).y);
                                        newSetPath.lineTo(point1);
                                        if(isBackWard)
                                        {
                                            newSetPath.moveTo(point1);
                                        }
                                        else
                                        {
                                            newSetPath.moveTo(point2);
                                        }
                                        if(isHline)
                                        {
                                            newSetPath.arcTo(arcRect,0,180);
                                            QL_DEBUG << point1 << point2 << interPoint << arcRect;
                                        }
                                        else
                                        {
                                            newSetPath.arcTo(arcRect,90,180);
                                        }

                                        newSetPath.moveTo(point2);
                                        newSetPath.lineTo(thisLine.elementAt(1).x,thisLine.elementAt(1).y);
//                                        QL_DEBUG << newSetPath;
                                    }
                                    break;
                                }
                            }
                        }
                        else
                        {
                            newSetPath.addPath(thisLine);
                        }
                    }
                    shapePathList = translateToLineList(newSetPath);
                    setPath(newSetPath);

                }
            }
        }
    }
    return true;
}

QList<QPainterPath> ElementWire::translateToLineList(const QPainterPath &originPath)
{
    QList<QPainterPath> resultList;

//    QL_DEBUG << originPath;
    for(int i=0;i<originPath.elementCount();i++)
    {
        QPainterPath linePath;
        QPainterPath::Element pathElement = originPath.elementAt(i);
        if(pathElement.isMoveTo())
        {
            linePath.moveTo(pathElement.x,pathElement.y);
            if((i+1)<originPath.elementCount())
            {
                pathElement = originPath.elementAt(i+1);
                if(pathElement.isLineTo())
                {
                    linePath.lineTo(pathElement.x,pathElement.y);
        //            QL_DEBUG << linePath;
                    resultList.append(linePath);
                    i++;
                }
            }
        }
    }
    return resultList;
}

//判断此线段是否有效。如果线上有多个输入口，则无效
bool ElementWire::judgePathIsValid(QPainterPath checkPath)
{
    bool result = !isHadOtherInSignals(checkPath);
    QL_DEBUG << result;
    return result;
}

QList<ElementPort*> ElementWire::getInPortCollidedWithPath(QPainterPath checkPath)
{
    if(!scene()) return QList<ElementPort*>();

    QList<ElementPort*> resultList;
    QPainterPath newPath = checkPath;
    if(parentItem())
    {
        newPath = mapToScene(newPath);
    }
    QList<QGraphicsItem*> currentCollidedPorts = scene()->items(newPath);
    QListIterator<QGraphicsItem*> itemListTemp(currentCollidedPorts);
    {
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                if(elePort && elePort->getPortDirection() == OutputDirection)
                {
                    resultList.append(elePort);
                }
            }
        }
    }
    return resultList;
}

//在path连接的port中是否还有其他的insignal信号。
//主要用于judgePathIsValid()有效还是无效时使用
bool ElementWire::isHadOtherInSignals(QPainterPath checkPath)
{
    if(!scene()) return false;

    QPainterPath newPath = checkPath;
    if(parentItem())
    {
        newPath = mapToScene(newPath);
    }
    BasePort* inTemp = NULL;
    QList<QGraphicsItem*> currentCollidedPorts = scene()->items(newPath);
//    QL_DEBUG << currentCollidedPorts.count();
//    if(currentCollidedPorts.count() > 2) return true;   //一次只能连接一个输入输出口，所以一个path上不能超过两个port。

    QListIterator<QGraphicsItem*> itemListTemp(currentCollidedPorts);
    {
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                if(elePort)
                {
                    //如果有inputSignal
                    if(inputSignal)
                    {
                        //elePort为wire的输出项时
                        if(elePort->getPortDirection() == InputDirection)
                        {
                            //port还有其他的inSignal，表示有多个inSignal
                            bool canConnect = elePort->canConnectOutPort(inputSignal);
                            if(!canConnect)return !canConnect;
                        }
                        //elePort为wire的输入项时
                        else
                        {
                            //port与inputSignal不同，表示有多个inSignal
                            ElementPort* testPort = dynamic_cast<ElementPort*>(inputSignal);
                            if(testPort)
                            {
                                QL_DEBUG << elePort->getPortDirection() << testPort->getPortDirection();
                            }
                            else
                            {
                                QL_DEBUG << elePort->getPortDirection() << inputSignal;
                            }
//                            QL_DEBUG << elePort->getPortDirection() << inputSignal->getPortDirection();
                            if(elePort != inputSignal)  return true;
                        }
                    }
                    //如果本身没有inputSiganl,则要检测到有两个不同的inSignal时，返回true。
                    else
                    {
                        //elePort为wire的输出项时
                        if(elePort->getPortDirection() == InputDirection)
                        {
                            if(elePort->getInSignal())
                            {
                                //port还有其他的inSignal，表示有多个inSignal
                                QL_DEBUG << ((inTemp) && (elePort->getInSignal() != inTemp));
                                if((inTemp) && (elePort->getInSignal() != inTemp)) return true;
                                //记录第一个insignal。
                                else inTemp = elePort->getInSignal();
                            }
                        }
                        //elePort为wire的输入项时
                        else
                        {
                            //port还有其他的inSignal，表示有多个inSignal
                            QL_DEBUG << ((inTemp) && (elePort != inTemp));
                            if((inTemp) && (elePort != inTemp)) return true;
                            //记录第一个insignal。
                            else inTemp = elePort;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void ElementWire::resetInfletionPosByPath(QPainterPath posPath)
{
    if(posPath.isEmpty()) return;
    QPointF lastPos = QPointF(posPath.elementAt(0).x,posPath.elementAt(0).y);

    infletionPosList.clear();

    for(int i=1;i<posPath.elementCount();i++)
    {
        QPainterPath::Element temp = posPath.elementAt(i);
        if(temp.isLineTo())
        {
            QPointF newPos = QPointF(temp.x,temp.y);
            if(lastPos != newPos)
            {
                infletionPosList.append(newPos);
            }
        }
    }
    if(!infletionPosList.isEmpty())
    {
        infletionPosList.removeLast();
    }
}

QList<QPointF> ElementWire::removeBeforeNeedlessPoint(QList<QPointF> listSrc, QList<QPointF> listReference)
{
    QList<QPointF> result = QList<QPointF>();
    if(listSrc.isEmpty()) return result;
    if(listReference.count() <= 1) return result;

//    QL_DEBUG << listSrc << listReference;

    QPointF startPos = listReference.first();

    int i=0;
    for(;i<listSrc.count();i++)
    {
        if(listSrc.at(i).x() == startPos.x()
                || listSrc.at(i).y() == startPos.y())
        {
            i++;
            break;
        }
    }

    for(;i<listSrc.count();i++)
    {
        result.append(listSrc.at(i));
    }

    return result;
}

QList<QPointF> ElementWire::removeAfterNeedlessPoint(QList<QPointF> listSrc, QList<QPointF> listReference)
{
    QList<QPointF> result = QList<QPointF>();
    if(listSrc.isEmpty()) return result;
    if(listReference.count() <= 1) return result;

//    QL_DEBUG << listSrc << listReference;

    QPointF endPos = listReference.last();
    int i=0;
    for(;i<listSrc.count();i++)
    {
        result.append(listSrc.at(i));
        if(listSrc.at(i).x() == endPos.x()
                || listSrc.at(i).y() == endPos.y())
        {
            break;
        }
    }

    return result;
}

QList<QPointF> ElementWire::removeBothNeedlessPoint(QList<QPointF> listSrc, QList<QPointF> listReference)
{
    QList<QPointF> result = QList<QPointF>();
    if(listSrc.isEmpty()) return result;
    if(listReference.count() <= 1) return result;

    QPointF startPos = listReference.first();

    int i=0;
    for(;i<listSrc.count();i++)
    {
        if(listSrc.at(i).x() == startPos.x()
                || listSrc.at(i).y() == startPos.y())
        {
            i++;
            break;
        }
    }

    QPointF endPos = listReference.last();
    for(;i<listSrc.count();i++)
    {
        result.append(listSrc.at(i));
        if(listSrc.at(i).x() == endPos.x()
                || listSrc.at(i).y() == endPos.y())
        {
            break;
        }
    }

    return result;
}

void ElementWire::setPosIn2Out()
{
    if(inputSignal == NULL) return;
    if(outPort == NULL) return;

    QPointF startPos = inputSignal->mapToScene(QPointF(inputSignal->boundingRect().center().x(),inputSignal->getRealBoundingRect().bottom()));
    QPointF endPos = outPort->mapToScene(QPointF(outPort->boundingRect().center().x(),outPort->getRealBoundingRect().top()));

    posIn2Out = startPos - endPos;
//    posIn2Out.setX(inputSignal->getRealBoundingRect().center().x()
//                   - outPort->getRealBoundingRect().center().x());
//    posIn2Out.setY(inputSignal->getRealBoundingRect().center().y()
//                   - outPort->getRealBoundingRect().center().y());

}

QList<QPointF> ElementWire::mapPosListFromPort(ElementPort *port, QList<QPointF> posList)
{
    QList<QPointF> result = QList<QPointF>();
    if(port)
    {
        for(int i=0;i<posList.count();i++)
        {
            result.append(port->mapToScene(posList.at(i)));
        }
    }
    return result;
}

QList<ElementPort*> ElementWire::getOutPortCollidedWithPath(QPainterPath checkPath)
{
    if(!scene()) return QList<ElementPort*>();

    QList<ElementPort*> resultList;
    QPainterPath newPath = checkPath;
    if(parentItem())
    {
        newPath = mapToScene(newPath);
    }
    QList<QGraphicsItem*> currentCollidedPorts = scene()->items(newPath);
    QListIterator<QGraphicsItem*> itemListTemp(currentCollidedPorts);
    {
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                if(elePort && elePort->getPortDirection() == InputDirection)
                {
                    resultList.append(elePort);
                }
            }
        }
    }
    return resultList;
}

bool ElementWire::isCollidedWithPorts()
{
    return hadConnectPorts.size()>0 ? true : false;
}

//设置collidedPorts，删除无效的，添加新的ports。并判断是否有碰撞的ports。
//在setpath后调用，更改path就要reset此项。
void ElementWire::resetConnectCollidedPorts()
{
//    if(isChangedColliedPorts)
//    {
////        setCollidedOtherWire();   //设置交点弧线
//        isChangedColliedPorts = false;
//        if(!scene()) return isCollidedPorts;

        QL_DEBUG;
        if(!scene()) return ;

        QPainterPath newPath = path();
        if(parentItem())
        {
            newPath = mapToScene(newPath);
        }

        QList<QGraphicsItem*> currentCollidedPorts = scene()->items(newPath);

//        QL_DEBUG << currentCollidedPorts << newPath;

        //先检测已有的collidedPorts各个item在最新的currentCollidedPorts中有没有包括
        //如果最新的currentCollidedPorts中包含此item，则此item不变。
        //如果最新的currentCollidedPorts中不包含此item了，则此item重新setcolor,update并在collidedPorts中删除此item。
        for(int i=0;i<hadConnectPorts.count();i++)
        {
            ElementPort* elePort = hadConnectPorts.at(i);
            if(elePort)
            {
                if(!currentCollidedPorts.contains(elePort))
                {
                    disconnectWithPort(elePort);
                }
            }
        }


//        QListIterator<QGraphicsItem*> itemListTemp(hadConnectPorts);
//        {
//            while (itemListTemp.hasNext())
//            {
//                QGraphicsItem *item = itemListTemp.next();
//                if(item)
//                {
//                    if(!currentCollidedPorts.contains(item))
//                    {
//                        ElementPort* elePort = dynamic_cast<ElementPort*>(item);
//                        if(elePort)
//                        {
//                            disconnectWithPort(elePort);
//                        }
//                    }
//                }
//            }
//        }
        QListIterator<QGraphicsItem*> itemListTemp = currentCollidedPorts;
        //再检测最新的currentCollidedPorts各个item在已有的collidedPorts中有没有包括
        //如果已有的collidedPorts中包含此item，则此item不变。
        //如果已有的collidedPorts中不包含此item了，则此item添加到collidedPorts中
        //并对此item中的collidedPorts添加this（item），并将isCollidedPorts设置为true，update。此时不会重新checkcollidedPorts。
        while (itemListTemp.hasNext())
        {
            QGraphicsItem *item = itemListTemp.next();
            if(item)
            {
                ElementPort* elePort = dynamic_cast<ElementPort*>(item);
                if((elePort))
                {
                    connectWithPort(elePort);
                }
            }
        }
//        isCollidedPorts = collidedPorts.size()>0 ? true : false;
//    }
        //    return isCollidedPorts;
}

void ElementWire::setMovetoCollidedPorts()
{
    QL_DEBUG;
    if(!scene()) return ;

    QPainterPath newPath = path();
    if(parentItem())
    {
        newPath = mapToScene(newPath);
    }

    QList<QGraphicsItem*> currentCollidedPorts = scene()->items(newPath);

    QListIterator<QGraphicsItem*> itemListTemp(currentCollidedPorts);

    //再检测最新的currentCollidedPorts各个item在已有的collidedPorts中有没有包括
    //如果已有的collidedPorts中包含此item，则此item不变。
    //如果已有的collidedPorts中不包含此item了，则此item添加到collidedPorts中
    //并对此item中的collidedPorts添加this（item），并将isCollidedPorts设置为true，update。此时不会重新checkcollidedPorts。
    while (itemListTemp.hasNext())
    {
        QGraphicsItem *item = itemListTemp.next();
        if(item)
        {
            ElementPort* elePort = dynamic_cast<ElementPort*>(item);
            if((elePort))
            {
//                connectWithPort(elePort);
            }
        }
    }
}

//重新设置wire的输入。并且重新设置wire连接的port的输入信号。
//inSignal为空则返回false。
bool ElementWire::resetInSignal(ElementPort *inPort)
{
    QL_DEBUG;
    if(inPort)
    {
        inputSignal = inPort;
        emit clearConnectPortInSignal();
        emit setConnectPortInSignal(inPort);
        return true;
    }
    return false;
}
bool ElementWire::clearInSignal()
{
    QL_DEBUG;
    emit clearConnectPortInSignal();
    return true;
}

void ElementWire::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    isChangedColliedPorts = true;
    if((posAtItem == PosHorLine) || (posAtItem == PosVerLine))
    {
        adjustedPath(event->pos());
        isWirePathChanged = true;
    }
    QGraphicsItem::mouseMoveEvent(event);
}


void ElementWire::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_ASSERT(outPort);
    outPort->oldPortInfletionPosList = outPort->portInfletionPosList;
    outPort->oldPortPressedPosList = outPort->portPressedPosList;

    isWirePathChanged = false;
    QGraphicsItem::mousePressEvent(event);
}

void ElementWire::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_ASSERT(outPort);
    if(isWirePathChanged)
    {
        outPort->resetPressedPos(pressedPosList);
        outPort->resetInfletionPos(infletionPosList);
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            rcaScene->pushCmdWireChanged(this);
        }
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void ElementWire::keyPressEvent(QKeyEvent *event)
{
    QL_DEBUG;
//    if(event->key() == Qt::Key_Delete)
//    {
//        QL_DEBUG;
//        if(isSelected())
//        {
//            QL_DEBUG;
//            deleteCollidedPortsConnect();
//            hadConnectPorts.clear();
////            delete this;
//            QList<ElementWire *> dleWireList;
//            dleWireList.clear();
//            dleWireList.append(this);
//            RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
//            if(rcaScene && !dleWireList.isEmpty())
//            {
//                rcaScene->cmdWireDeleted(dleWireList);
//            }
//        }
//    }
    QGraphicsItem::keyPressEvent(event);
}

void ElementWire::adjustedPath(QPointF pos)
{
    if(indexHoverAtPathLine >= 0)
    {
        QList<QPainterPath> thisLineList = translateToLineList(path());

        int hoverIndexAdjust = 0;   //调整indexHoverAtPathLine，当prevline增加一条线段的时候，indexHoverAtPathLine需要加1.

        QPainterPath newWirePath;
        int thisLineListCount = thisLineList.size();
        for(int i=0;i<thisLineListCount;i++)
        {
            if(i == (indexHoverAtPathLine-1))
            {
                //调整中间的线 //或者是调整最后一根线，才有prev的线
                if( (indexHoverAtPathLine > 0 && indexHoverAtPathLine < (thisLineListCount -1))
                        || (indexHoverAtPathLine == (thisLineListCount -1)) )
                {
                    //prevLine
                    QPainterPath tempPath = thisLineList.at(indexHoverAtPathLine-1);
                    QPainterPath prevLine;
                    prevLine.moveTo(tempPath.elementAt(0).x,tempPath.elementAt(0).y);

                    //如果prevline与thisline方向相同(同为横或竖），则需要添加一个转角线。不然会出现斜线。
                    if(judgePathIsLine(tempPath) == judgePathIsLine(thisLineList.at(indexHoverAtPathLine)))
                    {
                        hoverIndexAdjust = 1;
                        prevLine.lineTo(tempPath.elementAt(1).x,tempPath.elementAt(1).y);
                        prevLine.moveTo(tempPath.elementAt(1).x,tempPath.elementAt(1).y);
                        switch(judgePathIsLine(tempPath))
                        {
                            case DirectionHor:
                            {
                                qreal newY = pos.y() == tempPath.elementAt(0).y ? pos.y()+0.000001 : pos.y();
                                prevLine.lineTo(tempPath.elementAt(1).x,newY);
                                break;
                            }
                            default:
                            {
                                qreal newX = pos.x() == tempPath.elementAt(0).x ? pos.x()+0.000001 : pos.x();
                                prevLine.lineTo(newX,tempPath.elementAt(1).y);
                                break;
                            }
                        }
                    }
                    else
                    {
                        if(posAtItem == PosHorLine)
                        {
                            qreal newY = pos.y() == tempPath.elementAt(0).y ? pos.y()+0.000001 : pos.y();
                            prevLine.lineTo(tempPath.elementAt(1).x,newY);
                        }
                        else
                        {
                            qreal newX = pos.x() == tempPath.elementAt(0).x ? pos.x()+0.000001 : pos.x();
                            prevLine.lineTo(newX,tempPath.elementAt(1).y);
                        }
                    }
                    newWirePath.addPath(prevLine);
//                    QL_DEBUG << prevLine << newWirePath;
                }
            }
            else if(i == (indexHoverAtPathLine))
            {
                //thisLine
                QPainterPath tempPath = thisLineList.at(indexHoverAtPathLine);
                QPainterPath thisLine;
                if(posAtItem == PosHorLine)
                {
                    QList<QPointF> pressedPosListTemp;
                    for(int k=0;k<pressedPosList.count();k++)
                    {
                        if(tempPath.contains(pressedPosList.at(k)))
                        {
                            QPointF newPressPos = pressedPosList.at(k);
                            newPressPos.setY(pos.y());
                            pressedPosListTemp.append(newPressPos);
                            continue;
                        }
                        pressedPosListTemp.append(pressedPosList.at(k));
                    }
                    pressedPosList = pressedPosListTemp;
                    thisLine.moveTo(tempPath.elementAt(0).x,pos.y());
                    thisLine.lineTo(tempPath.elementAt(1).x,pos.y());
                }
                else
                {
                    QList<QPointF> pressedPosListTemp;
                    for(int k=0;k<pressedPosList.count();k++)
                    {
                        if(tempPath.contains(pressedPosList.at(k)))
                        {
                            QPointF newPressPos = pressedPosList.at(k);
                            newPressPos.setX(pos.x());
                            pressedPosListTemp.append(newPressPos);
                            continue;
                        }
                        pressedPosListTemp.append(pressedPosList.at(k));
                    }
                    pressedPosList = pressedPosListTemp;
                    thisLine.moveTo(pos.x(),tempPath.elementAt(0).y);
                    thisLine.lineTo(pos.x(),tempPath.elementAt(1).y);
                }
                newWirePath.addPath(thisLine);

//                QL_DEBUG << thisLine << newWirePath;
            }
            else if(i == (indexHoverAtPathLine+1))
            {
                //调整中间的线 //或者是调整第一根线，才有next线
                if((indexHoverAtPathLine > 0 && indexHoverAtPathLine < (thisLineListCount -1))
                        || (indexHoverAtPathLine == 0))
                {
                    //nextLine
                    QPainterPath tempPath = thisLineList.at(indexHoverAtPathLine+1);
                    QPainterPath nextLine;

                    //如果nextline与thisline方向相同(同为横或竖），则需要添加一个转角线。不然会出现斜线。
                    if(judgePathIsLine(tempPath) == judgePathIsLine(thisLineList.at(indexHoverAtPathLine)))
                    {
                        switch(judgePathIsLine(tempPath))
                        {
                            case DirectionHor:
                            {
                                qreal newY = pos.y() == tempPath.elementAt(1).y ? pos.y()+0.000001 : pos.y();
                                nextLine.moveTo(tempPath.elementAt(0).x,newY);
                                break;
                            }
                            default:
                            {
                                qreal newX = pos.x() == tempPath.elementAt(1).x ? pos.x()+0.000001 : pos.x();
                                nextLine.moveTo(newX,tempPath.elementAt(0).y);
                                break;
                            }
                        }
                        nextLine.lineTo(tempPath.elementAt(0).x,tempPath.elementAt(0).y);
                        nextLine.moveTo(tempPath.elementAt(0).x,tempPath.elementAt(0).y);
                        nextLine.lineTo(tempPath.elementAt(1).x,tempPath.elementAt(1).y);
                    }
                    else
                    {
                        if(posAtItem == PosHorLine)
                        {
                            qreal newY = pos.y() == tempPath.elementAt(1).y ? pos.y()+0.000001 : pos.y();
                            nextLine.moveTo(tempPath.elementAt(0).x,newY);
                        }
                        else
                        {
                            qreal newX = pos.x() == tempPath.elementAt(1).x ? pos.x()+0.000001 : pos.x();
                            nextLine.moveTo(newX,tempPath.elementAt(0).y);
                        }
                        nextLine.lineTo(tempPath.elementAt(1).x,tempPath.elementAt(1).y);
                    }
                    newWirePath.addPath(nextLine);

//                    QL_DEBUG << nextLine << newWirePath;
                }
            }
            else
            {
                newWirePath.addPath(thisLineList.at(i));
            }
        }
//        if(judgePathIsValid(newWirePath))
        {
            indexHoverAtPathLine = indexHoverAtPathLine + hoverIndexAdjust;
            wirePath = newWirePath;
            shapePathList = translateToLineList(wirePath);
            setPath(wirePath);
            resetInfletionPosByPath(wirePath);

            setWindowToModified();
//            resetConnectCollidedPorts();
        }
    }
}

WireDirection ElementWire::judgePathIsLine(const QPainterPath &originPath)
{
//    QL_DEBUG;
    if(originPath.elementCount() != 2) return DirectionError;
    if(originPath.elementAt(0).x == originPath.elementAt(1).x) return DirectionVer;
    else if(originPath.elementAt(0).y == originPath.elementAt(1).y) return DirectionHor;
    return DirectionError;
}

bool ElementWire::isContainsPoint(const QPainterPath &originPath,QPointF point)
{
    switch(judgePathIsLine(originPath))
    {
        case DirectionVer:
        {
            if(qAbs(originPath.elementAt(0).x - point.x()) < WIRE_ADJUEST_WIDTH)
            {
                if((point.y() >= qMin(originPath.elementAt(0).y,originPath.elementAt(1).y)) &&
                        (point.y() <= qMax(originPath.elementAt(0).y,originPath.elementAt(1).y)))
                {
                    return true;
                }
            }
        }
        case DirectionHor:
        {
            if(qAbs(originPath.elementAt(0).y - point.y()) < WIRE_ADJUEST_WIDTH)
            {
                if((point.x() >= qMin(originPath.elementAt(0).x,originPath.elementAt(1).x)) &&
                        (point.x() <= qMax(originPath.elementAt(0).x,originPath.elementAt(1).x)))
                {
                    return true;
                }
            }
        }
        default: break;
    }
    return false;
}

void ElementWire::setHoverCursor(QPointF pos)
{
    QList<QPainterPath> thisLineList = translateToLineList(path());
    if(thisLineList.count() < 3) return;
    for(int i=1;i<thisLineList.size()-1;i++)
    {
        QPainterPath thisLine = thisLineList.at(i);
//        QL_DEBUG << thisLine << pos << isContainsPoint(thisLine,pos);
        if(isContainsPoint(thisLine,pos))
        {
//            QL_DEBUG << judgePathIsLine(thisLine);
            switch(judgePathIsLine(thisLine))
            {
            case DirectionVer:setCursor(Qt::SizeHorCursor);posAtItem = PosVerLine;indexHoverAtPathLine = i;break;
            case DirectionHor:setCursor(Qt::SizeVerCursor);posAtItem = PosHorLine;indexHoverAtPathLine = i;break;
            case DirectionError:
            default:setCursor(Qt::ArrowCursor);
                qApp->restoreOverrideCursor();
                posAtItem = PosNone;indexHoverAtPathLine = -1;break;
            }
            break;
        }
    }
}

void ElementWire::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//    QL_DEBUG;
    if(isSelected())
    {
        setHoverCursor(event->pos());
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void ElementWire::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
//    QL_DEBUG;
    setCursor(Qt::ArrowCursor);
    qApp->restoreOverrideCursor();
    QGraphicsItem::hoverLeaveEvent(event);
}

void ElementWire::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
//    QL_DEBUG;
    if(isSelected())
    {
        setHoverCursor(event->pos());
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void ElementWire::connectWithPort(ElementPort* conPort)
{
    if(!hadConnectPorts.contains(conPort))
    {
        QL_DEBUG << hadConnectPorts << conPort;
        //如果新添加的inSignal,则重新设置resetInSignal(此函数会将已有的collidedPorts重新设置)
        if(conPort->getPortDirection() == OutputDirection)
        {
            resetInSignal(conPort);
        }
        //如果是output并且已有inSignal，则将新的port设置inSignal。
        else if(inputSignal)
        {
            conPort->setInSignalAllAttr(inputSignal);
        }
        hadConnectPorts.append(conPort);

        if((outPort == NULL) && (conPort->getPortDirection() == InputDirection))
        {
            QL_DEBUG;
            outPort = conPort;
        }

        switch(conPort->portAttr)
        {
            case PORT_FROM_WMEM:
            case PORT_FROM_RMEM:
            case PORT_FROM_INFIFO:
            case PORT_FORM_OUTFIFO:
                if(wirePen.widthF() == WIRE_DEFAULT_PEN_WIDTH)
                {
                    wirePen.setWidthF(WIRE_DARK_PEN1_WIDTH);
                    update();
                }
                break;
            default: break;
        }

        conPort->addWireOrMem(this);
//        conPort->setColor(WIRE_DRAG_ENTER_COLOR);
        conPort->update();
        connect(conPort,SIGNAL(portBeDeleted(ElementPort*)),this,SLOT(disconnectWithPort(ElementPort*)));
//        if(conPort->getPortDirection() == InputDirection)
        {
            connect(conPort,SIGNAL(portBeChanged(ElementPort*)),this,SLOT(handlePortBeChanged(ElementPort*)));
        }
        connect(this,SIGNAL(clearConnectPortInSignal()),conPort,SLOT(clearInSignalAllAttr()));
        connect(this,SIGNAL(setConnectPortInSignal(BasePort*)),conPort,SLOT(setInSignalAllAttr(BasePort*)));
    }
}

void ElementWire::handlePortBeChanged(ElementPort* port)
{
    if(port == NULL) return;
    if(outPort == NULL) return;
    if(inputSignal == NULL) return;

    QPointF startPos = inputSignal->mapToScene(QPointF(inputSignal->boundingRect().center().x(),inputSignal->getRealBoundingRect().bottom()));
    QPointF endPos = outPort->mapToScene(QPointF(outPort->boundingRect().center().x(),outPort->getRealBoundingRect().top()));

    pressedPonitF = startPos;
    QPainterPath pathTemp = QPainterPath();
    pathTemp.moveTo(pressedPonitF);

    //如果两个port相对位置不变，则wire的相对位置不变。
    if(posIn2Out == (startPos-endPos))
    {
        QL_DEBUG << "equal";
        pressedPosList = mapPosListFromPort(outPort,outPort->portPressedPosList);
        infletionPosList = mapPosListFromPort(outPort,outPort->portInfletionPosList);

        for(int i=0;i<infletionPosList.count();i++)
        {
            pathTemp.lineTo(infletionPosList.at(i));
            pathTemp.moveTo(infletionPosList.at(i));
        }
        pathTemp.lineTo(endPos);
    }
    //如果两个port的相对位置改变，则wire改变。
    else
    {
        QL_DEBUG << "different";
        posIn2Out = (startPos-endPos);
        if(pressedPosList.count() > 0)
        {
            if(port == inputSignal)
            {
                QL_DEBUG << "input";
                pathTemp = autoPath(pressedPosList.first(),pathTemp);
                QList<QPointF> needPosList = removeBeforeNeedlessPoint(infletionPosList,pressedPosList);
                for(int i=1;i<needPosList.count();i++)
                {
                    pathTemp.lineTo(needPosList.at(i));
                    pathTemp.moveTo(needPosList.at(i));
                }
                pathTemp.lineTo(pressedPosList.last());
                pathTemp.moveTo(pressedPosList.last());
                pathTemp = autoPath(endPos,pathTemp);
//                QL_DEBUG << needPosList << pathTemp;
            }
            else
            {
                QL_DEBUG << "output";
                pathTemp = autoPath(pressedPosList.first(),pathTemp);
                QList<QPointF> needPosList = removeAfterNeedlessPoint(infletionPosList,pressedPosList);
                for(int i=1;i<needPosList.count();i++)
                {
                    pathTemp.lineTo(needPosList.at(i));
                    pathTemp.moveTo(needPosList.at(i));
                }
                pathTemp.lineTo(pressedPosList.last());
                pathTemp.moveTo(pressedPosList.last());
                pathTemp = autoPath(endPos,pathTemp);
//                QL_DEBUG << needPosList << pathTemp;
            }
        }
        else
        {
            pathTemp = autoPath(endPos,pathTemp);
        }
    }
    wirePathPainting = pathTemp;
    wirePath = wirePathPainting;
    pressedPonitF = endPos;
    resetInfletionPosByPath(wirePath);
    outPort->resetInfletionPos(infletionPosList);
    outPort->resetPressedPos(pressedPosList);
    shapePathList = translateToLineList(wirePath);
    setPath(wirePath);
}

void ElementWire::disconnectWithPort(ElementPort* disPort)
{
    if(hadConnectPorts.contains(disPort))
    {
        QL_DEBUG << hadConnectPorts << disPort;
        disconnect(disPort,SIGNAL(portBeDeleted(ElementPort*)),this,SLOT(disconnectWithPort(ElementPort*)));
//        if(disPort->getPortDirection() == InputDirection)
        {
            disconnect(disPort,SIGNAL(portBeChanged(ElementPort*)),this,SLOT(handlePortBeChanged(ElementPort*)));
        }
        disconnect(this,SIGNAL(clearConnectPortInSignal()),disPort,SLOT(clearInSignalAllAttr()));
        disconnect(this,SIGNAL(setConnectPortInSignal(BasePort*)),disPort,SLOT(setInSignalAllAttr(BasePort*)));
        disPort->deleteWireOrMem(this);
        disPort->clearInSignalAllAttr();
//        disPort->setColor(WIRE_DRAG_LEAVE_COLOR);
        disPort->update();
        hadConnectPorts.removeOne(disPort);

        if(disPort == outPort)
        {
            QL_DEBUG;
            outPort = NULL;
        }

        if((disPort->getPortDirection() == OutputDirection) && (inputSignal == disPort))
        {
            QL_DEBUG << disPort;
            clearInSignal();
//            inputSignal = NULL;
        }
    }
}
