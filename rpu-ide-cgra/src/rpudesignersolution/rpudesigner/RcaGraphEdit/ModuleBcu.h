#ifndef MODULEBCU_H
#define MODULEBCU_H

#include "BaseItem.h"
#include "RcaGraphDef.h"
#include "ModuleRcu.h"
#include "ElementPort.h"

#include <QGraphicsItem>
#include <QString>
#include <QPainter>
#include <QKeyEvent>

#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "ElementPort.h"

using namespace rca_space;

class ModuleBcu : public BaseItem
{
    Q_OBJECT
public:
    friend class ItemCopyPaste;
    friend class CPropertyBrowser;
    explicit ModuleBcu(int bcu,QGraphicsItem *parent = 0);
    explicit ModuleBcu(ModuleBcu &copyBcu, QGraphicsItem *parent = 0);
    virtual ~ModuleBcu();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    int getIndexBcu() {return indexBcu;}
    bool setIndexBcu(int index);
    void setRealBoundingRect(QRectF rect);
    bool addRcuFromXml(QPointF posAtParent, ModuleRcu *moduleRcu);
    bool addBfuFromXml(QPointF posAtParent, ElementBfu* elementBfu);
    bool addCustomPEFromXml(QPointF posAtParent, ElementCustomPE* elementCustomPE);
    bool addSboxFromXml(QPointF posAtParent, ElementSbox* elementSbox);
    bool addBenesFromXml(QPointF posAtParent, ElementBenes* elementBenes);
    ModuleRcu* findRcuFromThis(int rcuIndex);
    void autoMakeAllRcu();
    void autoSetPosOfChildRcu();
    bool replaceRcuIndex(int indexSrc,int indexDst);
    void setChildrenRcuWidth(qreal parentLeft, qreal width);
    QRectF getChildrenRcuMinRect();
    void insertRcuToList(int rcuIndex,ModuleRcu*  xrcuModule);
    void deleteRcuList(bool isForced);
    void undoRectToRealBoundingRect(QRectF rect);
    virtual void pushItemToDeletedStack(bool isForced);

    ModuleRcu *pasteNewRcu(ModuleRcu &copyRcu, QPointF posAtThis);
    void pasteAllChildWithOtherBcu(ModuleBcu& otherBcu);
    void copyAllChildWithOtherBcu(ModuleBcu& otherBcu);
    bool copyOtherRcu(ModuleRcu &copyRcu);
    QList<ModuleRcu*> getSelectedRcu();
    void resetBoundingRectByPaste(QRectF newRect);
    QRectF enabledRect2PaintRect(QRectF rect);
    virtual QRectF enabledRect2BoundingRect(QRectF rect) Q_DECL_OVERRIDE;
protected:
    QString moduleBcuName;
    int indexBcu = 0;

protected:
//    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
//    virtual void showItemPortWire();
    virtual bool adjustRect(QPointF pos) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
private slots:
//    void handleFifoDeleted(QObject* delFifo);
signals:
    void rcuFullStatesChanged(bool isFulled);

public:

    ModuleBcuIndex srcDataFromBcu;//bcu 数据来源

    int burst = 0;
    int gap = 0;
    int infifoSrc = 0;
    int outfifoSrc = 0;
    int lfsrGroup = -1;
    int sort = 0;
    int readImdMode128_32 = 0;
    int writeWidth = 0;
    int writeDepth = 0;
    QString addressMode = "00000000";

    QString memorySrcConfig;
    QString inFifoSrcConfig;
    QString imdSrcConfig;

    QList<ModuleRcu*> rcuExistList;

//    int wRchMaxCount;
//    QList<int> wRchExistList;
//    bool isWRchFulled;
    void setSort(int value);
    bool autoMakeNewRcu(int rcuIndex);
private:
    //    QRectF inPortRect;
//    QRectF outPortRect;

    int rcuMaxCount;
    bool isRcuFulled;
    QRectF rightMarkRect;

private:
//    ElementPort * outFifo0;
//    ElementPort * outFifo1;
//    ElementPort * outFifo2;
//    ElementPort * outFifo3;

//    ElementPort * inFifo0;
//    ElementPort * inFifo1;
//    ElementPort * inFifo2;
//    ElementPort * inFifo3;

    bool addNewRcu(QPointF posAtThis);
    QRectF paintRect2EnabledRect(QRectF rect);
    void resetBoundingRectByChildrenBound();
    void setHoverCursor(QPointF pos);
private slots:
    void handleRcuBeDeleted(BaseItem *deletedItem);
};

#endif // MODULEBCU_H
