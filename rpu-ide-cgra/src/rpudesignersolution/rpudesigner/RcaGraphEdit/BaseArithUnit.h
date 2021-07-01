#ifndef BASEARITHUNIT_H
#define BASEARITHUNIT_H

#include "BaseItem.h"
#include "RcaGraphDef.h"
#include "ModuleWriteMem.h"
#include "ModuleReadMem.h"

#include <QString>
#include <QPainter>

#define DEBUG_BFU_INPUTCOUNT      3
#define DEBUG_SBOX_INPUTCOUNT     1
#define DEBUG_BENES_INPUTCOUNT    4

#define DEBUG_BFU_OUTPUTCOUNT     2
#define DEBUG_SBOX_OUTPUTCOUNT    4
#define DEBUG_BENES_OUTPUTCOUNT   4

enum ElementTypeId_del{
    DEBUG_BFU_del = 0,
    DEBUG_SBOX_del,
    DEBUG_BENES_del
};

const QMap<int,QString> mapElementName_del ={
    {DEBUG_BFU_del,"BFU"},
    {DEBUG_SBOX_del,"SBOX"},
    {DEBUG_BENES_del,"BENES"},
};

const QMap<int,int> mapElementInput_del ={
    {DEBUG_BFU_del,DEBUG_BFU_INPUTCOUNT},
    {DEBUG_SBOX_del,DEBUG_SBOX_INPUTCOUNT},
    {DEBUG_BENES_del,DEBUG_BENES_INPUTCOUNT}
};

const QMap<int,int> mapElementOutput_del ={
    {DEBUG_BFU_del,DEBUG_BFU_OUTPUTCOUNT},
    {DEBUG_SBOX_del,DEBUG_SBOX_OUTPUTCOUNT},
    {DEBUG_BENES_del,DEBUG_BENES_OUTPUTCOUNT}
};

class ElementInfo_del{
public:
    QString elementId;
    int typeId;
    int bcu;
    int rcu;
    int id;
    int isEnabled;
    QStringList inputList;
    QStringList outputList;
//    QList<bool> isInputTextChaged;  //前后周期数据更改标志位。
//    QList<bool> isOutputTextChaged; //前后周期数据更改标志位。
//    QList<bool> isInputPointBreak;
//    QList<bool> isOutputPointBreak;
    bool isEnableChanged;
    ElementInfo_del();
    ~ElementInfo_del();
};

using namespace rca_space;

class BaseArithUnit : public BaseItem
{
    Q_OBJECT
    friend class CPropertyBrowser;

public:
    explicit BaseArithUnit(ArithUnitId unitId,int bcu,int rcu,int index,QGraphicsItem *parent = 0);
    virtual ~BaseArithUnit();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;

    int getIndexBcu() const {return indexBcu;}
    int getIndexRcu() const {return indexRcu;}
    int getIndexInRcu() const { return indexInRcu;}
    bool setIndexBcu(int index);
    bool setIndexRcu(int index);
    bool setIndexInRcu(int index);
    virtual void pushItemToDeletedStack(bool isForced) override;

    QBitArray getCodeIndex() const;
    void setCodeIndex(const QBitArray &codeIndex);

protected slots:
    void handleAddWriteMemSuccess(ModuleWriteMem* writeMem);
    void handleAddReadMemSuccess(ModuleReadMem* readMem);
signals:
    void addWriteMemSuccessed(ModuleWriteMem *writeMem);
    void addReadMemSuccessed(ModuleReadMem *readMem);

protected:
    virtual void showItemPortWire() override;
    void deletePrevLineIn();
    void deleteNextLineOut();
    virtual void setChildrenSelected(bool enabled) override;

protected:
    ArithUnitId arithUnitId;
    QString arithUnitName;
    int indexBcu;
    int indexRcu;
    int indexInRcu;
    int inPortCount;
    int outPortCount;

    bool isElementInfoInit;
//    void setIndexInRcu(int index);
};

#endif // BASEARITHUNIT_H
