#include "DebugModifyData.h"
#include "ElementBfu.h"
#include "RcaGraphDef.h"
#include "RcaGraphScene.h"


#include <QGraphicsScene>
#include <QDebug>
#include <QDebug>

using namespace rca_space;

ElementBfu::ElementBfu(int bcu,int rcu,int index,QGraphicsItem* parent)
    : BaseArithUnit(BFU,bcu,rcu,index,parent)
    , inPortA(new ElementPort(InputDirection,this,PORT_FROM_BFU_A))
    , inPortB(new ElementPort(InputDirection,this,PORT_FROM_BFU_B))
    , inPortT(new ElementPort(InputDirection,this,PORT_FROM_BFU_T))
    , outPortX(new ElementPort(OutputDirection,this,PORT_FROM_BFU_X))
    , outPortY(new ElementPort(OutputDirection,this,PORT_FROM_BFU_Y))
    , funcIndex(Func_Au_Index)
    , funcExp(QString())
    , funcAuModIndex(0)
    , bypassIndex(0)
    , bypassExp(QString())
{
    unitId = BFU_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    realBoundingRect = BFU_ELEMENT_DEFAULT_RECT;
    textRect = QRectF(realBoundingRect.left(),realBoundingRect.top(),
                      ARITHUNIT_TEXT_RECT_WIDTH,realBoundingRect.height());

    paintRect = QRectF(textRect.right(),realBoundingRect.top(),
                       realBoundingRect.width() - ARITHUNIT_TEXT_RECT_WIDTH,realBoundingRect.height());
//    paintRect = QRectF(realBoundingRect.left() + ARITHUNIT_IN_SPACE,
//                       realBoundingRect.top() + ARITHUNIT_IN_SPACE + inPortA->getPaintRect().height(),
//                       realBoundingRect.width() - 2*ARITHUNIT_IN_SPACE,
//                       realBoundingRect.height() - 2*ARITHUNIT_IN_SPACE -
//                       inPortA->getPaintRect().height() - outPortX->getPaintRect().height());

    qreal posX = paintRect.left();
//    qreal posY = paintRect.top() - inPortA->getPaintRect().height();
    qreal posY = paintRect.top() + DEFAULT_PEN_WIDTH;
    qreal inPortSpace = (paintRect.width() - inPortA->getPaintRect().width()
                                           - inPortB->getPaintRect().width()
                                           - inPortT->getPaintRect().width())/(mapArithUnitInPort[BFU]+1);

    qreal outPortSpace = (paintRect.width() - outPortX->getPaintRect().width()
                                            - outPortY->getPaintRect().width())/(mapArithUnitOutPort[BFU]+1);

//    QL_DEBUG<< paintRect << inPortA->getPaintRect()<<inPortSpace<<outPortSpace;

    posX += inPortSpace;
    inPortA->setPos(posX,posY);
    posX += inPortA->getPaintRect().width() + inPortSpace;
    inPortB->setPos(posX,posY);
    posX += inPortB->getPaintRect().width() + inPortSpace;
    inPortT->setPos(posX,posY);

    posX = paintRect.left();
//    posY = paintRect.bottom();
    posY = paintRect.bottom() - outPortX->getPaintRect().height() - DEFAULT_PEN_WIDTH;
    posX += outPortSpace;
    outPortX->setPos(posX,posY);
    posX += outPortX->getPaintRect().width() + outPortSpace;
    outPortY->setPos(posX,posY);

//    paintPath.addRect(inPortA->mapRectToParent(inPortA->boundingRect()));
//    paintPath.addRect(inPortB->mapRectToParent(inPortB->boundingRect()));
//    paintPath.addRect(inPortT->mapRectToParent(inPortT->boundingRect()));
//    paintPath.addRect(outPortX->mapRectToParent(outPortX->boundingRect()));
//    paintPath.addRect(outPortY->mapRectToParent(outPortY->boundingRect()));
    paintPath.addRect(realBoundingRect);
    collidedItems.clear();

    connect(inPortA,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(inPortB,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(inPortT,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPortX,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPortY,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));

    connect(outPortX,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPortY,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));

    connect(inPortA,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    connect(inPortB,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    connect(inPortT,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
}

ElementBfu::ElementBfu(const ElementBfu &copyBfu, QGraphicsItem *parent)
    : BaseArithUnit(BFU,copyBfu.indexBcu,copyBfu.indexRcu,copyBfu.indexInRcu,parent)
    , inPortA(new ElementPort(InputDirection,this,PORT_FROM_BFU_A))
    , inPortB(new ElementPort(InputDirection,this,PORT_FROM_BFU_B))
    , inPortT(new ElementPort(InputDirection,this,PORT_FROM_BFU_T))
    , outPortX(new ElementPort(OutputDirection,this,PORT_FROM_BFU_X))
    , outPortY(new ElementPort(OutputDirection,this,PORT_FROM_BFU_Y))
    , funcIndex(Func_Au_Index)
    , funcExp(QString())
    , funcAuModIndex(0)
    , bypassIndex(0)
    , bypassExp(QString())
{
    unitId = BFU_ID;
    funcIndex = copyBfu.funcIndex;
    funcExp = copyBfu.funcExp;
    funcAuModIndex = copyBfu.funcAuModIndex;
    funcAuCarry = copyBfu.funcAuCarry;
    funcMuModIndex = copyBfu.funcMuModIndex;
    bypassIndex = copyBfu.bypassIndex;
    bypassExp = copyBfu.bypassExp;

    inPortA->copyOtherPort(*copyBfu.inPortA);
    inPortB->copyOtherPort(*copyBfu.inPortB);
    inPortT->copyOtherPort(*copyBfu.inPortT);

    outPortX->copyOtherPort(*copyBfu.outPortX);
    outPortY->copyOtherPort(*copyBfu.outPortY);

    this->setPos(copyBfu.pos());
}

ElementBfu::~ElementBfu()
{
    CHECK_AND_DELETE(inPortA);
    CHECK_AND_DELETE(inPortB);
    CHECK_AND_DELETE(inPortT);
    CHECK_AND_DELETE(outPortX);
    CHECK_AND_DELETE(outPortY);
    QL_DEBUG<<"~ElementBfu()";
}

QPainterPath ElementBfu::shape() const
{
    return paintPath;
}

void ElementBfu::handlePortDeleted(QObject *delPort)
{
    if(inPortA == delPort) inPortA = NULL;
    if(inPortB == delPort) inPortB = NULL;
    if(inPortT == delPort) inPortT = NULL;
    if(outPortX == delPort) outPortX = NULL;
    if(outPortY == delPort) outPortY = NULL;
}

void ElementBfu::setElementInfo(ElementInfo_del *elementInfo)
{
    if(!elementInfo) return;

    if(!isElementInfoInit)
    {
        isElementInfoInit = true;
        for(int i = 0;i<elementInfo->inputList.size();i++)
        {
            if(i == 0)
                inPortA->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);
            else if(i == 1)
                inPortB->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);
            else if(i == 2)
                inPortT->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);
        }

        for(int i = 0;i<elementInfo->outputList.size();i++)
        {
            if(i == 0)
                outPortX->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                            elementInfo->id,i,elementInfo->outputList.at(i),false);
            else if(i == 1)
                outPortY->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                      elementInfo->id,i,elementInfo->outputList.at(i),false);
        }
    }

    {
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());

        for(int i = 0;i<elementInfo->inputList.size();i++)
        {
            if(i == 0)
            {
                inPortA->dpInfo->isTextChanged = inPortA->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPortA->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPortA->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPortA->showItemWire(true);
                else if(!inPortA->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPortA->showItemWire(false);
            }
            else if(i == 1)
            {
                inPortB->dpInfo->isTextChanged = inPortB->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPortB->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPortB->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPortB->showItemWire(true);
                else if(!inPortB->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPortB->showItemWire(false);
            }
            else if(i == 2)
            {
                inPortT->dpInfo->isTextChanged = inPortT->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPortT->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPortT->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPortT->showItemWire(true);
                else if(!inPortT->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPortT->showItemWire(false);
            }
        }

        for(int i = 0;i<elementInfo->outputList.size();i++)
        {
            if(i == 0)
            {
                outPortX->dpInfo->isTextChanged = outPortX->dpInfo->elementStr == elementInfo->outputList.at(i) ? false : true;
                outPortX->dpInfo->elementStr = elementInfo->outputList.at(i);
                if(outPortX->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPortX->showItemWire(true);
                else if(!outPortX->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPortX->showItemWire(false);
            }
            else if(i == 1)
            {
                outPortY->dpInfo->isTextChanged = outPortY->dpInfo->elementStr == elementInfo->outputList.at(i) ? false : true;
                outPortY->dpInfo->elementStr = elementInfo->outputList.at(i);
                if(outPortY->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPortY->showItemWire(true);
                else if(!outPortY->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPortY->showItemWire(false);
            }
        }
    }
}

void ElementBfu::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    QL_DEBUG<<objectName();
    BaseItem::mousePressEvent(event);
}

void ElementBfu::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BaseItem::mouseReleaseEvent(event);
}

void ElementBfu::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && rcaScene->getIsDebugMode())
    {
        QString sendStr = QString("%1,%2,%3,%4").arg(outPortX->dpInfo->typeId).arg(outPortX->dpInfo->bcu) \
                .arg(outPortX->dpInfo->rcu).arg(outPortX->dpInfo->id);
        QStringList outputStrList;
        QList<QString> listString;
        listString.insert(0,outPortX->dpInfo->elementStr);
        listString.insert(1,outPortY->dpInfo->elementStr);
        outputStrList.append(listString);

        DebugModifyData dataForm(DEBUG_BFU_del,outputStrList);
        if(dataForm.exec() == QDialog::Accepted)
        {
            outPortX->dpInfo->elementStr = dataForm.getOutputStrList().at(0);
            outPortY->dpInfo->elementStr = dataForm.getOutputStrList().at(1);
            listString.clear();outputStrList.clear();
            listString.insert(0,outPortX->dpInfo->elementStr);
            listString.insert(1,outPortY->dpInfo->elementStr);
            outputStrList.append(listString);
            update();
            emit sendChangedElementInfo(sendStr,outputStrList);
        }
    }
}
