#include "DebugModifyData.h"
#include "ElementSbox.h"
#include "RcaGraphDef.h"
#include "RcaGraphScene.h"


#include <QGraphicsScene>
#include <QDebug>
#include <QDebug>

using namespace rca_space;

ElementSbox::ElementSbox(int bcu,int rcu,int index,QGraphicsItem* parent)
    : BaseArithUnit(SBOX,bcu,rcu,index,parent)
    , srcConfig("")
    , sboxMode(0)
    , sboxIncreaseMode(0)
    , sboxGroup(0)
    , sboxByteSel(0)
    , groupCnt0(SboxGroupValue_0)
    , groupCnt1(SboxGroupValue_0)
    , groupCnt2(SboxGroupValue_0)
    , groupCnt3(SboxGroupValue_0)
    , inPort0(new ElementPort(InputDirection,this,PORT_FROM_SBOX_IN))
    , outPort0(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
    , outPort1(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
    , outPort2(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
    , outPort3(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
{
    unitId = SBOX_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    realBoundingRect = SBOX_ELEMENT_DEFAULT_RECT;
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
    qreal inPortSpace = (paintRect.width() - inPort0->getPaintRect().width())/(mapArithUnitInPort[SBOX]+1);

    qreal outPortSpace = (paintRect.width() - outPort0->getPaintRect().width()
                                            - outPort1->getPaintRect().width()
                                            - outPort2->getPaintRect().width()
                                            - outPort3->getPaintRect().width())/(mapArithUnitOutPort[SBOX]+1);

//    QL_DEBUG<< paintRect<<inPortSpace<<outPortSpace;

    posX += inPortSpace;
    inPort0->setPos(posX,posY);

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
//    paintPath.addRect(outPort0->mapRectToParent(outPort0->boundingRect()));
//    paintPath.addRect(outPort1->mapRectToParent(outPort1->boundingRect()));
//    paintPath.addRect(outPort2->mapRectToParent(outPort2->boundingRect()));
//    paintPath.addRect(outPort3->mapRectToParent(outPort3->boundingRect()));
    paintPath.addRect(realBoundingRect);
    collidedItems.clear();

    connect(inPort0,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort0,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort1,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort2,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));
    connect(outPort3,SIGNAL(destroyed(QObject*)),this,SLOT(handlePortDeleted(QObject*)));

    connect(outPort0,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPort1,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPort2,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    connect(outPort3,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));

    connect(inPort0,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
}

ElementSbox::ElementSbox(const ElementSbox &copySbox, QGraphicsItem *parent)
    : BaseArithUnit(SBOX,copySbox.indexBcu,copySbox.indexRcu,copySbox.indexInRcu,parent)
    , srcConfig("")
    , sboxMode(0)
    , groupCnt0(SboxGroupValue_0)
    , groupCnt1(SboxGroupValue_0)
    , groupCnt2(SboxGroupValue_0)
    , groupCnt3(SboxGroupValue_0)
    , inPort0(new ElementPort(InputDirection,this,PORT_FROM_SBOX_IN))
    , outPort0(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
    , outPort1(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
    , outPort2(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
    , outPort3(new ElementPort(OutputDirection,this,PORT_FROM_SBOX_OUT))
{
    unitId = SBOX_ID;
    srcConfig = copySbox.srcConfig;
    sboxMode = copySbox.sboxMode;
    sboxIncreaseMode = copySbox.sboxIncreaseMode;
    sboxGroup = copySbox.sboxGroup;
    sboxByteSel = copySbox.sboxByteSel;
    groupCnt0 = copySbox.groupCnt0;
    groupCnt1 = copySbox.groupCnt1;
    groupCnt2 = copySbox.groupCnt2;
    groupCnt3 = copySbox.groupCnt3;

    inPort0->copyOtherPort(*(copySbox.inPort0));
    outPort0->copyOtherPort(*(copySbox.outPort0));
    outPort1->copyOtherPort(*(copySbox.outPort1));
    outPort2->copyOtherPort(*(copySbox.outPort2));
    outPort3->copyOtherPort(*(copySbox.outPort3));
    this->setPos(copySbox.pos());
}

ElementSbox::~ElementSbox()
{
    CHECK_AND_DELETE(inPort0);
    CHECK_AND_DELETE(outPort0);
    CHECK_AND_DELETE(outPort1);
    CHECK_AND_DELETE(outPort2);
    CHECK_AND_DELETE(outPort3);
//    QL_DEBUG<<"~ElementSbox()";
}

QPainterPath ElementSbox::shape() const
{
    return paintPath;
}

void ElementSbox::handlePortDeleted(QObject *delPort)
{
    if(inPort0 == delPort) inPort0 = NULL;
    if(outPort0 == delPort) outPort0 = NULL;
    if(outPort1 == delPort) outPort1 = NULL;
    if(outPort2 == delPort) outPort2 = NULL;
    if(outPort3 == delPort) outPort3 = NULL;
}

void ElementSbox::setElementInfo(ElementInfo_del *elementInfo)
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

void ElementSbox::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
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

        DebugModifyData dataForm(DEBUG_SBOX_del,outputStrList);
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
