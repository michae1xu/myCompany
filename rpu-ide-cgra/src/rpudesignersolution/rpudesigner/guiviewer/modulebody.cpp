/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    modulebody.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "modulebody.h"
#include <QDebug>
#define ISENTER(pos,rect) \
    (pos.x() > rect.x() && pos.x() < (rect.x() + rect.width()) &&\
                pos.y() > rect.y() && pos.y() < (rect.y() + rect.height()))
/**
 * ModuleBody::ModuleBody
 * \brief   constructor of ModuleBody
 * \param   pos
 * \param   element
 * \param   bcuid
 * \param   rcuid
 * \param   id
 * \author  zhangjun
 * \date    2016-10-12
 */
ModuleBody::ModuleBody(QPointF pos, ElementType element, int bcuid, int rcuid, int id)
{
    this->element = element;
    switch(element)
    {
    case ELEMENT_BFU:
    {
        inputCount = 3;
        outputCount = 2;
        elementName = "BFU";
        break;
    }
    case ELEMENT_SBOX:
    {
        inputCount = 1;
        outputCount = 4;
        elementName = "SBOX";
        break;
    }
    case ELEMENT_BENES:
    {
        inputCount = 4;
        outputCount = 4;
        elementName = "BENES";
        break;
    }
    case ELEMENT_INMEM:
    {
        inputCount = 0;
        outputCount = 4;
        elementName = "INMEMERY";
        break;
    }
    case ELEMENT_INFIFO:
    {
        inputCount = 0;
        outputCount = 4;
        elementName = "INFIFO";
        break;
    }
    case ELEMENT_OUTFIFO:
    {
        inputCount = 4;
        outputCount = 0;
        elementName = "OUTFIFO";
        break;
    }
    case ELEMENT_OUTMEM:
    {
        inputCount = 4;
        outputCount = 0;
        elementName = "OUTMEMERY";
        break;
    }
    default:
    {
        inputCount = 0;
        outputCount = 0;
        break;
    }
    }
//    rate = 1.0;

    this->bcuID = bcuid;
    this->rcuID = rcuid;
    this->ID = id;
    rect.setX(pos.x());
    rect.setY(pos.y());
    rect.setWidth(((inputCount>outputCount?inputCount:outputCount)+1)*GAPEACHIN);
    rect.setHeight(rect.width()*0.8);
    switch(element)
    {
    case ELEMENT_INFIFO:
    case ELEMENT_INMEM:
    case ELEMENT_OUTFIFO:
    case ELEMENT_OUTMEM:
    {
        rect.setHeight(MODULEHEIGHT);
        break;
    }
    default:break;
    }
    //touchpos
    for(int i = 0; i<8; i++)
    {
        touchPoses[i].setWidth(2*GAP);
        touchPoses[i].setHeight(2*GAP);
    }
    update();
}

/**
 * ModuleBody::~ModuleBody
 * \brief   destructor of ModuleBody
 * \author  zhangjun
 * \date    2016-10-12
 */
ModuleBody::~ModuleBody()
{
//    qDeleteAll(inputPosMap);
//    qDeleteAll(outputPosMap);
    qDebug() << "~ModuleBody()";
}

/**
 * ModuleBody::setInput
 * \brief   
 * \param   index
 * \param   wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::setInput(int index, ModuleWire *wire)
{
    if(inputCount <= index)return;
    inputPosMap.insert(inputPosMap.keys().at(index),wire);
    wire->point[3].setX(inputPosMap.keys().at(index)->x()+GAP);
    wire->point[3].setY(inputPosMap.keys().at(index)->y()+GAP);
}
/**
 * ModuleBody::addOutput
 * \brief   
 * \param   index
 * \param   wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::addOutput(int index, ModuleWire *wire)
{
    if(outputCount <= index || outputPosMap.values().at(index)->contains(wire))return;
    outputPosMap.values().at(index)->push_back(wire);
    wire->point[0].setX(outputPosMap.keys().at(index)->x()+GAP);
    wire->point[0].setY(outputPosMap.keys().at(index)->y()+GAP);
}
/**
 * ModuleBody::removeInputWire
 * \brief   
 * \param   wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::removeInputWire(ModuleWire *wire)
{
    for(int i=0; i<inputPosMap.count(); i++)
    {
        if(inputPosMap.values().at(i) == wire)
        {
            inputPosMap.insert(inputPosMap.keys().at(i),NULL);
            break;
        }
    }
}
/**
 * ModuleBody::removeInputWire
 * \brief   
 * \param   wire
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::removeInputWire(ModuleWire *wire, int index)
{
    if(inputPosMap.values().at(index) == wire)
    {
        inputPosMap.insert(inputPosMap.keys().at(index),NULL);
    }
}



/**
 * ModuleBody::removeOutputWire
 * \brief   
 * \param   wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::removeOutputWire(ModuleWire *wire)
{
    for(int i=0; i<outputPosMap.count(); i++)
    {
        if(outputPosMap.values().at(i)->contains(wire))
        {
            outputPosMap.values().at(i)->removeOne(wire);
            break;
        }
    }
}
/**
 * ModuleBody::removeOutputWire
 * \brief   
 * \param   wire
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::removeOutputWire(ModuleWire *wire, int index)
{
    if(outputPosMap.values().at(index)->contains(wire))
    {
        outputPosMap.values().at(index)->removeOne(wire);
    }
}

/**
 * ModuleBody::update
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::update()
{
    //touchpos
    touchPoses[0].setX(rect.x()-GAP);
    touchPoses[0].setY(rect.y()-GAP);
    touchPoses[1].setX(rect.x()+rect.width()/2-GAP);
    touchPoses[1].setY(rect.y()-GAP);
    touchPoses[2].setX(rect.x()+rect.width()-GAP);
    touchPoses[2].setY(rect.y()-GAP);
    touchPoses[3].setX(rect.x()-GAP);
    touchPoses[3].setY(rect.y()+rect.height()/2-GAP);
    touchPoses[4].setX(rect.x()+rect.width()-GAP);
    touchPoses[4].setY(rect.y()+rect.height()/2-GAP);
    touchPoses[5].setX(rect.x()-GAP);
    touchPoses[5].setY(rect.y()+rect.height()-GAP);
    touchPoses[6].setX(rect.x()+rect.width()/2-GAP);
    touchPoses[6].setY(rect.y()+rect.height()-GAP);
    touchPoses[7].setX(rect.x()+rect.width()-GAP);
    touchPoses[7].setY(rect.y()+rect.height()-GAP);
    for(int i = 0; i<8; i++)
    {
        touchPoses[i].setWidth(2*GAP);
        touchPoses[i].setHeight(2*GAP);
    }

    //
    if(inputPosMap.count() < inputCount)
    {
        QRectF* inputRect[inputCount];
        for(int i = 0; i < inputCount; i++)
        {
            inputRect[i] = new QRectF();
            inputRect[i]->setX(rect.x() + (rect.width()/(inputCount+1))*(i+1)-GAP);
            inputRect[i]->setY(rect.y() + GAP);
            inputRect[i]->setWidth(2*GAP);
            inputRect[i]->setHeight(2*GAP);
            inputPosMap.insert(inputRect[i],NULL);
        }
    }
    else
    {
        for(int i = 0; i < inputCount; i++)
        {
            inputPosMap.keys().at(i)->setX(rect.x() + (rect.width()/(inputCount+1))*(i+1)-GAP);
            inputPosMap.keys().at(i)->setY(rect.y() + GAP);
            inputPosMap.keys().at(i)->setWidth(2*GAP);
            inputPosMap.keys().at(i)->setHeight(2*GAP);
            if(inputPosMap.values().at(i) != NULL)
            {
                inputPosMap.values().at(i)->point[3].setX(inputPosMap.keys().at(i)->x()+GAP);
                inputPosMap.values().at(i)->point[3].setY(inputPosMap.keys().at(i)->y()+GAP);
                inputPosMap.values().at(i)->updateTempPos();
                inputPosMap.values().at(i)->update();
            }
        }
    }
    if(outputPosMap.count() < outputCount)
    {
        QRectF* outputRect[outputCount];
        for(int i = 0; i < outputCount; i++)
        {
            outputRect[i] = new QRectF();
            outputRect[i]->setX(rect.x() + (rect.width()/(outputCount+1))*(i+1)-GAP);
            outputRect[i]->setY(rect.y() + rect.height() -3*GAP);
            outputRect[i]->setWidth(2*GAP);
            outputRect[i]->setHeight(2*GAP);
            QList<ModuleWire*> *list = new QList<ModuleWire*>();
            outputPosMap.insert(outputRect[i],list);
        }
    }
    else
    {
        for(int i = 0; i < outputCount; i++)
        {
            outputPosMap.keys().at(i)->setX(rect.x() + (rect.width()/(outputCount+1))*(i+1)-GAP);
            outputPosMap.keys().at(i)->setY(rect.y() + rect.height() - 3*GAP);
            outputPosMap.keys().at(i)->setWidth(2*GAP);
            outputPosMap.keys().at(i)->setHeight(2*GAP);
            if(!outputPosMap.values().at(i)->isEmpty())
            {
                for(int j = 0; j<outputPosMap.values().at(i)->count(); j++)
                {
                    outputPosMap.values().at(i)->at(j)->point[0].setX(outputPosMap.keys().at(i)->x()+GAP);
                    outputPosMap.values().at(i)->at(j)->point[0].setY(outputPosMap.keys().at(i)->y()+GAP);
                    outputPosMap.values().at(i)->at(j)->updateTempPos();
                    outputPosMap.values().at(i)->at(j)->update();
                }
            }
        }
    }

    if(element == ELEMENT_INFIFO || element == ELEMENT_OUTFIFO)
    {
        labels.insert("FIFO",QPointF(rect.x()+10,rect.y()+15));
    }
    else if(element == ELEMENT_INMEM || element == ELEMENT_OUTMEM)
    {
        labels.insert("MEMRY",QPointF(rect.x()+10,rect.y()+15));
    }
    else
    {
        labels.clear();
        labels.insert("BCU:"+QString::number(bcuID),QPointF(rect.x()+10,rect.y()+30));
        labels.insert("RCU:"+QString::number(rcuID),QPointF(rect.x()+10,rect.y()+50));
        labels.insert(elementName+"ID:"+QString::number(ID),QPointF(rect.x()+10,rect.y()+70));
    }
}


/**
 * ModuleBody::getBrush
 * \brief   
 * \return  QBrush
 * \author  zhangjun
 * \date    2016-10-12
 */
QBrush ModuleBody::getBrush()
{
    return QBrush(QColor(20,135,34),Qt::NoBrush);
}
/**
 * ModuleBody::getTouchBrush
 * \brief   
 * \return  QBrush
 * \author  zhangjun
 * \date    2016-10-12
 */
QBrush ModuleBody::getTouchBrush()
{
    return QBrush(Qt::blue);
}


/**
 * ModuleBody::changeSizeByPos
 * \brief   
 * \param   index
 * \param   relatePos
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::changeSizeByPos(int index, QPointF relatePos)
{
    switch(index)
    {
    case 0:
    {
        rect.setX(rect.x() + relatePos.x());
        rect.setY(rect.y() + relatePos.y());
        break;
    }
    case 1:
    {
        rect.setY(rect.y() + relatePos.y());
        break;
    }
    case 2:
    {
        rect.setWidth(rect.width() + relatePos.x());
        rect.setY(rect.y() + relatePos.y());
        break;
    }
    case 3:
    {
        rect.setX(rect.x() + relatePos.x());
        break;
    }
    case 4:
    {
        rect.setWidth(rect.width() + relatePos.x());
        break;
    }
    case 5:
    {
        rect.setX(rect.x() + relatePos.x());
        rect.setHeight(rect.height() + relatePos.y());
        break;
    }
    case 6:
    {
        rect.setHeight(rect.height() + relatePos.y());
        break;
    }
    case 7:
    {
        rect.setWidth(rect.width() + relatePos.x());
        rect.setHeight(rect.height() + relatePos.y());
        break;
    }
    default:break;
    }

    update();
}

/**
 * ModuleBody::movePos
 * \brief   
 * \param   relatePos
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::movePos(QPointF relatePos)
{
    int w = rect.width();
    int h = rect.height();
    //
    rect.setX(rect.x() + relatePos.x());
    rect.setY(rect.y() + relatePos.y());
    rect.setWidth(w);
    rect.setHeight(h);
    update();
}
/**
 * ModuleBody::setPos
 * \brief   
 * \param   pos
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::setPos(QPointF pos)
{
    int w = rect.width();
    int h = rect.height();
    //
    rect.setX(pos.x());
    rect.setY(pos.y());
    rect.setWidth(w);
    rect.setHeight(h);
    update();
}
/**
 * ModuleBody::setRect
 * \brief   
 * \param   x
 * \param   y
 * \param   w
 * \param   h
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleBody::setRect(double x, double y, double w, double h)
{
    rect.setX(x);
    rect.setY(y);
    rect.setWidth(w);
    rect.setHeight(h);
    update();
}

/**
 * ModuleBody::isEnter
 * \brief   
 * \param   pos
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool ModuleBody::isEnter(QPointF pos)
{
     return ISENTER(pos,rect);
}
/**
 * ModuleBody::isEnterVertex
 * \brief   
 * \param   pos
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int ModuleBody::isEnterVertex(QPointF pos)
{
    for(int i = 0; i<8; i++)
    {
        if(ISENTER(pos,(touchPoses[i])))return i;
    }
    return -1;
}
/**
 * ModuleBody::isEnterInput
 * \brief   
 * \param   pos
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int ModuleBody::isEnterInput(QPointF pos)
{
    for(int i = 0; i<inputPosMap.count(); i++)
    {
        if(ISENTER(pos,(*inputPosMap.keys().at(i))))return i;
    }
    return -1;
}
/**
 * ModuleBody::isEnterOutput
 * \brief   
 * \param   pos
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int ModuleBody::isEnterOutput(QPointF pos)
{
    for(int i = 0; i<outputPosMap.count(); i++)
    {
        if(ISENTER(pos,(*outputPosMap.keys().at(i))))return i;
    }
    return -1;
}
