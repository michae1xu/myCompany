#include "DebugModifyData.h"
#include "ElementBenes.h"
#include "RcaGraphDef.h"
#include "RcaGraphScene.h"


#include <QGraphicsScene>
#include <QDebug>
#include <QDebug>

using namespace rca_space;

ElementBenes::ElementBenes(int bcu,int rcu,int index,QGraphicsItem* parent)
    : BaseArithUnit(BENES,bcu,rcu,index,parent)
    , srcConfig("")
    , inPort0(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , inPort1(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , inPort2(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , inPort3(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , outPort0(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
    , outPort1(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
    , outPort2(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
    , outPort3(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
{
    unitId = BENES_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    realBoundingRect = BENES_ELEMENT_DEFAULT_RECT;
    textRect = QRectF(realBoundingRect.left(),realBoundingRect.top(),
                      ARITHUNIT_TEXT_RECT_WIDTH,realBoundingRect.height());

    paintRect = QRectF(textRect.right(),realBoundingRect.top(),
                       realBoundingRect.width() - ARITHUNIT_TEXT_RECT_WIDTH,realBoundingRect.height());

//    paintRect = QRectF(realBoundingRect.left() + ARITHUNIT_IN_SPACE,
//                       realBoundingRect.top() + ARITHUNIT_IN_SPACE + inPort0->getPaintRect().height(),
//                       realBoundingRect.width() - 2*ARITHUNIT_IN_SPACE,
//                       realBoundingRect.height() - 2*ARITHUNIT_IN_SPACE -
//                       inPort0->getPaintRect().height() - outPort0->getPaintRect().height());

    qreal posX = paintRect.left();
//    qreal posY = paintRect.top() - inPort0->getPaintRect().height();
    qreal posY = paintRect.top() + DEFAULT_PEN_WIDTH;
    qreal inPortSpace = (paintRect.width()  - inPort0->getPaintRect().width()
                                            - inPort1->getPaintRect().width()
                                            - inPort2->getPaintRect().width()
                                            - inPort3->getPaintRect().width())/(mapArithUnitInPort[BENES]+1);

    qreal outPortSpace = (paintRect.width() - outPort0->getPaintRect().width()
                                            - outPort1->getPaintRect().width()
                                            - outPort2->getPaintRect().width()
                                            - outPort3->getPaintRect().width())/(mapArithUnitOutPort[BENES]+1);

//    QL_DEBUG<< paintRect<<inPortSpace<<outPortSpace;

    posX += inPortSpace;
    inPort0->setPos(posX,posY);
    posX += inPort0->getPaintRect().width() + inPortSpace;
    inPort1->setPos(posX,posY);
    posX += inPort1->getPaintRect().width() + inPortSpace;
    inPort2->setPos(posX,posY);
    posX += inPort2->getPaintRect().width() + inPortSpace;
    inPort3->setPos(posX,posY);

    posX = paintRect.left();
//    posY = paintRect.bottom();
    posY = paintRect.bottom() - outPort0->getPaintRect().height() - DEFAULT_PEN_WIDTH;
    posX += outPortSpace;
    outPort0->setPos(posX,posY);
    posX += outPort0->getPaintRect().width() + outPortSpace;
    outPort1->setPos(posX,posY);
    posX += outPort1->getPaintRect().width() + outPortSpace;
    outPort2->setPos(posX,posY);
    posX += outPort2->getPaintRect().width() + outPortSpace;
    outPort3->setPos(posX,posY);

//    paintPath.addRect(inPort0->mapRectToParent(inPort0->boundingRect()));
//    paintPath.addRect(inPort1->mapRectToParent(inPort1->boundingRect()));
//    paintPath.addRect(inPort2->mapRectToParent(inPort2->boundingRect()));
//    paintPath.addRect(inPort3->mapRectToParent(inPort3->boundingRect()));
//    paintPath.addRect(outPort0->mapRectToParent(outPort0->boundingRect()));
//    paintPath.addRect(outPort1->mapRectToParent(outPort1->boundingRect()));
//    paintPath.addRect(outPort2->mapRectToParent(outPort2->boundingRect()));
//    paintPath.addRect(outPort3->mapRectToParent(outPort3->boundingRect()));
    paintPath.addRect(realBoundingRect);
    collidedItems.clear();


    connect(inPort0,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(inPort1,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(inPort2,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(inPort3,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort0,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort1,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort2,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort3,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));

    connect(outPort0,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPort1,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPort2,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPort3,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));

    connect(inPort0,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    connect(inPort1,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    connect(inPort2,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    connect(inPort3,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
}

ElementBenes::ElementBenes(const ElementBenes &copyBenes, QGraphicsItem *parent)
    : BaseArithUnit(BENES,copyBenes.indexBcu,copyBenes.indexRcu,copyBenes.indexInRcu,parent)
    , srcConfig("")
    , inPort0(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , inPort1(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , inPort2(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , inPort3(new ElementPort(InputDirection,this,PORT_FROM_BENES_IN))
    , outPort0(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
    , outPort1(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
    , outPort2(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
    , outPort3(new ElementPort(OutputDirection,this,PORT_FROM_BENES_OUT))
{
    srcConfig = copyBenes.srcConfig;

    inPort0->copyOtherPort(*(copyBenes.inPort0));
    inPort1->copyOtherPort(*copyBenes.inPort1);
    inPort2->copyOtherPort(*copyBenes.inPort2);
    inPort3->copyOtherPort(*copyBenes.inPort3);
    outPort0->copyOtherPort(*copyBenes.outPort0);
    outPort1->copyOtherPort(*copyBenes.outPort1);
    outPort2->copyOtherPort(*copyBenes.outPort2);
    outPort3->copyOtherPort(*copyBenes.outPort3);

//    indexBcu = copyBenes.indexBcu;
//    indexRcu = copyBenes.indexRcu;
//    indexInRcu = copyBenes.indexInRcu;
    this->setPos(copyBenes.pos());
}

ElementBenes::~ElementBenes()
{
    CHECK_AND_DELETE(inPort0);
    CHECK_AND_DELETE(inPort1);
    CHECK_AND_DELETE(inPort2);
    CHECK_AND_DELETE(inPort3);
    CHECK_AND_DELETE(outPort0);
    CHECK_AND_DELETE(outPort1);
    CHECK_AND_DELETE(outPort2);
    CHECK_AND_DELETE(outPort3);
//    QL_DEBUG<<"~ElementBenes()";
}

QPainterPath ElementBenes::shape() const
{
    return paintPath;
}

void ElementBenes::handlePortDeleted(QObject *delPort)
{
    if(inPort0 == delPort) inPort0 = NULL;
    if(inPort1 == delPort) inPort1 = NULL;
    if(inPort2 == delPort) inPort2 = NULL;
    if(inPort3 == delPort) inPort3 = NULL;
    if(outPort0 == delPort) outPort0 = NULL;
    if(outPort1 == delPort) outPort1 = NULL;
    if(outPort2 == delPort) outPort2 = NULL;
    if(outPort3 == delPort) outPort3 = NULL;
}

void ElementBenes::setElementInfo(ElementInfo_del *elementInfo)
{
    if(!elementInfo) return;

    if(!isElementInfoInit)
    {
        isElementInfoInit = true;
        for(int i = 0;i<elementInfo->inputList.size();i++)
        {
            if(i == 0)
                inPort0->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);
            else if(i == 1)
                inPort1->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);
            else if(i == 2)
                inPort2->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);
            else if(i == 3)
                inPort3->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                          elementInfo->id,i,elementInfo->inputList.at(i),false);

        }

        for(int i = 0;i<elementInfo->outputList.size();i++)
        {
            if(i == 0)
                outPort0->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                            elementInfo->id,i,elementInfo->outputList.at(i),false);
            else if(i == 1)
                outPort1->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                      elementInfo->id,i,elementInfo->outputList.at(i),false);
            else if(i == 2)
                outPort2->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                      elementInfo->id,i,elementInfo->outputList.at(i),false);
            else if(i == 3)
                outPort3->initDebugPortInfo(elementInfo->typeId,elementInfo->bcu,elementInfo->rcu,
                                      elementInfo->id,i,elementInfo->outputList.at(i),false);
        }
    }

    {
        RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());

        for(int i = 0;i<elementInfo->inputList.size();i++)
        {
            if(i == 0)
            {
                inPort0->dpInfo->isTextChanged = inPort0->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPort0->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPort0->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort0->showItemWire(true);
                else if(!inPort0->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort0->showItemWire(false);
            }
            else if(i == 1)
            {
                inPort1->dpInfo->isTextChanged = inPort0->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPort1->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPort1->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort1->showItemWire(true);
                else if(!inPort1->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort1->showItemWire(false);
            }
            else if(i == 2)
            {
                inPort2->dpInfo->isTextChanged = inPort0->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPort2->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPort2->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort2->showItemWire(true);
                else if(!inPort2->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort2->showItemWire(false);
            }
            else if(i == 3)
            {
                inPort3->dpInfo->isTextChanged = inPort0->dpInfo->elementStr == elementInfo->inputList.at(i) ? false : true;
                inPort3->dpInfo->elementStr = elementInfo->inputList.at(i);
                if(inPort3->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort3->showItemWire(true);
                else if(!inPort3->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    inPort3->showItemWire(false);
            }
        }

        for(int i = 0;i<elementInfo->outputList.size();i++)
        {
            if(i == 0)
            {
                outPort0->dpInfo->isTextChanged = outPort0->dpInfo->elementStr == elementInfo->outputList.at(i) ? false : true;
                outPort0->dpInfo->elementStr = elementInfo->outputList.at(i);
                if(outPort0->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort0->showItemWire(true);
                else if(!outPort0->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort0->showItemWire(false);
            }
            else if(i == 1)
            {
                outPort1->dpInfo->isTextChanged = outPort1->dpInfo->elementStr == elementInfo->outputList.at(i) ? false : true;
                outPort1->dpInfo->elementStr = elementInfo->outputList.at(i);
                if(outPort1->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort1->showItemWire(true);
                else if(!outPort1->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort1->showItemWire(false);
            }
            else if(i == 2)
            {
                outPort2->dpInfo->isTextChanged = outPort2->dpInfo->elementStr == elementInfo->outputList.at(i) ? false : true;
                outPort2->dpInfo->elementStr = elementInfo->outputList.at(i);
                if(outPort2->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort2->showItemWire(true);
                else if(!outPort2->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort2->showItemWire(false);
            }
            else if(i == 3)
            {
                outPort3->dpInfo->isTextChanged = outPort3->dpInfo->elementStr == elementInfo->outputList.at(i) ? false : true;
                outPort3->dpInfo->elementStr = elementInfo->outputList.at(i);
                if(outPort3->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort3->showItemWire(true);
                else if(!outPort3->dpInfo->isTextChanged && !rcaScene->isAllWireShow)
                    outPort3->showItemWire(false);
            }
        }
    }
}

void ElementBenes::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene && rcaScene->getIsDebugMode())
    {
        QString sendStr = QString("%1,%2,%3,%4").arg(outPort0->dpInfo->typeId).arg(outPort0->dpInfo->bcu) \
                .arg(outPort0->dpInfo->rcu).arg(outPort0->dpInfo->id);
        QStringList outputStrList;
        QList<QString> listString;
        listString.insert(0,outPort0->dpInfo->elementStr);
        listString.insert(1,outPort1->dpInfo->elementStr);
        listString.insert(2,outPort2->dpInfo->elementStr);
        listString.insert(3,outPort3->dpInfo->elementStr);
        outputStrList.append(listString);

        DebugModifyData dataForm(DEBUG_BENES_del,outputStrList);
        if(dataForm.exec() == QDialog::Accepted)
        {
            outPort0->dpInfo->elementStr = dataForm.getOutputStrList().at(0);
            outPort1->dpInfo->elementStr = dataForm.getOutputStrList().at(1);
            outPort2->dpInfo->elementStr = dataForm.getOutputStrList().at(2);
            outPort3->dpInfo->elementStr = dataForm.getOutputStrList().at(3);
            listString.clear();outputStrList.clear();
            listString.insert(0,outPort0->dpInfo->elementStr);
            listString.insert(1,outPort1->dpInfo->elementStr);
            listString.insert(2,outPort2->dpInfo->elementStr);
            listString.insert(3,outPort3->dpInfo->elementStr);
            outputStrList.append(listString);
            update();
            emit sendChangedElementInfo(sendStr,outputStrList);
        }
    }

}

