#ifndef ELEMENTPORT_H
#define ELEMENTPORT_H

#include "BasePort.h"
#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBrush>
#include <export_typedef.h>

#include "RcaGraphDef.h"
#include "ModuleWriteMem.h"
#include "BaseArithUnit.h"
#include "ModuleReadImd.h"
#include "ModuleReadRch.h"
#include "ModuleWriteRch.h"
#include "ModuleReadLfsr.h"
#include "ModuleWriteLfsr.h"

class RcaGraphScene;

using namespace rca_space;

enum PortDirection{
//    NoDirection =0 ,
    InputDirection,
    OutputDirection
};

enum PortShape{
    ShapeTriangle = 0,
    ShapeRectangle,
    ShapeEllipsoid,
};

struct DebugPortInfo
{
    DebugPortInfo() {
        elementStr = "";
        isTextChanged = false;
    }
    int typeId;
    int bcu;
    int rcu;
    int id;    //一个rcu中，同类算子，第几个
    int portId;//算子里的第几个端口，输入输出端口分两类
    QString elementStr;
    bool isTextChanged;
};

class ElementWire;

class ElementPort : public BasePort
{
    Q_OBJECT
public:
    explicit ElementPort(PortDirection type,QGraphicsItem *parent,PortFromWhereAttr attr);
    virtual ~ElementPort();
//    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
//    void setPortType(PortType type) {portType = type;}
    void setPortShape(PortShape shape) {portShape = shape;}
//    void setPaintRect(QRectF rect);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
//    void setColor(QColor color) {piantColor = color;}
    void setPortBrush(QBrush brush) {paintBrush = brush;}
    PortDirection getPortDirection() { return portDirection;}
    bool isHadInSignal(); //表示是否已经有输入的信号源了，如果此port方向portType为in，才可以设置此项，否则此项一直为false。
    BasePort* getInSignal();
    InputPortType getInputType();
    void setInputType(InputPortType type);
    int getInputIndex();
    void setInputIndex(int index);
//    void setInputSignalFromXml();

    bool addWriteRchFromXml(InputPortType inType, int inIndex, int writeRchIndex, int writeRchAddress, BaseArithUnit *parentBaseArith);
    bool addWriteLfsrFromXml(InputPortType inType, int inIndex, int writeLfsrIndex, BaseArithUnit *parentBaseArith);
    bool addWriteMemFromXml(InputPortType inType, int inIndex, int writeMemIndex, BaseArithUnit *parentBaseArith);
    bool addReadMemFromXml(int readMemIndex);
    bool addReadImdFromXml(int readImdIndex);
    bool addReadRchFromXml(int readRchIndex);
    bool addReadLfsrFromXml(int readLfsrIndex);
    bool addReadMemWithoutUndo(int readMemIndex);

    void addWireOrMem(QGraphicsItem *wireOrMem);
    void deleteWireOrMem(QGraphicsItem* wireOrMem);

    void addWireOrOtherInputPort(QGraphicsItem *wireOrMem);
    int deleteAllConnectWire();
    void deleteAllConnectWireAndMem();
    int deleteAllConnectWireWithoutUndo();
    void resetConnectOutPortInputIndex(int index);
    void showItemWire(bool isShow);
    bool addNewReadMem();
    bool addNewReadImd();
    bool addNewReadRch();
    bool addNewReadLfsr();
    bool addNewWriteMem(BaseArithUnit *parentBaseArith);
    bool addNewWriteRch(BaseArithUnit *parentBaseArith);
    bool addNewWriteLfsr(BaseArithUnit *parentBaseArith);
    void pasteOtherPort(ElementPort &copyPort);
    bool pasteNewReadMem(ModuleReadMem &copyReadMem);
    bool pasteNewReadImd(ModuleReadImd &copyReadImd);
    bool pasteNewReadRch(ModuleReadRch &copyReadRch);
    bool pasteNewReadLfsr(ModuleReadLfsr &copyReadLfsr);
    bool pasteNewWriteMem(ModuleWriteMem &copyWriteMem);
    bool pasteNewWriteRch(ModuleWriteRch &copyWriteRch);
    bool pasteNewWriteLfsr(ModuleWriteLfsr &copyWriteLfsr);
    void copyOtherPort(ElementPort &copyPort);
    bool copyOtherReadMem(ModuleReadMem &copyReadMem);
    bool copyOtherReadImd(ModuleReadImd &copyReadImd);
    bool copyOtherReadRch(ModuleReadRch &copyReadRch);
    bool copyOtherReadLfsr(ModuleReadLfsr &copyReadLfsr);
    bool copyOtherWriteMem(ModuleWriteMem &copyWriteMem);
    bool copyOtherWriteRch(ModuleWriteRch &copyWriteRch);
    bool copyOtherWriteLfsr(ModuleWriteLfsr &copyWriteLfsr);
    bool insertWMemToList(int index,int curIndex);
    bool insertWRchToList(int index, int indexWRch);
    bool insertWLfsrToList(int index, int indexWLfsr);
    int getWireCountInPort();
    bool isContainsWire(ElementWire* wire);
    bool canConnectOutPort(ElementPort* outPort);
    bool canConnectOutPort(InputPortType type, int index, ElementPort **resultPort);//当返回true时，resultPort寻找到的port。
    int deletePrevLinePortIn(); //删除来自上一行所有的输入。
    int deleteNextLinePortOut();//删除输出到下一行的所有输出。
    virtual void pushItemToDeletedStack(bool isForced);
//    bool cleanAndCreateConnectPort(RcaGraphScene* rcaScene,InputPortType type,int index,ElementPort* inSignal);
    bool resetConnectPort(RcaGraphScene* rcaScene,InputPortType type,int index,ElementPort* inSignal);
    QString getPortInflectPos();
    QString getPortPressedPos();
    void setInfletionPosByXml(QString str);
    void resetInfletionPos(QList<QPointF> scenePosList);
    void setPressedPosByXml(QString str);
    void resetPressedPos(QList<QPointF> scenePosList);
    void deleteReadRchDiffGroup(int rchGroupId);

public slots:
//    bool resetInputSignal(InputPortType inType, int inIndex);
    bool connectInputSignal(BasePort* inPort);
    bool setInSignalAllAttr(BasePort* inPort);
    bool clearInSignalAllAttr();
    void handleWRMemBeDeleted(BaseItem* deletedItem);
    void handleRImdBeDeleted(BaseItem* deletedItem);
    void handleWRRchBeDeleted(BaseItem* deletedItem);
    void handleWRLfsrBeDeleted(BaseItem* deletedItem);
    void portXYChanged();

signals:
    void portBeDeleted(ElementPort* port);
    void addWriteMemSuccessed(ModuleWriteMem *writeMem);
    void addReadMemSuccessed(ModuleReadMem *readMem);
    void addReadImdSuccessed(ModuleReadImd* readImd);
    void addReadRchSuccessed(ModuleReadRch* readRch);
    void addReadLfsrSuccessed(ModuleReadLfsr* readLfsr);
    void addWriteRchSuccessed(ModuleWriteRch *writeRch);
    void addWriteLfsrSuccessed(ModuleWriteLfsr *writeLfsr);
    void portBeChanged(ElementPort* port);

    void addBreakPoint(rpu_esl_model::SBreak &breakHandle);
    void removeBreakPoint(rpu_esl_model::SBreak &breakHandle);
private:
    PortDirection portDirection;
    BasePort* inSignal;
    bool isBreakPoint;

private:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

protected:
    PortShape portShape;
    QRectF textRect;            //文字显示的区域
    QPainterPath paintPath;
    bool dragOver;
//    QColor piantColor;
    QBrush paintBrush;
    InputPortType inputType;
    int inputIndex;
    QList<QGraphicsItem*> wireThisPort;

public:
    HoverPortState hoverState;
    QList<QPointF> portPressedPosList;  //相对port的点的坐标信息。
    QList<QPointF> portInfletionPosList;
    QList<QPointF> oldPortPressedPosList;  //相对port的点的坐标信息。
    QList<QPointF> oldPortInfletionPosList;

public:
    DebugPortInfo *dpInfo;
    void initDebugPortInfo(int typeId,int bcu,int rcu,int id,int portId,QString elementStr,bool isTextChanged);

};

#endif // ELEMENTPORT_H
