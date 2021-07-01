#include "ItemCopyPaste.h"
#include "RcaGraphScene.h"
#include "BaseItem.h"
#include "ElementBfu.h"

ItemCopyPaste* ItemCopyPaste::m_instance = NULL;

ItemCopyPaste::ItemCopyPaste(QObject *parent) : QObject(parent)
//    , rcaScene(scene)
    , selectedItemsList(QList<BaseItem*>())
    , hadCopyItemsList(QList<BaseItem*>())
    , hadPasteItemsList(QList<BaseItem*>())
    , hadCopyWiresList(QList<ElementWire*>())
    , hadCopyUnitId(Module_None)
    , copyItemsRect(QRectF())
{
    mapCopyAndPaste.clear();
//    Q_ASSERT(rcaScene);
}

ItemCopyPaste::~ItemCopyPaste()
{
    clearItemsList();
}

void ItemCopyPaste::copyItems(QList<QGraphicsItem*> itemList, ModuleUnitId copyUnitId, bool hasWire)
{
    clearItemsList();
    hadCopyUnitId = copyUnitId;
    for(int i=0;i<itemList.count();i++)
    {
        BaseItem* baseItemTemp = dynamic_cast<BaseItem*>(itemList.at(i));
        if(baseItemTemp == NULL) continue;

        switch(copyUnitId)
        {
        case Module_Bcu:
        {
            if(baseItemTemp->getUnitId() == BCU_ID)
            {
                ModuleBcu *bcuTemp = dynamic_cast<ModuleBcu*>(baseItemTemp);
                if(bcuTemp)
                {
                    ModuleBcu* bcuCopy = new ModuleBcu(*bcuTemp);
                    selectedItemsList.append(bcuTemp);
                    hadCopyItemsList.append(bcuCopy);
                    copyItemsRect |= bcuTemp->mapRectToParent(bcuTemp->getRealBoundingRect());
                    QL_DEBUG;
                }
            }
            break;
        }
        case Module_Rcu:
        {
            if(baseItemTemp->getUnitId() == RCU_ID)
            {
                ModuleRcu *rcuTemp = dynamic_cast<ModuleRcu*>(baseItemTemp);
                if(rcuTemp)
                {
                    ModuleRcu* rcuCopy = new ModuleRcu(*rcuTemp);
                    selectedItemsList.append(rcuTemp);
                    hadCopyItemsList.append(rcuCopy);
                    copyItemsRect |= rcuTemp->mapRectToParent(rcuTemp->getRealBoundingRect());
                    QL_DEBUG;
                }
            }
            break;
        }
        case Arith_Unit:
        {
            if(baseItemTemp->getUnitId() == BFU_ID)
            {
                ElementBfu *bfuTemp = dynamic_cast<ElementBfu*>(baseItemTemp);
                if(bfuTemp)
                {
                    ElementBfu* bfuCopy = new ElementBfu(*bfuTemp);
                    selectedItemsList.append(bfuTemp);
                    hadCopyItemsList.append(bfuCopy);
                    copyItemsRect |= bfuTemp->mapRectToParent(bfuTemp->getRealBoundingRect());
                    QL_DEBUG;
                }
            }
            else if(baseItemTemp->getUnitId() == CUSTOMPE_ID)
            {
                ElementCustomPE *customPETemp = dynamic_cast<ElementCustomPE*>(baseItemTemp);
                if(customPETemp)
                {
                    ElementCustomPE* customPECopy = new ElementCustomPE(*customPETemp);
                    selectedItemsList.append(customPETemp);
                    hadCopyItemsList.append(customPECopy);
                    copyItemsRect |= customPETemp->mapRectToParent(customPETemp->getRealBoundingRect());
                    QL_DEBUG;
                }
            }
            else if(baseItemTemp->getUnitId() == SBOX_ID)
            {
                ElementSbox *sboxTemp = dynamic_cast<ElementSbox*>(baseItemTemp);
                if(sboxTemp)
                {
                    ElementSbox* sboxCopy = new ElementSbox(*sboxTemp);
                    selectedItemsList.append(sboxTemp);
                    hadCopyItemsList.append(sboxCopy);
                    copyItemsRect |= sboxTemp->mapRectToParent(sboxTemp->getRealBoundingRect());
                    QL_DEBUG;
                }
            }
            else if(baseItemTemp->getUnitId() == BENES_ID)
            {
                ElementBenes *benesTemp = dynamic_cast<ElementBenes*>(baseItemTemp);
                if(benesTemp)
                {
                    ElementBenes* benesCopy = new ElementBenes(*benesTemp);
                    selectedItemsList.append(benesTemp);
                    hadCopyItemsList.append(benesCopy);
                    copyItemsRect |= benesTemp->mapRectToParent(benesTemp->getRealBoundingRect());
                    QL_DEBUG;
                }
            }
            break;
        }
        default:break;
        }
    }
    sortHadCopyedItems();
    if(hasWire)
    {
        copyWireFromSelectedItems();
    }
    emit pasteStateChanged(canPaste());
}

//void ItemCopyPaste::cutItems(QList<QGraphicsItem *> itemList)
//{

//}

void ItemCopyPaste::pasteItems(RcaGraphScene * rcaScene, QGraphicsItem *hoverItem , QPointF posAtThis)
{
    if(rcaScene == NULL) return;

//    rcaScene->isAutoSetItemPos = true;

    QPointF posDistance = copyItemsRect.topLeft() - posAtThis;


    if(hoverItem)
    {
        switch(hadCopyUnitId)
        {
        case Module_Rcu:
        {
            ModuleBcu* parentBcu = dynamic_cast<ModuleBcu*>(hoverItem);
            if(parentBcu)
            {
                QRectF transRect = copyItemsRect;
                QL_DEBUG << transRect;
                transRect.moveTopLeft(posAtThis);
                QL_DEBUG << transRect;
                transRect = parentBcu->enabledRect2BoundingRect(transRect);
                QRectF newRect = parentBcu->getRealBoundingRect();
//                newRect.setRight(qMax(newRect.right(),transRect.right()));//xf 粘贴的rcu和复制的rcu一样宽
                newRect.setBottom(qMax(newRect.bottom(),transRect.bottom()));
                newRect.setLeft(qMin(newRect.left(),transRect.left()));
                newRect.setTop(qMin(newRect.top(),transRect.top()));
                parentBcu->resetBoundingRectByPaste(newRect);
                QL_DEBUG;
            }
            break;
        }
        case Arith_Unit:
        {
            ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(hoverItem);
            if(parentRcu)
            {
                QRectF transRect = copyItemsRect;
                transRect.moveTopLeft(posAtThis);
                transRect = parentRcu->enabledRect2BoundingRect(transRect);
                QRectF newRect = parentRcu->getRealBoundingRect();
                QL_DEBUG << transRect << newRect;
                if(transRect.right() > newRect.right())
                {
                    newRect.setWidth(transRect.right() - newRect.left());
                    QL_DEBUG << newRect;
                }
//                newRect.setRight(qMax(newRect.right(),transRect.right()));
//                newRect.setBottom(newRect.bottom());
//                newRect.setLeft(qMin(newRect.left(),transRect.left()));
//                newRect.setTop(newRect.top());
                parentRcu->resetBoundingRectByPaste(newRect);
                QL_DEBUG;
            }

            break;
        }
        default:break;
        }
    }
    rcaScene->beginPushMacro("paste items");
    for(int i=0;i<hadCopyItemsList.count();i++)
    {
        BaseItem* copyBaseItem = hadCopyItemsList.at(i);
        if(copyBaseItem)
        {
            ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(hoverItem);
            if(parentRcu)
            {
                ElementBfu *copyBfu = dynamic_cast<ElementBfu*>(copyBaseItem);
                if(copyBfu)
                {
                    ElementBfu* pasteBfu = parentRcu->pasteNewBfu(*copyBfu,copyBfu->pos()-posDistance);
                    if(pasteBfu)
                    {
                        hadPasteItemsList.append(pasteBfu);
                        mapCopyAndPaste.insert(copyBfu,pasteBfu);
                    }
                    continue;
                }
                ElementCustomPE *copyCustomPE = dynamic_cast<ElementCustomPE*>(copyBaseItem);
                if(copyCustomPE)
                {
                    ElementCustomPE* pasteCustomPE = parentRcu->pasteNewCustomPE(*copyCustomPE,copyCustomPE->pos()-posDistance);
                    if(pasteCustomPE)
                    {
                        hadPasteItemsList.append(pasteCustomPE);
                        mapCopyAndPaste.insert(copyCustomPE,pasteCustomPE);
                    }
                    continue;
                }
                ElementSbox *copySbox = dynamic_cast<ElementSbox*>(copyBaseItem);
                if(copySbox)
                {
                    ElementSbox* pasteSbox = parentRcu->pasteNewSbox(*copySbox,copySbox->pos()-posDistance);
                    if(pasteSbox)
                    {
                        hadPasteItemsList.append(pasteSbox);
                        mapCopyAndPaste.insert(copySbox,pasteSbox);
                    }
                    continue;
                }
                ElementBenes *copyBenes = dynamic_cast<ElementBenes*>(copyBaseItem);
                if(copyBenes)
                {
                    ElementBenes* pasteBenes = parentRcu->pasteNewBenes(*copyBenes,copyBenes->pos()-posDistance);
                    if(pasteBenes)
                    {
                        hadPasteItemsList.append(pasteBenes);
                        mapCopyAndPaste.insert(copyBenes,pasteBenes);
                    }
                    continue;
                }
            }

            ModuleBcu* parentBcu = dynamic_cast<ModuleBcu*>(hoverItem);
            if(parentBcu)
            {
                ModuleRcu *copyRcu = dynamic_cast<ModuleRcu*>(copyBaseItem);
                if(copyRcu)
                {
                    ModuleRcu* pasteRcu = parentBcu->pasteNewRcu(*copyRcu,copyRcu->pos()-posDistance);
                    if(pasteRcu)
                    {
                        hadPasteItemsList.append(pasteRcu);
                        mapCopyAndPaste.insert(copyRcu,pasteRcu);
                        mapCopyRcuPasteRcu(*copyRcu,*pasteRcu);
                    }
                    continue;
                }
            }
            if(hoverItem == NULL)
            {
                ModuleBcu *copyBcu = dynamic_cast<ModuleBcu*>(copyBaseItem);
                if(copyBcu)
                {
                    ModuleBcu* pasteBcu = rcaScene->pasteNewBcu(*copyBcu,copyBcu->pos()-posDistance);
                    if(pasteBcu)
                    {
                        hadPasteItemsList.append(pasteBcu);
                        mapCopyAndPaste.insert(copyBcu,pasteBcu);
                        mapCopyBcuPasteBcu(*copyBcu,*pasteBcu);
                    }
                    continue;
                }
            }
        }
    }

    for(int i=0;i<hadCopyWiresList.count();i++)
    {
        ElementWire* wire = hadCopyWiresList.at(i);
        if(wire)
        {
            pasteWireWithCopy(*wire,*rcaScene);
        }
    }
    rcaScene->endPushMacro();
//    rcaScene->isAutoSetItemPos = false;
}

bool ItemCopyPaste::canPaste()
{
    return hadCopyItemsList.count();
}

void ItemCopyPaste::clearItemsList()
{
    while(!hadCopyWiresList.isEmpty())
    {
        ElementWire* lastWire = hadCopyWiresList.last();
        if(lastWire)
        {
            hadCopyWiresList.removeLast();
            delete lastWire;
        }
    }

    while(!hadCopyItemsList.isEmpty())
    {
        BaseItem* lastItem = hadCopyItemsList.last();
        if(lastItem)
        {
            hadCopyItemsList.removeLast();
            delete lastItem;
        }
    }

    copyItemsRect = QRectF();
    mapCopyAndPaste.clear();
    hadPasteItemsList.clear();
    hadCopyWiresList.clear();
    hadCopyItemsList.clear();
    selectedItemsList.clear();
    hadCopyUnitId = Module_None;
}

ItemCopyPaste *ItemCopyPaste::instance(QObject *parent)
{
    if(!m_instance)
    {
        m_instance = new ItemCopyPaste(parent);
    }
    return m_instance;
}

void ItemCopyPaste::deleteInstance()
{
    if(m_instance)
    {
        m_instance->deleteLater();
        m_instance = NULL;
    }
}

void ItemCopyPaste::copyWireFromSelectedItems()
{
    for(int i=0;i<selectedItemsList.count();i++)
    {
        BaseItem* baseItemTemp = dynamic_cast<BaseItem*>(selectedItemsList.at(i));
        if(baseItemTemp == NULL) continue;

        switch(hadCopyUnitId)
        {
        case Module_Bcu:
        {
            if(baseItemTemp->getUnitId() == BCU_ID)
            {
                ModuleBcu *bcuTemp = dynamic_cast<ModuleBcu*>(baseItemTemp);
                if(bcuTemp)
                {
                    copyWireFromSelectedBcu(bcuTemp);
                }
            }
            break;
        }
        case Module_Rcu:
        {
            if(baseItemTemp->getUnitId() == RCU_ID)
            {
                ModuleRcu *rcuTemp = dynamic_cast<ModuleRcu*>(baseItemTemp);
                if(rcuTemp)
                {
                    copyWireFromSelectedRcu(rcuTemp);
                }
            }
            break;
        }
        case Arith_Unit:
        {
            BaseArithUnit *arithTemp = dynamic_cast<BaseArithUnit*>(baseItemTemp);
            if(arithTemp)
            {
                copyWireFromSelectedArith(arithTemp);
            }
            break;
        }
        default:break;
        }
    }
}

void ItemCopyPaste::copyWireFrom2Port(ElementPort &inPort, ElementPort &outPort)
{
    ElementPort* copyInPort = getCopyPortFromSelectedPort(inPort);
    ElementPort* copyOutPort = getCopyPortFromSelectedPort(outPort);

    if(copyInPort && copyOutPort)
    {
        QPointF posAtScene = copyOutPort->mapToScene(QPointF(copyOutPort->boundingRect().center().x(),
                                                            copyOutPort->getRealBoundingRect().top()));
        ElementWire* eleWire = new ElementWire(posAtScene,copyOutPort,WIRE_UNSELECTED_BRUSH,WIRE_DEFAULT_PEN_WIDTH);

        QPointF newPos = copyInPort->mapToScene(QPointF(copyInPort->boundingRect().center().x(),
                                                       copyInPort->getRealBoundingRect().top()));
        eleWire->addEndPortFromXml(copyInPort,newPos);
        hadCopyWiresList.append(eleWire);
        QL_DEBUG << hadCopyWiresList.count();
    }
}

void ItemCopyPaste::pasteWireWithCopy(ElementWire &wire, RcaGraphScene& rcaScene)
{
    ElementPort* copyInPort = wire.getInputSignal();
    ElementPort* copyOutPort = wire.getOutPort();

    if((copyInPort == NULL) || (copyOutPort == NULL)) return;

    ElementPort* pasteOutPort = getPastePortFromCopyPort(*copyInPort);
    ElementPort* pasteInPort = getPastePortFromCopyPort(*copyOutPort);

    if((pasteInPort == NULL) || (pasteOutPort == NULL)) return;

    QL_DEBUG;
    pasteInPort->setInSignalAllAttr(pasteOutPort);
    rcaScene.addElementWireFromXml(pasteInPort,pasteOutPort);

}
ElementPort* ItemCopyPaste::getCopyPortFromSelectedPort(ElementPort &elePort)
{
    switch(elePort.portAttr)
    {
        case PORT_FROM_BFU_A:
        case PORT_FROM_BFU_B:
        case PORT_FROM_BFU_T:
        case PORT_FROM_BFU_X:
        case PORT_FROM_BFU_Y:
        {
            ElementBfu* selectedBfu = dynamic_cast<ElementBfu*>(elePort.parentItem());
            if(selectedBfu == NULL) return NULL;
            ElementBfu* copyBfu = getCopyBfu(selectedBfu->getIndexBcu(),
                                             selectedBfu->getIndexRcu(),
                                             selectedBfu->getIndexInRcu());
            if(copyBfu == NULL) return NULL;

            if(selectedBfu->inPortA == &elePort)   return copyBfu->inPortA;
            if(selectedBfu->inPortB == &elePort)   return copyBfu->inPortB;
            if(selectedBfu->inPortT == &elePort)   return copyBfu->inPortT;

            if(selectedBfu->outPortX == &elePort)   return copyBfu->outPortX;
            if(selectedBfu->outPortY == &elePort)   return copyBfu->outPortY;
            break;
        }
        case PORT_FROM_CUSTOMPE_IN:
        case PORT_FROM_CUSTOMPE_OUT:
        {
            ElementCustomPE* selectedCustomPE = dynamic_cast<ElementCustomPE*>(elePort.parentItem());
            if(selectedCustomPE == NULL) return NULL;
            ElementCustomPE* copyCustomPE = getCopyCustomPE(selectedCustomPE->getIndexBcu(),
                                             selectedCustomPE->getIndexRcu(),
                                             selectedCustomPE->getIndexInRcu());
            if(copyCustomPE == NULL) return NULL;

            for (int i = 0; i < selectedCustomPE->inputNum(); ++i)
            {
                if(selectedCustomPE->m_inputPortVector.at(i) == &elePort)
                    return copyCustomPE->m_inputPortVector.at(i);
            }

            for (int i = 0; i < selectedCustomPE->outputNum(); ++i)
            {
                if(selectedCustomPE->m_outputPortVector.at(i) == &elePort)
                    return copyCustomPE->m_outputPortVector.at(i);
            }
            break;
        }
        case PORT_FROM_SBOX_IN:
        case PORT_FROM_SBOX_OUT:
        {
            ElementSbox* selectedSbox = dynamic_cast<ElementSbox*>(elePort.parentItem());
            if(selectedSbox == NULL) return NULL;
            ElementSbox* copySbox = getCopySbox(selectedSbox->getIndexBcu(),
                                             selectedSbox->getIndexRcu(),
                                             selectedSbox->getIndexInRcu());
            if(copySbox == NULL) return NULL;

            if(selectedSbox->inPort0 == &elePort)   return copySbox->inPort0;

            if(selectedSbox->outPort0 == &elePort)   return copySbox->outPort0;
            if(selectedSbox->outPort1 == &elePort)   return copySbox->outPort1;
            if(selectedSbox->outPort2 == &elePort)   return copySbox->outPort2;
            if(selectedSbox->outPort3 == &elePort)   return copySbox->outPort3;
            break;
        }
        case PORT_FROM_BENES_IN:
        case PORT_FROM_BENES_OUT:
        {
            ElementBenes* selectedBenes = dynamic_cast<ElementBenes*>(elePort.parentItem());
            if(selectedBenes == NULL) return NULL;
            ElementBenes* copyBenes = getCopyBenes(selectedBenes->getIndexBcu(),
                                             selectedBenes->getIndexRcu(),
                                             selectedBenes->getIndexInRcu());
            if(copyBenes == NULL) return NULL;

            if(selectedBenes->inPort0 == &elePort)   return copyBenes->inPort0;
            if(selectedBenes->inPort1 == &elePort)   return copyBenes->inPort1;
            if(selectedBenes->inPort2 == &elePort)   return copyBenes->inPort2;
            if(selectedBenes->inPort3 == &elePort)   return copyBenes->inPort3;

            if(selectedBenes->outPort0 == &elePort)   return copyBenes->outPort0;
            if(selectedBenes->outPort1 == &elePort)   return copyBenes->outPort1;
            if(selectedBenes->outPort2 == &elePort)   return copyBenes->outPort2;
            if(selectedBenes->outPort3 == &elePort)   return copyBenes->outPort3;
            break;
        }
        case PORT_FROM_INFIFO:
        case PORT_FORM_OUTFIFO:
        {
            ModuleRcu* selectedRcu = dynamic_cast<ModuleRcu*>(elePort.parentItem());
            if(selectedRcu == NULL) return NULL;
            ModuleRcu* copyRcu = getCopyRcu(selectedRcu->getIndexBcu(), selectedRcu->getIndexRcu());
            if(copyRcu == NULL) return NULL;

            if(selectedRcu->outFifo0 == &elePort)   return copyRcu->outFifo0;
            if(selectedRcu->outFifo1 == &elePort)   return copyRcu->outFifo1;
            if(selectedRcu->outFifo2 == &elePort)   return copyRcu->outFifo2;
            if(selectedRcu->outFifo3 == &elePort)   return copyRcu->outFifo3;

            if(selectedRcu->inFifo0 == &elePort)   return copyRcu->inFifo0;
            if(selectedRcu->inFifo1 == &elePort)   return copyRcu->inFifo1;
            if(selectedRcu->inFifo2 == &elePort)   return copyRcu->inFifo2;
            if(selectedRcu->inFifo3 == &elePort)   return copyRcu->inFifo3;
            break;
        }
        default:break;
    }
    return NULL;
}

ElementPort *ItemCopyPaste::getPastePortFromCopyPort(ElementPort &elePort)
{
    switch(elePort.portAttr)
    {
        case PORT_FROM_BFU_A:
        case PORT_FROM_BFU_B:
        case PORT_FROM_BFU_T:
        case PORT_FROM_BFU_X:
        case PORT_FROM_BFU_Y:
        {
            ElementBfu* copyBfu = dynamic_cast<ElementBfu*>(elePort.parentItem());
            if(copyBfu == NULL) return NULL;

            BaseItem* copyItem = copyBfu;
            BaseItem* pasteItem = mapCopyAndPaste.value(copyItem,NULL);

            ElementBfu* pasteBfu = dynamic_cast<ElementBfu*>(pasteItem);
            if(pasteBfu == NULL) return NULL;

            if(copyBfu->inPortA == &elePort)   return pasteBfu->inPortA;
            if(copyBfu->inPortB == &elePort)   return pasteBfu->inPortB;
            if(copyBfu->inPortT == &elePort)   return pasteBfu->inPortT;

            if(copyBfu->outPortX == &elePort)   return pasteBfu->outPortX;
            if(copyBfu->outPortY == &elePort)   return pasteBfu->outPortY;
            break;
        }
        case PORT_FROM_SBOX_IN:
        case PORT_FROM_SBOX_OUT:
        {
            ElementSbox* copySbox = dynamic_cast<ElementSbox*>(elePort.parentItem());
            if(copySbox == NULL) return NULL;

            BaseItem* copyItem = copySbox;
            BaseItem* pasteItem = mapCopyAndPaste.value(copyItem,NULL);

            ElementSbox* pasteSbox = dynamic_cast<ElementSbox*>(pasteItem);
            if(pasteSbox == NULL) return NULL;

            if(copySbox->inPort0 == &elePort)   return pasteSbox->inPort0;

            if(copySbox->outPort0 == &elePort)   return pasteSbox->outPort0;
            if(copySbox->outPort1 == &elePort)   return pasteSbox->outPort1;
            if(copySbox->outPort2 == &elePort)   return pasteSbox->outPort2;
            if(copySbox->outPort3 == &elePort)   return pasteSbox->outPort3;
            break;
        }
        case PORT_FROM_BENES_IN:
        case PORT_FROM_BENES_OUT:
        {
            ElementBenes* copyBenes = dynamic_cast<ElementBenes*>(elePort.parentItem());
            if(copyBenes == NULL) return NULL;

            BaseItem* copyItem = copyBenes;
            BaseItem* pasteItem = mapCopyAndPaste.value(copyItem,NULL);

            ElementBenes* pasteBenes = dynamic_cast<ElementBenes*>(pasteItem);
            if(pasteBenes == NULL) return NULL;

            if(copyBenes->inPort0 == &elePort)   return pasteBenes->inPort0;
            if(copyBenes->inPort1 == &elePort)   return pasteBenes->inPort1;
            if(copyBenes->inPort2 == &elePort)   return pasteBenes->inPort2;
            if(copyBenes->inPort3 == &elePort)   return pasteBenes->inPort3;

            if(copyBenes->outPort0 == &elePort)   return pasteBenes->outPort0;
            if(copyBenes->outPort1 == &elePort)   return pasteBenes->outPort1;
            if(copyBenes->outPort2 == &elePort)   return pasteBenes->outPort2;
            if(copyBenes->outPort3 == &elePort)   return pasteBenes->outPort3;
            break;
        }
        case PORT_FROM_INFIFO:
        case PORT_FORM_OUTFIFO:
        {
            ModuleRcu* copyRcu = dynamic_cast<ModuleRcu*>(elePort.parentItem());
            if(copyRcu == NULL) return NULL;

            BaseItem* copyItem = copyRcu;
            BaseItem* pasteItem = mapCopyAndPaste.value(copyItem,NULL);

            ModuleRcu* pasteRcu = dynamic_cast<ModuleRcu*>(pasteItem);
            if(pasteRcu == NULL) return NULL;

            if(copyRcu->outFifo0 == &elePort)   return pasteRcu->outFifo0;
            if(copyRcu->outFifo1 == &elePort)   return pasteRcu->outFifo1;
            if(copyRcu->outFifo2 == &elePort)   return pasteRcu->outFifo2;
            if(copyRcu->outFifo3 == &elePort)   return pasteRcu->outFifo3;

            if(copyRcu->inFifo0 == &elePort)   return pasteRcu->inFifo0;
            if(copyRcu->inFifo1 == &elePort)   return pasteRcu->inFifo1;
            if(copyRcu->inFifo2 == &elePort)   return pasteRcu->inFifo2;
            if(copyRcu->inFifo3 == &elePort)   return pasteRcu->inFifo3;
            break;
        }
        default:break;
    }
    return NULL;
}

ModuleBcu *ItemCopyPaste::getCopyBcu(int indexBcu)
{
    if(hadCopyUnitId == Module_Bcu)
    {
        for(int i=0;i<hadCopyItemsList.count();i++)
        {
            ModuleBcu* temp = dynamic_cast<ModuleBcu*>(hadCopyItemsList.at(i));
            if(temp && (temp->getIndexBcu() == indexBcu))
            {
                return temp;
            }
        }
    }
    return NULL;
}

ModuleRcu *ItemCopyPaste::getCopyRcu(int indexBcu, int indexRcu)
{
    if(hadCopyUnitId == Module_Bcu)
    {
        ModuleBcu* temp = getCopyBcu(indexBcu);
        if(temp)
        {
            QList<ModuleRcu*> rcuList = temp->rcuExistList;
            for(int i=0;i<rcuList.count();i++)
            {
                ModuleRcu* temp = rcuList.at(i);
                if(temp && (temp->getIndexBcu() == indexBcu) && (temp->getIndexRcu() == indexRcu))
                {
                    return temp;
                }
            }
        }
    }
    else if(hadCopyUnitId == Module_Rcu)
    {
        for(int i=0;i<hadCopyItemsList.count();i++)
        {
            ModuleRcu* temp = dynamic_cast<ModuleRcu*>(hadCopyItemsList.at(i));
            if(temp && (temp->getIndexBcu() == indexBcu) && (temp->getIndexRcu() == indexRcu))
            {
                return temp;
            }
        }
    }
    return NULL;
}

ElementBfu *ItemCopyPaste::getCopyBfu(int indexBcu, int indexRcu, int index)
{
    if((hadCopyUnitId == Module_Bcu) || (hadCopyUnitId == Module_Rcu))
    {
        ModuleRcu* temp = getCopyRcu(indexBcu,indexRcu);
        if(temp)
        {
            QList<ElementBfu*> bfuList = temp->bfuExistList;

            for(int i=0;i<bfuList.count();i++)
            {
                ElementBfu* temp = bfuList.at(i);
                if(temp && (temp->getIndexBcu() == indexBcu)
                        && (temp->getIndexRcu() == indexRcu)
                        && (temp->getIndexInRcu() == index))
                {
                    return temp;
                }
            }
        }
    }
    else if(hadCopyUnitId == Arith_Unit)
    {
        for(int i=0;i<hadCopyItemsList.count();i++)
        {
            ElementBfu* temp = dynamic_cast<ElementBfu*>(hadCopyItemsList.at(i));
            if(temp && (temp->getIndexBcu() == indexBcu)
                    && (temp->getIndexRcu() == indexRcu)
                    && (temp->getIndexInRcu() == index))
            {
                return temp;
            }
        }
    }

    return NULL;
}

ElementCustomPE *ItemCopyPaste::getCopyCustomPE(int indexBcu, int indexRcu, int index)
{
    if((hadCopyUnitId == Module_Bcu) || (hadCopyUnitId == Module_Rcu))
    {
        ModuleRcu* temp = getCopyRcu(indexBcu,indexRcu);
        if(temp)
        {
            QList<ElementCustomPE*> customPEList = temp->customPEExistList;

            for(int i=0;i<customPEList.count();i++)
            {
                ElementCustomPE* temp = customPEList.at(i);
                if(temp && (temp->getIndexBcu() == indexBcu)
                        && (temp->getIndexRcu() == indexRcu)
                        && (temp->getIndexInRcu() == index))
                {
                    return temp;
                }
            }
        }
    }
    else if(hadCopyUnitId == Arith_Unit)
    {
        for(int i=0;i<hadCopyItemsList.count();i++)
        {
            ElementCustomPE* temp = dynamic_cast<ElementCustomPE*>(hadCopyItemsList.at(i));
            if(temp && (temp->getIndexBcu() == indexBcu)
                    && (temp->getIndexRcu() == indexRcu)
                    && (temp->getIndexInRcu() == index))
            {
                return temp;
            }
        }
    }

    return NULL;
}

ElementSbox *ItemCopyPaste::getCopySbox(int indexBcu, int indexRcu, int index)
{
    if((hadCopyUnitId == Module_Bcu) || (hadCopyUnitId == Module_Rcu))
    {
        ModuleRcu* temp = getCopyRcu(indexBcu,indexRcu);
        if(temp)
        {
            QList<ElementSbox*> sboxList = temp->sboxExistList;
            for(int i=0;i<sboxList.count();i++)
            {
                ElementSbox* temp = sboxList.at(i);
                if(temp && (temp->getIndexBcu() == indexBcu)
                        && (temp->getIndexRcu() == indexRcu)
                        && (temp->getIndexInRcu() == index))
                {
                    return temp;
                }
            }
        }
    }
    else if(hadCopyUnitId == Arith_Unit)
    {
        for(int i=0;i<hadCopyItemsList.count();i++)
        {
            ElementSbox* temp = dynamic_cast<ElementSbox*>(hadCopyItemsList.at(i));
            if(temp && (temp->getIndexBcu() == indexBcu)
                    && (temp->getIndexRcu() == indexRcu)
                    && (temp->getIndexInRcu() == index))
            {
                return temp;
            }
        }
    }
    return NULL;
}

ElementBenes *ItemCopyPaste::getCopyBenes(int indexBcu, int indexRcu, int index)
{
    if((hadCopyUnitId == Module_Bcu) || (hadCopyUnitId == Module_Rcu))
    {
        ModuleRcu* temp = getCopyRcu(indexBcu,indexRcu);
        if(temp)
        {
            QList<ElementBenes*> benesList = temp->benesExistList;
            for(int i=0;i<benesList.count();i++)
            {
                ElementBenes* temp = benesList.at(i);
                if(temp && (temp->getIndexBcu() == indexBcu)
                        && (temp->getIndexRcu() == indexRcu)
                        && (temp->getIndexInRcu() == index))
                {
                    return temp;
                }
            }
        }
    }
    else if(hadCopyUnitId == Arith_Unit)
    {
        for(int i=0;i<hadCopyItemsList.count();i++)
        {
            ElementBenes* temp = dynamic_cast<ElementBenes*>(hadCopyItemsList.at(i));
            if(temp && (temp->getIndexBcu() == indexBcu)
                    && (temp->getIndexRcu() == indexRcu)
                    && (temp->getIndexInRcu() == index))
            {
                return temp;
            }
        }
    }

    return NULL;
}

void ItemCopyPaste::copyWireFromSelectedBcu(ModuleBcu *bcuItem)
{
    if(bcuItem == NULL) return;

    for(int i=0;i<bcuItem->rcuExistList.count();i++)
    {
        copyWireFromSelectedRcu(bcuItem->rcuExistList.at(i));
    }

}

void ItemCopyPaste::copyWireFromSelectedRcu(ModuleRcu *rcuItem)
{
    if(rcuItem == NULL) return;

    for(int i=0;i<rcuItem->bfuExistList.size();i++)
    {
        copyWireFromSelectedArith(rcuItem->bfuExistList.at(i));
    }

    for(int i=0;i<rcuItem->sboxExistList.size();i++)
    {
        copyWireFromSelectedArith(rcuItem->sboxExistList.at(i));
    }

    for(int i=0;i<rcuItem->benesExistList.size();i++)
    {
        copyWireFromSelectedArith(rcuItem->benesExistList.at(i));
    }

    ElementPort* inPort0 = dynamic_cast<ElementPort*>(rcuItem->outFifo0->getInSignal());
    if(inPort0)
    {
        copyWireFrom2Port(*rcuItem->outFifo0,*inPort0);
    }

    ElementPort* inPort1 = dynamic_cast<ElementPort*>(rcuItem->outFifo1->getInSignal());
    if(inPort1)
    {
        copyWireFrom2Port(*rcuItem->outFifo1,*inPort1);
    }

    ElementPort* inPort2 = dynamic_cast<ElementPort*>(rcuItem->outFifo2->getInSignal());
    if(inPort2)
    {
        copyWireFrom2Port(*rcuItem->outFifo2,*inPort2);
    }

    ElementPort* inPort3 = dynamic_cast<ElementPort*>(rcuItem->outFifo3->getInSignal());
    if(inPort3)
    {
        copyWireFrom2Port(*rcuItem->outFifo3,*inPort3);
    }

}

void ItemCopyPaste::copyWireFromSelectedArith(BaseArithUnit *arithItem)
{
    if(arithItem == NULL) return;

    ElementBfu* bfuItem = dynamic_cast<ElementBfu*>(arithItem);
    if(bfuItem)
    {
        ElementPort* inPortA = dynamic_cast<ElementPort*>(bfuItem->inPortA->getInSignal());
        if(inPortA)
        {
            copyWireFrom2Port(*bfuItem->inPortA,*inPortA);
        }
        ElementPort* inPortB = dynamic_cast<ElementPort*>(bfuItem->inPortB->getInSignal());
        if(inPortB)
        {
            copyWireFrom2Port(*bfuItem->inPortB,*inPortB);
        }
        ElementPort* inPortT = dynamic_cast<ElementPort*>(bfuItem->inPortT->getInSignal());
        if(inPortT)
        {
            copyWireFrom2Port(*bfuItem->inPortT,*inPortT);
        }
        return;
    }

    ElementSbox* sboxItem = dynamic_cast<ElementSbox*>(arithItem);
    if(sboxItem)
    {
        ElementPort* inPort0 = dynamic_cast<ElementPort*>(sboxItem->inPort0->getInSignal());
        if(inPort0)
        {
            copyWireFrom2Port(*sboxItem->inPort0,*inPort0);
        }
        return;
    }

    ElementBenes* benesItem = dynamic_cast<ElementBenes*>(arithItem);
    if(benesItem)
    {
        ElementPort* inPort0 = dynamic_cast<ElementPort*>(benesItem->inPort0->getInSignal());
        if(inPort0)
        {
            copyWireFrom2Port(*benesItem->inPort0,*inPort0);
        }
        ElementPort* inPort1 = dynamic_cast<ElementPort*>(benesItem->inPort1->getInSignal());
        if(inPort1)
        {
            copyWireFrom2Port(*benesItem->inPort1,*inPort1);
        }
        ElementPort* inPort2 = dynamic_cast<ElementPort*>(benesItem->inPort2->getInSignal());
        if(inPort2)
        {
            copyWireFrom2Port(*benesItem->inPort2,*inPort2);
        }
        ElementPort* inPort3 = dynamic_cast<ElementPort*>(benesItem->inPort3->getInSignal());
        if(inPort3)
        {
            copyWireFrom2Port(*benesItem->inPort3,*inPort3);
        }
        return;
    }
}

void ItemCopyPaste::mapCopyBcuPasteBcu(ModuleBcu &copyBcu, ModuleBcu &pasteBcu)
{
    if(copyBcu.rcuExistList.count() == pasteBcu.rcuExistList.count())
    {
        for(int i=0;i<copyBcu.rcuExistList.count();i++)
        {
            ModuleRcu *copyRcu = copyBcu.rcuExistList.at(i);
            ModuleRcu *pasteRcu = pasteBcu.rcuExistList.at(i);
            mapCopyAndPaste.insert(copyRcu,pasteRcu);
            mapCopyRcuPasteRcu(*copyRcu,*pasteRcu);
        }
    }
}

void ItemCopyPaste::mapCopyRcuPasteRcu(ModuleRcu &copyRcu, ModuleRcu &pasteRcu)
{
    if(copyRcu.bfuExistList.count() == pasteRcu.bfuExistList.count())
    {
        for(int i=0;i<copyRcu.bfuExistList.count();i++)
        {
            mapCopyAndPaste.insert(copyRcu.bfuExistList.at(i),pasteRcu.bfuExistList.at(i));
        }
    }
    if(copyRcu.sboxExistList.count() == pasteRcu.sboxExistList.count())
    {
        for(int i=0;i<copyRcu.sboxExistList.count();i++)
        {
            mapCopyAndPaste.insert(copyRcu.sboxExistList.at(i),pasteRcu.sboxExistList.at(i));
        }
    }
    if(copyRcu.benesExistList.count() == pasteRcu.benesExistList.count())
    {
        for(int i=0;i<copyRcu.benesExistList.count();i++)
        {
            mapCopyAndPaste.insert(copyRcu.benesExistList.at(i),pasteRcu.benesExistList.at(i));
        }
    }
}

void ItemCopyPaste::sortHadCopyedItems()
{
    switch(hadCopyUnitId)
    {
    case Module_Bcu:
    {
        for(int i = 0; i<hadCopyItemsList.length(); i++)
        {
            for (int j = i+1; j < hadCopyItemsList.length(); j++)
            {
                ModuleBcu *copyBcu0 = dynamic_cast<ModuleBcu*>(hadCopyItemsList.at(i));
                ModuleBcu *copyBcu1 = dynamic_cast<ModuleBcu*>(hadCopyItemsList.at(j));
                if(copyBcu0 && copyBcu1 &&copyBcu0->getIndexBcu() > copyBcu1->getIndexBcu())
                {
                    hadCopyItemsList.swap(i,j);
                }
            }
        }
        break;
    }
    case Module_Rcu:
    {
        for(int i = 0; i<hadCopyItemsList.length(); i++)
        {
            for (int j = i+1; j < hadCopyItemsList.length(); j++)
            {
                ModuleRcu *copyRcu0 = dynamic_cast<ModuleRcu*>(hadCopyItemsList.at(i));
                ModuleRcu *copyRcu1 = dynamic_cast<ModuleRcu*>(hadCopyItemsList.at(j));
                if(copyRcu0 && copyRcu1 &&copyRcu0->getIndexRcu() > copyRcu1->getIndexRcu())
                {
                    hadCopyItemsList.swap(i,j);
                }
            }
        }
        break;
    }
    case Arith_Unit:
    {
        for(int i = 0; i<hadCopyItemsList.length(); i++)
        {
            for (int j = i+1; j < hadCopyItemsList.length(); j++)
            {
                BaseArithUnit *copyArith0 = dynamic_cast<BaseArithUnit*>(hadCopyItemsList.at(i));
                BaseArithUnit *copyArith1 = dynamic_cast<BaseArithUnit*>(hadCopyItemsList.at(j));
                if(copyArith0 && copyArith1 &&copyArith0->getIndexInRcu() > copyArith1->getIndexInRcu())
                {
                    hadCopyItemsList.swap(i,j);
                }
            }
        }
        break;
    }
    default:break;
    }
}
