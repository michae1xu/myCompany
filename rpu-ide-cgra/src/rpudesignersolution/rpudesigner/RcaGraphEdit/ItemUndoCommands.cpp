#include "ItemUndoCommands.h"
#include "BaseItem.h"
#include "RcaGraphScene.h"
#include "ElementWire.h"
#include "ModuleBcu.h"
#include "ModuleRcu.h"
#include "BaseArithUnit.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "elementcustompe.h"
#include "ModuleReadMem.h"
#include "ModuleWriteMem.h"

#include <QGraphicsScene>

ItemUndoCommands::ItemUndoCommands(QUndoCommand *parent) : QUndoCommand(parent)
{
    oldSceneModified = false;
    newSceneModified = false;
}

//void ItemUndoCommands::modifiedChanged(bool modified)
//{
//    oldSceneModified = true;
//    newSceneModified = modified;
//}

void ItemUndoCommands::setToModified()
{
    oldSceneModified = true;
    newSceneModified = true;
}

void ItemUndoCommands::setNoModified()
{
    oldSceneModified = true;
    newSceneModified = false;
}

MoveBaseItem::MoveBaseItem(BaseItem *item, QPointF oldPos, QPointF newPos)
    : baseItem(item)
    , myOldPos(oldPos)
    , myNewPos(newPos)
{
//    baseItem = item;
//    myOldPos = oldPos;
//    myNewPos = newPos;
}

ItemMoveCommand::ItemMoveCommand(RcaGraphScene* scene,QList<BaseItem *> itemList, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , moveItemList(QList<MoveBaseItem>())
{
    Q_ASSERT(rcaScene);
    for(int i=0; i<itemList.count(); i++)
    {
        BaseItem *itemTemp = itemList.at(i);
        if((itemTemp) && itemTemp->pos() != itemTemp->getOldPos())
        {
            moveItemList.append(MoveBaseItem(itemTemp,itemTemp->getOldPos(),itemTemp->pos()));
        }
    }
    oldSceneModified = rcaScene->isModified;
    newSceneModified = itemList.count() > 0 ? true : false;
}

void ItemMoveCommand::undo()
{
    for(int i=0; i<moveItemList.count(); i++)
    {
        moveItemList.at(i).baseItem->setPos(moveItemList.at(i).myOldPos);
        moveItemList.at(i).baseItem->setSelected(true);
        moveItemList.at(i).baseItem->scene()->update();
    }
    rcaScene->sendWindowToModified(oldSceneModified);
}

void ItemMoveCommand::redo()
{
    for(int i=0; i<moveItemList.count(); i++)
    {
        moveItemList.at(i).baseItem->setPos(moveItemList.at(i).myNewPos);
        moveItemList.at(i).baseItem->setSelected(true);
        moveItemList.at(i).baseItem->scene()->update();
//        moveItemList.at(i).baseItem->setWindowToModified();
    }
    rcaScene->sendWindowToModified(newSceneModified);
}


ItemRectChangedCommand::ItemRectChangedCommand(RcaGraphScene* scene,BaseItem *item,QRectF oldRect,QRectF newRect,QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , changedItem(item)
    , itemOldRect(oldRect)
    , itemNewRect(newRect)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(changedItem);
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void ItemRectChangedCommand::undo()
{
    if(changedItem->getUnitId() == BCU_ID)
    {
        ModuleBcu* bcuModule = dynamic_cast<ModuleBcu*>(changedItem);
        if(bcuModule)
        {
            bcuModule->undoRectToRealBoundingRect(itemOldRect);
        }
    }
    else
    {
        changedItem->setRealBoundingRect(itemOldRect);
    }

    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void ItemRectChangedCommand::redo()
{
    if(changedItem->getUnitId() == BCU_ID)
    {
        ModuleBcu* bcuModule = dynamic_cast<ModuleBcu*>(changedItem);
        if(bcuModule)
        {
            bcuModule->undoRectToRealBoundingRect(itemNewRect);
        }
    }
    else
    {
        changedItem->setRealBoundingRect(itemNewRect);
    }
    emit rcaScene->sendWindowToModified(newSceneModified);
}

DeleteWireCommand::DeleteWireCommand(RcaGraphScene *scene, QList<ElementWire *> wireList, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , delWireList(wireList)
    , rcaScene(scene)
{
    Q_ASSERT(scene);
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

DeleteWireCommand::~DeleteWireCommand()
{
    QL_DEBUG;
    for(int i=0;i<delWireList.count();i++)
    {
        ElementWire* wire = delWireList.at(i);
//        if(wire && (!wire->scene()))
        if(wire && (!wire->scene()) && (wire->lastCmd == this))
        {
            wire->deleteLater();
            wire = NULL;
        }
    }
    delWireList.clear();
}

void DeleteWireCommand::undo()
{
    QL_DEBUG << "DeleteWireCommand";
    if(rcaScene == NULL) return;

    for(int i=0;i<delWireList.count();i++)
    {
        ElementWire* wire = delWireList.at(i);
        if(wire)
        {
            wire->setSelected(true);
            wire->lastCmd = this;
            wire->isHadToShow = true;
            if(wire->parentItem() == NULL)
            {
                rcaScene->addItem(wire);
            }
            rcaScene->addWireToList(wire);
            wire->setWireVisibleForUndo(true);
        }
    }
    emit rcaScene->sendWindowToModified(oldSceneModified);
    rcaScene->update();
}

void DeleteWireCommand::redo()
{
    QL_DEBUG << "DeleteWireCommand";
    if(rcaScene == NULL) return;
    for(int i=0;i<delWireList.count();i++)
    {
        ElementWire* wire = delWireList.at(i);
        if(wire)
        {
            wire->lastCmd = this;
            wire->isHadToShow = true;
            if(wire->parentItem() == NULL)
            {
                rcaScene->removeItem(wire);
            }
            rcaScene->removeWireFromList(wire);
            emit rcaScene->sendWindowToModified(newSceneModified);
            wire->setWireVisibleForUndo(false);
        }
    }
    rcaScene->update();
}

AddWireCommand::AddWireCommand(RcaGraphScene *scene, QPointF posAtScene, ElementPort *inSingal, QBrush penBrush, qreal penWidth, QGraphicsItem *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addWire(NULL)
    , rcaScene(scene)
    , wireParent(itemParent)
{
    addWire = new ElementWire(posAtScene,inSingal,penBrush,penWidth,itemParent);
    addWire->lastCmd = this;
    if(wireParent == nullptr)
    {
        rcaScene->addItem(addWire);
    }
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddWireCommand::~AddWireCommand()
{
    QL_DEBUG;
    if(addWire && (!addWire->scene()) && (addWire->lastCmd == this))
    {
        rcaScene->addItem(addWire);
        addWire->deleteLater();
        addWire = NULL;
    }
}

void AddWireCommand::undo()
{
    QL_DEBUG;
    if(rcaScene)
    {
        if(wireParent == NULL)
        {
            rcaScene->removeItem(addWire);
        }
        addWire->lastCmd = this;
        addWire->isHadToShow = true;
        rcaScene->removeWireFromList(addWire);
        addWire->setWireVisibleForUndo(false);
        emit rcaScene->sendWindowToModified(oldSceneModified);
        rcaScene->update();
    }
}

void AddWireCommand::redo()
{
    QL_DEBUG;
    if(rcaScene)
    {
        if(wireParent == NULL && addWire->scene() == NULL)
        {
            rcaScene->addItem(addWire);
        }
        addWire->lastCmd = this;
        addWire->isHadToShow = true;
        rcaScene->addWireToList(addWire);
        addWire->setWireVisibleForUndo(true);
        emit rcaScene->sendWindowToModified(newSceneModified);
        rcaScene->update();
    }
}

WireChangeCommand::WireChangeCommand(RcaGraphScene *scene, ElementWire *wire, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , changedWire(wire)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(changedWire);
    if(changedWire && changedWire->getOutPort())
    {
        ElementPort* outPort = changedWire->getOutPort();
        newPortPressedPosList = outPort->portPressedPosList;
        newPortInfletionPosList = outPort->portInfletionPosList;
        oldPortPressedPosList = outPort->oldPortPressedPosList;
        oldPortInfletionPosList = outPort->oldPortInfletionPosList;
    }
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void WireChangeCommand::undo()
{
    changedWire->setSelected(true);
    changedWire->paintWireByUndoCmd(oldPortPressedPosList,oldPortInfletionPosList);
    changedWire->setWindowToModified();
    changedWire->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void WireChangeCommand::redo()
{
    changedWire->setSelected(true);
    changedWire->paintWireByUndoCmd(newPortPressedPosList,newPortInfletionPosList);
    changedWire->setWindowToModified();
    changedWire->update();
    emit rcaScene->sendWindowToModified(newSceneModified);
}

//AddItemCommand::AddItemCommand(RcaGraphScene *scene, ElementUnitId id, BaseItem *itemParent, QUndoCommand *parent)
//    : ItemUndoCommands(parent)
//    , addItem(NULL)
//    , rcaScene(scene)
//    , parentItem(itemParent)
//    , unitId(id)
//{
//    switch(unitId)
//    {
//    case BCU_ID:break;
//    case RCU_ID:break;
//    case BFU_ID:break;
//    case SBOX_ID:break;
//    case BENES_ID:break;
//    default:break;
//    }
//}

//AddItemCommand::~AddItemCommand()
//{

//}

//void AddItemCommand::undo()
//{

//}

//void AddItemCommand::redo()
//{

//}

//int AddItemCommand::id() const
//{

//}

AddBcuCommand::AddBcuCommand(RcaGraphScene *scene, int bcuIndex, /*QPointF pos,*/QGraphicsItem *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addBcu(NULL)
    , rcaScene(scene)
    , bcuParent(itemParent)
//    , posAtParent(pos)
{
    Q_ASSERT(scene);
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
    addBcu = new ModuleBcu(bcuIndex,bcuParent);
    if(bcuParent == NULL)
    {
        rcaScene->addItem(addBcu);
    }
    addBcu->lastCmd = this;
}

AddBcuCommand::~AddBcuCommand()
{
    QL_DEBUG;
    if(addBcu && (!addBcu->scene()) && (addBcu->lastCmd == this))
    {
        addBcu->deleteLater();
        addBcu = NULL;
    }
}

void AddBcuCommand::undo()
{
    if(bcuParent == NULL)
    {
        rcaScene->removeItem(addBcu);
    }
    addBcu->setSelected(true);
    addBcu->lastCmd = this;
    addBcu->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddBcuCommand::redo()
{
    if(bcuParent == NULL)
    {
        rcaScene->addItem(addBcu);
    }
    addBcu->setSelected(true);
    addBcu->recheckCollidedItems();
    addBcu->lastCmd = this;
//        addBcu->setPos(posAtParent);
    rcaScene->insertBcuToList(addBcu->getIndexBcu(),addBcu);
    rcaScene->connect(addBcu,SIGNAL(itemBeDeleted(BaseItem*)),rcaScene,SLOT(handleBcuBeDeleted(BaseItem*)));
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}

AddRcuCommand::AddRcuCommand(RcaGraphScene *scene, int bcuIndex, int rcuIndex, ModuleBcu *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addRcu(NULL)
    , rcaScene(scene)
    , parentBcu(itemParent)
{
    Q_ASSERT(parentBcu);
    Q_ASSERT(rcaScene);
    addRcu = new ModuleRcu(bcuIndex,rcuIndex,parentBcu);
    addRcu->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddRcuCommand::~AddRcuCommand()
{
    QL_DEBUG;
    if(addRcu && (!addRcu->scene())
//            && (!parentBcu->childItems().contains(addRcu))
            && (addRcu->lastCmd == this))
    {
        addRcu->deleteLater();
        addRcu = NULL;
    }
}

void AddRcuCommand::undo()
{
    Q_ASSERT(addRcu);
    Q_ASSERT(rcaScene);
    Q_ASSERT(parentBcu);
    rcaScene->removeItem(addRcu);
    addRcu->lastCmd = this;
    addRcu->setSelected(true);
    addRcu->deleteThisForUndo();
    parentBcu->update();
    rcaScene->sendWindowToModified(oldSceneModified);
}

void AddRcuCommand::redo()
{
    Q_ASSERT(addRcu);
    Q_ASSERT(parentBcu);
    Q_ASSERT(rcaScene);

    addRcu->setParentItem(parentBcu);
    addRcu->recheckCollidedItems();
    addRcu->lastCmd = this;
    addRcu->setSelected(true);
    parentBcu->insertRcuToList(addRcu->getIndexRcu(),addRcu);
    parentBcu->connect(addRcu,SIGNAL(itemBeDeleted(BaseItem*)),parentBcu,SLOT(handleRcuBeDeleted(BaseItem*)));
    emit rcaScene->sendWindowToModified(newSceneModified);
    parentBcu->update();
}

AddBfuCommand::AddBfuCommand(RcaGraphScene *scene, int bcuIndex, int rcuIndex, int indexInRcu, ModuleRcu *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addBfu(NULL)
    , rcaScene(scene)
    , parentRcu(itemParent)
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(rcaScene);
    addBfu = new ElementBfu(bcuIndex,rcuIndex,indexInRcu,parentRcu);
    addBfu->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddBfuCommand::~AddBfuCommand()
{
    QL_DEBUG;
    if(addBfu && (!addBfu->scene()) && (addBfu->lastCmd == this))
    {
        addBfu->deleteLater();
        addBfu = NULL;
    }
}

void AddBfuCommand::undo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addBfu);
    Q_ASSERT(rcaScene);
    rcaScene->removeItem(addBfu);
    addBfu->lastCmd = this;
    addBfu->setSelected(true);
    addBfu->deleteThisForUndo();
    parentRcu->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddBfuCommand::redo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addBfu);
    Q_ASSERT(rcaScene);
    addBfu->setParentItem(parentRcu);
    addBfu->recheckCollidedItems();
    addBfu->lastCmd = this;
    addBfu->setSelected(true);
    parentRcu->insertBfuToList(addBfu->getIndexInRcu(),addBfu);
    parentRcu->connect(addBfu,SIGNAL(itemBeDeleted(BaseItem*)),parentRcu,SLOT(handleBfuBeDeleted(BaseItem*)));
    parentRcu->connect(addBfu,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),parentRcu,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    parentRcu->connect(addBfu,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),parentRcu,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    emit rcaScene->sendWindowToModified(newSceneModified);
    parentRcu->update();
}

AddSboxCommand::AddSboxCommand(RcaGraphScene *scene, int bcuIndex, int rcuIndex, int indexInRcu, ModuleRcu *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addSbox(NULL)
    , rcaScene(scene)
    , parentRcu(itemParent)
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(rcaScene);
    addSbox = new ElementSbox(bcuIndex,rcuIndex,indexInRcu,parentRcu);
    addSbox->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddSboxCommand::~AddSboxCommand()
{
    QL_DEBUG;
    if(addSbox && (!addSbox->scene()) && (addSbox->lastCmd == this))
    {
        addSbox->deleteLater();
        addSbox = NULL;
    }
}

void AddSboxCommand::undo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addSbox);
    Q_ASSERT(rcaScene);
    rcaScene->removeItem(addSbox);
    addSbox->lastCmd = this;
    addSbox->setSelected(true);
    addSbox->deleteThisForUndo();
    parentRcu->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddSboxCommand::redo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addSbox);
    Q_ASSERT(rcaScene);

    addSbox->setParentItem(parentRcu);
    addSbox->recheckCollidedItems();
    addSbox->lastCmd = this;
    addSbox->setSelected(true);
    parentRcu->insertSboxToList(addSbox->getIndexInRcu(),addSbox);
    parentRcu->connect(addSbox,SIGNAL(itemBeDeleted(BaseItem*)),parentRcu,SLOT(handleSboxBeDeleted(BaseItem*)));
    parentRcu->connect(addSbox,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),parentRcu,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    parentRcu->connect(addSbox,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),parentRcu,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    emit rcaScene->sendWindowToModified(newSceneModified);
    parentRcu->update();
}

AddBenesCommand::AddBenesCommand(RcaGraphScene *scene, int bcuIndex, int rcuIndex, int indexInRcu, ModuleRcu *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addBenes(NULL)
    , rcaScene(scene)
    , parentRcu(itemParent)
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(rcaScene);
    addBenes = new ElementBenes(bcuIndex,rcuIndex,indexInRcu,parentRcu);
    addBenes->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddBenesCommand::~AddBenesCommand()
{
    QL_DEBUG;
    if(addBenes && (!addBenes->scene()) && (addBenes->lastCmd == this))
    {
        addBenes->deleteLater();
        addBenes = NULL;
    }
}
AddCustomPECommand::AddCustomPECommand(RcaGraphScene* scene,int bcuIndex,int rcuIndex,int indexInRcu,ModuleRcu *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addCustomPE(NULL)
    , rcaScene(scene)
    , parentRcu(itemParent)
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(rcaScene);
    addCustomPE = new ElementCustomPE(bcuIndex,rcuIndex,indexInRcu, parentRcu);
    addCustomPE->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;

}

AddCustomPECommand::~AddCustomPECommand()
{
    QL_DEBUG;
    if(addCustomPE && (!addCustomPE->scene()) && (addCustomPE->lastCmd == this))
    {
        addCustomPE->deleteLater();
        addCustomPE = NULL;
    }
}

void AddCustomPECommand::undo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addCustomPE);
    Q_ASSERT(rcaScene);
    rcaScene->removeItem(addCustomPE);
    addCustomPE->lastCmd = this;
    addCustomPE->setSelected(true);
    addCustomPE->deleteThisForUndo();
    parentRcu->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddCustomPECommand::redo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addCustomPE);
    Q_ASSERT(rcaScene);
    addCustomPE->setParentItem(parentRcu);
    addCustomPE->recheckCollidedItems();
    addCustomPE->lastCmd = this;
    addCustomPE->setSelected(true);
    parentRcu->insertCustomPEToList(addCustomPE->getIndexInRcu(),addCustomPE);
    parentRcu->connect(addCustomPE,SIGNAL(itemBeDeleted(BaseItem*)),parentRcu,SLOT(handleCustomPEBeDeleted(BaseItem*)));
    parentRcu->connect(addCustomPE,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),parentRcu,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    parentRcu->connect(addCustomPE,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),parentRcu,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    emit rcaScene->sendWindowToModified(newSceneModified);
    parentRcu->update();
}
void AddBenesCommand::undo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addBenes);
    Q_ASSERT(rcaScene);

    rcaScene->removeItem(addBenes);
    addBenes->lastCmd = this;
    addBenes->setSelected(true);
    addBenes->deleteThisForUndo();
    parentRcu->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddBenesCommand::redo()
{
    Q_ASSERT(parentRcu);
    Q_ASSERT(addBenes);
    Q_ASSERT(rcaScene);

    addBenes->setParentItem(parentRcu);
    addBenes->recheckCollidedItems();
    addBenes->lastCmd = this;
    addBenes->setSelected(true);
    parentRcu->insertBenesToList(addBenes->getIndexInRcu(),addBenes);
    parentRcu->connect(addBenes,SIGNAL(itemBeDeleted(BaseItem*)),parentRcu,SLOT(handleBenesBeDeleted(BaseItem*)));
    parentRcu->connect(addBenes,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),parentRcu,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
    parentRcu->connect(addBenes,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),parentRcu,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
    emit rcaScene->sendWindowToModified(newSceneModified);
    parentRcu->update();
}

DeleteItemCommand::DeleteItemCommand(RcaGraphScene *scene,BaseItem* item, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , delItem(item)
    , rcaScene(scene)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(delItem);
    parentItem = delItem->parentItem();
    delItem->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

DeleteItemCommand::~DeleteItemCommand()
{
    QL_DEBUG;
    if(delItem && (!delItem->scene()) && (delItem->lastCmd == this))
    {
        delItem->deleteLater();
        delItem = NULL;
    }
}

void DeleteItemCommand::undo()
{
    switch(delItem->getUnitId())
    {
    case BCU_ID:
    {
        ModuleBcu* delBcu = dynamic_cast<ModuleBcu*>(delItem);
        if(delBcu)
        {
            rcaScene->addItem(delBcu);
            delBcu->recheckCollidedItems();
            delBcu->lastCmd = this;
            delBcu->setSelected(true);
            rcaScene->insertBcuToList(delBcu->getIndexBcu(),delBcu);
            rcaScene->connect(delBcu,SIGNAL(itemBeDeleted(BaseItem*)),rcaScene,SLOT(handleBcuBeDeleted(BaseItem*)));
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();
        }
        break;
    }
    case RCU_ID:
    {
        ModuleBcu* parentBcu = dynamic_cast<ModuleBcu*>(parentItem);
        ModuleRcu* delRcu = dynamic_cast<ModuleRcu*>(delItem);
        if(delRcu && parentBcu)
        {
            delRcu->setParentItem(parentBcu);
            delRcu->recheckCollidedItems();
            delRcu->lastCmd = this;
            delRcu->setSelected(true);
            parentBcu->insertRcuToList(delRcu->getIndexRcu(),delRcu);
            parentBcu->connect(delRcu,SIGNAL(itemBeDeleted(BaseItem*)),parentBcu,SLOT(handleRcuBeDeleted(BaseItem*)));
            emit rcaScene->sendWindowToModified(oldSceneModified);
            parentBcu->update();
        }
        break;
    }
    case BFU_ID:
    case CUSTOMPE_ID:
    case SBOX_ID:
    case BENES_ID:
    {
        ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(parentItem);
        BaseArithUnit* delArith = dynamic_cast<BaseArithUnit*>(delItem);
        if(delArith && parentRcu)
        {
            delArith->setParentItem(parentRcu);
            delArith->recheckCollidedItems();
            delArith->lastCmd = this;
            delArith->setSelected(true);
            parentRcu->insertArithToList(delArith->getIndexInRcu(),delArith);
            parentRcu->connect(delArith,SIGNAL(itemBeDeleted(BaseItem*)),parentRcu,SLOT(handleBfuBeDeleted(BaseItem*)));
            parentRcu->connect(delArith,SIGNAL(addWriteMemSuccessed(ModuleWriteMem*)),parentRcu,SLOT(handleAddWriteMemSuccess(ModuleWriteMem*)));
            parentRcu->connect(delArith,SIGNAL(addReadMemSuccessed(ModuleReadMem*)),parentRcu,SLOT(handleAddReadMemSuccess(ModuleReadMem*)));
            emit rcaScene->sendWindowToModified(oldSceneModified);
            parentRcu->update();
        }
        break;
    }
    case RMEM_ID:
    {
        ModuleReadMem* delRMem = dynamic_cast<ModuleReadMem*>(delItem);
        ElementPort* readMemParent = dynamic_cast<ElementPort*>(parentItem);
        if(delRMem && readMemParent && rcaScene)
        {
            delRMem->setParentItem(readMemParent);

            delRMem->recheckCollidedItems();
            delRMem->lastCmd = this;
            readMemParent->addWireOrMem(delRMem);
            readMemParent->connect(delRMem,SIGNAL(itemBeDeleted(BaseItem*)),readMemParent,SLOT(handleWRMemBeDeleted(BaseItem*)));
            readMemParent->setInSignalAllAttr(delRMem);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();
        }
        break;
    }
    case WMEM_ID:
    {
        ModuleWriteMem* delWMem = dynamic_cast<ModuleWriteMem*>(delItem);
        ElementPort* writeMemParent = dynamic_cast<ElementPort*>(parentItem);
        if(delWMem && writeMemParent && rcaScene)
        {
            delWMem->setParentItem(writeMemParent);
            delWMem->recheckCollidedItems();
            delWMem->lastCmd = this;
            writeMemParent->addWireOrMem(delWMem);
            writeMemParent->insertWMemToList(delWMem->getWriteMemIndex(),delWMem->getWriteMemIndex());
            writeMemParent->connect(delWMem,SIGNAL(itemBeDeleted(BaseItem*)),writeMemParent,SLOT(handleWRMemBeDeleted(BaseItem*)));
            writeMemParent->setInSignalAllAttr(delWMem);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();

        }
        break;
    }
    case RIMD_ID:
    {
        ModuleReadImd* delRImd = dynamic_cast<ModuleReadImd*>(delItem);
        ElementPort* readImdParent = dynamic_cast<ElementPort*>(parentItem);
        if(delRImd && readImdParent && rcaScene)
        {
            delRImd->setParentItem(readImdParent);

            delRImd->recheckCollidedItems();
            delRImd->lastCmd = this;
            readImdParent->addWireOrOtherInputPort(delRImd);
            readImdParent->connect(delRImd,SIGNAL(itemBeDeleted(BaseItem*)),readImdParent,SLOT(handleRImdBeDeleted(BaseItem*)));
            readImdParent->setInSignalAllAttr(delRImd);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();
        }
        break;
    }
    case RRCH_ID:
    {
        ModuleReadRch* delRRch = dynamic_cast<ModuleReadRch*>(delItem);
        ElementPort* readRchParent = dynamic_cast<ElementPort*>(parentItem);
        if(delRRch && readRchParent && rcaScene)
        {
            delRRch->setParentItem(readRchParent);

            delRRch->recheckCollidedItems();
            delRRch->lastCmd = this;
            readRchParent->addWireOrOtherInputPort(delRRch);
            readRchParent->connect(delRRch,SIGNAL(itemBeDeleted(BaseItem*)),readRchParent,SLOT(handleWRRchBeDeleted(BaseItem*)));
            readRchParent->setInSignalAllAttr(delRRch);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();
        }
        break;
    }
    case WRCH_ID:
    {
        ModuleWriteRch* delWRch = dynamic_cast<ModuleWriteRch*>(delItem);
        ElementPort* writeRchParent = dynamic_cast<ElementPort*>(parentItem);
        if(delWRch && writeRchParent && rcaScene)
        {
            delWRch->setParentItem(writeRchParent);
            delWRch->recheckCollidedItems();
            delWRch->lastCmd = this;
            writeRchParent->addWireOrOtherInputPort(delWRch);
            writeRchParent->insertWRchToList(delWRch->getWriteRchIndex()%WRITE_RCH_MAX_COUNT,delWRch->getWriteRchIndex());
            writeRchParent->connect(delWRch,SIGNAL(itemBeDeleted(BaseItem*)),writeRchParent,SLOT(handleWRRchBeDeleted(BaseItem*)));
            writeRchParent->setInSignalAllAttr(delWRch);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();

        }
        break;
    }
    case RLFSR_ID:
    {
        ModuleReadLfsr* delRLfsr = dynamic_cast<ModuleReadLfsr*>(delItem);
        ElementPort* readLfsrParent = dynamic_cast<ElementPort*>(parentItem);
        if(delRLfsr && readLfsrParent && rcaScene)
        {
            delRLfsr->setParentItem(readLfsrParent);

            delRLfsr->recheckCollidedItems();
            delRLfsr->lastCmd = this;
            readLfsrParent->addWireOrOtherInputPort(delRLfsr);
            readLfsrParent->connect(delRLfsr,SIGNAL(itemBeDeleted(BaseItem*)),readLfsrParent,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
            readLfsrParent->setInSignalAllAttr(delRLfsr);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();
        }
        break;
    }
    case WLFSR_ID:
    {
        ModuleWriteLfsr* delWLfsr = dynamic_cast<ModuleWriteLfsr*>(delItem);
        ElementPort* writeLfsrParent = dynamic_cast<ElementPort*>(parentItem);
        if(delWLfsr && writeLfsrParent && rcaScene)
        {
            delWLfsr->setParentItem(writeLfsrParent);
            delWLfsr->recheckCollidedItems();
            delWLfsr->lastCmd = this;
            writeLfsrParent->addWireOrOtherInputPort(delWLfsr);
            writeLfsrParent->insertWLfsrToList(delWLfsr->getWriteLfsrIndex()%WRITE_RCH_MAX_COUNT,delWLfsr->getWriteLfsrIndex());
            writeLfsrParent->connect(delWLfsr,SIGNAL(itemBeDeleted(BaseItem*)),writeLfsrParent,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
            writeLfsrParent->setInSignalAllAttr(delWLfsr);
            emit rcaScene->sendWindowToModified(oldSceneModified);
            rcaScene->update();

        }
        break;
    }
    default:break;
    }
}

void DeleteItemCommand::redo()
{
    switch(delItem->getUnitId())
    {
//    case BCU_ID:
//    case RCU_ID:
//    case BFU_ID:
//    case SBOX_ID:
//    case BENES_ID:
//    case RMEM_ID:
//    case WMEM_ID:
//    {
//        Q_ASSERT(delItem);
//        Q_ASSERT(rcaScene);

//        rcaScene->removeItem(delItem);
//        delItem->lastCmd = this;
//        delItem->deleteThisForUndo();
//        rcaScene->update();
//        break;
//    }
    case BCU_ID:
    {
        ModuleBcu* delBcu = dynamic_cast<ModuleBcu*>(delItem);
        if(delBcu)
        {
            rcaScene->removeItem(delBcu);
            delBcu->lastCmd = this;
            delBcu->deleteThisForUndo();
            rcaScene->update();
            emit rcaScene->sendWindowToModified(newSceneModified);
        }
        break;
    }
    case RCU_ID:
    {
        ModuleBcu* parentBcu = dynamic_cast<ModuleBcu*>(parentItem);
        ModuleRcu* delRcu = dynamic_cast<ModuleRcu*>(delItem);
        if(delRcu && parentBcu)
        {
            rcaScene->removeItem(delRcu);
            delRcu->lastCmd = this;
            delRcu->deleteThisForUndo();
            parentBcu->update();
            emit rcaScene->sendWindowToModified(newSceneModified);
        }
        break;
    }
    case BFU_ID:
    case CUSTOMPE_ID:
    case SBOX_ID:
    case BENES_ID:
    {
        ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(parentItem);
        BaseArithUnit* delArith = dynamic_cast<BaseArithUnit*>(delItem);
        if(delArith && parentRcu)
        {
            rcaScene->removeItem(delArith);
            delArith->lastCmd = this;
            delArith->deleteThisForUndo();
            parentRcu->update();
            emit rcaScene->sendWindowToModified(newSceneModified);
        }
        break;
    }
    case RMEM_ID:
    case WMEM_ID:
    case RIMD_ID:
    case RRCH_ID:
    case WRCH_ID:
    case RLFSR_ID:
    case WLFSR_ID:
    {
        BasePort* delMem = dynamic_cast<BasePort*>(delItem);
        if(delMem)
        {
            rcaScene->removeItem(delMem);
            delMem->lastCmd = this;
            delMem->deleteThisForUndo();
            rcaScene->update();
            emit rcaScene->sendWindowToModified(newSceneModified);
        }
        break;
    }
    default:break;
    }
}

AddReadMemCommand::AddReadMemCommand(RcaGraphScene *scene, int index, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addReadMem(NULL)
    , rcaScene(scene)
    , readMemParent(itemParent)
{
    Q_ASSERT(readMemParent);
    Q_ASSERT(rcaScene);

    addReadMem = new ModuleReadMem(index,readMemParent);
    addReadMem->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddReadMemCommand::~AddReadMemCommand()
{
    QL_DEBUG;
    if(addReadMem && (!addReadMem->scene()) && (addReadMem->lastCmd == this))
    {
        addReadMem->deleteLater();
        addReadMem = NULL;
    }
}

void AddReadMemCommand::undo()
{
    Q_ASSERT(readMemParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadMem);

    rcaScene->removeItem(addReadMem);
    addReadMem->lastCmd = this;
    addReadMem->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddReadMemCommand::redo()
{
    Q_ASSERT(readMemParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadMem);

    addReadMem->setParentItem(readMemParent);

    addReadMem->recheckCollidedItems();
    addReadMem->lastCmd = this;
    readMemParent->addWireOrMem(addReadMem);
    readMemParent->connect(addReadMem,SIGNAL(itemBeDeleted(BaseItem*)),readMemParent,SLOT(handleWRMemBeDeleted(BaseItem*)));
    readMemParent->setInSignalAllAttr(addReadMem);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}


AddReadImdCommand::AddReadImdCommand(RcaGraphScene *scene, int index, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addReadImd(NULL)
    , rcaScene(scene)
    , readImdParent(itemParent)
{
    Q_ASSERT(readImdParent);
    Q_ASSERT(rcaScene);

    addReadImd = new ModuleReadImd(index,readImdParent);
    addReadImd->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddReadImdCommand::~AddReadImdCommand()
{
    QL_DEBUG;
    if(addReadImd && (!addReadImd->scene()) && (addReadImd->lastCmd == this))
    {
        QL_DEBUG;
        addReadImd->deleteLater();
        addReadImd = NULL;
    }
}

void AddReadImdCommand::undo()
{
    Q_ASSERT(readImdParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadImd);

    rcaScene->removeItem(addReadImd);
    addReadImd->lastCmd = this;
    addReadImd->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddReadImdCommand::redo()
{
    Q_ASSERT(readImdParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadImd);

    addReadImd->setParentItem(readImdParent);

    addReadImd->recheckCollidedItems();
    addReadImd->lastCmd = this;
    readImdParent->addWireOrOtherInputPort(addReadImd);
    readImdParent->connect(addReadImd,SIGNAL(itemBeDeleted(BaseItem*)),readImdParent,SLOT(handleRImdBeDeleted(BaseItem*)));
    readImdParent->setInSignalAllAttr(addReadImd);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}


AddReadRchCommand::AddReadRchCommand(RcaGraphScene *scene, int index, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addReadRch(NULL)
    , rcaScene(scene)
    , readRchParent(itemParent)
{
    Q_ASSERT(readRchParent);
    Q_ASSERT(rcaScene);

    addReadRch = new ModuleReadRch(index,readRchParent);
    addReadRch->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddReadRchCommand::~AddReadRchCommand()
{
    QL_DEBUG;
    if(addReadRch && (!addReadRch->scene()) && (addReadRch->lastCmd == this))
    {
        QL_DEBUG;
        addReadRch->deleteLater();
        addReadRch = NULL;
    }
}

void AddReadRchCommand::undo()
{
    Q_ASSERT(readRchParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadRch);

    rcaScene->removeItem(addReadRch);
    addReadRch->lastCmd = this;
    addReadRch->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddReadRchCommand::redo()
{
    Q_ASSERT(readRchParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadRch);

    addReadRch->setParentItem(readRchParent);

    addReadRch->recheckCollidedItems();
    addReadRch->lastCmd = this;
    readRchParent->addWireOrOtherInputPort(addReadRch);
    readRchParent->connect(addReadRch,SIGNAL(itemBeDeleted(BaseItem*)),readRchParent,SLOT(handleWRRchBeDeleted(BaseItem*)));
    readRchParent->setInSignalAllAttr(addReadRch);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}

AddReadLfsrCommand::AddReadLfsrCommand(RcaGraphScene *scene, int index, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addReadLfsr(NULL)
    , rcaScene(scene)
    , readLfsrParent(itemParent)
{
    Q_ASSERT(readLfsrParent);
    Q_ASSERT(rcaScene);

    addReadLfsr = new ModuleReadLfsr(index,readLfsrParent);
    addReadLfsr->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddReadLfsrCommand::~AddReadLfsrCommand()
{
    QL_DEBUG;
    if(addReadLfsr && (!addReadLfsr->scene()) && (addReadLfsr->lastCmd == this))
    {
        QL_DEBUG;
        addReadLfsr->deleteLater();
        addReadLfsr = NULL;
    }
}

void AddReadLfsrCommand::undo()
{
    Q_ASSERT(readLfsrParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadLfsr);

    rcaScene->removeItem(addReadLfsr);
    addReadLfsr->lastCmd = this;
    addReadLfsr->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddReadLfsrCommand::redo()
{
    Q_ASSERT(readLfsrParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addReadLfsr);

    addReadLfsr->setParentItem(readLfsrParent);

    addReadLfsr->recheckCollidedItems();
    addReadLfsr->lastCmd = this;
    readLfsrParent->addWireOrOtherInputPort(addReadLfsr);
    readLfsrParent->connect(addReadLfsr,SIGNAL(itemBeDeleted(BaseItem*)),readLfsrParent,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
    readLfsrParent->setInSignalAllAttr(addReadLfsr);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}

AddWriteRchCommand::AddWriteRchCommand(RcaGraphScene *scene, int index, int address, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addWriteRch(NULL)
    , rcaScene(scene)
    , writeRchParent(itemParent)
{
    Q_ASSERT(writeRchParent);
    Q_ASSERT(rcaScene);

    addWriteRch = new ModuleWriteRch(index, writeRchParent);
    addWriteRch->setWriteAddress(address);
    addWriteRch->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddWriteRchCommand::~AddWriteRchCommand()
{
    QL_DEBUG;
    if(addWriteRch && (!addWriteRch->scene()) && (addWriteRch->lastCmd == this))
    {
        QL_DEBUG;
        addWriteRch->deleteLater();
        addWriteRch = NULL;
    }
}

void AddWriteRchCommand::undo()
{
    Q_ASSERT(writeRchParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addWriteRch);

    rcaScene->removeItem(addWriteRch);
    addWriteRch->lastCmd = this;
    addWriteRch->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddWriteRchCommand::redo()
{
    Q_ASSERT(writeRchParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addWriteRch);

    addWriteRch->setParentItem(writeRchParent);
    addWriteRch->recheckCollidedItems();
    addWriteRch->lastCmd = this;
    writeRchParent->addWireOrOtherInputPort(addWriteRch);
    writeRchParent->insertWRchToList(addWriteRch->getWriteRchIndex()%WRITE_RCH_MAX_COUNT,addWriteRch->getWriteRchIndex());
    writeRchParent->connect(addWriteRch,SIGNAL(itemBeDeleted(BaseItem*)),writeRchParent,SLOT(handleWRRchBeDeleted(BaseItem*)));
    writeRchParent->setInSignalAllAttr(addWriteRch);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}

AddWriteLfsrCommand::AddWriteLfsrCommand(RcaGraphScene *scene, int index, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addWriteLfsr(NULL)
    , rcaScene(scene)
    , writeLfsrParent(itemParent)
{
    Q_ASSERT(writeLfsrParent);
    Q_ASSERT(rcaScene);

    addWriteLfsr = new ModuleWriteLfsr(index, writeLfsrParent);
    addWriteLfsr->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddWriteLfsrCommand::~AddWriteLfsrCommand()
{
    QL_DEBUG;
    if(addWriteLfsr && (!addWriteLfsr->scene()) && (addWriteLfsr->lastCmd == this))
    {
        QL_DEBUG;
        addWriteLfsr->deleteLater();
        addWriteLfsr = NULL;
    }
}

void AddWriteLfsrCommand::undo()
{
    Q_ASSERT(writeLfsrParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addWriteLfsr);

    rcaScene->removeItem(addWriteLfsr);
    addWriteLfsr->lastCmd = this;
    addWriteLfsr->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddWriteLfsrCommand::redo()
{
    Q_ASSERT(writeLfsrParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addWriteLfsr);

    addWriteLfsr->setParentItem(writeLfsrParent);
    addWriteLfsr->recheckCollidedItems();
    addWriteLfsr->lastCmd = this;
    writeLfsrParent->addWireOrOtherInputPort(addWriteLfsr);
    writeLfsrParent->insertWLfsrToList(addWriteLfsr->getWriteLfsrIndex()%WRITE_LFSR_MAX_COUNT,addWriteLfsr->getWriteLfsrIndex());
    writeLfsrParent->connect(addWriteLfsr,SIGNAL(itemBeDeleted(BaseItem*)),writeLfsrParent,SLOT(handleWRLfsrBeDeleted(BaseItem*)));
    writeLfsrParent->setInSignalAllAttr(addWriteLfsr);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}

AddWriteMemCommand::AddWriteMemCommand(RcaGraphScene *scene, int index, ElementPort *itemParent, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , addWriteMem(NULL)
    , rcaScene(scene)
    , writeMemParent(itemParent)
{
    Q_ASSERT(writeMemParent);
    Q_ASSERT(rcaScene);

    addWriteMem = new ModuleWriteMem(index,writeMemParent);
    addWriteMem->lastCmd = this;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

AddWriteMemCommand::~AddWriteMemCommand()
{
    QL_DEBUG;
    if(addWriteMem && (!addWriteMem->scene()) && (addWriteMem->lastCmd == this))
    {
        addWriteMem->deleteLater();
        addWriteMem = NULL;
    }
}

void AddWriteMemCommand::undo()
{
    Q_ASSERT(writeMemParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addWriteMem);

    rcaScene->removeItem(addWriteMem);
    addWriteMem->lastCmd = this;
    addWriteMem->deleteThisForUndo();
    rcaScene->update();
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void AddWriteMemCommand::redo()
{
    Q_ASSERT(writeMemParent);
    Q_ASSERT(rcaScene);
    Q_ASSERT(addWriteMem);

    addWriteMem->setParentItem(writeMemParent);
    addWriteMem->recheckCollidedItems();
    addWriteMem->lastCmd = this;
    writeMemParent->addWireOrMem(addWriteMem);
    writeMemParent->insertWMemToList(addWriteMem->getWriteMemIndex(),addWriteMem->getWriteMemIndex());
    writeMemParent->connect(addWriteMem,SIGNAL(itemBeDeleted(BaseItem*)),writeMemParent,SLOT(handleWRMemBeDeleted(BaseItem*)));
    writeMemParent->setInSignalAllAttr(addWriteMem);
    emit rcaScene->sendWindowToModified(newSceneModified);
    rcaScene->update();
}

SetBcuIndexCommand::SetBcuIndexCommand(RcaGraphScene *scene, ModuleBcu *bcuItem, int index, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , bcuModule(bcuItem)
    , newIndex(index)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bcuModule);

    oldIndex = bcuModule->getIndexBcu();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetBcuIndexCommand::undo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bcuModule);

    rcaScene->setBcuIndex(bcuModule,oldIndex);
    rcaScene->update();
    bcuModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(bcuModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetBcuIndexCommand::redo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bcuModule);

    rcaScene->setBcuIndex(bcuModule,newIndex);
    rcaScene->update();
    bcuModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(bcuModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetRcuIndexCommand::SetRcuIndexCommand(RcaGraphScene *scene, ModuleRcu *rcuItem, int index, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , rcuModule(rcuItem)
    , newIndex(index)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rcuModule);

    oldIndex = rcuModule->getIndexRcu();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetRcuIndexCommand::undo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rcuModule);

    rcuModule->setIndexRcu(oldIndex);
    rcuModule->update();
    rcaScene->checkUndoItemIsShownItem(rcuModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetRcuIndexCommand::redo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rcuModule);

    rcuModule->setIndexRcu(newIndex);
    rcuModule->update();
    rcaScene->checkUndoItemIsShownItem(rcuModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetArithIndexCommand::SetArithIndexCommand(RcaGraphScene *scene, BaseArithUnit *arithItem, int index, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , arithElement(arithItem)
    , newIndex(index)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(arithElement);

    oldIndex = arithElement->getIndexInRcu();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetArithIndexCommand::undo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(arithElement);

    arithElement->setIndexInRcu(oldIndex);
    arithElement->update();
    arithElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(arithElement);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetArithIndexCommand::redo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(arithElement);

    arithElement->setIndexInRcu(newIndex);
    arithElement->update();
    arithElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(arithElement);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetRMemIndexCommand::SetRMemIndexCommand(RcaGraphScene *scene, ModuleReadMem *rMemItem, int index, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , rMemModule(rMemItem)
    , newIndex(index)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rMemModule);

    oldIndex = rMemModule->getReadMemIndex();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetRMemIndexCommand::undo()
{
    rMemModule->setReadMemIndex(oldIndex);
    rMemModule->update();
    rcaScene->checkUndoItemIsShownItem(rMemModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetRMemIndexCommand::redo()
{
    rMemModule->setReadMemIndex(newIndex);
    rMemModule->update();
    rcaScene->checkUndoItemIsShownItem(rMemModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetWMemIndexCommand::SetWMemIndexCommand(RcaGraphScene *scene, ModuleWriteMem *wMemItem, int index, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , wMemModule(wMemItem)
    , newIndex(index)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(wMemModule);

    oldIndex = wMemModule->getWriteMemIndex();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetWMemIndexCommand::undo()
{
    wMemModule->setWriteMemIndex(oldIndex);
    wMemModule->update();
    wMemModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(wMemModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetWMemIndexCommand::redo()
{
    wMemModule->setWriteMemIndex(newIndex);
    wMemModule->update();
    wMemModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(wMemModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetRImdAttrCommand::SetRImdAttrCommand(RcaGraphScene *scene, ModuleReadImd *rImdItem, ModuleReadImd *newImd, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , rImdModule(rImdItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rImdModule);

    newIndex = newImd->getReadImdIndex();

    oldIndex = rImdModule->getReadImdIndex();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetRImdAttrCommand::undo()
{
    rImdModule->setReadImdIndex(oldIndex);
    rImdModule->update();
    rImdModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rImdModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetRImdAttrCommand::redo()
{
    rImdModule->setReadImdIndex(newIndex);
    rImdModule->update();
    rImdModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rImdModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetRRchAttrCommand::SetRRchAttrCommand(RcaGraphScene *scene, ModuleReadRch *rRchItem, ModuleReadRch *newRch, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , rRchModule(rRchItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rRchModule);

    newIndex = newRch->getReadRchIndex();

    oldIndex = rRchModule->getReadRchIndex();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetRRchAttrCommand::undo()
{
    rRchModule->setReadRchIndex(oldIndex);
    rRchModule->update();
    rRchModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rRchModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetRRchAttrCommand::redo()
{
    rRchModule->setReadRchIndex(newIndex);
    rRchModule->update();
    rRchModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rRchModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetWRchAttrCommand::SetWRchAttrCommand(RcaGraphScene *scene, ModuleWriteRch *wRchItem, int index, int address,QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , wRchModule(wRchItem)
    , newIndex(index)
    , newAddress(address)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(wRchModule);

    oldIndex = wRchModule->getWriteRchIndex();
    oldAddress = wRchModule->getWriteAddress();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetWRchAttrCommand::undo()
{
    wRchModule->setWriteRchIndex(oldIndex);
    wRchModule->setWriteAddress(oldAddress);
    wRchModule->update();
    wRchModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(wRchModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetWRchAttrCommand::redo()
{
    wRchModule->setWriteRchIndex(newIndex);
    wRchModule->setWriteAddress(newAddress);
    wRchModule->update();
    wRchModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(wRchModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetRLfsrAttrCommand::SetRLfsrAttrCommand(RcaGraphScene *scene, ModuleReadLfsr *rLfsrItem, ModuleReadLfsr *newLfsr, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , rLfsrModule(rLfsrItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rLfsrModule);

    newIndex = newLfsr->getReadLfsrIndex();

    oldIndex = rLfsrModule->getReadLfsrIndex();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetRLfsrAttrCommand::undo()
{
    rLfsrModule->setReadLfsrIndex(oldIndex);
    rLfsrModule->update();
    rLfsrModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rLfsrModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetRLfsrAttrCommand::redo()
{
    rLfsrModule->setReadLfsrIndex(newIndex);
    rLfsrModule->update();
    rLfsrModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rLfsrModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetWLfsrAttrCommand::SetWLfsrAttrCommand(RcaGraphScene *scene, ModuleWriteLfsr *wLfsrItem, int index, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , wLfsrModule(wLfsrItem)
    , newIndex(index)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(wLfsrModule);

    oldIndex = wLfsrModule->getWriteLfsrIndex();
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetWLfsrAttrCommand::undo()
{
    wLfsrModule->setWriteLfsrIndex(oldIndex);
    wLfsrModule->update();
    wLfsrModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(wLfsrModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetWLfsrAttrCommand::redo()
{
    wLfsrModule->setWriteLfsrIndex(newIndex);
    wLfsrModule->update();
    wLfsrModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(wLfsrModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}

SetBcuAttrCommand::SetBcuAttrCommand(RcaGraphScene *scene, ModuleBcu *bcuItem, ModuleBcu *newBcu, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , bcuModule(bcuItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bcuModule);
    Q_ASSERT(newBcu);

    newBcuSrc = newBcu->srcDataFromBcu;
    newSort = newBcu->sort;
    newBurst = newBcu->burst;
    newGap = newBcu->gap;
    newInfifoSrc = newBcu->infifoSrc;
    newOutfifoSrc = newBcu->outfifoSrc;
    newLfsrGroup = newBcu->lfsrGroup;
    newReadImdMode128_32 = newBcu->readImdMode128_32;
    newImdSrcConfig = newBcu->imdSrcConfig;
    newInFifoSrcConfig = newBcu->inFifoSrcConfig;
    newMemorySrcConfig = newBcu->memorySrcConfig;
    newWriteDepth = newBcu->writeDepth;
    newWriteWidth = newBcu->writeWidth;
    newAddressMode = newBcu->addressMode;

    oldBcuSrc = bcuModule->srcDataFromBcu;
    oldSort = bcuModule->sort;
    oldBurst = bcuModule->burst;
    oldGap = bcuModule->gap;
    oldInfifoSrc = bcuModule->infifoSrc;
    oldOutfifoSrc = bcuModule->outfifoSrc;
    oldLfsrGroup = bcuModule->lfsrGroup;
    oldImdSrcConfig = bcuModule->imdSrcConfig;
    oldInFifoSrcConfig = bcuModule->inFifoSrcConfig;
    oldMemorySrcConfig = bcuModule->memorySrcConfig;
    oldWriteDepth = bcuModule->writeDepth;
    oldWriteWidth = bcuModule->writeWidth;
    oldAddressMode = bcuModule->addressMode;
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
}

void SetBcuAttrCommand::undo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bcuModule);

    bcuModule->srcDataFromBcu = oldBcuSrc;
    bcuModule->sort = oldSort;
    bcuModule->burst = oldBurst;
    bcuModule->gap = oldGap;
    bcuModule->infifoSrc = oldInfifoSrc;
    bcuModule->outfifoSrc = oldOutfifoSrc;
    bcuModule->lfsrGroup = oldLfsrGroup;
    bcuModule->readImdMode128_32 = oldReadImdMode128_32;
    bcuModule->imdSrcConfig = oldImdSrcConfig;
    bcuModule->inFifoSrcConfig = oldInFifoSrcConfig;
    bcuModule->memorySrcConfig = oldMemorySrcConfig;
    bcuModule->writeDepth = oldWriteDepth;
    bcuModule->writeWidth = oldWriteWidth;
    bcuModule->addressMode = oldAddressMode;
    bcuModule->update();
    rcaScene->checkUndoItemIsShownItem(bcuModule);
    bcuModule->setSelected(true);
    bcuModule->setIndexBcu(bcuModule->getIndexBcu());
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetBcuAttrCommand::redo()
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bcuModule);

    bcuModule->srcDataFromBcu = newBcuSrc;
    bcuModule->sort = newSort;
    bcuModule->burst = newBurst;
    bcuModule->gap = newGap;
    bcuModule->infifoSrc = newInfifoSrc;
    bcuModule->outfifoSrc = newOutfifoSrc;
    bcuModule->lfsrGroup = newLfsrGroup;
    bcuModule->readImdMode128_32 = newReadImdMode128_32;
    bcuModule->imdSrcConfig = newImdSrcConfig;
    bcuModule->inFifoSrcConfig = newInFifoSrcConfig;
    bcuModule->memorySrcConfig = newMemorySrcConfig;
    bcuModule->writeDepth = newWriteDepth;
    bcuModule->writeWidth = newWriteWidth;
    bcuModule->addressMode = newAddressMode;
    bcuModule->update();
    bcuModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(bcuModule);

    bcuModule->setIndexBcu(bcuModule->getIndexBcu());
    emit rcaScene->sendWindowToModified(newSceneModified);
}

SetRcuAttrCommand::SetRcuAttrCommand(RcaGraphScene *scene, ModuleRcu *rcuItem, ModuleRcu* newRcu, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , rcuModule(rcuItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(rcuModule);
    Q_ASSERT(newRcu);
    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;

    newLoopStartEndFlag = newRcu->loopStartEndFlag;
    newLoopTimes = newRcu->loopTimes;
    newLoopStartEndFlag2 = newRcu->loopStartEndFlag2;
    newLoopTimes2 = newRcu->loopTimes2;
    newReadMemMode128_32 = newRcu->readMemMode128_32;
    newReadMemMode = newRcu->readMemMode;
    newReadMemAddr1 = newRcu->readMemAddr1;
    newReadMemAddr2 = newRcu->readMemAddr2;
    newReadMemAddr3 = newRcu->readMemAddr3;
    newReadMemAddr4 = newRcu->readMemAddr4;
    newReadMemOffset = newRcu->readMemOffset;
    newReadMemThreashold = newRcu->readMemThreashold;
    newWriteMemMode = newRcu->writeMemMode;
    newWriteMemAddr = newRcu->writeMemAddr;
    newWriteMemMask = newRcu->writeMemMask;
    newWriteMemOffset = newRcu->writeMemOffset;
    newWriteMemThreashold = newRcu->writeMemThreashold;
    newRchMode = newRcu->rchMode;
    newWriteRchAddr1 = newRcu->writeRchAddr1;
    newWriteRchAddr2 = newRcu->writeRchAddr2;
    newWriteRchAddr3 = newRcu->writeRchAddr3;
    newWriteRchAddr4 = newRcu->writeRchAddr4;

    oldLoopStartEndFlag = rcuModule->loopStartEndFlag;
    oldLoopTimes = rcuModule->loopTimes;
    oldLoopStartEndFlag2 = rcuModule->loopStartEndFlag2;
    oldLoopTimes2 = rcuModule->loopTimes2;
    oldReadMemMode128_32 = rcuModule->readMemMode128_32;
    oldReadMemMode = rcuModule->readMemMode;
    oldReadMemAddr1 = rcuModule->readMemAddr1;
    oldReadMemAddr2 = rcuModule->readMemAddr2;
    oldReadMemAddr3 = rcuModule->readMemAddr3;
    oldReadMemAddr4 = rcuModule->readMemAddr4;
    oldReadMemOffset = rcuModule->readMemOffset;
    oldReadMemThreashold = rcuModule->readMemThreashold;
    oldWriteMemMode = rcuModule->writeMemMode;
    oldWriteMemAddr = rcuModule->writeMemAddr;
    oldWriteMemMask = rcuModule->writeMemMask;
    oldWriteMemOffset = rcuModule->writeMemOffset;
    oldWriteMemThreashold = rcuModule->writeMemThreashold;
    oldRchMode = rcuModule->rchMode;
    oldWriteRchAddr1 = rcuModule->writeRchAddr1;
    oldWriteRchAddr2 = rcuModule->writeRchAddr2;
    oldWriteRchAddr3 = rcuModule->writeRchAddr3;
    oldWriteRchAddr4 = rcuModule->writeRchAddr4;
}

void SetRcuAttrCommand::undo()
{
    rcuModule->loopStartEndFlag = oldLoopStartEndFlag;
    rcuModule->loopTimes = oldLoopTimes;
    rcuModule->loopStartEndFlag2 = oldLoopStartEndFlag2;
    rcuModule->loopTimes2 = oldLoopTimes2;
    rcuModule->readMemMode128_32 = oldReadMemMode128_32;
    rcuModule->readMemMode = oldReadMemMode;
    rcuModule->readMemAddr1 = oldReadMemAddr1;
    rcuModule->readMemAddr2 = oldReadMemAddr2;
    rcuModule->readMemAddr3 = oldReadMemAddr3;
    rcuModule->readMemAddr4 = oldReadMemAddr4;
    rcuModule->readMemOffset = oldReadMemOffset;
    rcuModule->readMemThreashold = oldReadMemThreashold;
    rcuModule->writeMemMode = oldWriteMemMode;
    rcuModule->writeMemAddr = oldWriteMemAddr;
    rcuModule->writeMemMask = oldWriteMemMask;
    rcuModule->writeMemOffset = oldWriteMemOffset;
    rcuModule->writeMemThreashold = oldWriteMemThreashold;
    rcuModule->rchMode = oldRchMode;
    rcuModule->writeRchAddr1 = oldWriteRchAddr1;
    rcuModule->writeRchAddr2 = oldWriteRchAddr2;
    rcuModule->writeRchAddr3 = oldWriteRchAddr3;
    rcuModule->writeRchAddr4 = oldWriteRchAddr4;
    rcuModule->update();
    rcuModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rcuModule);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetRcuAttrCommand::redo()
{
    rcuModule->loopStartEndFlag = newLoopStartEndFlag;
    rcuModule->loopTimes = newLoopTimes;
    rcuModule->loopStartEndFlag2 = newLoopStartEndFlag2;
    rcuModule->loopTimes2 = newLoopTimes2;
    rcuModule->readMemMode128_32 = newReadMemMode128_32;
    rcuModule->readMemMode = newReadMemMode;
    rcuModule->readMemAddr1 = newReadMemAddr1;
    rcuModule->readMemAddr2 = newReadMemAddr2;
    rcuModule->readMemAddr3 = newReadMemAddr3;
    rcuModule->readMemAddr4 = newReadMemAddr4;
    rcuModule->readMemOffset = newReadMemOffset;
    rcuModule->readMemThreashold = newReadMemThreashold;
    rcuModule->writeMemMode = newWriteMemMode;
    rcuModule->writeMemAddr = newWriteMemAddr;
    rcuModule->writeMemMask = newWriteMemMask;
    rcuModule->writeMemOffset = newWriteMemOffset;
    rcuModule->writeMemThreashold = newWriteMemThreashold;
    rcuModule->rchMode = newRchMode;
    rcuModule->writeRchAddr1 = newWriteRchAddr1;
    rcuModule->writeRchAddr2 = newWriteRchAddr2;
    rcuModule->writeRchAddr3 = newWriteRchAddr3;
    rcuModule->writeRchAddr4 = newWriteRchAddr4;
    rcuModule->update();
    rcuModule->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(rcuModule);
    emit rcaScene->sendWindowToModified(newSceneModified);
}


SetBfuAttrCommand::SetBfuAttrCommand(RcaGraphScene *scene, ElementBfu *bfuItem, ElementBfu *newBfu, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , bfuElement(bfuItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(bfuElement);
    Q_ASSERT(newBfu);

    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
    newFuncIndex = newBfu->funcIndex;
    newFuncExp = newBfu->funcExp;
    newBypassIndex = newBfu->bypassIndex;
    newFuncAuModIndex = newBfu->funcAuModIndex;
    newFuncAuCarry = newBfu->funcAuCarry;
    newFuncMuModIndex = newBfu->funcMuModIndex;
    newBypassExp = newBfu->bypassExp;

    oldFuncIndex = bfuElement->funcIndex;
    oldFuncExp = bfuElement->funcExp;
    oldBypassIndex = bfuElement->bypassIndex;
    oldFuncAuModIndex = bfuElement->funcAuModIndex;
    oldFuncAuCarry = bfuElement->funcAuCarry;
    oldFuncMuModIndex = bfuElement->funcMuModIndex;
    oldBypassExp = bfuElement->bypassExp;
}

void SetBfuAttrCommand::undo()
{
    bfuElement->funcIndex = oldFuncIndex;
    bfuElement->funcExp = oldFuncExp;
    bfuElement->bypassIndex = oldBypassIndex;
    bfuElement->funcAuModIndex = oldFuncAuModIndex;
    bfuElement->funcMuModIndex = oldFuncMuModIndex;
    bfuElement->funcAuCarry = oldFuncAuCarry;
    bfuElement->bypassExp = oldBypassExp;
    bfuElement->update();
    bfuElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(bfuElement);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetBfuAttrCommand::redo()
{
    bfuElement->funcIndex = newFuncIndex;
    bfuElement->funcExp = newFuncExp;
    bfuElement->bypassIndex = newBypassIndex;
    bfuElement->funcAuModIndex = newFuncAuModIndex;
    bfuElement->funcMuModIndex = newFuncMuModIndex;
    bfuElement->funcAuCarry = newFuncAuCarry;
    bfuElement->bypassExp = newBypassExp;
    bfuElement->update();
    rcaScene->checkUndoItemIsShownItem(bfuElement);
    emit rcaScene->sendWindowToModified(newSceneModified);
}

SetCustomPEAttrCommand::SetCustomPEAttrCommand(RcaGraphScene *scene, ElementCustomPE *customPEItem, ElementCustomPE *newCustomPE, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , customPEElement(customPEItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(customPEItem);
    Q_ASSERT(newCustomPE);

    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
    newFuncIndex = newCustomPE->funcIndex;
    newInputNum = newCustomPE->inputNum();
    newOutputNum = newCustomPE->outputNum();
    newFuncExp = newCustomPE->funcExp;
    newBypassIndex = newCustomPE->bypassIndex;
    newFuncAuModIndex = newCustomPE->funcAuModIndex;
//    newFuncAuCarry = newCustomPE->funcAuCarry;
    newFuncMuModIndex = newCustomPE->funcMuModIndex;
    newBypassExp = newCustomPE->bypassExp;

    oldFuncIndex = customPEItem->funcIndex;
    oldInputNum = customPEItem->inputNum();
    oldOutputNum = customPEItem->outputNum();
    oldFuncExp = customPEItem->funcExp;
    oldBypassIndex = customPEItem->bypassIndex;
    oldFuncAuModIndex = customPEItem->funcAuModIndex;
//    oldFuncAuCarry = customPEItem->funcAuCarry;
    oldFuncMuModIndex = customPEItem->funcMuModIndex;
    oldBypassExp = customPEItem->bypassExp;
}

void SetCustomPEAttrCommand::undo()
{
    customPEElement->funcIndex = oldFuncIndex;
    customPEElement->setInputNum(oldInputNum);
    customPEElement->changeInputPort(oldInputNum);
    customPEElement->changeOutputPort(oldOutputNum);
    customPEElement->setOutputNum(oldOutputNum);
    customPEElement->funcExp = oldFuncExp;
    customPEElement->bypassIndex = oldBypassIndex;
    customPEElement->funcAuModIndex = oldFuncAuModIndex;
//    customPEElement->funcAuCarry = oldFuncAuCarry;
    customPEElement->funcMuModIndex = oldFuncMuModIndex;
    customPEElement->bypassExp = oldBypassExp;
    customPEElement->update();
    customPEElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(customPEElement);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetCustomPEAttrCommand::redo()
{
    customPEElement->funcIndex = newFuncIndex;
    customPEElement->setInputNum(newInputNum);
    customPEElement->changeInputPort(newInputNum);
    customPEElement->changeOutputPort(newOutputNum);
    customPEElement->setOutputNum(newOutputNum);
    customPEElement->funcExp = newFuncExp;
    customPEElement->bypassIndex = newBypassIndex;
    customPEElement->funcAuModIndex = newFuncAuModIndex;
//    customPEElement->funcAuCarry = newFuncAuCarry;
    customPEElement->funcMuModIndex = newFuncMuModIndex;
    customPEElement->bypassExp = newBypassExp;
    customPEElement->update();
    rcaScene->checkUndoItemIsShownItem(customPEElement);
    emit rcaScene->sendWindowToModified(newSceneModified);
}

SetSboxAttrCommand::SetSboxAttrCommand(RcaGraphScene *scene, ElementSbox *sboxItem, ElementSbox *newSbox, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , sboxElement(sboxItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(sboxElement);
    Q_ASSERT(newSbox);

    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
    newGroupCnt0 = newSbox->groupCnt0;
    newGroupCnt1 = newSbox->groupCnt1;
    newGroupCnt2 = newSbox->groupCnt2;
    newGroupCnt3 = newSbox->groupCnt3;
    newSboxMode = newSbox->sboxMode;
    newSboxGroup = newSbox->sboxGroup;
    newSboxByteSel = newSbox->sboxByteSel;
    newSboxIncreaseMode = newSbox->sboxIncreaseMode;
    newSrcConfig = newSbox->srcConfig;

    oldGroupCnt0 = sboxElement->groupCnt0;
    oldGroupCnt1 = sboxElement->groupCnt1;
    oldGroupCnt2 = sboxElement->groupCnt2;
    oldGroupCnt3 = sboxElement->groupCnt3;
    oldSboxMode = sboxElement->sboxMode;
    oldSboxGroup = sboxElement->sboxGroup;
    oldSboxByteSel = sboxElement->sboxByteSel;
    oldSboxIncreaseMode = sboxElement->sboxIncreaseMode;
    oldSrcConfig = sboxElement->srcConfig;
}

void SetSboxAttrCommand::undo()
{
    sboxElement->groupCnt0 = oldGroupCnt0;
    sboxElement->groupCnt1 = oldGroupCnt1;
    sboxElement->groupCnt2 = oldGroupCnt2;
    sboxElement->groupCnt3 = oldGroupCnt3;
    sboxElement->sboxMode = oldSboxMode;
    sboxElement->sboxGroup = oldSboxGroup;
    sboxElement->sboxByteSel = oldSboxByteSel;
    sboxElement->sboxIncreaseMode = oldSboxIncreaseMode;
    sboxElement->srcConfig = oldSrcConfig;
    sboxElement->update();
    sboxElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(sboxElement);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetSboxAttrCommand::redo()
{
    sboxElement->groupCnt0 = newGroupCnt0;
    sboxElement->groupCnt1 = newGroupCnt1;
    sboxElement->groupCnt2 = newGroupCnt2;
    sboxElement->groupCnt3 = newGroupCnt3;
    sboxElement->sboxMode = newSboxMode;
    sboxElement->sboxGroup = newSboxGroup;
    sboxElement->sboxByteSel = newSboxByteSel;
    sboxElement->sboxIncreaseMode = newSboxIncreaseMode;
    sboxElement->srcConfig = newSrcConfig;
    sboxElement->update();
    sboxElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(sboxElement);
    emit rcaScene->sendWindowToModified(newSceneModified);
}

SetBenesAttrCommand::SetBenesAttrCommand(RcaGraphScene *scene, ElementBenes *benesItem, ElementBenes *newBenes, QUndoCommand *parent)
    : ItemUndoCommands(parent)
    , rcaScene(scene)
    , benesElement(benesItem)
{
    Q_ASSERT(rcaScene);
    Q_ASSERT(benesElement);
    Q_ASSERT(newBenes);

    oldSceneModified = rcaScene->isModified;
    newSceneModified = true;
    newSrcConfig = newBenes->srcConfig;
    oldSrcConfig = benesElement->srcConfig;
}

void SetBenesAttrCommand::undo()
{
    benesElement->srcConfig = oldSrcConfig;
    benesElement->update();
    benesElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(benesElement);
    emit rcaScene->sendWindowToModified(oldSceneModified);
}

void SetBenesAttrCommand::redo()
{
    benesElement->srcConfig = newSrcConfig;
    benesElement->update();
    benesElement->setSelected(true);
    rcaScene->checkUndoItemIsShownItem(benesElement);
    emit rcaScene->sendWindowToModified(newSceneModified);
}

//SetPortConnectCommand::SetPortConnectCommand(RcaGraphScene *scene, ElementPort *port, InputPortType type, int index, ElementPort *inSignal, QUndoCommand *parent)
//    : ItemUndoCommands(parent)
//    , rcaScene(scene)
//    , portElement(port)
//    , newType(type)
//    , newIndex(index)
//    , newInSignal(inSignal)
//    , oldType(InputPort_NULL)
//    , oldIndex(0)
//    , oldInSignal(NULL)
//    , cmdAddWire(NULL)
//    , cmdDelWire(NULL)
//{
//    Q_ASSERT(rcaScene);
//    Q_ASSERT(portElement);

//    oldSceneModified = rcaScene->isModified;
//    newSceneModified = true;
//    oldType = portElement->getInputType();
//    oldIndex = portElement->getInputIndex();
//    oldInSignal = dynamic_cast<ElementPort*>(portElement->getInSignal());
//}

//void SetPortConnectCommand::undo()
//{
//    Q_ASSERT(rcaScene);
//    Q_ASSERT(portElement);

//    portElement->cleanAndCreateConnectPort(rcaScene,oldType,oldIndex,oldInSignal);
//    portElement->update();
//    if(portElement->parentItem())
//    {
//        BaseItem* parentBase = dynamic_cast<BaseItem*>(portElement->parentItem());
//        if(parentBase)
//        {
//            rcaScene->checkUndoItemIsShownItem(parentBase);
//        }
//    }
//    emit rcaScene->sendWindowToModified(oldSceneModified);
//}

//void SetPortConnectCommand::redo()
//{
//    Q_ASSERT(rcaScene);
//    Q_ASSERT(portElement);
//    portElement->cleanAndCreateConnectPort(rcaScene,newType,newIndex,newInSignal);
//    portElement->update();
//    if(portElement->parentItem())
//    {
//        BaseItem* parentBase = dynamic_cast<BaseItem*>(portElement->parentItem());
//        if(parentBase)
//        {
//            rcaScene->checkUndoItemIsShownItem(parentBase);
//        }
//    }
//    emit rcaScene->sendWindowToModified(newSceneModified);
//}
