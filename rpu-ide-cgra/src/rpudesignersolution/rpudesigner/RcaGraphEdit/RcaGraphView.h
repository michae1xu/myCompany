#ifndef RCAGRAPHVIEW_H
#define RCAGRAPHVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QAction>

#include <QPrinter>
#include <QPrintDialog>
#include <QtPrintSupport>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#endif
#include "export_typedef.h"

QT_BEGIN_NAMESPACE
class QPrinter;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class CProjectResolver;

#include "RcaGraphDef.h"
#include "fileresolver/cxmlresolver.h"
#include "RcaGraphScene.h"
#include "ModuleBcu.h"
#include "RcaScaleView.h"
#include "RcaGridItem.h"
#include "datafile.h"

using namespace rca_space;

class RcaGraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RcaGraphView(QHash<int, SCustomPEConfig> *customPEConfigHash, QWidget *parent = nullptr, CProjectResolver *projectResolver = nullptr);
    ~RcaGraphView();

    QAction* getAction() { return m_action; }
    void newFile(const QString &logFile = QString());
    bool open(QString fileName = 0);
    void setCurrentFile(const QString &fileName);
    QString getCurFile() { return m_curFile;}
    CXmlResolver *getXml();
    bool getShowAllWire();
    bool getIsProject();
    void setIsProject(bool isProject);
    bool getIsModified();
    bool getIsUntitled();
    bool saveFile();
    bool saveFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool okToContinue();
    void changeStatus(RcaSceneStates state);
    RcaSceneStates getStatus();
    int getCurrentScalePercent() { return m_currentScalePercent;}
    RcaScaleView* getRcaScaleView();
    void autoAddBcu();
    void autoAddNewRcuCfg();
    void autoSetPosOfAllItem(EBcuComposing bcuCom);
    void showAllWire();
    void hideAllWire();
    void activeViewChanged();
    void setProject(CProjectResolver *projectResolver)
    {m_rcaScene->setProject(projectResolver);}
    CProjectResolver *getProject(){return m_rcaScene->getProject();}
    void windowFocusedInvalid();
    void projectChanged();
    void showCurrentItemSetting();
    QString setBaseItemAttr(BaseItem *baseItem);
    void restoreViewToNormal();
    int zoomOneValue(int value);
    void zoomIn();
    void zoomOut();
    void showSecenAllItem();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void deleteSelectedItems();
    void selectAllItems();
    bool alignSelectedItems(ItemsAlignPosAttr alignAttr);
    bool equiSelectedItems(bool isHorEqui);
    SelectedItemsAttr getCurrentSelectedAttr();

    QPoint getGridViewStep(){ return m_gridStepPoint;}
    void setGridViewStep(QPoint step);
    bool isGridViewHidden(){ return m_isGridHidden;}
    void setGridViewVisable(bool enabled);
    void exportSvg(const QString& fileName);

    void actionUndo();
    void actionRedo();

    QBrush getWirePenBrush();
    qreal getWirePenWidth();
    bool setWirePenBrush(QBrush brush);
    bool setWirePenWidth(qreal width);
    void copySelectedItems(bool hasWire);
    void cutSelectedItems();
    void pasteSelectedItems();
    void printPreview();
    void printView();
    QStringList getSelectedItemsInfo();
    CItemAttributeSetting *getCurItemSetting();
    CPropertyBrowser *getCurPropertyBrowser();

    bool getIsDebugMode();


signals:
    void RcaGraphViewClose(QString fileName);
    void activeWindowChanged(); //激活的subwindow改变
    void setStatus(QString);
    void statusChanged(rca_space::RcaSceneStates);
    void viewItemBeDoubleClicked(BaseItem* baseItem);
    void shownItemBeUndo(BaseItem* baseItem);
    void displayedItemBeDelected(bool);
    void mousePosChanged(QPoint pos);
    void percentChanged(int percent);
//    void alignEnableChanged(bool);
    void selectedItemsAttrChanged(rca_space::SelectedItemsAttr);
    void dropXml(QStringList xmlList, QStringList unSupportList);
    void gridHiddenChanged(bool isHidden);
    void gridStepPointChanged(QPoint step);
    void displaySelectedItemsWave(QStringList strList);

    void sendChangedElementInfo(QString str,QStringList outputList);
    void addBreakPoint(const rpu_esl_model::SBreak &);
    void removeBreakPoint(const rpu_esl_model::SBreak &);
public slots:
    void setCenterOn(QPointF posOnScene);
    void handleAddBcuSuccess(ModuleBcu* newBcu);
    void handleAddRcuSuccess(ModuleRcu* newRcu);
    void handleAddBfuSuccess(ElementBfu* newBfu);
    void handleAddSboxSuccess(ElementSbox* newSbox);
    void handleAddBenesSuccess(ElementBenes* newBenes);
    void handleAddReadMemSuccess(int bcuIndex,int rcuIndex,ModuleReadMem* newReadMem);
    void handleAddWriteMemSuccess(int bcuIndex, int rcuIndex, ModuleWriteMem* newWriteMem);
    void handleAddWireSuccess(ElementWire* newWire);

    void handleBcuBeDeleted(ModuleBcu* deletedBcu);
    void handleRcuBeDeleted(ModuleRcu* deletedRcu);
    void handleBfuBeDeleted(ElementBfu* deletedBfu);
    void handleSboxBeDeleted(ElementSbox* deletedSbox);
    void handleBenesBeDeleted(ElementBenes* deletedBenes);
    void handleReadMemBeDeleted(int bcuIndex,int rcuIndex,ModuleReadMem* readMem);
    void handleWriteMemBeDeleted(int bcuIndex,int rcuIndex,ModuleWriteMem* writeMem);
    void handleWireBeDeleted(ElementWire* deleteWire);

    void setWindowToModified(bool modified);
    void resetScaleViewFit(const QList<QRectF> &region);
    void slotPaintRequested(QPrinter *printer);

    void endDebugMode();
    void startDebugMode();
    void setAllElementInfo(const vector<rpu_esl_model::SPEValue> &peValueList);
    void handleChangedElementInfo(QString str, QStringList outputList);
    void handleAddBreakPoint(rpu_esl_model::SBreak &breakHandle);
    void handleRemoveBreakPoint(rpu_esl_model::SBreak &breakHandle);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void initUI(const QString &logFile = QString());
    void saveAllItemToXml();
    void scaleView(QPoint pos, int percent);
    void setScalePercent(int percent);
    void setGridBackGround();
    void setIsDebugMode(bool value);

    ElementInfo_del* getElementInfo(const QStringList &infoList, int typeId, int bcu, int rcu, int id);
private:
    QHash<int, SCustomPEConfig> *m_customPEConfigHash = nullptr;
    QString m_curFile;
    CXmlResolver *m_xml;
    bool m_isUntitled;
    bool m_isProject;
//    bool m_isModified;
    QAction *m_action;
    RcaScaleView *m_rcaScaleView;
    RcaGraphScene *m_rcaScene;
    int m_currentScalePercent;//显示比例，初始化100，最小10，最大500
    QPoint m_gridStepPoint;
    bool m_isGridHidden;
    //    RcaGridItem* gridItem;
    QPoint m_mouseMovePos;
    bool m_moveMode = false;
    bool m_isMoved = false;
};


#endif // RCAGRAPHVIEW_H
