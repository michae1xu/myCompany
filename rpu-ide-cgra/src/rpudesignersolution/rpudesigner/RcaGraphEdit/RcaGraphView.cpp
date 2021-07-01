#include "RcaGraphView.h"
#include <QDebug>
#include <QScrollBar>
#include <QtMath>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <basepe/basepe.h>
#include <QtEndian>

#if defined(QT_PRINTSUPPORT_LIB)
#include <QPrintPreviewDialog>
#endif
#include <QProgressDialog>

//#include "RcaItemSetting/RcaSetting.h"
#include "common/cappenv.hpp"

#define SCALE_BASE_NUM  1.1
#define NORMAL_SCALE_PERCENT  100
#define MIN_SCALE_PERCENT  10
#define MAX_SCALE_PERCENT  500
#define MOVE_ACTION_SPACE  100

#include "ElementPort.h"
#define DEFAULT_KEY_MAP_ELEMENT -1
#define ITEM_TEXT_INIT          "00000000"

#ifndef LK_NO_DEBUG_OUTPUT   //rpudesigner.pro中注释后可开启日志
#define QL_DEBUG qDebug() << QString("%1 > %2 > %3 >").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).toLocal8Bit().data()
#else
#define QL_DEBUG noDebug()
#endif

RcaGraphView::RcaGraphView(QHash<int, SCustomPEConfig> *customPEConfigHash, QWidget *parent, CProjectResolver *projectResolver)
    : QGraphicsView(parent)
      , m_customPEConfigHash(customPEConfigHash)
      , m_curFile(QString())
      , m_xml(NULL)
      , m_isUntitled(false)
      , m_isProject(false)
      //    , rcaScene->isModified(false)
      , m_action(new QAction(this))
      , m_rcaScaleView(new RcaScaleView())
      , m_rcaScene(new RcaGraphScene(customPEConfigHash, this, projectResolver))
      , m_currentScalePercent(NORMAL_SCALE_PERCENT)
      //    , gridItem(NULL)
      , m_gridStepPoint(QPoint(MIN_STEP_COUNT,MIN_STEP_COUNT))
      , m_isGridHidden(false)
{
    setDragMode(QGraphicsView::RubberBandDrag);
    setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    setAcceptDrops(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    //    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_action->setCheckable(true);
    connect(m_action, SIGNAL(triggered()), this, SLOT(show()));
    connect(m_action, SIGNAL(triggered()), this, SLOT(setFocus()));
    connect(m_action, SIGNAL(triggered()), this, SIGNAL(activeWindowChanged()));
    setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);

    m_rcaScene->setSceneRect(0, 0, 8000, 20000);
    resize(1000, 1000);
    setScene(m_rcaScene);
    centerOn(0,0);
    setGridBackGround();

    //    gridItem = new RcaGridItem(rcaScene);
    //    rcaScene->addItem(gridItem);

    m_rcaScaleView->resize(300, 300);
    m_rcaScaleView->setScene(m_rcaScene);
    m_rcaScaleView->fitInView(m_rcaScene->sceneRect(),Qt::KeepAspectRatio);
    m_rcaScaleView->centerOn(0,0);
    m_rcaScaleView->setBackgroundBrush(QBrush(QColor("white")));

    //会出现滑动轨迹
    //    connect(rcaScene,SIGNAL(changed(QList<QRectF>)),this,SLOT(resetScaleViewFit(QList<QRectF>)));

    //    connect(rcaScene,SIGNAL(viewDisplayChanged()),this,SLOT(handleViewChanged()));
    connect(m_rcaScene,SIGNAL(addNewBcuSuccess(ModuleBcu*)),this,SLOT(handleAddBcuSuccess(ModuleBcu*)));
    connect(m_rcaScene,SIGNAL(addNewRcuSuccess(ModuleRcu*)),this,SLOT(handleAddRcuSuccess(ModuleRcu*)));
    connect(m_rcaScene,SIGNAL(addNewBfuSuccess(ElementBfu*)),this,SLOT(handleAddBfuSuccess(ElementBfu*)));
    connect(m_rcaScene,SIGNAL(addNewSboxSuccess(ElementSbox*)),this,SLOT(handleAddSboxSuccess(ElementSbox*)));
    connect(m_rcaScene,SIGNAL(addNewBenesSuccess(ElementBenes*)),this,SLOT(handleAddBenesSuccess(ElementBenes*)));
    connect(m_rcaScene,SIGNAL(addNewReadMemSuccess(int,int,ModuleReadMem*)),this,SLOT(handleAddReadMemSuccess(int,int,ModuleReadMem*)));
    connect(m_rcaScene,SIGNAL(addNewWriteMemSuccess(int,int,ModuleWriteMem*)),this,SLOT(handleAddWriteMemSuccess(int,int,ModuleWriteMem*)));
    connect(m_rcaScene,SIGNAL(addNewWireSuccess(ElementWire*)),this,SLOT(handleAddWireSuccess(ElementWire*)));
    connect(m_rcaScene,SIGNAL(statusChanged(rca_space::RcaSceneStates)),this,SIGNAL(statusChanged(rca_space::RcaSceneStates)));
    connect(m_rcaScene,SIGNAL(deleteBcuSuccessed(ModuleBcu*)),this,SLOT(handleBcuBeDeleted(ModuleBcu*)));
    connect(m_rcaScene,SIGNAL(deleteRcuSuccess(ModuleRcu*)),this,SLOT(handleRcuBeDeleted(ModuleRcu*)));
    connect(m_rcaScene,SIGNAL(deleteBfuSuccess(ElementBfu*)),this,SLOT(handleBfuBeDeleted(ElementBfu*)));
    connect(m_rcaScene,SIGNAL(deleteSboxSuccess(ElementSbox*)),this,SLOT(handleSboxBeDeleted(ElementSbox*)));
    connect(m_rcaScene,SIGNAL(deleteBenesSuccess(ElementBenes*)),this,SLOT(handleBenesBeDeleted(ElementBenes*)));
    connect(m_rcaScene,SIGNAL(deleteReadMemSuccess(int,int,ModuleReadMem*)),this,SLOT(handleReadMemBeDeleted(int,int,ModuleReadMem*)));
    connect(m_rcaScene,SIGNAL(deleteWriteMemSuccess(int,int,ModuleWriteMem*)),this,SLOT(handleWriteMemBeDeleted(int,int,ModuleWriteMem*)));
    connect(m_rcaScene,SIGNAL(deleteWireSuccess(ElementWire*)),this,SLOT(handleWireBeDeleted(ElementWire*)));
    connect(m_rcaScene,SIGNAL(sendWindowToModified(bool)),this,SLOT(setWindowToModified(bool)));
    connect(m_rcaScene,SIGNAL(itemBeDoubleClicked(BaseItem*)),this,SIGNAL(viewItemBeDoubleClicked(BaseItem*)));
    connect(m_rcaScene,SIGNAL(shownItemBeUndo(BaseItem*)),this,SIGNAL(shownItemBeUndo(BaseItem*)));
    connect(m_rcaScene,SIGNAL(deletedItemBakSuccess(bool)),this,SIGNAL(displayedItemBeDelected(bool)));
    //    connect(rcaScene,SIGNAL(alignEnableChanged(bool)),this,SIGNAL(alignEnableChanged(bool)));
    connect(m_rcaScene,SIGNAL(selectedItemsAttrChanged(rca_space::SelectedItemsAttr)),this,SIGNAL(selectedItemsAttrChanged(rca_space::SelectedItemsAttr)));
    //    connect(rcaScene,SIGNAL(gridHiddenChanged(bool)),this,SLOT(gridHiddenChanged(bool)));
    //    connect(rcaScene,SIGNAL(gridStepPointChanged(QPoint)),this,SLOT(gridStepPointChanged(QPoint)));

    connect(m_rcaScaleView,SIGNAL(sendMousePosChanged(QPointF)),this,SLOT(setCenterOn(QPointF)));

    //    RcaSetting::instance(rcaScene,parent);
}

RcaGraphView::~RcaGraphView()
{
    CHECK_AND_DELETE(m_action);
    CHECK_AND_DELETE(m_xml);
    CHECK_AND_DELETE(m_rcaScene);
    CHECK_AND_DELETE(m_rcaScaleView);

    QL_DEBUG<<"~RcaGraphView";
}

void RcaGraphView::initUI(const QString &logFile)
{
    if(m_xml == NULL)   return;

    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            bool isAutoResetPos = m_xml->initXmlToScene(*rcaScene);
            rcaScene->initRcuRectByBcu();

            // zhang jun add at 202000708
            if(!logFile.isEmpty()){
                autoAddBcu();
                m_xml->initLogToScene(*rcaScene, logFile);
            }

            rcaScene->initWireAndMemConPort();
            if(isAutoResetPos)
            {
                autoSetPosOfAllItem(BcuVCom);
            }
            rcaScene->hideAllWire();
            rcaScene->clearUndoStack();
        }
    }
}

void RcaGraphView::saveAllItemToXml()
{
    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            QList<ModuleBcu*> sceneBcuList = rcaScene->bcuExistList;
            QList<ModuleBcu*>::iterator bcuIter;
            for(bcuIter=sceneBcuList.begin();bcuIter!=sceneBcuList.end();bcuIter++)
            {
                if((*bcuIter))
                {
                    ModuleBcu* moduleBcu = (*bcuIter);

                    m_xml->setBcuElementAttribute(moduleBcu);

                    QList<QGraphicsItem*> bcuChildItemList = moduleBcu->childItems();
                    QList<QGraphicsItem*>::iterator bcuIterChild;
                    for(bcuIterChild=bcuChildItemList.begin();bcuIterChild!=bcuChildItemList.end();bcuIterChild++)
                    {
                        if((*bcuIterChild))
                        {
                            QGraphicsItem* bcuChildItem = (*bcuIterChild);
                            ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(bcuChildItem);
                            if(moduleRcu)
                            {
                                m_xml->setRcuElementAttribute(moduleRcu);

                                QList<QGraphicsItem*> rcuChildItemList = moduleRcu->childItems();
                                QList<QGraphicsItem*>::iterator rcuIterChild;
                                for(rcuIterChild=rcuChildItemList.begin();rcuIterChild!=rcuChildItemList.end();rcuIterChild++)
                                {
                                    if((*rcuIterChild))
                                    {
                                        QGraphicsItem* rcuChildItem = (*rcuIterChild);
                                        BaseArithUnit* arithUnit = dynamic_cast<BaseArithUnit*>(rcuChildItem);
                                        if(arithUnit)
                                        {
                                            m_xml->setArithUnitElementAttribute(arithUnit);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void RcaGraphView::scaleView(QPoint pos, int percent)
{
    QL_DEBUG << pos << percent << m_currentScalePercent;

    int changedPercent = m_currentScalePercent + percent*10;
    if(changedPercent > MAX_SCALE_PERCENT)
    {
        changedPercent = MAX_SCALE_PERCENT;
    }
    else if(changedPercent < MIN_SCALE_PERCENT)
    {
        changedPercent = MIN_SCALE_PERCENT;
    }

    //    if(((currentScalePercent > MIN_SCALE_PERCENT) && (percent < 0))
    //            || ((currentScalePercent < MAX_SCALE_PERCENT) && (percent > 0)))
    //    {
    QL_DEBUG << pos << percent << changedPercent;
    qreal factor = qreal(changedPercent)/qreal(m_currentScalePercent);
    //        currentScalePercent = changedPercent;
    setScalePercent(changedPercent);
    const QPointF posToScene = mapToScene(pos);
    scale(factor,factor);

    const QPointF posFromScenc = mapFromScene(posToScene);
    const QPointF posMove = posFromScenc - pos;
    horizontalScrollBar()->setValue(posMove.x() + horizontalScrollBar()->value());
    verticalScrollBar()->setValue(posMove.y() + verticalScrollBar()->value());
    //    }
}

void RcaGraphView::setScalePercent(int percent)
{
    m_currentScalePercent = percent;
    emit percentChanged(percent);
}

void RcaGraphView::setGridBackGround()
{
    if(m_isGridHidden)
    {
        setBackgroundBrush(Qt::transparent);
    }
    else
    {
        int posXSetp = m_gridStepPoint.x();
        int posYSetp = m_gridStepPoint.y();

        QPixmap pixmap = QPixmap(posXSetp, posYSetp);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);

        painter.setRenderHints(QPainter::Antialiasing|
                               QPainter::TextAntialiasing);

        qreal penWidth =1;
        QPen pen(Qt::lightGray,penWidth);
        painter.setPen(pen);
        painter.setBrush(Qt::lightGray);
        QPointF points[4] = {QPointF(0,0),
                             QPointF(posXSetp,0),
                             QPointF(0,posYSetp),
                             QPointF(posXSetp,posYSetp)};
        painter.drawPoints(points,4);
        setBackgroundBrush(pixmap);
    }
}

void RcaGraphView::newFile(const QString &logFile)
{
    static int documentNumber = 1;

    m_curFile = CAppEnv::getTmpDirPath() + "/" + tr(u8"untitled%1.xml").arg(documentNumber);
    setWindowTitle(CAppEnv::stpToNm(m_curFile) + "[*]");
    m_action->setText(m_curFile);

    m_xml = new CXmlResolver(m_curFile);  //xml文件解析
    m_rcaScene->xmlInView = m_xml;

    ++documentNumber;

    //用xml解析器初始化界面
    initUI(logFile);

    m_isUntitled = true;
    m_rcaScene->isModified = !logFile.isEmpty();
    setWindowModified(m_rcaScene->isModified);
}

bool RcaGraphView::open(QString fileName)
{
    QL_DEBUG << fileName;

    //log文件载入到xml zhangjun add at 20200708
    QFileInfo fileInfo(fileName);
    if(fileInfo.suffix().toLower() == "rpulog") {
        newFile(fileName);
        fileName.chop(6);
        const QString &xmlFileName = fileName + "xml";
//        if(QFileInfo(xmlFileName).exists()) { //选择覆盖
//            return true;
//        }
        this->saveFile(xmlFileName);
        return true;
    }

    setCurrentFile(fileName);

    m_xml = new CXmlResolver(m_curFile);  //xml文件解析
    m_rcaScene->xmlInView = m_xml;

    initUI();

    m_rcaScene->isModified = false;
    setWindowModified(m_rcaScene->isModified);
    return true;
}

void RcaGraphView::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    m_isUntitled = false;
    m_action->setText(CAppEnv::stpToNm(m_curFile));
    m_rcaScene->isModified = false;
    setWindowModified(m_rcaScene->isModified);
    setWindowTitle(CAppEnv::stpToNm(m_curFile) + "[*]");
}

CXmlResolver* RcaGraphView::getXml()
{
    return m_xml;
}

bool RcaGraphView::getShowAllWire()
{
    if(m_rcaScene == NULL) return false;
    return m_rcaScene->isAllWireShow;
}

bool RcaGraphView::getIsProject()
{
    return m_isProject;
}

void RcaGraphView::setIsProject(bool isProject)
{
    m_isProject = isProject;
}

bool RcaGraphView::getIsModified()
{
    return m_rcaScene->isModified;
}

bool RcaGraphView::getIsUntitled()
{
    return m_isUntitled;
}

bool RcaGraphView::saveFile()
{
    m_xml->cleanRcaDomElement();
    saveAllItemToXml();
    if (m_xml->save())
    {
        m_rcaScene->saveCmdModified();
        setCurrentFile(m_curFile);
        return true;
    }
    else
    {
        return false;
    }
}

bool RcaGraphView::saveFile(const QString &fileName)
{
    m_xml->cleanRcaDomElement();
    saveAllItemToXml();
    if (m_xml->saveAs(fileName))
    {
        m_rcaScene->saveCmdModified();
        setCurrentFile(fileName);
        return true;
    }
    else
    {
        return false;
    }
}

bool RcaGraphView::okToContinue()
{
    if (m_rcaScene->isModified) {
        //        QDialog *dlg = new QDialog(parentWidget());
        //        dlg->setModal(Qt::WindowModal);
        //        dlg->setWindowModality(
        int ret = QMessageBox::warning(parentWidget(), qApp->applicationName(),
                                       tr(u8"是否保存对 “%0” 的修改？")
                                           .arg(CAppEnv::stpToNm(m_curFile)),
                                       QMessageBox::Save | QMessageBox::Discard |
                                           QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return save();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

#define enumTostr_value(v) case v:return #v

QString enumTostr(RcaSceneStates s)
{
    switch(s)
    {
    enumTostr_value(Normal);
    enumTostr_value(Add_Bcu);
    enumTostr_value(Add_Rcu);
    enumTostr_value(Add_Bfu);
    enumTostr_value(Add_CustomPE);
    enumTostr_value(Add_Sbox);
    enumTostr_value(Add_Benes);
    enumTostr_value(Add_Wire);
    enumTostr_value(Add_Imd);
    enumTostr_value(Add_Rch);
    enumTostr_value(Add_Mem);
    enumTostr_value(Add_Lfsr);
    enumTostr_value(Painting_wire);
    }
    return QString();
}

void RcaGraphView::changeStatus(RcaSceneStates state)
{
    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene && !rcaScene->getIsDebugMode())
        {
            rcaScene->setSceneState(state);
            //            emit setStatus(trUtf8("状态:")+enumTostr(state));
        }
    }
}

RcaSceneStates RcaGraphView::getStatus()
{
    if(m_rcaScene)
    {
        return m_rcaScene->getSceneState();
    }
    return RcaSceneStates::Normal;
}

void RcaGraphView::autoAddBcu()
{
    if(m_rcaScene == NULL) return;
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    m_rcaScene->autoAddBcu();
    viewport()->update();
    qApp->restoreOverrideCursor();
}

void RcaGraphView::autoAddNewRcuCfg()
{
    if(m_rcaScene == NULL) return;
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    m_rcaScene->autoAddNewRcuCfg();
    viewport()->update();
    qApp->restoreOverrideCursor();
}

void RcaGraphView::autoSetPosOfAllItem(EBcuComposing bcuCom)
{
    if(m_rcaScene == NULL) return;
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    m_rcaScene->clearAllWire();
    m_rcaScene->autoSetPosOfChildBcu(bcuCom);
    viewport()->update();
    //    rcaScene->isModified = true;
    setWindowModified(m_rcaScene->isModified);
    qApp->restoreOverrideCursor();
}

void RcaGraphView::showAllWire()
{
    if(m_rcaScene == NULL) return;
    m_rcaScene->showAllWire();
    viewport()->update();
}

void RcaGraphView::hideAllWire()
{
    if(m_rcaScene == NULL) return;
    m_rcaScene->hideAllWire();
    viewport()->update();
}

void RcaGraphView::windowFocusedInvalid()
{
    //    RcaSetting* rcaSetting = RcaSetting::instance();
    //    if(rcaSetting)
    //    {
    //        rcaSetting->hideAll();
    //    }
}

void RcaGraphView::projectChanged()
{
    if(m_rcaScene)
    {
        m_rcaScene->setResourceMap();
    }
}

void RcaGraphView::showCurrentItemSetting()
{
    //    RcaSetting* rcaSetting = RcaSetting::instance();
    //    if(rcaSetting)
    //    {
    //        rcaSetting->showCurrentItem();
    //    }

}

RcaScaleView* RcaGraphView::getRcaScaleView()
{
    return m_rcaScaleView;
}


bool RcaGraphView::save()
{
    if (m_isUntitled) {
        return saveAs();
    } else {
        return saveFile();
    }
}

bool RcaGraphView::saveAs()
{
    //    QString str = CAppEnv::getXmlOutDirPath() + "/" + CAppEnv::stpToNm(m_curFile);
    QString fileName = QFileDialog::getSaveFileName(this, tr(u8"另存为"), ""/*str*/, tr(u8"XML 文件 (*.xml)"));
    if (fileName.isEmpty())
    {
        return true;
    }

    return saveFile(fileName);
}

void RcaGraphView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::CTRL)
    {
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        int pointY = 0;

        if (!numPixels.isNull()) {
            //            QL_DEBUG<<"numPixels " << numPixels;
            pointY = numPixels.y();
        } else if (!numDegrees.isNull()) {
            QPoint numSteps = numDegrees / 15;
            //            QL_DEBUG<<"numSteps " << numSteps;
            pointY = numSteps.y();
        }

        scaleView(event->pos(),pointY);
        //        qreal factor = qPow(SCALE_BASE_NUM, pointY);
        //        const QPointF posToScene = mapToScene(event->pos());
        //        scale(factor,factor);

        //        const QPointF posFromScenc = mapFromScene(posToScene);
        //        const QPointF posMove = posFromScenc - event->pos();
        //        horizontalScrollBar()->setValue(posMove.x() + horizontalScrollBar()->value());
        //        verticalScrollBar()->setValue(posMove.y() + verticalScrollBar()->value());

        event->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void RcaGraphView::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if (okToContinue()) {
        emit RcaGraphViewClose(m_curFile);
        event->accept();
    } else {
        event->ignore();
    }
}

void RcaGraphView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_rcaScene->getSceneState() != Painting_wire && m_moveMode) {
        qDebug() << m_rcaScene->getSceneState();
        QPoint distance = m_mouseMovePos - event->pos();
        if(distance.manhattanLength() > 5) {
            m_mouseMovePos = event->pos();
            m_isMoved = true;
            horizontalScrollBar()->setSliderPosition(horizontalScrollBar()->sliderPosition() + distance.x());
            verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() + distance.y());
        }
    } else {
        QPoint scenePos = QPointF(mapToScene(event->pos())).toPoint();
        emit mousePosChanged(scenePos);
        QGraphicsView::mouseMoveEvent(event);
    }
}

void RcaGraphView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        if(m_rcaScene->getSceneState() != Painting_wire)
        {
            m_mouseMovePos = event->pos();
            m_moveMode = true;
        }
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void RcaGraphView::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveMode = false;
    setDragMode(QGraphicsView::RubberBandDrag);
    if(m_isMoved) {
        m_isMoved = false;
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void RcaGraphView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton) {
        showSecenAllItem();
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void RcaGraphView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        if(!m_rcaScene->getIsDebugMode())
            deleteSelectedItems();
        return;
    }
    //    QL_DEBUG << event->modifiers();
    //    if(event->modifiers() == (Qt::ALT))
    if(event->modifiers() == (Qt::SHIFT | Qt::CTRL))
    {
        //        QL_DEBUG;
        if(event->key() == Qt::Key_W)
        {
            emit displaySelectedItemsWave(getSelectedItemsInfo());
        }
    }
    if(event->modifiers() == Qt::CTRL && event->key() == Qt::Key_C)
    {
        if(!m_rcaScene->getIsDebugMode())
            copySelectedItems(false);
    }
    else if(event->modifiers() == Qt::CTRL && event->key() == Qt::Key_V)
    {
        if(!m_rcaScene->getIsDebugMode())
            pasteSelectedItems();
    }
    else if(event->modifiers() == Qt::CTRL && event->key() == Qt::Key_X)
    {
        if(!m_rcaScene->getIsDebugMode())
            cutSelectedItems();
    }
    else if(event->modifiers() == Qt::SHIFT && event->key() == Qt::Key_Q)
    {
        if(m_rcaScene->getIsDebugMode())
            endDebugMode();
    }
    QGraphicsView::keyPressEvent(event);
}

void RcaGraphView::setCenterOn(QPointF posOnScene)
{
    QL_DEBUG << posOnScene << mapFromScene(posOnScene.toPoint()) << sceneRect();
    centerOn(posOnScene);
}

void RcaGraphView::handleAddBcuSuccess(ModuleBcu* newBcu)
{
    Q_UNUSED(newBcu)
    QL_DEBUG;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->setBcuElementAttribute(newBcu);
}

void RcaGraphView::handleAddRcuSuccess(ModuleRcu* newRcu)
{
    Q_UNUSED(newRcu)
    QL_DEBUG;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->setRcuElementAttribute(newRcu);
}

void RcaGraphView::handleAddBfuSuccess(ElementBfu* newBfu)
{
    Q_UNUSED(newBfu)
    QL_DEBUG;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->setBfuElementAttribute(newBfu);
}

void RcaGraphView::handleAddSboxSuccess(ElementSbox* newSbox)
{
    Q_UNUSED(newSbox)
    QL_DEBUG;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->setSboxElementAttribute(newSbox);
}

void RcaGraphView::handleAddBenesSuccess(ElementBenes* newBenes)
{
    Q_UNUSED(newBenes)
    QL_DEBUG;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->setBenesElementAttribute(newBenes);
}

void RcaGraphView::handleAddReadMemSuccess(int bcuIndex, int rcuIndex, ModuleReadMem* newReadMem)
{
    Q_UNUSED(bcuIndex);
    Q_UNUSED(rcuIndex);
    Q_UNUSED(newReadMem);

    QL_DEBUG;
    //    if(newReadMem == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);
}

void RcaGraphView::handleAddWireSuccess(ElementWire* newWire)
{
    QL_DEBUG;
    if(newWire == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

}

void RcaGraphView::handleAddWriteMemSuccess(int bcuIndex, int rcuIndex, ModuleWriteMem* newWriteMem)
{
    Q_UNUSED(bcuIndex)
    Q_UNUSED(rcuIndex)
    Q_UNUSED(newWriteMem)
    QL_DEBUG;
    //    if(newWriteMem == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->setWriteMemAttribute(bcuIndex,rcuIndex,newWriteMem);
}

void RcaGraphView::handleBcuBeDeleted(ModuleBcu *deletedBcu)
{
    QL_DEBUG;
    if(deletedBcu == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->removeBcuElement(deletedBcu->getIndexBcu());
}

void RcaGraphView::handleRcuBeDeleted(ModuleRcu* deletedRcu)
{
    QL_DEBUG;
    if(deletedRcu == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->removeRcuElement(deletedRcu->getIndexBcu(),deletedRcu->getIndexRcu());
}

void RcaGraphView::handleBfuBeDeleted(ElementBfu* deletedBfu)
{
    QL_DEBUG;
    if(deletedBfu == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->removeElement(deletedBfu->getIndexBcu(),deletedBfu->getIndexRcu(),ELEMENT_BFU,deletedBfu->getIndexInRcu());
}

void RcaGraphView::handleSboxBeDeleted(ElementSbox* deletedSbox)
{
    QL_DEBUG;
    if(deletedSbox == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->removeElement(deletedSbox->getIndexBcu(),deletedSbox->getIndexRcu(),ELEMENT_SBOX,deletedSbox->getIndexInRcu());
}

void RcaGraphView::handleBenesBeDeleted(ElementBenes* deletedBenes)
{
    QL_DEBUG;
    if(deletedBenes == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->removeElement(deletedBenes->getIndexBcu(),deletedBenes->getIndexRcu(),ELEMENT_BENES,deletedBenes->getIndexInRcu());
}

void RcaGraphView::handleReadMemBeDeleted(int bcuIndex,int rcuIndex,ModuleReadMem* readMem)
{
    Q_UNUSED(bcuIndex);
    Q_UNUSED(rcuIndex);

    QL_DEBUG;
    if(readMem == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);
}

void RcaGraphView::handleWriteMemBeDeleted(int bcuIndex,int rcuIndex,ModuleWriteMem* writeMem)
{
    Q_UNUSED(bcuIndex)
    Q_UNUSED(rcuIndex)
    Q_UNUSED(writeMem)
    QL_DEBUG;
    //    if(writeMem == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

    //    m_xml->removeRcuMemElemtnt(bcuIndex,rcuIndex,writeMem->getWriteMemIndex());
}

void RcaGraphView::handleWireBeDeleted(ElementWire *deleteWire)
{
    Q_UNUSED(deleteWire)
    QL_DEBUG;
    //    if(deleteWire == NULL) return;
    //    rcaScene->isModified = true;
    //    setWindowModified(rcaScene->isModified);

}

void RcaGraphView::setWindowToModified(bool modified)
{
    QL_DEBUG << modified;
    m_rcaScene->isModified = modified;
    setWindowModified(m_rcaScene->isModified);
}

void RcaGraphView::resetScaleViewFit(const QList<QRectF> &region)
{
    //    QL_DEBUG << region;
    Q_UNUSED(region)
    m_rcaScaleView->fitInView(m_rcaScene->itemsBoundingRect(),Qt::KeepAspectRatio);
}

void RcaGraphView::slotPaintRequested(QPrinter *printer)
{
    QPainter painter(printer);
    QRectF sceneRect = m_rcaScene->itemsBoundingRect();
    sceneRect.adjust(-20,-20,20,20);
    m_rcaScene->render(&painter,QRectF(),sceneRect/*,Qt::KeepAspectRatioByExpanding*/);
}

#define ELE_INFO_SPACE_NUM 3    //数据切割后列表数量
ElementInfo_del* parseInfoLine(QString strLine,int typeId, int bcu, int rcu, int id)
{
    QStringList textLineList = strLine.replace("]","").replace(" ","").split("[");
    ElementInfo_del *eleTemp = NULL;
    // read line info to element;

    if(textLineList.size() >= ELE_INFO_SPACE_NUM)
    {
        QStringList tempId = QString(textLineList.at(0)).split(":");//tempId[0] = BFU  (2,2,1); tempId[1] = 0 / 1;
        if(tempId.size() < 2) return nullptr;

        QStringList strListTemp = QString(tempId.at(0)).replace(")","").split("("); //strListTemp[0] = BFU; strListTemp[1] = 2,2,1;
        if(strListTemp.size() < 2) return nullptr;

        QString eleName = strListTemp.at(0);
        QStringList indexListTemp = QString(strListTemp.at(1)).split(",");
        if(indexListTemp.size() < 3) return nullptr;
        int indexBcu = QString(indexListTemp.at(0)).toInt();
        int indexRcu = QString(indexListTemp.at(1)).toInt();
        int indexId  = QString(indexListTemp.at(2)).toInt();

        int indexType = mapElementName_del.key(eleName,DEFAULT_KEY_MAP_ELEMENT);
        if (indexType == DEFAULT_KEY_MAP_ELEMENT) return nullptr;

        QStringList inputListTemp = QString(textLineList.at(1)).split(",");
        QStringList outputListTemp = QString(textLineList.at(2)).split(",");

        if(indexType != typeId || indexBcu != bcu || indexRcu != rcu || indexId != id) return nullptr;

        // 算子还没初始化
        eleTemp = new ElementInfo_del();
        eleTemp->elementId = tempId.at(0);
        eleTemp->typeId = indexType;
        eleTemp->bcu = indexBcu;
        eleTemp->rcu = indexRcu;
        eleTemp->id = indexId;
        eleTemp->isEnabled = QString(tempId.at(1)).toInt();
        eleTemp->isEnableChanged = eleTemp->isEnabled;

        eleTemp->inputList = inputListTemp;
        eleTemp->outputList = outputListTemp;
        return eleTemp;
    }
}

ElementInfo_del* RcaGraphView::getElementInfo(const QStringList &infoList, int typeId, int bcu, int rcu, int id)
{
    if(infoList.size() <= 0) return NULL;

    ElementInfo_del *eleTemp;
    for(int index=0; index<infoList.size();index++)
    {
        QString textLine = infoList.at(index);
        eleTemp = parseInfoLine(textLine,typeId,bcu,rcu,id);
        if(eleTemp)
            return eleTemp;
    }

    QString strInfo;
    if(typeId == DEBUG_BFU_del)
        strInfo += "BFU (0,"+QString::number(rcu)+","+QString::number(id)+"):0 [00000000,00000000,00000000] [00000000,00000000] ";
    else if(typeId == DEBUG_SBOX_del)
        strInfo += "SBOX (0,"+QString::number(rcu)+","+QString::number(id)+"):0 [00000000] [00000000,00000000,00000000,00000000] ";
    else if(typeId == DEBUG_BENES_del)
        strInfo += "BENES (0,"+QString::number(rcu)+","+QString::number(id)+"):0 [00000000,00000000,00000000,00000000] [00000000,00000000,00000000,00000000] ";
    eleTemp = parseInfoLine(strInfo,typeId,bcu,rcu,id);

    return eleTemp;
}

QString printUint32To16Str(const uint32_t i32)
{
    // zhangjun modified at 20210625
    auto _temp = qToBigEndian(i32);
    return QString(QByteArray((const char *)&_temp, sizeof(uint32_t)).toHex());

//    const auto& _temp = QBitArray::fromBits((const char*)(&i32), 32);
//    QString strTemp;QBitArray bitTemp(4,false);
//    for (int i = 0; i < _temp.size();)
//    {
//        for (int j = 0; j < 4; ++j)
//            bitTemp.setBit(j,_temp.at(i+j));
//        strTemp += BinToHex(bitToHexStr(bitTemp));
//        i = i + 4;
//    }
//    return strTemp;
}

QString printPeDebugValue(const QString &strPeName, const rpu_esl_model::SPEValue& pe,
                          int iInputNum, int iOutputNum)
{
    int iPeIndex = pe.index.peIndex;
    if(strPeName=="SBOX")
        iPeIndex -= 8;
    else if(strPeName=="BENES")
        iPeIndex -= 12;
    QString strInfo;
    strInfo += strPeName + " ("+
            QString::number(pe.index.bcuIndex)+","+
            QString::number(pe.index.rcuIndex)+","+
            QString::number(iPeIndex)+"):0 [";
    for(int j = 0; j < iInputNum; j++)
    {
        if(j!=iInputNum-1)
            strInfo += printUint32To16Str(pe.inPort[j]) + ",";
        else
            strInfo += printUint32To16Str(pe.inPort[j]) + "] [";
    }
    for(int j = 0; j < iOutputNum; j++)
    {
        if(j!=iOutputNum-1)
            strInfo += printUint32To16Str(pe.outPort[j]) + ",";
        else
            strInfo += printUint32To16Str(pe.outPort[j]) + "] ";
    }
    return strInfo;
}

void RcaGraphView::setAllElementInfo(const vector<rpu_esl_model::SPEValue> &peValueList)
{
    QStringList infoList;
    QString strInfo;
    for (int i = 0; i<peValueList.size();i++)
    {
        auto pe = peValueList.at(i);
        if(pe.index.peIndex < 8) // zhangjun fixed at 20210610: vector内数据不一定按照BFU-SBOX-BENES的顺序，改用peIndex
            strInfo = printPeDebugValue("BFU",pe,3,2);
        else if(pe.index.peIndex < 12)
            strInfo = printPeDebugValue("SBOX",pe,1,4);
        else
            strInfo = printPeDebugValue("BENES",pe,4,4);
        infoList.append(strInfo);
    }

    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            QList<ModuleBcu*> sceneBcuList = rcaScene->bcuExistList;
            QList<ModuleBcu*>::iterator bcuIter;
            for(bcuIter=sceneBcuList.begin();bcuIter!=sceneBcuList.end();bcuIter++)
            {
                if((*bcuIter))
                {
                    ModuleBcu* moduleBcu = (*bcuIter);
                    QList<QGraphicsItem*> bcuChildItemList = moduleBcu->childItems();
                    QList<QGraphicsItem*>::iterator bcuIterChild;
                    for(bcuIterChild=bcuChildItemList.begin();bcuIterChild!=bcuChildItemList.end();bcuIterChild++)
                    {
                        if((*bcuIterChild))
                        {
                            QGraphicsItem* bcuChildItem = (*bcuIterChild);
                            ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(bcuChildItem);
                            if(moduleRcu)
                            {
                                QList<QGraphicsItem*> rcuChildItemList = moduleRcu->childItems();
                                QList<QGraphicsItem*>::iterator rcuIterChild;
                                for(rcuIterChild=rcuChildItemList.begin();rcuIterChild!=rcuChildItemList.end();rcuIterChild++)
                                {
                                    if((*rcuIterChild))
                                    {
                                        QGraphicsItem* rcuChildItem = (*rcuIterChild);
                                        BaseArithUnit* arithUnit = dynamic_cast<BaseArithUnit*>(rcuChildItem);
                                        if(arithUnit)
                                        {
                                            int indexBcu = arithUnit->getIndexBcu();
                                            int indexRcu = arithUnit->getIndexRcu();
                                            int indexId = arithUnit->getIndexInRcu();

                                            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(arithUnit);
                                            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(arithUnit);
                                            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(arithUnit);

                                            if(elementBfu)
                                            {
                                                elementBfu->setElementInfo(getElementInfo(infoList,DEBUG_BFU_del,indexBcu,indexRcu,indexId));
                                            }
                                            else if(elementSbox)
                                            {
                                                elementSbox->setElementInfo(getElementInfo(infoList,DEBUG_SBOX_del,indexBcu,indexRcu,indexId));
                                            }
                                            else if(elementBenes)
                                            {
                                                elementBenes->setElementInfo(getElementInfo(infoList,DEBUG_BENES_del,indexBcu,indexRcu,indexId));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    viewport()->update();
}

void RcaGraphView::handleChangedElementInfo(QString str, QStringList outputList)
{
    QL_DEBUG << str << outputList;
    emit sendChangedElementInfo(str,outputList);
}

void RcaGraphView::handleAddBreakPoint(rpu_esl_model::SBreak &breakHandle)
{
    QL_DEBUG << QString::fromStdString(breakHandle.index.getString());
    emit addBreakPoint(breakHandle);
}

void RcaGraphView::handleRemoveBreakPoint(rpu_esl_model::SBreak &breakHandle)
{
    QL_DEBUG << QString::fromStdString(breakHandle.index.getString());
    emit removeBreakPoint(breakHandle);
}

void RcaGraphView::endDebugMode()
{
    //禁用恢复：算子等移动
    //断开信号和槽：断点、data
    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            QList<ModuleBcu*> sceneBcuList = rcaScene->bcuExistList;
            QList<ModuleBcu*>::iterator bcuIter;
            for(bcuIter=sceneBcuList.begin();bcuIter!=sceneBcuList.end();bcuIter++)
            {
                if((*bcuIter))
                {
                    ModuleBcu* moduleBcu = (*bcuIter);
                    moduleBcu->setFlag(QGraphicsItem::ItemIsMovable);
                    QList<QGraphicsItem*> bcuChildItemList = moduleBcu->childItems();
                    QList<QGraphicsItem*>::iterator bcuIterChild;
                    for(bcuIterChild=bcuChildItemList.begin();bcuIterChild!=bcuChildItemList.end();bcuIterChild++)
                    {
                        if((*bcuIterChild))
                        {
                            QGraphicsItem* bcuChildItem = (*bcuIterChild);
                            ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(bcuChildItem);
                            moduleRcu->setFlag(QGraphicsItem::ItemIsMovable);

                            if(moduleRcu)
                            {
                                QList<QGraphicsItem*> rcuChildItemList = moduleRcu->childItems();
                                QList<QGraphicsItem*>::iterator rcuIterChild;
                                for(rcuIterChild=rcuChildItemList.begin();rcuIterChild!=rcuChildItemList.end();rcuIterChild++)
                                {
                                    if((*rcuIterChild))
                                    {
                                        QGraphicsItem* rcuChildItem = (*rcuIterChild);
                                        BaseArithUnit* arithUnit = dynamic_cast<BaseArithUnit*>(rcuChildItem);
                                        if(arithUnit)
                                        {
                                            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(arithUnit);
                                            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(arithUnit);
                                            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(arithUnit);

                                            if(elementBfu)
                                            {
                                                elementBfu->setFlag(QGraphicsItem::ItemIsMovable);

                                                disconnect(elementBfu->inPortA,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->inPortB,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->inPortT,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->outPortX,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->outPortY,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));

                                                disconnect(elementBfu->inPortA,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->inPortB,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->inPortT,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->outPortX,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBfu->outPortY,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));

                                                disconnect(elementBfu,SIGNAL(sendChangedElementInfo(QString,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList,QStringList)));
                                            }
                                            else if(elementSbox)
                                            {
                                                elementSbox->setFlag(QGraphicsItem::ItemIsMovable);

                                                disconnect(elementSbox->inPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort1,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort2,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort3,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));

                                                disconnect(elementSbox->inPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort1,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort2,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementSbox->outPort3,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));

                                                disconnect(elementSbox,SIGNAL(sendChangedElementInfo(QString,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList,QStringList)));
                                            }
                                            else if(elementBenes)
                                            {
                                                elementBenes->setFlag(QGraphicsItem::ItemIsMovable);

                                                disconnect(elementBenes->inPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->inPort1,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->inPort2,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->inPort3,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort1,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort2,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort3,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));

                                                disconnect(elementBenes->inPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->inPort1,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->inPort2,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->inPort3,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort1,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort2,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                disconnect(elementBenes->outPort3,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));

                                                disconnect(elementBenes,SIGNAL(sendChangedElementInfo(QString,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList,QStringList)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    setIsDebugMode(false);
    viewport()->update();
}

void RcaGraphView::startDebugMode()
{
    //禁用：算子等移动
    //启用信号和槽：断点、data

    if(scene())
    {
        RcaGraphScene* rcaScene = dynamic_cast<RcaGraphScene*>(scene());
        if(rcaScene)
        {
            QList<ModuleBcu*> sceneBcuList = rcaScene->bcuExistList;
            QList<ModuleBcu*>::iterator bcuIter;
            for(bcuIter=sceneBcuList.begin();bcuIter!=sceneBcuList.end();bcuIter++)
            {
                if((*bcuIter))
                {
                    ModuleBcu* moduleBcu = (*bcuIter);
                    moduleBcu->setFlag(QGraphicsItem::ItemIsMovable,false);
                    QList<QGraphicsItem*> bcuChildItemList = moduleBcu->childItems();
                    QList<QGraphicsItem*>::iterator bcuIterChild;
                    for(bcuIterChild=bcuChildItemList.begin();bcuIterChild!=bcuChildItemList.end();bcuIterChild++)
                    {
                        if((*bcuIterChild))
                        {
                            QGraphicsItem* bcuChildItem = (*bcuIterChild);
                            ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(bcuChildItem);
                            moduleRcu->setFlag(QGraphicsItem::ItemIsMovable,false);

                            if(moduleRcu)
                            {
                                QList<QGraphicsItem*> rcuChildItemList = moduleRcu->childItems();
                                QList<QGraphicsItem*>::iterator rcuIterChild;
                                for(rcuIterChild=rcuChildItemList.begin();rcuIterChild!=rcuChildItemList.end();rcuIterChild++)
                                {
                                    if((*rcuIterChild))
                                    {
                                        QGraphicsItem* rcuChildItem = (*rcuIterChild);
                                        BaseArithUnit* arithUnit = dynamic_cast<BaseArithUnit*>(rcuChildItem);
                                        if(arithUnit)
                                        {
                                            ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(arithUnit);
                                            ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(arithUnit);
                                            ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(arithUnit);

                                            if(elementBfu)
                                            {
                                                elementBfu->setFlag(QGraphicsItem::ItemIsMovable,false);

                                                connect(elementBfu->inPortA,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->inPortB,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->inPortT,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->outPortX,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->outPortY,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));

                                                connect(elementBfu->inPortA,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->inPortB,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->inPortT,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->outPortX,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBfu->outPortY,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));

                                                connect(elementBfu,SIGNAL(sendChangedElementInfo(QString,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList)));
                                            }
                                            else if(elementSbox)
                                            {
                                                elementSbox->setFlag(QGraphicsItem::ItemIsMovable,false);

                                                connect(elementSbox->inPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort1,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort2,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort3,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));

                                                connect(elementSbox->inPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort1,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort2,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementSbox->outPort3,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));

                                                connect(elementSbox,SIGNAL(sendChangedElementInfo(QString,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList)));
                                            }
                                            else if(elementBenes)
                                            {
                                                elementBenes->setFlag(QGraphicsItem::ItemIsMovable,false);

                                                connect(elementBenes->inPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->inPort1,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->inPort2,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->inPort3,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort0,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort1,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort2,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort3,SIGNAL(addBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleAddBreakPoint(rpu_esl_model::SBreak&)));

                                                connect(elementBenes->inPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->inPort1,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->inPort2,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->inPort3,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort0,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort1,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort2,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));
                                                connect(elementBenes->outPort3,SIGNAL(removeBreakPoint(rpu_esl_model::SBreak&)),this,SLOT(handleRemoveBreakPoint(rpu_esl_model::SBreak&)));

                                                connect(elementBenes,SIGNAL(sendChangedElementInfo(QString,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    emit statusChanged(Normal);
    setIsDebugMode(true);
}


QString RcaGraphView::setBaseItemAttr(BaseItem *baseItem)
{
    if(baseItem == NULL) return QString(tr(u8"Item无效"));
    if(m_rcaScene == NULL) return QString(tr(u8"没有场景"));

    return m_rcaScene->setBaseItemAttr(baseItem);
}

void RcaGraphView::restoreViewToNormal()
{
    qreal factor = qreal(NORMAL_SCALE_PERCENT)/qreal(m_currentScalePercent);
    //    currentScalePercent = NORMAL_SCALE_PERCENT;
    setScalePercent(NORMAL_SCALE_PERCENT);
    scale(factor,factor);
}

int RcaGraphView::zoomOneValue(int value)
{
    if(value >= 10 && value <= 500)
    {
        qreal factor = qreal(value)/qreal(m_currentScalePercent);
        //        currentScalePercent = value;
        setScalePercent(value);
        scale(factor,factor);
    }
    return m_currentScalePercent;
}

void RcaGraphView::zoomIn()
{
    int changedPercent = m_currentScalePercent + 10;
    if(changedPercent > MAX_SCALE_PERCENT)
    {
        changedPercent = MAX_SCALE_PERCENT;
    }
    else if(changedPercent < MIN_SCALE_PERCENT)
    {
        changedPercent = MIN_SCALE_PERCENT;
    }
    qreal factor = qreal(changedPercent)/qreal(m_currentScalePercent);
    //    currentScalePercent = changedPercent;
    setScalePercent(changedPercent);
    scale(factor,factor);
}

void RcaGraphView::zoomOut()
{
    int changedPercent = m_currentScalePercent - 10;
    if(changedPercent > MAX_SCALE_PERCENT)
    {
        changedPercent = MAX_SCALE_PERCENT;
    }
    else if(changedPercent < MIN_SCALE_PERCENT)
    {
        changedPercent = MIN_SCALE_PERCENT;
    }
    qreal factor = qreal(changedPercent)/qreal(m_currentScalePercent);
    //    currentScalePercent = changedPercent;
    setScalePercent(changedPercent);
    scale(factor,factor);
}

void RcaGraphView::showSecenAllItem()
{
    if(m_rcaScene == NULL) return;

    QL_DEBUG;
    QRectF allItemRect = m_rcaScene->itemsBoundingRect();

    int margin = 2;
    QRectF viewRect = viewport()->rect().adjusted(margin, margin, -margin, -margin);

    qreal xratio = viewRect.width() / allItemRect.width();
    qreal yratio = viewRect.height() / allItemRect.height();
    xratio = yratio = qMin(xratio, yratio);

    //    currentScalePercent = xratio*100;
    setScalePercent(xratio*100);
    QL_DEBUG << m_currentScalePercent;

    fitInView(allItemRect,Qt::KeepAspectRatio);
}

void RcaGraphView::moveUp()
{
    QScrollBar *vBar = verticalScrollBar();
    vBar->setValue(vBar->value() - MOVE_ACTION_SPACE);
}

void RcaGraphView::moveDown()
{
    QScrollBar *vBar = verticalScrollBar();
    vBar->setValue(vBar->value() + MOVE_ACTION_SPACE);
}

void RcaGraphView::moveLeft()
{
    QScrollBar *hBar = horizontalScrollBar();
    hBar->setValue(hBar->value() - MOVE_ACTION_SPACE);
}

void RcaGraphView::moveRight()
{
    QScrollBar *hBar = horizontalScrollBar();
    hBar->setValue(hBar->value() + MOVE_ACTION_SPACE);
}

void RcaGraphView::deleteSelectedItems()
{
    if(m_rcaScene == NULL) return;
    QL_DEBUG;

    //    QList<QGraphicsItem *> selectedList = rcaScene->selectedItems();

    //    qDeleteAll(selectedList);
    m_rcaScene->deleteSelectedItems();

    //    QList<ElementWire *> dleWireList;
    //    dleWireList.clear();
    //    QList<QGraphicsItem *> selectedList = rcaScene->selectedItems();
    //    while (!selectedList.isEmpty())
    //    {
    //       QGraphicsItem* itemTemp = selectedList.last();
    //       BaseItem* baseTemp = dynamic_cast<BaseItem*>(itemTemp);
    //       if(baseTemp)
    //       {
    //           ModuleBcu* bcuTemp = dynamic_cast<ModuleBcu*>(baseTemp);
    //           ModuleRcu* rcuTemp = dynamic_cast<ModuleRcu*>(baseTemp);
    //           BaseArithUnit* arithUnitTemp = dynamic_cast<BaseArithUnit*>(baseTemp);
    //           ModuleReadMem* readMemTemp = dynamic_cast<ModuleReadMem*>(baseTemp);
    //           ModuleWriteMem* writeMemTemp = dynamic_cast<ModuleWriteMem*>(baseTemp);

    //           if(bcuTemp || rcuTemp || arithUnitTemp || readMemTemp || writeMemTemp)
    //           {
    //               baseTemp->setSelected(false);
    //               DeleteItemCommand* cmdDelItem = new DeleteItemCommand(rcaScene,baseTemp);
    //               rcaScene->cmdDeleteItem(cmdDelItem);

    ////               baseTemp->deleteThisItem();

    //               selectedList = rcaScene->selectedItems();
    //               continue;
    //           }
    //       }
    //       ElementWire* wireTemp = dynamic_cast<ElementWire*>(itemTemp);
    //       if(wireTemp)
    //       {
    //           wireTemp->setSelected(false);
    //           dleWireList.append(wireTemp);

    ////           delete wireTemp;
    //           selectedList = rcaScene->selectedItems();
    //           continue;
    //       }
    //       selectedList.removeLast();
    //    }
    //    if(dleWireList.count() > 0)
    //    {
    //        rcaScene->cmdWireDeleted(dleWireList);
    //    }
}

void RcaGraphView::selectAllItems()
{
    if(m_rcaScene == NULL) return;

    m_rcaScene->selectAllItems();
}

bool RcaGraphView::alignSelectedItems(ItemsAlignPosAttr alignAttr)
{
    if(m_rcaScene == NULL) return false;

    return m_rcaScene->alignSelectedItems(alignAttr);
}

bool RcaGraphView::equiSelectedItems(bool isHorEqui)
{
    if(m_rcaScene == NULL) return false;

    return m_rcaScene->equiSelectedItems(isHorEqui);
}

SelectedItemsAttr RcaGraphView::getCurrentSelectedAttr()
{
    if(m_rcaScene == NULL) return SelectedAttrNone;

    return m_rcaScene->getCurrentSelectedAttr();
}

void RcaGraphView::setGridViewStep(QPoint step)
{
    int newX = step.x();
    int newY = step.y();
    if(newX < MIN_STEP_COUNT)
    {
        newX = MIN_STEP_COUNT;
    }
    else if(newX > MAX_STEP_COUNT)
    {
        newX = MAX_STEP_COUNT;
    }

    if(newY < MIN_STEP_COUNT)
    {
        newY = MIN_STEP_COUNT;
    }
    else if(newY > MAX_STEP_COUNT)
    {
        newY = MAX_STEP_COUNT;
    }

    m_gridStepPoint = QPoint(newX,newY);
    setGridBackGround();
    emit gridStepPointChanged(m_gridStepPoint);
}

void RcaGraphView::setGridViewVisable(bool enabled)
{
    if(m_isGridHidden != !enabled)
    {
        m_isGridHidden = !enabled;
        setGridBackGround();
        emit gridHiddenChanged(m_isGridHidden);
    }
}

void RcaGraphView::exportSvg(const QString &fileName)
{
    m_rcaScene->exportSvg(fileName);
}
void RcaGraphView::actionUndo()
{
    if(m_rcaScene == NULL) return;

    m_rcaScene->actionUndo();
}

void RcaGraphView::actionRedo()
{
    if(m_rcaScene == NULL) return;

    m_rcaScene->actionRedo();
}

QBrush RcaGraphView::getWirePenBrush()
{
    if(m_rcaScene == NULL) return WIRE_UNSELECTED_BRUSH;

    return m_rcaScene->wirePenBrush;
}

qreal RcaGraphView::getWirePenWidth()
{
    if(m_rcaScene == NULL) return WIRE_DEFAULT_PEN_WIDTH;

    return m_rcaScene->wirePenWidth;
}

bool RcaGraphView::setWirePenBrush(QBrush brush)
{
    if(m_rcaScene == NULL) return false;

    m_rcaScene->wirePenBrush = brush;
    QList<QGraphicsItem *> selectedList = m_rcaScene->selectedItems();
    for(int i=0;i<selectedList.count();i++)
    {
        QGraphicsItem* itemTemp = selectedList.at(i);
        ElementWire* wireTemp = dynamic_cast<ElementWire*>(itemTemp);
        if(wireTemp)
        {
            wireTemp->setWirePenBrush(brush);
            wireTemp->update();
        }
    }
    return true;
}

bool RcaGraphView::setWirePenWidth(qreal width)
{
    if(m_rcaScene == NULL) return false;

    m_rcaScene->wirePenWidth = width;
    QList<QGraphicsItem *> selectedList = m_rcaScene->selectedItems();
    for(int i=0;i<selectedList.count();i++)
    {
        QGraphicsItem* itemTemp = selectedList.at(i);
        ElementWire* wireTemp = dynamic_cast<ElementWire*>(itemTemp);
        if(wireTemp)
        {
            wireTemp->setWirePenWidth(width);
            wireTemp->update();
        }
    }
    return true;
}

void RcaGraphView::copySelectedItems(bool hasWire)
{
    if(m_rcaScene == NULL) return;

    m_rcaScene->copySelectedItems(hasWire);
}

void RcaGraphView::cutSelectedItems()
{
    if(m_rcaScene == NULL) return;

    m_rcaScene->cutSelectedItems();
}

void RcaGraphView::pasteSelectedItems()
{
    if(m_rcaScene == NULL) return;

    m_rcaScene->pasteSelectedItems();
}

void RcaGraphView::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
            this, SLOT(slotPaintRequested(QPrinter*)));
    preview.exec();
}

void RcaGraphView::printView()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);

    connect(&dialog, SIGNAL(accepted(QPrinter*)), this, SLOT(slotPaintRequested(QPrinter*)));

    dialog.exec();
}

QStringList RcaGraphView::getSelectedItemsInfo()
{
    QStringList resList;
    if(m_rcaScene == NULL) return resList;
    foreach(QGraphicsItem * item,m_rcaScene->selectedItems())
    {
        BaseArithUnit* baseUnit = dynamic_cast<BaseArithUnit*>(item);
        if(baseUnit)
        {
            switch (baseUnit->getUnitId())
            {
            case BFU_ID:   resList.append(QString("BFU(%1,%2,%3)").arg(baseUnit->getIndexBcu())
                                   .arg(baseUnit->getIndexRcu())
                                   .arg(baseUnit->getIndexInRcu())); break;
            case SBOX_ID:  resList.append(QString("SBOX(%1,%2,%3)").arg(baseUnit->getIndexBcu())
                                   .arg(baseUnit->getIndexRcu())
                                   .arg(baseUnit->getIndexInRcu())); break;
            case BENES_ID: resList.append(QString("BENES(%1,%2,%3)").arg(baseUnit->getIndexBcu())
                                   .arg(baseUnit->getIndexRcu())
                                   .arg(baseUnit->getIndexInRcu())); break;
            default:break;
            }
        }
    }
    QL_DEBUG << resList;
    return resList;
}

void RcaGraphView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void RcaGraphView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void RcaGraphView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QStringList xmlList, unSupportList;
        QList<QUrl> urlList = mimeData->urls();
        for(int i = 0; i < urlList.count(); ++i)
        {
            QString fileName = urlList.at(i).toLocalFile();
            if(fileName.endsWith("xml", Qt::CaseInsensitive))
            {
                xmlList.append(fileName);
            }
            else
            {
                unSupportList.append(fileName);
            }
        }
        emit dropXml(xmlList, unSupportList);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

CItemAttributeSetting *RcaGraphView::getCurItemSetting()
{
    return m_rcaScene->getCurItemSetting();
}

CPropertyBrowser *RcaGraphView::getCurPropertyBrowser()
{
    return m_rcaScene->getCurPropertyBrowser();
}

bool RcaGraphView::getIsDebugMode()
{
    return m_rcaScene->getIsDebugMode();
}

void RcaGraphView::setIsDebugMode(bool value)
{
    m_rcaScene->setIsDebugMode(value);
}

