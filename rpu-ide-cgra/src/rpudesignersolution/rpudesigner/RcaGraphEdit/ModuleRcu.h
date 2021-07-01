#ifndef MODULERCU_H
#define MODULERCU_H

#include "BaseItem.h"
#include "RcaGraphDef.h"

#include <QGraphicsItem>
#include <QString>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPointF>
#include <QGraphicsProxyWidget>
#include <QKeyEvent>

#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "elementcustompe.h"
#include "datafile.h"
using namespace rca_space;

class ModuleRcu : public BaseItem
{
    Q_OBJECT
    friend class CPropertyBrowser;
public:
    friend class ItemCopyPaste;
    explicit ModuleRcu(int bcu,int rcu,QGraphicsItem *parent = 0);
    explicit ModuleRcu(ModuleRcu &copyRcu, QGraphicsItem *parent = 0);
    virtual ~ModuleRcu();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;

    int getIndexRcu() { return indexRcu;}
    bool setIndexBcu(int index);
    bool setIndexRcu(int index);
    int getIndexBcu() { return indexBcu;}
    void setRealBoundingRect(QRectF rect) override;

    bool addBfuFromXml(QPointF posAtParent , ElementBfu* elementBfu);
    bool addCustomPEFromXml(QPointF posAtParent , ElementCustomPE* elementCustomPE);
    bool addSboxFromXml(QPointF posAtParent , ElementSbox* elementSbox);
    bool addBenesFromXml(QPointF posAtParent , ElementBenes* elementBenes);

    ElementBfu* findBfuFromRcu(int indexInRcu);//如果没有此bfu则返回NULL;
    ElementCustomPE* findCustomPEFromRcu(int indexInRcu);//如果没有此customPE则返回NULL;
    ElementSbox* findSboxFromRcu(int indexInRcu);//如果没有此sbox则返回NULL;
    ElementBenes* findBenesFromRcu(int indexInRcu);//如果没有此benes则返回NULL;

    void autoMakeAllArithUnit();
    void autoSetPosOfChildArithUnit();

    bool replaceBfuIndex(int indexSrc,int indexDst);
    bool replaceSboxIndex(int indexSrc,int indexDst);
    bool replaceBenesIndex(int indexSrc,int indexDst);

    QRectF getChildrenMinRect();

    void insertArithToList(int index,BaseArithUnit* arith);
    void insertBfuToList(int index,ElementBfu* bfuElement);
    void insertSboxToList(int index,ElementSbox* sboxElement);
    void insertBenesToList(int index,ElementBenes* benesElement);
    void insertCustomPEToList(int index, ElementCustomPE *customPEElement);

    void deleteArithList(bool isForced);
    virtual void pushItemToDeletedStack(bool isForced) override;

    ElementBfu* pasteNewBfu(ElementBfu &copyBfu, QPointF posAtThis);
    bool copyOtherBfu(ElementBfu &copyBfu);
    ElementCustomPE* pasteNewCustomPE(ElementCustomPE &copyCustomPE, QPointF posAtThis);
    bool copyOtherCustomPE(ElementCustomPE &copyCustomPE);
    ElementSbox* pasteNewSbox(ElementSbox &copySbox, QPointF posAtThis);
    bool copyOtherSbox(ElementSbox &copySbox);
    ElementBenes* pasteNewBenes(ElementBenes &copyBenes, QPointF posAtThis);
    bool copyOtherBenes(ElementBenes &copyBenes);

    void pasteAllChildWithOtherRcu(ModuleRcu& otherRcu);
    void copyAllChildWithOtherRcu(ModuleRcu& otherRcu);

    QList<BaseArithUnit*> getSelectedArith();
    void resetBoundingRectByPaste(QRectF newRect);
    void reaetRchChannelId(int rchGroupId);
signals:
//    void rcuBeDeleted(ModuleRcu* deletedRcu);
    void bfuFullStatesChanged(bool isFulled);
    void sboxFullStatesChanged(bool isFulled);
    void benesFullStatesChanged(bool isFulled);

private slots:
    void handleBfuBeDeleted(BaseItem* deletedItem);
    void handleCustomPEBeDeleted(BaseItem* deletedItem);
    void handleSboxBeDeleted(BaseItem* deletedItem);
    void handleBenesBeDeleted(BaseItem* deletedItem);
    void handleAddWriteMemSuccess(ModuleWriteMem* writeMem);
    void handleAddReadMemSuccess(ModuleReadMem* readMem);
    void handleFifoDeleted(QObject* delFifo);
protected:
    QString moduleRcuName;
    int indexBcu;
    int indexRcu;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void showItemPortWire() override;
    QRectF enabledRect2PaintRect(QRectF rect);
    virtual QRectF enabledRect2BoundingRect(QRectF rect) override;
private:
    bool autoMakeNewBfu(int indexId);
    bool autoMakeNewCustomPE(int indexId);
    bool autoMakeNewSbox(int indexId);
    bool autoMakeNewBenes(int indexId);
    bool addNewBfu(QPointF posAtThis);
    bool addNewCustomPE(QPointF posAtThis);
    bool addNewSbox(QPointF posAtThis);
    bool addNewBenes(QPointF posAtThis);
    void resetBoundingRectByChildrenBound();
    QRectF paintRect2EnabledRect(QRectF rect);
private:
    int bfuMaxCount;
    QList<ElementBfu*> bfuExistList;
    bool isBfuFulled;

    QList<ElementCustomPE*> customPEExistList;

    int sboxMaxCount;
    QList<ElementSbox*> sboxExistList;
    bool isSboxFulled;

    int benesMaxCount;
    QList<ElementBenes*> benesExistList;
    bool isBenesFulled;

private:
    QRectF inPortRect;
    QRectF outPortRect;

public:
    ElementPort * inFifo0;
    ElementPort * inFifo1;
    ElementPort * inFifo2;
    ElementPort * inFifo3;

    ElementPort * outFifo0;
    ElementPort * outFifo1;
    ElementPort * outFifo2;
    ElementPort * outFifo3;

    int wMemMaxCount;
    QList<int> wMemExistList;
    bool isWMemFulled;

    int wRchMaxCount;
    QList<int> wRchExistList;
    bool isWRchFulled;

    int wLfsrMaxCount;
    QList<int> wLfsrExistList;
    bool isWLfsrFulled;

//    int rchChannel;

    int loopStartEndFlag = 0;
    int loopTimes = 0;
    int loopStartEndFlag2 = 0;
    int loopTimes2 = 0;

    int writeMemOffset;
    int writeMemMask;
    int writeMemMode;
    int writeMemThreashold;
    int writeMemAddr;

    int readMemOffset;
    int readMemMode;
    int readMemMode128_32;
    int readMemThreashold;
    int readMemAddr1;
    int readMemAddr2;
    int readMemAddr3;
    int readMemAddr4;

    int rchMode;
    int writeRchAddr1;
    int writeRchAddr2;
    int writeRchAddr3;
    int writeRchAddr4;
};

#endif // MODULERCU_H
