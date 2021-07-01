/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    modulewire.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "modulewire.h"
#include <QDebug>

#define GAP 4
#define ISENTER(pos,rect) \
    (pos.x() > rect.x() && pos.x() < (rect.x() + rect.width()) &&\
                pos.y() > rect.y() && pos.y() < (rect.y() + rect.height()))
/**
 * ModuleWire::ModuleWire
 * \brief   constructor of ModuleWire
 * \param   spos
 * \param   epos
 * \author  zhangjun
 * \date    2016-10-12
 */
ModuleWire::ModuleWire(QPointF spos, QPointF epos)
{
    isTouched = false;
    this->point[0] = spos;
    this->point[3] = epos;
    height = abs(epos.y()-spos.y())/2;
    input = NULL;
    output = NULL;
    inputPosIndex = -1;
    outputPosIndex = -1;
    updateTempPos();
    update();
}

ModuleWire::~ModuleWire()
{
    qDebug() << "~ModuleWire()";
}
/**
 * ModuleWire::isEnterVertex
 * \brief   
 * \param   pos
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int ModuleWire::isEnterVertex(QPointF pos)
{
    for(int i = 0; i<4; i++)
    {
        if(ISENTER(pos,touchPoses[i]))
        {
            return i;
        }
    }
    return -1;
}


/**
 * ModuleWire::isEnter
 * \brief   
 * \param   pos
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool ModuleWire::isEnter(QPointF pos)
{
    QRectF rect[3];
    for(int i = 0; i<3; i++)
    {
        if(point[i].x() == point[i+1].x() && point[i].y() <= point[i+1].y())  //
        {
            rect[i].setX(point[i].x()-GAP);
            rect[i].setY(point[i].y());
            rect[i].setWidth(2*GAP);
            rect[i].setHeight(point[i+1].y()-point[i].y());
        }
        else if(point[i].x() == point[i+1].x() && point[i].y() > point[i+1].y())
        {
            rect[i].setX(point[i+1].x()-GAP);
            rect[i].setY(point[i+1].y());
            rect[i].setWidth(2*GAP);
            rect[i].setHeight(point[i].y()-point[i+1].y());
        }
        else if(point[i].y() == point[i+1].y() && point[i].x() <= point[i+1].x())
        {
            rect[i].setX(point[i].x());
            rect[i].setY(point[i].y()-GAP);
            rect[i].setWidth(point[i+1].x()-point[i].x());
            rect[i].setHeight(2*GAP);
        }
        else if(point[i].y() == point[i+1].y() && point[i].x() > point[i+1].x())
        {
            rect[i].setX(point[i+1].x());
            rect[i].setY(point[i+1].y()-GAP);
            rect[i].setWidth(point[i].x()-point[i+1].x());
            rect[i].setHeight(2*GAP);
        }
        if(ISENTER(pos,rect[i]))return true;
    }
    return false;
}

/**
 * ModuleWire::movePos
 * \brief   
 * \param   pos
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::movePos(QPointF pos)
{
    for(int i = 0; i<4; i++)
    {
        point[i] += pos;
    }
    updateTempPos();
    update();
}
/**
 * ModuleWire::movePoint
 * \brief   
 * \param   index
 * \param   pos
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::movePoint(int index, QPointF pos)
{
    point[index].setX(point[index].x()+pos.x());
    point[index].setY(point[index].y()+pos.y());
    if(index == 1 || index == 2)    //中间两个点
    {
        height = abs(point[index].y()-point[3].y());
    }
    updateTempPos();
    update();
}
/**
 * ModuleWire::changeHeight
 * \brief   
 * \param   pos
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int ModuleWire::changeHeight(QPointF pos)
{
    point[2].setX(point[2].x()+pos.x());
    point[2].setY(point[2].y()+pos.y());
    height = abs(point[2].y()-point[3].y());
    updateTempPos();
    update();
    return height;
}
/**
 * ModuleWire::setHeight
 * \brief   
 * \param   height
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::setHeight(double height)
{
    this->height = height;
    updateTempPos();
    update();
}

/**
 * ModuleWire::setPoint
 * \brief   
 * \param   index
 * \param   pos
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::setPoint(int index, QPointF pos)
{
    point[index].setX(pos.x());
    point[index].setY(pos.y());
    height = abs(point[0].y()-point[3].y())/2;
    updateTempPos();
    update();
}

/**
 * ModuleWire::updateTempPos
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::updateTempPos()
{
    if(point[0].x() == point[3].x())
    {
        point[1].setX(point[0].x());
        point[1].setY(point[0].y()+(point[3].y()-point[0].y())/3);
        point[2].setX(point[0].x());
        point[2].setY(point[0].y()+(point[3].y()-point[0].y())*2/3);
    }
    else if(point[0].y() == point[3].y())
    {
        point[1].setY(point[0].y());
        point[1].setX(point[0].x()+(point[3].x()-point[0].x())/3);
        point[2].setY(point[0].y());
        point[2].setX(point[0].x()+(point[3].x()-point[0].x())*2/3);
    }
    else
    {
        point[1].setX(point[0].x());
        point[1].setY(point[3].y()-height);
        point[2].setX(point[3].x());
        point[2].setY(point[1].y());
    }
}
/**
 * ModuleWire::getBrush
 * \brief   
 * \return  QBrush
 * \author  zhangjun
 * \date    2016-10-12
 */
QBrush ModuleWire::getBrush()
{
    return QBrush(Qt::black);
}
/**
 * ModuleWire::getTouchBrush
 * \brief   
 * \return  QBrush
 * \author  zhangjun
 * \date    2016-10-12
 */
QBrush ModuleWire::getTouchBrush()
{
    return QBrush(Qt::green);
}
/**
 * ModuleWire::setInput
 * \brief   
 * \param   body
 * \param   posindex
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::setInput(ModuleBody *body, int posindex)
{
    this->input = body;
    this->inputPosIndex = posindex;
    updateTempPos();
    update();
}
/**
 * ModuleWire::updateHeight
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::updateHeight()
{
    this->height = abs(point[3].y()-point[0].y())/2;
    updateTempPos();
    update();
}

/**
 * ModuleWire::setOutput
 * \brief   
 * \param   body
 * \param   posindex
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::setOutput(ModuleBody *body, int posindex)
{
    this->output = body;
    this->outputPosIndex = posindex;
    updateTempPos();
    update();
}

/**
 * ModuleWire::update
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ModuleWire::update()
{
    for(int i = 0; i<4; i++)
    {
        touchPoses[i].setRect(point[i].x()-GAP,point[i].y()-GAP,2*GAP,2*GAP);
    }
}
