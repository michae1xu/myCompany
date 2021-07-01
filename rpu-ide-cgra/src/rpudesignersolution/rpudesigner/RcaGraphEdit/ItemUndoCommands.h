#ifndef ITEMUNDOCOMMANDS_H
#define ITEMUNDOCOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>

#include "ModuleReadLfsr.h"
#include "ModuleWriteLfsr.h"
#include "RcaGraphDef.h"
#include "datafile.h"
using namespace rca_space;

class BaseItem;
class RcaGraphScene;
class ElementWire;
class ElementPort;
class ModuleBcu;
class ModuleRcu;
class ElementBfu;
class ElementCustomPE;
class ElementSbox;
class ElementBenes;
class BaseArithUnit;
class ModuleReadMem;
class ModuleWriteMem;
class ModuleReadRch;
class ModuleWriteRch;
class ModuleReadImd;

enum CommandsId{
    MoveItemCmdId = 1,
    MoveWireCmdId,
    ChangeWireCmdId,
    AddWireId,
    AddBfuId,
    AddCustomId,
    AddSboxId,
    AddBenesId,
    AddRcuId,
    AddBcuId,
    AddReadMemId,
    AddWriteMemId,
    AddReadImdId,
    AddReadRchId,
    AddReadLfsrId,
    AddWriteRchId,
    AddWriteLfsrId,
    DelWireId,
    DelBfuId,
    DelSboxId,
    DelBenesId,
    DelRcuId,
    DelBcuId,
    DelItemId,
    SetBcuIndexId,
    SetBcuAttrId,
    SetRcuIndexId,
    SetRcuAttrId,
    SetBfuIndexId,
    SetBfuAttrId,
    SetCustomPEAttrId,
    SetSboxIndexId,
    SetSboxAttrId,
    SetBenesIndexId,
    SetBenesAttrId,
    SetArithIndexId,
    SetArithAttrId,
    SetRMemIndexId,
    SetWMemIndexId,
    SetRImdAttrId,
    SetRRchAttrId,
    SetWRchAttrId,
    SetRLfsrAttrId,
    SetWLfsrAttrId,
    SetPortConnectId,
};

class ItemUndoCommands : public QUndoCommand
{
public:
    ItemUndoCommands(QUndoCommand *parent = 0);
//    void modifiedChanged(bool modified);
    void setToModified();
    void setNoModified();
protected:
    bool oldSceneModified;
    bool newSceneModified;
};

class MoveBaseItem {
public:
    MoveBaseItem(BaseItem* item,QPointF oldPos, QPointF newPos);
    BaseItem* baseItem;
    QPointF myOldPos;
    QPointF myNewPos;
};

class ItemMoveCommand : public ItemUndoCommands
{
public:
    ItemMoveCommand(RcaGraphScene* scene,QList<BaseItem*> itemList,QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return MoveItemCmdId; }

private:
    RcaGraphScene* rcaScene;
    QList<MoveBaseItem> moveItemList;


};

class ItemRectChangedCommand : public ItemUndoCommands
{
public:
    ItemRectChangedCommand(RcaGraphScene* scene,BaseItem* item,QRectF oldRect,QRectF newRect,QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return MoveItemCmdId; }

private:
    RcaGraphScene* rcaScene;
    BaseItem* changedItem;
    QRectF itemOldRect;
    QRectF itemNewRect;


};

class DeleteWireCommand : public ItemUndoCommands
{
public:
    DeleteWireCommand(RcaGraphScene* scene,QList<ElementWire*> wireList,QUndoCommand *parent = 0);
    ~DeleteWireCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return DelWireId; }
private:
    QList<ElementWire*> delWireList;
    RcaGraphScene* rcaScene;


};

class AddWireCommand : public ItemUndoCommands
{
public:
    AddWireCommand(RcaGraphScene* scene, QPointF posAtScene, ElementPort* inSingal, QBrush penBrush, qreal penWidth, QGraphicsItem *itemParent = 0, QUndoCommand *parent = 0);
    ~AddWireCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddWireId; }
    ElementWire* getElementWire() { return addWire;}
private:
    ElementWire* addWire;
    RcaGraphScene* rcaScene;
    QGraphicsItem* wireParent;


};

class WireChangeCommand : public ItemUndoCommands
{
public:
    WireChangeCommand(RcaGraphScene* scene,ElementWire* wire,QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return ChangeWireCmdId; }

private:
    RcaGraphScene* rcaScene;
    ElementWire* changedWire;
    QList<QPointF> newPortPressedPosList;  //相对port的点的坐标信息。
    QList<QPointF> newPortInfletionPosList;
    QList<QPointF> oldPortPressedPosList;  //相对port的点的坐标信息。
    QList<QPointF> oldPortInfletionPosList;
};


//class AddItemCommand : public ItemUndoCommands
//{
//public:
//    AddItemCommand(RcaGraphScene* scene, ElementUnitId id,BaseItem *itemParent = 0, QUndoCommand *parent = 0);
//    ~AddItemCommand();
//    void undo() Q_DECL_OVERRIDE;
//    void redo() Q_DECL_OVERRIDE;
//    int id() const Q_DECL_OVERRIDE;
//private:
//    BaseItem* addItem;
//    RcaGraphScene* rcaScene;
//    BaseItem* parentItem;
//    ElementUnitId unitId;
//};

class AddReadMemCommand : public ItemUndoCommands
{
public:
    AddReadMemCommand(RcaGraphScene* scene,int index,ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddReadMemCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddReadMemId; }
    ModuleReadMem* getReadMemModule() {return addReadMem;}
private:
    ModuleReadMem* addReadMem;
    RcaGraphScene* rcaScene;
    ElementPort* readMemParent;


};

class AddReadImdCommand : public ItemUndoCommands
{
public:
    AddReadImdCommand(RcaGraphScene* scene,int index,ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddReadImdCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddReadImdId; }
    ModuleReadImd* getReadImdModule() {return addReadImd;}
private:
    ModuleReadImd* addReadImd;
    RcaGraphScene* rcaScene;
    ElementPort* readImdParent;


};

class AddReadRchCommand : public ItemUndoCommands
{
public:
    AddReadRchCommand(RcaGraphScene* scene,int index,ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddReadRchCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddReadRchId; }
    ModuleReadRch* getReadRchModule() {return addReadRch;}
private:
    ModuleReadRch* addReadRch;
    RcaGraphScene* rcaScene;
    ElementPort* readRchParent;


};

class AddReadLfsrCommand : public ItemUndoCommands
{
public:
    AddReadLfsrCommand(RcaGraphScene* scene,int index,ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddReadLfsrCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddReadLfsrId; }
    ModuleReadLfsr* getReadLfsrModule() {return addReadLfsr;}
private:
    ModuleReadLfsr* addReadLfsr;
    RcaGraphScene* rcaScene;
    ElementPort* readLfsrParent;
};

class AddWriteRchCommand : public ItemUndoCommands
{
public:
    AddWriteRchCommand(RcaGraphScene* scene, int index,int address, ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddWriteRchCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddWriteRchId; }
    ModuleWriteRch* getWriteRchModule() {return addWriteRch;}
private:
    ModuleWriteRch* addWriteRch;
    RcaGraphScene* rcaScene;
    ElementPort* writeRchParent;
};

class AddWriteLfsrCommand : public ItemUndoCommands
{
public:
    AddWriteLfsrCommand(RcaGraphScene* scene, int index, ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddWriteLfsrCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddWriteLfsrId; }
    ModuleWriteLfsr* getWriteLfsrModule() {return addWriteLfsr;}
private:
    ModuleWriteLfsr* addWriteLfsr;
    RcaGraphScene* rcaScene;
    ElementPort* writeLfsrParent;
};

class AddWriteMemCommand : public ItemUndoCommands
{
public:
    AddWriteMemCommand(RcaGraphScene* scene,int index,ElementPort *itemParent = 0, QUndoCommand *parent = 0);
    ~AddWriteMemCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddWriteMemId; }
    ModuleWriteMem* getWriteMemModule() {return addWriteMem;}
private:
    ModuleWriteMem* addWriteMem;
    RcaGraphScene* rcaScene;
    ElementPort* writeMemParent;


};

class AddBcuCommand : public ItemUndoCommands
{
public:
    AddBcuCommand(RcaGraphScene* scene,int bcuIndex,/*QPointF pos,*/QGraphicsItem *itemParent = 0, QUndoCommand *parent = 0);
    ~AddBcuCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddBcuId; }
    ModuleBcu* getBcuModule() {return addBcu;}
private:
    ModuleBcu* addBcu;
    RcaGraphScene* rcaScene;
    QGraphicsItem* bcuParent;


//    QPointF posAtParent;
};

class DeleteItemCommand : public ItemUndoCommands
{
public:
    DeleteItemCommand(RcaGraphScene* scene,BaseItem* item,QUndoCommand *parent = 0);
    ~DeleteItemCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return DelItemId; }
private:
    BaseItem* delItem;
    RcaGraphScene* rcaScene;
    QGraphicsItem* parentItem;


//    QPointF posAtParent;
};

class AddRcuCommand : public ItemUndoCommands
{
public:
    AddRcuCommand(RcaGraphScene* scene,int bcuIndex,int rcuIndex,ModuleBcu *itemParent = 0, QUndoCommand *parent = 0);
    ~AddRcuCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddRcuId; }
    ModuleRcu* getRcuModule() {return addRcu;}
private:
    ModuleRcu* addRcu;
    RcaGraphScene* rcaScene;
    ModuleBcu* parentBcu;


};

class AddBfuCommand : public ItemUndoCommands
{
public:
    AddBfuCommand(RcaGraphScene* scene,int bcuIndex,int rcuIndex,int indexInRcu,ModuleRcu *itemParent, QUndoCommand *parent = 0);
    virtual ~AddBfuCommand();
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddBfuId; }
    ElementBfu* getBfuElement() {return addBfu;}
protected:
    ElementBfu* addBfu;
    RcaGraphScene* rcaScene;
    ModuleRcu* parentRcu;


};

class AddSboxCommand : public ItemUndoCommands
{
public:
    AddSboxCommand(RcaGraphScene* scene,int bcuIndex,int rcuIndex,int indexInRcu,ModuleRcu *itemParent, QUndoCommand *parent = 0);
    virtual ~AddSboxCommand();
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddSboxId; }
    ElementSbox* getSboxElement() {return addSbox;}
protected:
    ElementSbox* addSbox;
    RcaGraphScene* rcaScene;
    ModuleRcu* parentRcu;


};

class AddBenesCommand : public ItemUndoCommands
{
public:
    AddBenesCommand(RcaGraphScene* scene,int bcuIndex,int rcuIndex,int indexInRcu,ModuleRcu *itemParent, QUndoCommand *parent = 0);
    virtual ~AddBenesCommand();
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddBenesId; }
    ElementBenes* getBenesElement() {return addBenes;}
protected:
    ElementBenes* addBenes;
    RcaGraphScene* rcaScene;
    ModuleRcu* parentRcu;


};

class AddCustomPECommand : public ItemUndoCommands
{
public:
    AddCustomPECommand(RcaGraphScene* scene,int bcuIndex,int rcuIndex,int indexInRcu,ModuleRcu *itemParent, QUndoCommand *parent = 0);
    virtual ~AddCustomPECommand();
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return AddCustomId; }
    ElementCustomPE* getCustomPEElement() {return addCustomPE;}
protected:
    ElementCustomPE* addCustomPE;
    RcaGraphScene* rcaScene;
    ModuleRcu* parentRcu;



};
class SetBcuIndexCommand : public ItemUndoCommands
{
public:
    SetBcuIndexCommand(RcaGraphScene* scene,ModuleBcu* bcuItem,int index,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetBcuIndexId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleBcu* bcuModule;
    int oldIndex;
    int newIndex;


};

class SetRcuIndexCommand : public ItemUndoCommands
{
public:
    SetRcuIndexCommand(RcaGraphScene* scene,ModuleRcu* rcuItem,int index,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetRcuIndexId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleRcu* rcuModule;
    int oldIndex;
    int newIndex;


};

class SetArithIndexCommand : public ItemUndoCommands
{
public:
    SetArithIndexCommand(RcaGraphScene* scene,BaseArithUnit* arithItem,int index,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetArithIndexId; }
protected:
    RcaGraphScene* rcaScene;
    BaseArithUnit* arithElement;
    int oldIndex;
    int newIndex;


};

class SetRMemIndexCommand : public ItemUndoCommands
{
public:
    SetRMemIndexCommand(RcaGraphScene* scene,ModuleReadMem* rMemItem,int index,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetRMemIndexId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleReadMem* rMemModule;
    int oldIndex;
    int newIndex;


};

class SetWMemIndexCommand : public ItemUndoCommands
{
public:
    SetWMemIndexCommand(RcaGraphScene* scene,ModuleWriteMem* wMemItem,int index,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetWMemIndexId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleWriteMem* wMemModule;
    int oldIndex;
    int newIndex;


};

class SetRImdAttrCommand : public ItemUndoCommands
{
public:
    SetRImdAttrCommand(RcaGraphScene* scene,ModuleReadImd* rImdItem,ModuleReadImd* newImd,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetRImdAttrId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleReadImd* rImdModule;
    int oldIndex;
    int newIndex;


};

class SetRRchAttrCommand : public ItemUndoCommands
{
public:
    SetRRchAttrCommand(RcaGraphScene* scene,ModuleReadRch* rRchItem,ModuleReadRch* newRch,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetRRchAttrId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleReadRch* rRchModule;
    int oldIndex;
    int newIndex;


};

class SetWRchAttrCommand : public ItemUndoCommands
{
public:
    SetWRchAttrCommand(RcaGraphScene* scene,ModuleWriteRch* wRchItem,int index,int address,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetWRchAttrId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleWriteRch* wRchModule;
    int oldIndex;
    int newIndex;
    int oldAddress;
    int newAddress;
};

class SetRLfsrAttrCommand : public ItemUndoCommands
{
public:
    SetRLfsrAttrCommand(RcaGraphScene* scene,ModuleReadLfsr* rLfsrItem,ModuleReadLfsr* newLfsr,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetRLfsrAttrId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleReadLfsr* rLfsrModule;
    int oldIndex;
    int newIndex;
};

class SetWLfsrAttrCommand : public ItemUndoCommands
{
public:
    SetWLfsrAttrCommand(RcaGraphScene* scene,ModuleWriteLfsr* wLfsrItem,int index, QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetWLfsrAttrId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleWriteLfsr* wLfsrModule;
    int oldIndex;
    int newIndex;
};

class SetBcuAttrCommand : public ItemUndoCommands
{
public:
    SetBcuAttrCommand(RcaGraphScene* scene,ModuleBcu* bcuItem,ModuleBcu* newBcu,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetBcuAttrId; }
protected:
    RcaGraphScene* rcaScene;
    ModuleBcu* bcuModule;
    ModuleBcuIndex oldBcuSrc;
    ModuleBcuIndex newBcuSrc;

    int newSort = 0;
    int oldSort = 0;
    int newBurst = 0;
    int oldBurst = 0;
    int newGap = 0;
    int oldGap = 0;
    int newInfifoSrc = 0;
    int oldInfifoSrc = 0;
    int newOutfifoSrc = 0;
    int oldOutfifoSrc = 0;
    int newLfsrGroup = -1;
    int oldLfsrGroup = -1;

    int newReadImdMode128_32;
    int oldReadImdMode128_32;

    int newWriteDepth = 0;
    int oldWriteDepth = 0;
    int newWriteWidth = 0;
    int oldWriteWidth = 0;
    QString newAddressMode;
    QString oldAddressMode;

    QString newMemorySrcConfig;
    QString oldMemorySrcConfig;
    QString newInFifoSrcConfig;
    QString oldInFifoSrcConfig;
    QString newImdSrcConfig;
    QString oldImdSrcConfig;
};

class SetRcuAttrCommand : public ItemUndoCommands
{
public:
    SetRcuAttrCommand(RcaGraphScene* scene,ModuleRcu* rcuItem,ModuleRcu* newRcu,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetRcuAttrId; }
protected:
    RcaGraphScene* rcaScene;


    ModuleRcu* rcuModule;
    int newLoopStartEndFlag;
    int newLoopTimes;
    int newLoopStartEndFlag2;
    int newLoopTimes2;
    int newReadMemMode128_32;
    int newReadMemMode;
    int newReadMemAddr1;
    int newReadMemAddr2;
    int newReadMemAddr3;
    int newReadMemAddr4;
    int newReadMemOffset;
    int newReadMemThreashold;
    int newWriteMemMode;
    int newWriteMemAddr;
    int newWriteMemMask;
    int newWriteMemOffset;
    int newWriteMemThreashold;
    int newRchMode;
    int newWriteRchAddr1;
    int newWriteRchAddr2;
    int newWriteRchAddr3;
    int newWriteRchAddr4;

    int oldLoopStartEndFlag;
    int oldLoopTimes;
    int oldLoopStartEndFlag2;
    int oldLoopTimes2;
    int oldReadMemMode128_32;
    int oldReadMemMode;
    int oldReadMemAddr1;
    int oldReadMemAddr2;
    int oldReadMemAddr3;
    int oldReadMemAddr4;
    int oldReadMemOffset;
    int oldReadMemThreashold;
    int oldWriteMemMode;
    int oldWriteMemAddr;
    int oldWriteMemMask;
    int oldWriteMemOffset;
    int oldWriteMemThreashold;
    int oldRchMode;
    int oldWriteRchAddr1;
    int oldWriteRchAddr2;
    int oldWriteRchAddr3;
    int oldWriteRchAddr4;
};

class SetBfuAttrCommand : public ItemUndoCommands
{
public:
    SetBfuAttrCommand(RcaGraphScene* scene,ElementBfu* bfuItem,ElementBfu* newBfu,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetBfuAttrId; }
protected:
    RcaGraphScene* rcaScene;


    ElementBfu* bfuElement;
    BfuFuncIndex newFuncIndex;
    QString newFuncExp;
    int newBypassIndex;
    int newFuncAuModIndex;
    bool newFuncAuCarry;
    int newFuncMuModIndex;
    QString newBypassExp;
    BfuFuncIndex oldFuncIndex;
    QString oldFuncExp;
    int oldBypassIndex;
    int oldFuncAuModIndex;
    bool oldFuncAuCarry;
    int oldFuncMuModIndex;
    QString oldBypassExp;
};

class SetCustomPEAttrCommand : public ItemUndoCommands
{
public:
    SetCustomPEAttrCommand(RcaGraphScene* scene,ElementCustomPE* customPEItem,ElementCustomPE* newCustomPE,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetCustomPEAttrId; }
protected:
    RcaGraphScene* rcaScene;

    ElementCustomPE* customPEElement;
    BfuFuncIndex newFuncIndex;
    int newInputNum;
    int newOutputNum;
    QString newFuncExp;
    int newBypassIndex;
    int newFuncAuModIndex;
    bool newFuncAuCarry;
    int newFuncMuModIndex;
    QString newBypassExp;

    BfuFuncIndex oldFuncIndex;
    int oldInputNum;
    int oldOutputNum;
    QString oldFuncExp;
    int oldBypassIndex;
    int oldFuncAuModIndex;
    bool oldFuncAuCarry;
    int oldFuncMuModIndex;
    QString oldBypassExp;
};


class SetSboxAttrCommand : public ItemUndoCommands
{
public:
    SetSboxAttrCommand(RcaGraphScene* scene,ElementSbox* sboxItem,ElementSbox* newSbox,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetSboxAttrId; }
protected:
    RcaGraphScene* rcaScene;

    ElementSbox* sboxElement;
    SboxGroupValue newGroupCnt0;
    SboxGroupValue newGroupCnt1;
    SboxGroupValue newGroupCnt2;
    SboxGroupValue newGroupCnt3;
    int newSboxMode;
    int newSboxGroup;
    int newSboxByteSel;
    int newSboxIncreaseMode;
    QString newSrcConfig;

    SboxGroupValue oldGroupCnt0;
    SboxGroupValue oldGroupCnt1;
    SboxGroupValue oldGroupCnt2;
    SboxGroupValue oldGroupCnt3;
    int oldSboxMode;
    int oldSboxGroup;
    int oldSboxByteSel;
    int oldSboxIncreaseMode;
    QString oldSrcConfig;
};

class SetBenesAttrCommand : public ItemUndoCommands
{
public:
    SetBenesAttrCommand(RcaGraphScene* scene,ElementBenes* benesItem,ElementBenes* newBenes,QUndoCommand *parent = 0);
    virtual void undo() Q_DECL_OVERRIDE;
    virtual void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE { return SetBenesAttrId; }
protected:
    RcaGraphScene* rcaScene;


    ElementBenes* benesElement;
    QString newSrcConfig;
    QString oldSrcConfig;
};

//class SetPortConnectCommand : public ItemUndoCommands
//{
//public:
//    SetPortConnectCommand(RcaGraphScene* scene,ElementPort* port,InputPortType type,int index,ElementPort* inSignal,QUndoCommand *parent = 0);
//    virtual void undo() Q_DECL_OVERRIDE;
//    virtual void redo() Q_DECL_OVERRIDE;
//    int id() const Q_DECL_OVERRIDE { return SetPortConnectId; }
//protected:
//    RcaGraphScene* rcaScene;


//    ElementPort* portElement;
//    InputPortType newType;
//    int newIndex;
//    ElementPort* newInSignal;
//    InputPortType oldType;
//    int oldIndex;
//    ElementPort* oldInSignal;
//    AddWireCommand* cmdAddWire;
//    DeleteWireCommand* cmdDelWire;
//};

#endif // ITEMUNDOCOMMANDS_H
