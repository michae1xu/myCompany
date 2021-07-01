#include "BaseArithUnit.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "ModuleRcu.h"
#include "RcaGraphScene.h"
#include <QDebug>

ElementInfo_del::ElementInfo_del()
{
}

ElementInfo_del::~ElementInfo_del()
{
}

BaseArithUnit::BaseArithUnit(ArithUnitId unitId,int bcu,int rcu,int index,QGraphicsItem *parent)
    : BaseItem(parent)
    , arithUnitId(unitId)
    , indexBcu(bcu)
    , indexRcu(rcu)
    , indexInRcu(index)
    , inPortCount(mapArithUnitInPort[unitId])
    , outPortCount(mapArithUnitOutPort[unitId])
    , isElementInfoInit(false)
{
    //设置为false后，item不允许改变边框大小
    isEnabledAdjustRect = false;
//    arithUnitName = QString(mapArithUnitName[unitId] + "(%1,%2,%3)").arg(bcu).arg(rcu).arg(index);
    arithUnitName = QString(mapArithUnitName[unitId] + ":%1").arg(index);
    setObjectName(ARITH_UNIT_OBJECT_NAME);
}

BaseArithUnit::~BaseArithUnit()
{
//    QL_DEBUG << "~BaseArithUnit()";
}

//void BaseArithUnit::setIndexInRcu(int index)
//{
//    indexInRcu = index;
//}

bool BaseArithUnit::setIndexBcu(int index)
{
    if(index >= BCU_MAX_COUNT) return false;
    if(indexRcu == RCU_FIRST_COUNT)
    {
        deletePrevLineIn();
    }
    else if(indexRcu == RCU_LAST_COUNT)
    {
        deleteNextLineOut();
    }
    indexBcu = index;
    return true;
}

void BaseArithUnit::showItemPortWire()
{
    QL_DEBUG;
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(this);
    if(elementBfu && elementBfu->outPortX && elementBfu->outPortY
            && elementBfu->inPortA && elementBfu->inPortB && elementBfu->inPortT)
    {
        elementBfu->outPortX->showItemWire(this->isSelected());
        elementBfu->outPortY->showItemWire(this->isSelected());

        elementBfu->inPortA->showItemWire(this->isSelected());
        elementBfu->inPortB->showItemWire(this->isSelected());
        elementBfu->inPortT->showItemWire(this->isSelected());

        return;
    }

    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(this);
    if(elementSbox && elementSbox->outPort0 && elementSbox->outPort1
            && elementSbox->outPort2 && elementSbox->outPort3 && elementSbox->inPort0)
    {
        elementSbox->outPort0->showItemWire(this->isSelected());
        elementSbox->outPort1->showItemWire(this->isSelected());
        elementSbox->outPort2->showItemWire(this->isSelected());
        elementSbox->outPort3->showItemWire(this->isSelected());

        elementSbox->inPort0->showItemWire(this->isSelected());
        return;
    }

    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(this);
    if(elementBenes && elementBenes->outPort0 && elementBenes->outPort1
            && elementBenes->outPort2 && elementBenes->outPort3
            && elementBenes->inPort0 && elementBenes->inPort1
            && elementBenes->inPort2 &&elementBenes->inPort3)
    {
        elementBenes->outPort0->showItemWire(this->isSelected());
        elementBenes->outPort1->showItemWire(this->isSelected());
        elementBenes->outPort2->showItemWire(this->isSelected());
        elementBenes->outPort3->showItemWire(this->isSelected());

        elementBenes->inPort0->showItemWire(this->isSelected());
        elementBenes->inPort1->showItemWire(this->isSelected());
        elementBenes->inPort2->showItemWire(this->isSelected());
        elementBenes->inPort3->showItemWire(this->isSelected());
        return;
    }
}

void BaseArithUnit::deletePrevLineIn()
{
    //此时需要将input不合法的连接线删除。
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(this);
    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(this);
    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(this);
    if(elementBfu)
    {
        if(elementBfu->inPortA && elementBfu->inPortB && elementBfu->inPortT)
        {
            elementBfu->inPortA->deletePrevLinePortIn();
            elementBfu->inPortB->deletePrevLinePortIn();
            elementBfu->inPortT->deletePrevLinePortIn();
        }
    }
    else if(elementSbox)
    {
        if(elementSbox->inPort0)
        {
            elementSbox->inPort0->deletePrevLinePortIn();
        }
    }
    else if(elementBenes)
    {
        if(elementBenes->inPort0 && elementBenes->inPort1
                && elementBenes->inPort2 &&elementBenes->inPort3)
        {
            elementBenes->inPort0->deletePrevLinePortIn();
            elementBenes->inPort1->deletePrevLinePortIn();
            elementBenes->inPort2->deletePrevLinePortIn();
            elementBenes->inPort3->deletePrevLinePortIn();
        }
    }
}

void BaseArithUnit::deleteNextLineOut()
{
    //此时需要将output不合法的连接线删除。
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(this);
    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(this);
    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(this);
    if(elementBfu)
    {
        if(elementBfu->outPortX && elementBfu->outPortY)
        {
            elementBfu->outPortX->deleteNextLinePortOut();
            elementBfu->outPortY->deleteNextLinePortOut();
        }
    }
    else if(elementSbox)
    {
        if(elementSbox->outPort0 && elementSbox->outPort1
                && elementSbox->outPort2 && elementSbox->outPort3)
        {
            elementSbox->outPort0->deleteNextLinePortOut();
            elementSbox->outPort1->deleteNextLinePortOut();
            elementSbox->outPort2->deleteNextLinePortOut();
            elementSbox->outPort3->deleteNextLinePortOut();
        }
    }
    else if(elementBenes)
    {
        if(elementBenes->outPort0 && elementBenes->outPort1
                && elementBenes->outPort2 && elementBenes->outPort3)
        {
            elementBenes->outPort0->deleteNextLinePortOut();
            elementBenes->outPort1->deleteNextLinePortOut();
            elementBenes->outPort2->deleteNextLinePortOut();
            elementBenes->outPort3->deleteNextLinePortOut();
        }
    }
}

void BaseArithUnit::setChildrenSelected(bool enabled)
{
    for(int i=0;i<childItems().count();i++)
    {
        ElementPort* childPort = dynamic_cast<ElementPort*>(childItems().at(i));
        if(childPort)
        {
            for(int j=0;j<childPort->childItems().count();j++)
            {
                QGraphicsItem* childItem = dynamic_cast<QGraphicsItem*>(childPort->childItems().at(j));
                if(childItem)
                {
                   childItem->setSelected(enabled);
                }
            }
        }
    }
}

bool BaseArithUnit::setIndexInRcu(int index)
{
    if(parentItem())
    {
        ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(parentItem());
        if(parentRcu)
        {
            switch(arithUnitId)
            {
                case BFU:
                {
                    if(index >= BFU_MAX_COUNT) return false;
//                    if(parentRcu->findBfuFromRcu(index) == NULL)
                    if(parentRcu->replaceBfuIndex(indexInRcu,index))
                    {
                        ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(this);
                        if(elementBfu)
                        {
                            RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                            if(rcaScene)
                            {
                                emit rcaScene->deleteBfuSuccess(elementBfu);
                            }

                            indexInRcu = index;
                            arithUnitName = QString(mapArithUnitName[arithUnitId] + ":%1").arg(index);
                            elementBfu->outPortX->resetConnectOutPortInputIndex(indexInRcu);
                            elementBfu->outPortY->resetConnectOutPortInputIndex(indexInRcu);
                            return true;
                        }
                    }
                    break;
                }
                case SBOX:
                {
                    if(index >= SBOX_MAX_COUNT) return false;
//                    if(parentRcu->findSboxFromRcu(index) == NULL)
                    if(parentRcu->replaceSboxIndex(indexInRcu,index))
                    {
                        ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(this);
                        if(elementSbox)
                        {
                            RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                            if(rcaScene)
                            {
                                emit rcaScene->deleteSboxSuccess(elementSbox);
                            }
                            indexInRcu = index;
                            arithUnitName = QString(mapArithUnitName[arithUnitId] + ":%1").arg(index);
                            elementSbox->outPort0->resetConnectOutPortInputIndex(indexInRcu * SBOX_OUTPUTCOUNT);
                            elementSbox->outPort1->resetConnectOutPortInputIndex(indexInRcu * SBOX_OUTPUTCOUNT + 1);
                            elementSbox->outPort2->resetConnectOutPortInputIndex(indexInRcu * SBOX_OUTPUTCOUNT + 2);
                            elementSbox->outPort3->resetConnectOutPortInputIndex(indexInRcu * SBOX_OUTPUTCOUNT + 3);
                            return true;
                        }
                    }
                    break;
                }
                case BENES:
                {
                    if(index >= BENES_MAX_COUNT) return false;
//                    if(parentRcu->findBenesFromRcu(index) == NULL)
                    if(parentRcu->replaceBenesIndex(indexInRcu,index))
                    {
                        ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(this);
                        if(elementBenes)
                        {
                            RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
                            if(rcaScene)
                            {
                                emit rcaScene->deleteBenesSuccess(elementBenes);
                            }
                            indexInRcu = index;
                            arithUnitName = QString(mapArithUnitName[arithUnitId] + ":%1").arg(index);
                            elementBenes->outPort0->resetConnectOutPortInputIndex(indexInRcu * BENES_OUTPUTCOUNT);
                            elementBenes->outPort1->resetConnectOutPortInputIndex(indexInRcu * BENES_OUTPUTCOUNT + 1);
                            elementBenes->outPort2->resetConnectOutPortInputIndex(indexInRcu * BENES_OUTPUTCOUNT + 2);
                            elementBenes->outPort3->resetConnectOutPortInputIndex(indexInRcu * BENES_OUTPUTCOUNT + 3);
                            return true;
                        }
                    }
                    break;
                }
                default: break;
            }
        }
    }
    return false;
}

void BaseArithUnit::pushItemToDeletedStack(bool isForced)
{
    QL_DEBUG;
    isForced |= isSelected();
    switch(unitId)
    {
    case BFU_ID:
    {
        ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(this);
        if(elementBfu && elementBfu->outPortX && elementBfu->outPortY
                && elementBfu->inPortA && elementBfu->inPortB && elementBfu->inPortT)
        {
            elementBfu->outPortX->pushItemToDeletedStack(isForced);
            elementBfu->outPortY->pushItemToDeletedStack(isForced);

            elementBfu->inPortA->pushItemToDeletedStack(isForced);
            elementBfu->inPortB->pushItemToDeletedStack(isForced);
            elementBfu->inPortT->pushItemToDeletedStack(isForced);
        }
        break;
    }
    case CUSTOMPE_ID:
    {
        ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(this);
        if(elementCustomPE)
        {
            for(int i = 0;i < elementCustomPE->inputNum();i++)
            {
                elementCustomPE->m_inputPortVector.at(i)->pushItemToDeletedStack(isForced);
            }
            for(int i = 0;i < elementCustomPE->outputNum();i++)
            {
                elementCustomPE->m_outputPortVector.at(i)->pushItemToDeletedStack(isForced);
            }
        }
        break;
    }
    case SBOX_ID:
    {
        ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(this);
        if(elementSbox && elementSbox->outPort0 && elementSbox->outPort1
                && elementSbox->outPort2 && elementSbox->outPort3 && elementSbox->inPort0)
        {
            elementSbox->outPort0->pushItemToDeletedStack(isForced);
            elementSbox->outPort1->pushItemToDeletedStack(isForced);
            elementSbox->outPort2->pushItemToDeletedStack(isForced);
            elementSbox->outPort3->pushItemToDeletedStack(isForced);

            elementSbox->inPort0->pushItemToDeletedStack(isForced);
        }
        break;
    }
    case BENES_ID:
    {
        ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(this);
        if(elementBenes && elementBenes->outPort0 && elementBenes->outPort1
                && elementBenes->outPort2 && elementBenes->outPort3
                && elementBenes->inPort0 && elementBenes->inPort1
                && elementBenes->inPort2 &&elementBenes->inPort3)
        {
            elementBenes->outPort0->pushItemToDeletedStack(isForced);
            elementBenes->outPort1->pushItemToDeletedStack(isForced);
            elementBenes->outPort2->pushItemToDeletedStack(isForced);
            elementBenes->outPort3->pushItemToDeletedStack(isForced);

            elementBenes->inPort0->pushItemToDeletedStack(isForced);
            elementBenes->inPort1->pushItemToDeletedStack(isForced);
            elementBenes->inPort2->pushItemToDeletedStack(isForced);
            elementBenes->inPort3->pushItemToDeletedStack(isForced);
        }
        break;
    }
    default:break;
    }

    BaseItem::pushItemToDeletedStack(isForced);
}

bool BaseArithUnit::setIndexRcu(int index)
{
    if(index >= RCU_MAX_COUNT) return false;
    deletePrevLineIn();
    deleteNextLineOut();
    indexRcu = index;
    return true;
}


void BaseArithUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);


    painter->setBrush(ARITH_BRUSH_FILLIN);
    painter->setPen(ARITH_BOUNDING_PEN);
    painter->fillRect(paintRect,ARITH_BRUSH_FILLIN);
    painter->drawRect(paintRect);
//    QL_DEBUG<<scene()->collidingItems(this,Qt::IntersectsItemBoundingRect).size();

    switch(arithUnitId)
    {
    case SBOX:
    {
        for(int i=0;i<SBOX_MARK_RECT_COUNT;i++)
        {
            painter->drawRect(SBOX_ELEMENT_MARK_RECT[i]);
        }
    }break;
    case BENES:painter->drawRect(BENES_ELEMENT_MARK_RECT);break;
    case BFU:
    default:break;
    }

    if(isSelected())
    {
        painter->setBrush(ARITH_TEXT_SELECTED_BRUSH_FILLIN);
        painter->setPen(ARITH_BOUNDING_PEN);
        painter->fillRect(textRect,ARITH_TEXT_SELECTED_BRUSH_FILLIN);
        painter->drawRect(textRect);
    }
    else
    {
        painter->setBrush(ARITH_TEXT_NORMAL_BRUSH_FILLIN);
        painter->setPen(ARITH_BOUNDING_PEN);
        painter->fillRect(textRect,ARITH_TEXT_NORMAL_BRUSH_FILLIN);
        painter->drawRect(textRect);
    }

    if(checkCollidedItems(Qt::IntersectsItemBoundingRect))
//    if(isCollidedItems)
    {
        painter->setBrush(ARITH_COLLIDED_BRUSH_FILLIN);
        painter->setPen(ARITH_COLLIDED_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,ARITH_COLLIDED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }

    QFont font;
    font.setPixelSize(ARITH_TEXT_PIXELSIZE);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(ARITH_TEXT_PEN);

    painter->rotate(90);
    painter->drawText(textRect.top() + 2,
                      (- (textRect.width() - font.pixelSize())/2 - textRect.left()),
                      arithUnitName);
//    painter->drawText(textRect, Qt::AlignHCenter|Qt::AlignTop, arithUnitName);

//    painter->drawText(textRect, Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, arithUnitName.split("", QString::SkipEmptyParts).join("\n"));
}

void BaseArithUnit::handleAddWriteMemSuccess(ModuleWriteMem* writeMem)
{
    QL_DEBUG;
    emit addWriteMemSuccessed(writeMem);
}

void BaseArithUnit::handleAddReadMemSuccess(ModuleReadMem* readMem)
{
    QL_DEBUG;
    emit addReadMemSuccessed(readMem);
}
