#ifndef RCAGRAPHSCENE_H
#define RCAGRAPHSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QPointF>
#include <QList>
#include <QGraphicsItem>
#include <QUndoStack>

#include "RcaGraphDef.h"
#include "ModuleBcu.h"
#include "ModuleRcu.h"
#include "ElementWire.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "ElementPort.h"
#include "ModuleReadMem.h"
#include "ModuleWriteMem.h"
#include "ItemUndoCommands.h"
#include "ItemCopyPaste.h"
//#include "RcaGridItem.h"
#include "CPropertyBrowser.h"
#include "ModuleWriteLfsr.h"
#include "datafile.h"

class CProjectResolver;
class AddWireCommand;
class CItemAttributeSetting;
class CXmlResolver;

using namespace rca_space;

class RcaGraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit RcaGraphScene(QHash<int, SCustomPEConfig> *customPEConfigHash, QObject *parent = 0, CProjectResolver *projectResolver = 0);
    ~RcaGraphScene();

    bool addBcuFromXml(QPointF posAtParent, ModuleBcu* moduleBcu);
    bool addRcuFromXml(QPointF posAtParent, ModuleRcu* moduleRcu);
    bool addBfuFromXml(QPointF posAtParent, ElementBfu* elementBfu);
    bool addCustomPEFromXml(QPointF posAtParent, ElementCustomPE* elementCustomPE);
    bool addSboxFromXml(QPointF posAtParent, ElementSbox* elementSbox);
    bool addBenesFromXml(QPointF posAtParent, ElementBenes* elementBenes);
    bool addElementWireFromXml(ElementPort* inputPort, ElementPort* outputPort);
    bool addElementWireBySet(ElementPort* inputPort, ElementPort* outputPort);
//    bool addElementWireWithoutUndo(ElementPort* inputPort,ElementPort* outputPort);

    ModuleBcu* findBcuFromScene(int bcuIndex);//如果没有此bcu则返回NULL;
    ModuleRcu* findRcuFromScene(int bcuIndex,int rcuIndex);//如果没有此rcu则返回NULL;
    ElementBfu* findBfuFromScene(int bcuIndex,int rcuIndex,int indexInRcu);//如果没有此bfu则返回NULL;
    ElementCustomPE* findCustomPEFromScene(int bcuIndex,int rcuIndex,int indexInRcu);//如果没有此customPE则返回NULL;
    ElementSbox* findSboxFromScene(int bcuIndex,int rcuIndex,int indexInRcu);//如果没有此sbox则返回NULL;
    ElementBenes* findBenesFromScene(int bcuIndex,int rcuIndex,int indexInRcu);//如果没有此benes则返回NULL;

    void initWireAndMemConPort();
    void initRcuRectByBcu();
//    bool resetInputPortConnectWire(ElementPort *elePort, InputPortType inputType , int inputIndex);
    void handleDeleteWire(ElementWire* deleteWire);
    void autoAddBcu();
    void autoAddNewRcuCfg();
    void addNewRcuCfg(int rcuCfgID);
    void addNewRcuCfg();
    void autoSetPosOfChildBcu(EBcuComposing bcuCom);
    void clearAllWire();
    void showAllWire();
    void hideAllWire();
    void setResourceMap();
    void setProject(CProjectResolver *projectResolver);
    CProjectResolver *getProject(){return m_projectResolver;}
    void showAllWire(bool isShow);

    QString setBaseItemAttr(BaseItem *baseItem);
    QString setBcuItemAttr(ModuleBcu* moduleBcu);
    QString setRcuItemAttr(ModuleRcu* moduleRcu);
    QString setBfuItemAttr(ElementBfu* elementBfu);
    QString setCustomPEItemAttr(ElementCustomPE* elementCustomPE);
    QString setSboxItemAttr(ElementSbox* elementSbox);
    QString setBenesItemAttr(ElementBenes* elementBenes);
    QString setReadMemItemAttr(ModuleReadMem* readMem);
    QString setWriteMemItemAttr(ModuleWriteMem* writeMem);
    QString setReadImdItemAttr(ModuleReadImd* readImd);
    QString setReadRchItemAttr(ModuleReadRch* readRch);
    QString setWriteRchItemAttr(ModuleWriteRch* writeRch);
    QString setReadLfsrItemAttr(ModuleReadLfsr* readLfsr);
    QString setWriteLfsrItemAttr(ModuleWriteLfsr* writeLfsr);

    void itemDoubleClicked(BaseItem *baseItem);
    void isItemBakBeDeleted(BaseItem* baseItem);
    void selectAllItems();
    bool alignSelectedItems(ItemsAlignPosAttr alignAttr);
    bool equiSelectedItems(bool isHorEqui);

    void setSceneState(RcaSceneStates state);
    RcaSceneStates getSceneState() { return sceneState;}
    SelectedItemsAttr getCurrentSelectedAttr(){return selectedAttr;}

    void pushCmdItemMoved(QList<BaseItem*> movedItemList);
    void pushCmdAddWire(AddWireCommand *cmdAddWire);
    void pushCmdWireDeleted(QList<ElementWire*> delWireList);
    void pushCmdWireChanged(ElementWire* changedWire);
    void pushCmdAddBcu(AddBcuCommand* cmdAddBcu);
    void pushCmdAddRcu(AddRcuCommand* cmdAddRcu);
    void pushCmdAddBfu(AddBfuCommand* cmdAddBfu);
    void pushCmdAddSbox(AddSboxCommand* cmdAddSbox);
    void pushCmdAddBenes(AddBenesCommand* cmdAddBenes);
    void pushCmdAddRMem(AddReadMemCommand* cmdAddRMem);
    void pushCmdAddRImd(AddReadImdCommand* cmdAddRImd);
    void pushCmdAddRRch(AddReadRchCommand* cmdAddRRch);
    void pushCmdAddRLfsr(AddReadLfsrCommand* cmdAddRLfsr);
    void pushCmdAddWMem(AddWriteMemCommand* cmdAddWMem);
    void pushCmdAddWRch(AddWriteRchCommand* cmdAddWRch);
    void pushCmdAddWLfsr(AddWriteLfsrCommand* cmdAddWLfsr);
    void pushCmdAddCustomPE(AddCustomPECommand* cmdAddCustomPE);
    void pushCmdDeleteItem(DeleteItemCommand* cmdDelItem);
    void pushCmdSetBcuIndex(SetBcuIndexCommand* cmdSetBcuId);
    void pushCmdSetRcuIndex(SetRcuIndexCommand* cmdSetRcuId);
    void pushCmdSetArithIndex(SetArithIndexCommand* cmdSetArithId);
    void pushCmdSetRMemIndex(SetRMemIndexCommand* cmdSetRMemId);
    void pushCmdSetWMemIndex(SetWMemIndexCommand* cmdSetWMemId);
    void pushCmdSetRImdAttr(SetRImdAttrCommand* cmdSetRImdAttr);
    void pushCmdSetRRchAttr(SetRRchAttrCommand* cmdSetRRchAttr);
    void pushCmdSetWRchAttr(SetWRchAttrCommand* cmdSetWRchAttr);
    void pushCmdSetRLfsrAttr(SetRLfsrAttrCommand* cmdSetRLfsrAttr);
    void pushCmdSetWLfsrAttr(SetWLfsrAttrCommand* cmdSetWLfsrAttr);
    void pushCmdSetBcuAttr(SetBcuAttrCommand* cmdSetBcuAttr);
    void pushCmdSetRcuAttr(SetRcuAttrCommand* cmdSetRcuAttr);
    void pushCmdSetBfuAttr(SetBfuAttrCommand* cmdSetBfuAttr);
    void pushCmdSetCustomPEAttr(SetCustomPEAttrCommand *cmdSetCustomPEAttr);
    void pushCmdSetSboxAttr(SetSboxAttrCommand* cmdSetSboxAttr);
    void pushCmdSetBenesAttr(SetBenesAttrCommand* cmdSetBenesAttr);
    void pushCmdItemRectChanged(ItemRectChangedCommand* cmdChangeItemRect);
    void beginPushMacro(QString str);
    void endPushMacro();

//    void pushCmdSetPortCon(SetPortConnectCommand* cmdSetPortCon);

    void actionUndo();
    void actionRedo();
    void checkUndoItemIsShownItem(BaseItem* baseItem);

    void setBcuIndex(ModuleBcu* bcuItem,int index);

    void addWireToList(ElementWire* wire);
    void removeWireFromList(ElementWire* wire);

    void insertBcuToList(int index,ModuleBcu* bcu);
    void removeBcuFromList(ModuleBcu* bcu);

    void deleteSelectedItems();

    void clearUndoStack();

    void copySelectedItems(bool hasWire);
    void cutSelectedItems();
    void pasteSelectedItems();

    ModuleBcu *pasteNewBcu(ModuleBcu &copyBcu, QPointF posAtThis);
    void saveCmdModified();
//    QPoint getGridViewStep() { return gridStepPoint;}
//    void setGridViewStep(QPoint step);
//    bool isGridViewHidden() { return isGridHidden;}
//    void setGridViewVisable(bool enabled);

    bool isOkChangePortNum();

    void exportSvg(const QString& fileName);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE;

    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    void drawForeground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;

    void slotLayerBottom(BaseItem* itemSelected);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

signals:
    void viewDisplayChanged();
    void bcuFullStatesChanged(bool isFulled);
    //add signal
    void addNewBcuSuccess(ModuleBcu*);
    void addNewRcuSuccess(ModuleRcu*);
    void addNewBfuSuccess(ElementBfu*);
    void addNewSboxSuccess(ElementSbox*);
    void addNewBenesSuccess(ElementBenes*);
    void addNewCustomPESuccess(ElementCustomPE*);
    void addNewReadMemSuccess(int bcuIndex,int rcuIndex,ModuleReadMem*);
    void addNewWriteMemSuccess(int bcuIndex,int rcuIndex,ModuleWriteMem*);
    void addNewWireSuccess(ElementWire*);
    void statusChanged(rca_space::RcaSceneStates);
    //delete signal
    void deleteBcuSuccessed(ModuleBcu *deletedBcu);
    void deleteRcuSuccess(ModuleRcu*);
    void deleteBfuSuccess(ElementBfu*);
    void deleteCustomPESuccess(ElementCustomPE*);
    void deleteSboxSuccess(ElementSbox*);
    void deleteBenesSuccess(ElementBenes*);
    void deleteReadMemSuccess(int bcuIndex,int rcuIndex,ModuleReadMem*);
    void deleteWriteMemSuccess(int bcuIndex,int rcuIndex,ModuleWriteMem*);
    void deleteReadImdSuccess(int bcuIndex,int rcuIndex,ModuleReadImd*);
    void deleteReadRchSuccess(int bcuIndex,int rcuIndex,ModuleReadRch*);
    void deleteReadLfsrSuccess(int bcuIndex,int rcuIndex,ModuleReadLfsr*);
    void deleteWriteRchSuccess(int bcuIndex,int rcuIndex,ModuleWriteRch*);
    void deleteWriteLfsrSuccess(int bcuIndex,int rcuIndex,ModuleWriteLfsr*);
    void deleteWireSuccess(ElementWire*);
    void deletedItemBakSuccess(bool);

    void sendWindowToModified(bool modified);
    void itemBeDoubleClicked(BaseItem* baseItem);
    void shownItemBeUndo(BaseItem* baseItem);
//    void alignEnableChanged(bool);
    void selectedItemsAttrChanged(rca_space::SelectedItemsAttr);
//    void gridHiddenChanged(bool isHidden);
//    void gridStepPointChanged(QPoint step);


private slots:
    void handleBcuBeDeleted(BaseItem *deletedItem);

private:
    QHash<int, SCustomPEConfig> *m_customPEConfigHash = nullptr;
    int bcuMaxCount;
//    QList<int> bcuExistList;
    bool isBcuFulled;
    AddWireCommand* currentAddWireCmd;
    ElementWire* currentPaintingWire;
    ElementPort* currentMouseInPort;
    QList<ElementWire*> allWireList;
    CProjectResolver *m_projectResolver;

    SelectedItemsAttr selectedAttr;

//    bool isAlignEnabled;

    //select align
    ModuleUnitId whatItemAlign;
    ModuleUnitId whatItemCopy;
    QList<ModuleBcu*> selectedBcuList;
//    ModuleBcu* selectedOneBcu;
    QList<ModuleRcu*> selectedRcuList;
//    ModuleRcu* selectedOneRcu;
    QList<BaseArithUnit*> selectedArithList;
    //select align
    RcaSceneStates sceneState;

    QUndoStack *undoStack;
    ItemCopyPaste* funCopyPaste;
    bool isDebugMode;
//    QAction *undoAction;
//    QAction *redoAction;

//    QPoint gridStepPoint;
//    bool isGridHidden;
//    RcaGridItem* gridItem;
    EBcuComposing bcuCom = BcuVCom;
public:

    QList<ModuleBcu*> bcuExistList;
    bool isAutoSetItemPos;
    CXmlResolver *xmlInView;
    bool isAllWireShow;
    BaseItem* doubleClickedItemBak;
    QBrush wirePenBrush;
    qreal wirePenWidth;
    QPointF curMouseScenePos;
    bool isModified;
    CItemAttributeSetting *curItemSetting;

    CItemAttributeSetting *getCurItemSetting() const;
    CPropertyBrowser *curPropertyBrowser;
    CPropertyBrowser *getCurPropertyBrowser() const;


    bool getIsDebugMode() const;
    void setIsDebugMode(bool value);
    QHash<int, SCustomPEConfig> *getCustomPEConfigHash() const;

private:
    bool addNewBcu(QPointF posAtScene);
    bool autoMakeNewBcu(int bcuIndex);
    bool addNewWire(QPointF posAtScene, ElementPort* inSingal);
    void clearAllSelectedStatus();
    bool alignSelectedBcu(ItemsAlignPosAttr alignAttr);
    bool alignSelectedRcu(ItemsAlignPosAttr alignAttr);
    bool alignSelectedArith(ItemsAlignPosAttr alignAttr);
    bool equiSelectedBcu(bool isHorEqui);
    bool equiSelectedRcu(bool isHorEqui);
    bool equiSelectedArith(bool isHorEqui);
    void resetSelectedAttr();
    void paintScene(QPainter* painter);
//    void setGridBackGround();

};

#endif // RCAGRAPHSCENE_H
