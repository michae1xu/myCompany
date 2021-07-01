/*!
 ********************************************************************
 * Copyright (C) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    mainwindow.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  ZhangJun<zhangjun@tsinghua-wx.org>
 * \date    2016-10-12
 * 修改描述：
 *
 ********************************************************************
 */
#include <QClipboard>
#include <QDialog>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QPropertyAnimation>
#include <QDesktopServices>
#include <QMdiSubWindow>
#include <QSettings>
#include <QCryptographicHash>
#include <QPluginLoader>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QFontMetrics>
//#include <QDesktopWidget>
//#include <QtConcurrent/QtConcurrent>
//#include <QFuture>
#include <QProgressBar>
#include <QComboBox>
#include <QTimer>
#include <QMimeData>
#include <QTextCodec>
#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>
#include <QNetworkProxy>
#include <QHostInfo>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "version.h"
#include "rpuwaveinterface.h"
#include "common/cappenv.hpp"
#if defined(Q_OS_WIN)
#include "windows.h"
#include "shellapi.h"
#include "Shlobj.h"
#include <QtWinExtras>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QWinJumpList>
#include <QWinJumpListCategory>
#else
#include <unistd.h>
#endif
#include "connectserver/ctcpclient.h"
#include "connectserver/cclientdialog.h"
#include "firmware/cmaker.h"
#include "form/helpform.h"
#include "form/newform.h"
#include "form/versionform.h"
#include "form/deletexmlform.h"
#include "form/newsrcform.h"
#include "form/resourcetypeform.h"
#include "form/settingform.h"
#include "form/sessionmanagerform.h"
#include "form/waittingform.h"
#include "form/updateprogressform.h"
#include "form/licenseform.h"
#include "form/performancestatisticsform.h"
#include "fileresolver/cprojectresolver.h"
#include "firmware/cchipdebug.h"
#include "firmware/cfirmwareupdatedialog.h"
#include "firmware/cotpdialog.h"
#include "fileeditor/cfileeditor.h"
#include "prosetting/cprojectsettingmodel.h"
#include "prosetting/cprojectsettingspinboxdelegate.h"
#include "prosetting/cprojectsettingcomboboxdelegate.h"
#include "projecttree/cprojecttreeitemmodel.h"
#include "projecttree/cprojecttreeitem.h"
#include "projecttree/cprojectfoldernodetreeitem.h"
#include "openedfilelist/copenedfilelistmodel.h"
#include "resourceeditor/cresourceeditortablemodel.h"
#include "resourceeditor/cbeneseditortablemodel.h"
#include "resourceeditor/cbeneseditorspinboxdelegate.h"
#include "session/csessionmanager.h"
#include "session/cdebugsession.h"
#include "uistyle/cprogressstatusbar.h"
#include "uistyle/cviewinfostatusbar.h"
#include "uistyle/cdevicestatusbar.h"
#include "uistyle/ctitlebar.h"
#include "uistyle/spinbox.h"
#include "uistyle/toolbar.h"
#include "uistyle/cnoclosewidget.h"
#include "RcaGraphView.h"
#include "citemattributesetting.h"
#include "cfgdebuglog/CfgDebugLog.h"
#include "code/ccode.h"
#include "csimulatorcontrol.h"


namespace {
constexpr qint32 g_configMagicNumber = 0x5A78CE9;
constexpr qint16 g_configVersionNumber = 1;

class SimCtrlRAII
{
public:
    explicit SimCtrlRAII(rpu_esl_model::CSimulatorControl **simCtrl = nullptr) :
        m_simCtrlP(simCtrl)
    {
        m_simCtrl = rpu_esl_model::CSimulatorControl::getInstance(CAppEnv::getCustomEtcSimulatorConfigFilePath().toStdString());
        if(m_simCtrlP)
            *m_simCtrlP = m_simCtrl;
    }
    ~SimCtrlRAII()
    {
        m_simCtrl = nullptr;
        if(m_simCtrlP)
            *m_simCtrlP = nullptr;
        rpu_esl_model::CSimulatorControl::releaseInstance();
    }

    Q_DISABLE_COPY(SimCtrlRAII)

    rpu_esl_model::CSimulatorControl *getSimCtrl() const
    {
        return m_simCtrl;
    }

private:
    rpu_esl_model::CSimulatorControl* m_simCtrl = nullptr;
    rpu_esl_model::CSimulatorControl** m_simCtrlP = nullptr;
};
}



/*!
 * MainWindow::MainWindow
 * \brief   constructor of MainWindow
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CWaitDialog::getInstance(this);
    disableUnavaliable();
    initIcon();
    initDockWidget();
    initStatusBar();
    initToolBar();
    initCmdWidget();
    initProjectSetting();
    initResourceEditer();
    initProjectTree();
    initOpenedFileList();
    initDemoList();
    initXmlEditorElement();
    readSettings();
    initSession();
    settingChanged();
    initRecentMenu();
    initWelcomeWidget();
    initSysTray();
    initRunResult();
    initXmlEditorMdiArea();
    initWaveWidget();
    initServer();
    on_stackedWidget_currentChanged(0);
    initAnimation();
}

/*!
 * MainWindow::~MainWindow
 * \brief   destructor of MainWindow
 * \author  zhangjun
 * \date    2016-10-12
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete m_rpuWave;
    delete []m_recentFileActions;
    delete []m_recentProActions;
//    m_simCtrl = nullptr;
    rpu_esl_model::CSimulatorControl::releaseInstance();
    qInfo() << tr(u8"程序关闭");
}


void MainWindow::disableUnavaliable()
{
    //暂未开通功能
    ui->action_bringToFront->setVisible(false);
    ui->action_SendToBackup->setVisible(false);
    ui->action_rotate->setVisible(false);
    ui->action_selectSame->setVisible(false);
    ui->action_find->setVisible(false);
    ui->action_flipHorizintal->setVisible(false);
    ui->action_flipVertical->setVisible(false);
    ui->action_sameHeight->setVisible(false);
    ui->action_sameSize->setVisible(false);
    ui->action_sameWidth->setVisible(false);
    ui->label_animation->setVisible(false);
#ifdef RELEASE
    ui->actiondebug->setVisible(false);
    ui->actiondebug_2->setVisible(false);
#endif
}


/*!
 * MainWindow::initIcon
 * \brief   初始化图标
 * \author  zhangjun
 * \date    2016-10-24
 */
void MainWindow::initIcon()
{
    const QString imgPth = CAppEnv::getImageDirPath() + "/128/";
    const QString cstmImgPth = CAppEnv::getImageDirPath() + "/";
    ui->action_NewFile->setIcon(QIcon(imgPth + "New-file128.png"));
    ui->action_Open->setIcon(QIcon(imgPth + "Open-file128.png"));
    ui->action_Save->setIcon(QIcon(imgPth + "Save128.png"));
    ui->action_SaveAs->setIcon(QIcon(cstmImgPth + "Saveas.png"));
    ui->action_exportSvg->setIcon(QIcon(cstmImgPth + "svg.png"));
    ui->action_print->setIcon(QIcon(imgPth + "printer-blue128.png"));
    ui->action_printPreview->setIcon(QIcon(imgPth + "Search-file128.png"));
    ui->action_printScreen->setIcon(QIcon(imgPth + "application2128.png"));
    ui->action_Quit->setIcon(QIcon(imgPth + "close128.png"));
    ui->action_ShowLine->setIcon(QIcon(imgPth + "Connect-point-tool128.png"));
    ui->action_ShowValue->setIcon(QIcon(imgPth + "Numbers128.png"));
    ui->action_About->setIcon(QIcon(imgPth + "info128.png"));
    ui->action_Run->setIcon(QIcon(imgPth + "start128.png"));
    ui->action_Next->setIcon(QIcon(imgPth + "navigate-right128.png"));
    ui->action_LastClock->setIcon(QIcon(imgPth + "continue128.png"));
    ui->action_saveWave->setIcon(QIcon(cstmImgPth + "wave.png"));
    ui->action_Help->setIcon(QIcon(imgPth + "Help-desk128.png"));
    ui->action_Bug->setIcon(QIcon(imgPth + "Message-already-read128.png"));
    ui->action_Update->setIcon(QIcon(imgPth + "upload128.png"));
    ui->action_Step->setIcon(QIcon(cstmImgPth + "debug128.png"));
    ui->action_debugWindow->setIcon(QIcon(cstmImgPth + "debug.png"));
    ui->action_Setting->setIcon(QIcon(imgPth + "wheel128.png"));
    ui->action_Cut->setIcon(QIcon(imgPth + "cut128.png"));
    ui->action_Copy->setIcon(QIcon(cstmImgPth + "copy.png"));
    ui->action_Paste->setIcon(QIcon(imgPth + "paste128.png"));
    ui->action_Undo->setIcon(QIcon(imgPth + "undo128.png"));
    ui->action_Redo->setIcon(QIcon(imgPth + "go-into128.png"));
    ui->action_selectAll->setIcon(QIcon(imgPth + "Success128.png"));
    ui->action_Tile->setIcon(QIcon(imgPth + "organization128.png"));
    ui->action_Cascade->setIcon(QIcon(imgPth + "Theme128.png"));
    ui->action_Log->setIcon(QIcon(imgPth + "Product-documentation128.png"));
    ui->action_Code->setIcon(QIcon(imgPth + "backup-restore128.png"));
    ui->action_openSolution->setIcon(QIcon(imgPth + "Inventory-maintenance128.png"));
    ui->action_newSolution->setIcon(QIcon(imgPth + "Package-Add128.png"));
    ui->action_Stop->setIcon(QIcon(imgPth + "stop-red128.png"));
    ui->action_Cmd->setIcon(QIcon(cstmImgPth + "cmd.png"));
    ui->action_Continue->setIcon(QIcon(imgPth + "hide-right128.png"));
    ui->action_PauseRun->setIcon(QIcon(imgPth + "pause128.png"));
//    ui->action_copyWithLine->setIcon(QIcon(/*cstmImgPth + "copyDeep.png"*/));
    ui->action_selectSame->setIcon(QIcon(imgPth + "options128.png"));
    ui->action_delete->setIcon(QIcon(cstmImgPth + "delete.png"));
    ui->action_find->setIcon(QIcon(imgPth + "Search-file128.png"));
    ui->action_moveLeft->setIcon(QIcon(imgPth + "back128.png"));
    ui->action_moveRight->setIcon(QIcon(imgPth + "forward128.png"));
    ui->action_moveUp->setIcon(QIcon(imgPth + "up128.png"));
    ui->action_moveDown->setIcon(QIcon(imgPth + "down128.png"));
    ui->action_zoomIn->setIcon(QIcon(imgPth + "zoom-in128.png"));
    ui->action_zoomOut->setIcon(QIcon(imgPth + "Zoom-out128.png"));
    ui->action_showAll->setIcon(QIcon(cstmImgPth + "showall.png"));
    ui->action_rotate->setIcon(QIcon(imgPth + "Counterclockwise-arrow128.png"));
    ui->action_flipHorizintal->setIcon(QIcon(cstmImgPth + "fliphorizontal.png"));
    ui->action_flipVertical->setIcon(QIcon(cstmImgPth + "flipvertical.png"));
    ui->action_autoMakeAllItem->setIcon(QIcon(imgPth + "content-tree128.png"));
    ui->action_autoSetPosOfAllItem->setIcon(QIcon(imgPth + "content-reorder128.png"));
    ui->action_align->setIcon(QIcon(imgPth + "Top-align128.png"));
    ui->action_alignTopEdge->setIcon(QIcon(imgPth + "Top-align128.png"));
    ui->action_alignBottomEdge->setIcon(QIcon(imgPth + "Bottom-align128.png"));
    ui->action_alignLeftEdge->setIcon(QIcon(imgPth + "Left-align128.png"));
    ui->action_alignRightEdge->setIcon(QIcon(imgPth + "Right-align128.png"));
    ui->action_horizintalEquidistant->setIcon(QIcon(cstmImgPth + "horizintalEquidistant.png"));
    ui->action_VerticalEquidistant->setIcon(QIcon(cstmImgPth + "verticalEquidistant.png"));
    ui->action_fullScreen->setIcon(QIcon(cstmImgPth + "full.png"));
    ui->action_welcome->setIcon(QIcon(imgPth + "home128.png"));
    ui->action_edit->setIcon(QIcon(imgPth + "Tools1-128.png"));
    ui->action_proSetting->setIcon(QIcon(cstmImgPth + "projectsetting.png"));
    ui->action_resource->setIcon(QIcon(imgPth + "addons128.png"));
    ui->action_grid->setIcon(QIcon(imgPth + "layout128.png"));
    ui->action_zoomNormal->setIcon(QIcon(cstmImgPth + "nozoom.png"));
    ui->action_analyze->setIcon(QIcon(imgPth + "Line-chart128.png"));
    ui->action_connectToDevice->setIcon(QIcon(cstmImgPth + "connected.png"));
    ui->action_disconnectToDevice->setIcon(QIcon(cstmImgPth + "disconnect.png"));
    ui->action_updateFirmware->setIcon(QIcon(imgPth + "mailbox-receive-message2128.png"));
    ui->action_maker->setIcon(QIcon(imgPth + "Edit-Male-User128.png"));
    ui->action_community->setIcon(QIcon(imgPth + "share128.png"));
    ui->action_sessionManager->setIcon(QIcon(imgPth + "Library128.png"));
}


/*!
 * MainWindow::initDockWidgetElement
 * \brief   初始化元素按钮停靠窗
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initXmlEditorElement()
{
    QStringList elementList;
    elementList << "MOUSE" << "WIRE" << "RCU" << "RCU CFG" << "BFU" << "SBOX" << "BENES" << "IMD" << "RCH" << "MEM" << "LFSR";
    int i = 0;
    for (; i < elementList.count(); ++i)
    {
        QString str = elementList.at(i);
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_element);
        item->setData(Qt::UserRole, i + Normal);
        item->setText(str);
        item->setToolTip(str);
        switch (i) {
        case 0:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/mouse.png"));
            break;
        case 1:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/line.png"));
            break;
        case 2:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/bcu.png"));
            break;
        case 3:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/rcu.png"));
            break;
        case 4:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/bfu.png"));
            break;
        case 5:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/sbox.png"));
            break;
        case 6:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/benes.png"));
            break;
        case 7:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/imd.png"));
            break;
        case 8:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/rch.png"));
            break;
        case 9:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/memory.png"));
            break;
        case 10:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/lfsr.png"));
            break;
        default:
            item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/rpu.png"));
            break;
        }
        item->setSizeHint(iconSize() * 3);
    }

    // zhangjun added at 20201026, 增加动态自定义PE
    QDir dir(CAppEnv::getEtcCustomPeDirPath());
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QFileInfoList list = dir.entryInfoList();
    for (int j = 0; j < list.size(); ++j) {
        QFileInfo fileInfo = list.at(j);

        /*解析json文件*/
        QFile file(fileInfo.absoluteFilePath());
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QString value = file.readAll();
        file.close();

        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug()<<tr("解析json文件错误！");
            continue;
        }
        QJsonObject jsonObject = document.object();
        SCustomPEConfig customPeConfig;
        customPeConfig.peName = jsonObject["peName"].toString();
        customPeConfig.iconFile = jsonObject["iconFile"].toString();
        m_customPEConfigHash.insert(i + j + Normal, customPeConfig);

        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_element);
        item->setData(Qt::UserRole, i + j + Normal);
        item->setText(customPeConfig.peName);
        item->setToolTip(customPeConfig.peName);
        item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/operator/" + customPeConfig.iconFile));
        item->setSizeHint(iconSize() * 3);

//        if(jsonObject.contains(QStringLiteral("secondName")))
//        {
//            QJsonValue jsonValueList = jsonObject.value(QStringLiteral("secondName"));
//            QJsonObject item = jsonValueList.toObject();
//            qDebug()<<"item[thirdName]=="<<item["thirdName"].toString();
//        }
//        if(jsonObject.contains(QStringLiteral("recoveryPrimaryNode")))
//        {
//            QJsonValue arrayValue = jsonObject.value(QStringLiteral("recoveryPrimaryNode"));
//            if(arrayValue.isArray())
//            {
//                QJsonArray array = arrayValue.toArray();
//                for(int i=0;i<array.size();i++)
//                {
//                    QJsonValue iconArray = array.at(i);
//                    QJsonObject icon = iconArray.toObject();
//                    QString id = icon["id"].toString();
//                    QString iconTxt = icon["iconTxt"].toString();
//                    QString iconName = icon["iconName"].toString();
//                    qDebug()<<"id="<<id<<"iconTxt="<<iconTxt<<"iconName="<<iconName;
//                }
//            }
//        }
    }

    ui->listWidget_element->setIconSize(iconSize() * 3);
}

/*!
 * MainWindow::initWelcomeWidget
 * \brief   初始化首页
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initWelcomeWidget()
{
    ui->pushButton_newFile->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/New-file128.png"));
    ui->pushButton_openFile->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Open-file128.png"));
    ui->pushButton_newPro->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Package-Add128.png"));
    ui->pushButton_openPro->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Inventory-maintenance128.png"));
    ui->pushButton_sessionManager->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Library128.png"));

    QPalette palette;
    //    palette.setBrush(QPalette::Base, QBrush(QColor(240, 240, 240)));
    palette.setColor(QPalette::Base, ui->frame_4->palette().color(QPalette::Background));
    ui->listWidget_files->setPalette(palette);
    ui->listWidget_pros->setPalette(palette);
    ui->listWidget_demos->setPalette(palette);

    ui->label_line->setFixedHeight(2);
    ui->label_line2->setFixedHeight(2);
    ui->label_line3->setFixedHeight(2);
    palette.setColor(QPalette::Window, QColor("#4DB6EA"));
    ui->label_line->setPalette(palette);
    ui->label_line2->setPalette(palette);
    ui->label_line3->setPalette(palette);
}

/*!
 * MainWindow::initCmdWidget
 * \brief   初始化CMD命令行界面
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initCmdWidget()
{
//    QImage image;
//    image.load(CAppEnv::getImageDirPath() + "/cmd.png");
//    image = image.scaled(18, 18, Qt::KeepAspectRatio);
//    ui->label_cmdLogo->setPixmap(QPixmap::fromImage(image));

    ui->pushButton_cmdFontSizeZoomIn->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/zoom-in128.png"));
    ui->pushButton_cmdFontSizeZoomOut->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Zoom-out128.png"));
    ui->pushButton_cmdFontSizeDefault->setIcon(QIcon(CAppEnv::getImageDirPath() + "/nozoom.png"));
    ui->pushButton_cmdClear->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Trash-can128.png"));
    ui->pushButton_hideDebugLogWidget->setIcon(QIcon(CAppEnv::getImageDirPath() + "/close.png"));

    QSize size = iconSize() * 2/3;
    ui->pushButton_cmdFontSizeZoomIn->setIconSize(size);
    ui->pushButton_cmdFontSizeZoomOut->setIconSize(size);
    ui->pushButton_cmdFontSizeDefault->setIconSize(size);
    ui->pushButton_cmdClear->setIconSize(size);
    ui->pushButton_hideDebugLogWidget->setIconSize(size);

    QAction *search = new QAction(ui->lineEdit_searchCmd);
    search->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Search128.png"));
    connect(search, SIGNAL(triggered(bool)), this, SLOT(searchInCmd()));
    ui->lineEdit_searchCmd->addAction(search, QLineEdit::TrailingPosition);
    connect(ui->lineEdit_searchCmd, SIGNAL(returnPressed()), this, SLOT(searchInCmd()));

    ui->widget_cmd->setVisible(false);
    ui->splitter->setChildrenCollapsible(false);

    connect(this, SIGNAL(sendMsg(const QString&)), this, SLOT(appendToTextEdit(const QString&)));
}

/*!
 * MainWindow::initProSetting()
 * \brief   初始化项目设置
 * \author  zhangjun
 * \date    2016-10-26
 */
void MainWindow::initProjectSetting()
{
    ui->tableView_projectSetting->verticalHeader()
            ->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignRight);

    ui->pushButton_proSettingSave->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Save-as128.png"));
    ui->pushButton_proSettingSelectAll->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/checklist128.png"));
    ui->pushButton_proSettingUp->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/upload128.png"));
    ui->pushButton_proSettingDown->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/download1128.png"));
    ui->pushButton_forecastClock->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/alarm128.png"));
    ui->pushButton_statistics->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Line-chart128.png"));

    ui->label_filesystem->setText(tr(u8"文件系统>").split("", QString::SkipEmptyParts).join("\n"));
    ui->label_resource->setText(tr(u8"项目资源>").split("", QString::SkipEmptyParts).join("\n"));

    CProjectSettingSpinBoxDelegate *spinBoxDelegate = new CProjectSettingSpinBoxDelegate(this);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingOnlyRsm, spinBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingRMode0, spinBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingRMode1, spinBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingGap, spinBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingBurstNum, spinBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingLoopNum, spinBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingRepeatNum, spinBoxDelegate);

    CProjectSettingComboBoxDelegate *comboBoxDelegate = new CProjectSettingComboBoxDelegate(this);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingSource, comboBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(ProjectSettingDest, comboBoxDelegate);
    ui->tableView_projectSetting->setItemDelegateForColumn(projectSettingLFSRGroup, comboBoxDelegate);
    connect(ui->widget_projectSetting, SIGNAL(save()), this, SLOT(on_pushButton_proSettingSave_clicked()));

    QFileSystemModel *dirModel = new QFileSystemModel(this);
    dirModel->setFilter(QDir::Files | QDir::Dirs |QDir::NoDotAndDotDot);
    dirModel->setReadOnly(true);
    dirModel->setRootPath(QDir::homePath());
    ui->treeView_explorer->setModel(dirModel);
    ui->treeView_explorer->setColumnWidth(0, 200);

    m_explorerChoiceContextMenu = new QMenu(this);
    QAction *action1 = new QAction(m_explorerChoiceContextMenu);
    action1->setText(tr(u8"选择"));
    connect(action1, SIGNAL(triggered(bool)), this, SLOT(fileSystemTreeActionTriggled()));
    m_explorerChoiceContextMenu->addAction(action1);
    ui->treeView_explorer->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView_explorer, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showExplorerChoiceContextMenu(QPoint)));

    m_resourceChoiceContextMenu = new QMenu(this);
    QAction *action2 = new QAction(m_resourceChoiceContextMenu);
    action2->setText(tr(u8"选择"));
    connect(action2, SIGNAL(triggered(bool)), this, SLOT(resourceListActionTriggled()));
    QAction *actionDelete = new QAction(m_resourceChoiceContextMenu);
    actionDelete->setText(tr(u8"移除资源"));
    connect(actionDelete, SIGNAL(triggered(bool)), this, SLOT(removeSrcInContextMenu()));
    m_resourceChoiceContextMenu->addAction(action2);
    m_resourceChoiceContextMenu->addAction(actionDelete);
    ui->listWidget_resource->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget_resource, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showResourceChoiceContextMenu(QPoint)));

    ui->listWidget_resource->setAcceptDrops(true);
    connect(ui->listWidget_resource, SIGNAL(dropped(QString)), this, SLOT(dropSrcToProject(QString)));

    // 去除资源关联 20210609
    ui->label_resource->setVisible(false);
    ui->label_filesystem->setVisible(false);
    ui->treeView_explorer->setVisible(false);
    ui->listWidget_resource->setVisible(false);
    ui->page_proSetting->layout()->removeItem(ui->horizontalLayout_7);
}

void MainWindow::initResourceEditer()
{
    ui->widget_benesEditor->setVisible(false);
    ui->pushButton_saveResource->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Save128.png"));
    ui->pushButton_addFifoResourceRowBefore->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/add2128.png"));
    ui->pushButton_addFifoResourceRowAfter->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/add2128.png"));
    ui->pushButton_deleteFifoResourceRow->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/delete2128.png"));
    ui->pushButton_benesEditorLoad->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/download1128.png"));
    ui->pushButton_benesEditorGenerate->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/upload128.png"));
    ui->pushButton_benesEditorReset->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/order128.png"));
    connect(ui->widget_resourceEdit, SIGNAL(save()), this, SLOT(on_pushButton_saveResource_clicked()));
}


void MainWindow::fileSystemTreeActionTriggled()
{
    const QModelIndex &index = ui->treeView_explorer->currentIndex();
    if(!index.isValid())
        return;

    if(!ui->tableView_projectSetting->currentIndex().isValid())
        return;
    int column = ui->tableView_projectSetting->currentIndex().column();
    if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
        return;

    QFileSystemModel *model = static_cast<QFileSystemModel*>(ui->treeView_explorer->model());
    if(!model)
        return;
    QFileInfo fileInfo(model->fileInfo(index));
    if(!fileInfo.isFile())
        return;
    m_projectSettingModel->setData(ui->tableView_projectSetting->currentIndex(),
                                   fileInfo.absoluteFilePath(), Qt::EditRole);
}

void MainWindow::resourceListActionTriggled()
{
    const QModelIndex &index = ui->tableView_projectSetting->currentIndex();
    if(!index.isValid())
        return;
    int column = index.column();
    if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
        return;

    QString str = ":/" + ui->listWidget_resource->currentItem()->text();
    m_projectSettingModel->setData(index, str, Qt::EditRole);
}


/*!
 * MainWindow::initProjectTree
 * \brief   初始化项目树
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initProjectTree()
{
    ui->treeView_project->installEventFilter(this);
    m_projectTreeModel = new CProjectTreeItemModel(this);
    connect(m_projectTreeModel, SIGNAL(projectSettingReset()),
            this, SLOT(resetProjectSettingTable()));
    connect(m_projectTreeModel, SIGNAL(resourceSelectReset()),
            this, SLOT(resetResourceSelectTable()));
    connect(m_projectTreeModel, SIGNAL(projectSettingChanged()),
            this, SLOT(refreshProjectSettingTable()));
    connect(m_projectTreeModel, SIGNAL(projectSettingSaved(QString)),
            this, SLOT(projectParameterSaved(QString)));
    connect(m_projectTreeModel, SIGNAL(activeProjectChanged()),
            this, SLOT(currentActiveProjectChanged()));
    connect(m_projectTreeModel, SIGNAL(setProjectResolverForXml(QString, CProjectResolver *)),
            this, SLOT(setProjectResolverToXmlEditor(QString, CProjectResolver *)));
    connect(m_projectTreeModel, SIGNAL(xmlFileRenamed(QString, QString)),
            this, SLOT(xmlFileRenamed(QString, QString)));
    connect(m_projectTreeModel, SIGNAL(srcFileRenamed(QString, QString)),
            this, SLOT(srcFileRenamed(QString, QString)));
    connect(m_projectTreeModel, SIGNAL(projectFileRenamed(QString, QString)),
            this, SLOT(projectFileRenamed(QString, QString)));
    connect(m_projectTreeModel, SIGNAL(solutionFileRenamed(QString, QString)),
            this, SLOT(solutionFileRenamed(QString,QString)));
    connect(m_projectTreeModel, SIGNAL(renameFileFinished(QModelIndex)),
            this, SLOT(renameFileFinished(QModelIndex)));
    ui->treeView_project->setModel(m_projectTreeModel);
    ui->treeView_project->setHeaderHidden(true);
    ui->treeView_project->setContextMenuPolicy(Qt::CustomContextMenu);
    initProjectContextMenu();
    connect(ui->treeView_project, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showProjectContextMenu(QPoint)));
    connect(ui->treeView_project, SIGNAL(dropProject(QStringList,QStringList)),
            this, SLOT(dropProjectToProjectTree(QStringList, QStringList)));
}

/*!
 * MainWindow::initOpenedFileList
 * \brief   初始化已打开文档列表model
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initOpenedFileList()
{
    m_openedFileListContextMenu = new QMenu(this);

    //    QAction *addToPro = new QAction;
    //    addToPro->setText(tr(u8"添加到项目..."));
    //    addToPro->setIcon(QIcon("")); //icon
    //    connect(addToPro, SIGNAL(triggered()), this, SLOT(addToProject()));

    QAction *explore = new QAction(m_openedFileListContextMenu);
    explore->setText(tr(u8"在Explorer中显示"));
    explore->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/file-explorer128.png"));
    connect(explore, SIGNAL(triggered()), this, SLOT(exploreOpenedFileTriggered()));

    QAction *close = new QAction(m_openedFileListContextMenu);
    close->setText(tr(u8"关闭"));
//    close->setIcon(QIcon(""));
    connect(close, SIGNAL(triggered()), this, SLOT(closeXmlEditorSubWindow()));

    QAction *closeAll = new QAction(m_openedFileListContextMenu);
    closeAll->setText(tr(u8"全部关闭"));
//    closeAll->setIcon(QIcon(""));
    connect(closeAll, SIGNAL(triggered()), this, SLOT(on_action_CloseAll_triggered()));

    m_openedFileListContextMenu->addAction(explore);
    m_openedFileListContextMenu->addSeparator();
    //    m_openedFileListContextMenu->addAction(addToPro);
    m_openedFileListContextMenu->addAction(close);
    m_openedFileListContextMenu->addAction(closeAll);

    ui->listView_openedFile->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView_openedFile, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showOpenedFileContextMenu(QPoint)));
    connect(ui->listView_openedFile, SIGNAL(dropXml(QStringList,QStringList)),
            this, SLOT(dropXmlToXmlEditor(QStringList, QStringList)));

    m_openedFileListModel = new COpenedFileListModel(&m_openedFiles, this);
    ui->listView_openedFile->setModel(m_openedFileListModel);
}

void MainWindow::initDemoList()
{
    const QString& demoDir = CAppEnv::getDemoProOutDirPath();

    QStringList fileList;
    CAppEnv::findFile(fileList, demoDir, "rpusln");

    ui->listWidget_demos->addItems(fileList);

    for(int i = 0; i < ui->listWidget_demos->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_demos->item(i);
        item->setToolTip(item->text());
        item->setText(QFileInfo(item->text()).baseName());
        item->setSizeHint(QSize(0, 40));
        //        item->setTextColor(Qt::white);
        item->setTextColor(QColor("#404244"));
        item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Inventory-maintenance128.png"));
    }
}

/*!
 * MainWindow::initProjectContextMenu
 * \brief   初始化项目列表上下文菜单
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initProjectContextMenu()
{
    m_solutionFolderContextMenu = new QMenu(this);
    QAction *newPro = new QAction(m_solutionFolderContextMenu);
    newPro->setText(tr(u8"新建项目..."));
    newPro->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Package-Add128.png")); //icon
    connect(newPro, SIGNAL(triggered(bool)), this, SLOT(newProject()));

    QAction *addPro = new QAction(m_solutionFolderContextMenu);
    addPro->setText(tr(u8"添加现有项目..."));
    addPro->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/product128.png")); //icon
    connect(addPro, SIGNAL(triggered(bool)), this, SLOT(addProject()));

    QAction *share = new QAction(m_solutionFolderContextMenu);
    share->setText(tr(u8"打包并共享..."));
    share->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/share128.png")); //icon
    connect(share, SIGNAL(triggered(bool)), this, SLOT(shareSolution()));

    QAction *close = new QAction(m_solutionFolderContextMenu);
    close->setText(tr(u8"关闭解决方案"));
//    close->setIcon(QIcon("")); //icon
    connect(close, SIGNAL(triggered(bool)), this, SLOT(closeSolutionTriggered()));

    QAction *closeOther = new QAction(m_solutionFolderContextMenu);
    closeOther->setText(tr(u8"关闭其他解决方案"));
//    closeOther->setIcon(QIcon("")); //icon
    connect(closeOther, SIGNAL(triggered(bool)), this, SLOT(closeOtherSolutionTriggered()));

    QAction *collapseAll = new QAction(m_solutionFolderContextMenu);
    collapseAll->setText(tr(u8"折叠全部"));
    collapseAll->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/question-type-one-correct128.png")); //icon
    connect(collapseAll, SIGNAL(triggered()), ui->treeView_project, SLOT(collapseAll()));

    m_solutionFolderContextMenu->addAction(newPro);
    m_solutionFolderContextMenu->addAction(addPro);
    m_solutionFolderContextMenu->addSeparator();
//    m_solutionFolderContextMenu->addAction(share);
    m_solutionFolderContextMenu->addSeparator();
    m_solutionFolderContextMenu->addAction(ui->action_closeAllSolution);
    m_solutionFolderContextMenu->addAction(closeOther);
    m_solutionFolderContextMenu->addAction(close);
    m_solutionFolderContextMenu->addSeparator();
    m_solutionFolderContextMenu->addAction(collapseAll);

    m_projectFolderContextMenu = new QMenu(this);
    QAction *curProject = new QAction(m_projectFolderContextMenu);
    curProject->setText(tr(u8"设为活动项目"));
//    curProject->setIcon(QIcon("")); //icon
    connect(curProject, SIGNAL(triggered()), this, SLOT(setCurProject()));

    m_runAction = new QAction(m_projectFolderContextMenu);
    m_runAction->setText(tr(u8"运行"));
    m_runAction->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/start128.png")); //icon
    connect(m_runAction, SIGNAL(triggered()), this, SLOT(runProjectTriggered()));

    m_debugAction = new QAction(m_projectFolderContextMenu);
    m_debugAction->setText(tr(u8"调试"));
    m_debugAction->setIcon(QIcon(CAppEnv::getImageDirPath() + "/debug128.png")); //icon
    connect(m_debugAction, SIGNAL(triggered()), this, SLOT(debugProjectTriggered()));

    m_lastRunResult = new QAction(m_projectFolderContextMenu);
    m_lastRunResult->setText(tr(u8"上一次运行结果"));
//    m_lastRunResult->setIcon(QIcon("")); //icon
    connect(m_lastRunResult, SIGNAL(triggered()), this, SLOT(viewLastRunResult()));

    //    QAction *code = new QAction;
    //    code->setText(tr(u8"生成配置码"));
    //    code->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/backup-restore128.png")); //icon
    //    connect(code, SIGNAL(triggered()), this, SLOT(on_action_Code_triggered()));

    QAction *newXmlToProject = new QAction(m_projectFolderContextMenu);
    newXmlToProject->setText(tr(u8"添加新文件..."));
    newXmlToProject->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Xml-tool128.png")); //icon
    connect(newXmlToProject, SIGNAL(triggered()), this, SLOT(newXmlToProject()));

    QAction *addXmlToProject = new QAction(m_projectFolderContextMenu);
    addXmlToProject->setText(tr(u8"添加现有文件..."));
    addXmlToProject->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Xml-tool128.png")); //icon
    connect(addXmlToProject, SIGNAL(triggered()), this, SLOT(addXmlToProject()));

    QAction *newSrcToProject = new QAction(m_projectFolderContextMenu);
    newSrcToProject->setText(tr(u8"添加新资源..."));
    newSrcToProject->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/puzzle128.png")); //icon
    connect(newSrcToProject, SIGNAL(triggered()), this, SLOT(newSrcToProject()));

    QAction *addSrcToProject = new QAction(m_projectFolderContextMenu);
    addSrcToProject->setText(tr(u8"添加现有资源..."));
    addSrcToProject->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/puzzle128.png")); //icon
    connect(addSrcToProject, SIGNAL(triggered()), this, SLOT(addSrcToProject()));

    QAction *removeProject = new QAction(m_projectFolderContextMenu);
    removeProject->setText(tr(u8"移除项目"));
//    removeProject->setIcon(QIcon("")); //icon
    connect(removeProject, SIGNAL(triggered()), this, SLOT(removeProject()));

    m_projectFolderContextMenu->addAction(curProject);
    m_projectFolderContextMenu->addAction(m_runAction);
    m_projectFolderContextMenu->addAction(m_debugAction);
    m_projectFolderContextMenu->addAction(ui->action_Code);
    m_projectFolderContextMenu->addSeparator();
    m_projectFolderContextMenu->addAction(m_lastRunResult);
    m_projectFolderContextMenu->addSeparator();
    m_projectFolderContextMenu->addAction(newXmlToProject);
    m_projectFolderContextMenu->addAction(addXmlToProject);
    m_projectFolderContextMenu->addAction(newSrcToProject);
    m_projectFolderContextMenu->addAction(addSrcToProject);
    m_projectFolderContextMenu->addSeparator();
    m_projectFolderContextMenu->addAction(removeProject);

    m_solutionFileContextMenu = new QMenu(this);
    m_projectFileContextMenu = new QMenu(this);
    QAction *projectSetting = new QAction(m_projectFileContextMenu);
    projectSetting->setText(tr(u8"项目参数设置"));
    projectSetting->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/item-configuration1128.png")); //icon
    connect(projectSetting, SIGNAL(triggered()), this, SLOT(projectSettingTriggered()));

    QAction *copyPath = new QAction(m_projectFileContextMenu);
    copyPath->setText(tr(u8"复制路径到剪贴板"));
    copyPath->setIcon(QIcon(/*CAppEnv::getImageDirPath() + "/128/file-explorer128.png"*/)); //icon
    connect(copyPath, SIGNAL(triggered()), this, SLOT(copyPathTriggered()));

    QAction *explore = new QAction(m_projectFileContextMenu);
    explore->setText(tr(u8"在Explorer中显示"));
    explore->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/file-explorer128.png")); //icon
    connect(explore, SIGNAL(triggered()), this, SLOT(exploreProjectFileTriggered()));

    QAction *rename = new QAction(m_projectFileContextMenu);
    rename->setText(tr(u8"重命名"));
    rename->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/rename128.png")); //icon
    connect(rename, SIGNAL(triggered()), this, SLOT(renameFileTriggered()));

    //    m_solutionFileContextMenu->addAction(editAlg);
    //    m_solutionFileContextMenu->addSeparator();
    m_solutionFileContextMenu->addAction(copyPath);
    m_solutionFileContextMenu->addAction(explore);
    m_solutionFileContextMenu->addSeparator();
    m_solutionFileContextMenu->addAction(rename);

    m_projectFileContextMenu->addAction(projectSetting);
    m_projectFileContextMenu->addSeparator();
    m_projectFileContextMenu->addAction(copyPath);
    m_projectFileContextMenu->addAction(explore);
    m_projectFileContextMenu->addSeparator();
    m_projectFileContextMenu->addAction(rename);

    m_projectXmlFolderContextMenu = new QMenu(this);
    m_projectXmlFolderContextMenu->addAction(newXmlToProject);
    m_projectXmlFolderContextMenu->addAction(addXmlToProject);

    m_projectSrcFolderContextMenu = new QMenu(this);
    m_projectSrcFolderContextMenu->addAction(newSrcToProject);
    m_projectSrcFolderContextMenu->addAction(addSrcToProject);

    m_projectXmlFileContextMenu = new QMenu(this);
    QAction *openXml = new QAction(m_projectXmlFileContextMenu);
    openXml->setText(tr(u8"打开文件"));
    openXml->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Open-file128.png")); //icon
    connect(openXml, SIGNAL(triggered()), this, SLOT(openXmlTriggered()));

    QAction *deleteXml = new QAction(m_projectXmlFileContextMenu);
    deleteXml->setShortcut(Qt::Key_Delete);
    deleteXml->setText(tr(u8"移除文件"));
    deleteXml->setIcon(QIcon("")); //icon
    connect(deleteXml, SIGNAL(triggered()), this, SLOT(deleteFileTriggered()));

    m_projectXmlFileContextMenu->addAction(openXml);
    m_projectXmlFileContextMenu->addSeparator();
    m_projectXmlFileContextMenu->addAction(copyPath);
    m_projectXmlFileContextMenu->addAction(explore);
    m_projectXmlFileContextMenu->addSeparator();
    m_projectXmlFileContextMenu->addAction(deleteXml);
    m_projectXmlFileContextMenu->addAction(rename);
}

/*!
 * MainWindow::initDockWiget()
 * \brief   初始化停靠窗
 * \author  zhangjun
 * \date    2016-10-26
 */
void MainWindow::initDockWidget()
{
    QPalette pe;
    pe.setColor(QPalette::Background, QColor("#404244"));

    CTitleBar *projectTitleBar = new CTitleBar(tr(u8"解决方案资源管理器"), ui->dockWidget_project);
    connect(projectTitleBar, SIGNAL(closeDockWidget(bool)),
            this, SLOT(on_action_solution_triggered(bool)));
    ui->dockWidget_project->setTitleBarWidget(projectTitleBar);
    ui->dockWidget_project->titleBarWidget()->setAutoFillBackground(true);
    ui->dockWidget_project->titleBarWidget()->setPalette(pe);

    CTitleBar *openedFileTitleBar = new CTitleBar(tr(u8"打开的算法文件"), ui->dockWidget_openedFile);
    connect(openedFileTitleBar, SIGNAL(closeDockWidget(bool)),
            this, SLOT(on_action_openedFile_triggered(bool)));
    ui->dockWidget_openedFile->setTitleBarWidget(openedFileTitleBar);
    ui->dockWidget_openedFile->titleBarWidget()->setAutoFillBackground(true);
    ui->dockWidget_openedFile->titleBarWidget()->setPalette(pe);

    CTitleBar *elementTitleBar = new CTitleBar(tr(u8"元素"), ui->dockWidget_element);
    connect(elementTitleBar, SIGNAL(closeDockWidget(bool)),
            this, SLOT(on_action_element_triggered(bool)));
    ui->dockWidget_element->setTitleBarWidget(elementTitleBar);
    ui->dockWidget_element->titleBarWidget()->setAutoFillBackground(true);
    ui->dockWidget_element->titleBarWidget()->setPalette(pe);

    CTitleBar *scaleTitleBar = new CTitleBar(tr(u8"缩略图"), ui->dockWidget_scaleView);
    connect(scaleTitleBar, SIGNAL(closeDockWidget(bool)),
            this, SLOT(on_action_scale_triggered(bool)));
    ui->dockWidget_scaleView->setTitleBarWidget(scaleTitleBar);
    ui->dockWidget_scaleView->titleBarWidget()->setAutoFillBackground(true);
    ui->dockWidget_scaleView->titleBarWidget()->setPalette(pe);

    CTitleBar *attributeTitleBar = new CTitleBar(tr(u8"属性设置"), ui->dockWidget_attribute);
    connect(attributeTitleBar, SIGNAL(closeDockWidget(bool)),
            ui->dockWidget_attribute, SLOT(setVisible(bool)));
    ui->dockWidget_attribute->setTitleBarWidget(attributeTitleBar);
    ui->dockWidget_attribute->titleBarWidget()->setAutoFillBackground(true);
    ui->dockWidget_attribute->titleBarWidget()->setPalette(pe);

    //    this->tabifyDockWidget(ui->dock_element, ui->dockWidget_debug);
    //    ui->dock_element->raise();
    //    ui->dock_element->show();
}

#if defined(Q_OS_WIN)
QWinTaskbarButton * MainWindow::initTaskButton()
{
    this->winId();

    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);

    QWinTaskbarButton *button = new QWinTaskbarButton(this);
    button->setWindow(this->windowHandle());
//    button->setOverlayIcon(QIcon(":/continue128.png"));
    return button;
//    return button->progress();
}
#endif

/*!
 * MainWindow::initStatusBar()
 * \brief   初始化状态栏
 * \author  zhangjun
 * \date    2016-10-26
 */
void MainWindow::initStatusBar()
{
    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor("#D2D2D2"));
    ui->statusBar->setPalette(pe);
    //    m_statusBarLabel = new QLabel(tr(u8"准备就绪"));
    //    m_statusBarLabel->setIndent(5);
    //    ui->statusBar->addPermanentWidget(m_statusBarLabel);

    m_viewInfoStatuBar = new CViewInfoStatusBar(ui->statusBar);
    connect(m_viewInfoStatuBar, SIGNAL(zoomIn()), this, SLOT(on_action_zoomIn_triggered()));
    connect(m_viewInfoStatuBar, SIGNAL(zoomOut()), this, SLOT(on_action_zoomOut_triggered()));
    connect(m_viewInfoStatuBar, SIGNAL(zoomTo(int)), this, SLOT(xmlEditorZoomTo(int)));
    connect(m_viewInfoStatuBar, SIGNAL(showAll()), this, SLOT(on_action_showAll_triggered()));
    connect(m_viewInfoStatuBar, SIGNAL(zoomNormal()), this, SLOT(on_action_zoomNormal_triggered()));
    ui->statusBar->addPermanentWidget(m_viewInfoStatuBar);

#if defined(Q_OS_WIN)
    m_progressStatusBar = new CProgressStatusBar(initTaskButton(), ui->statusBar);
#else
    m_progressStatusBar = new CProgressStatusBar(ui->statusBar);
#endif
    ui->statusBar->addPermanentWidget(m_progressStatusBar);

    m_deviceStatusBar = new CDeviceStatusBar(ui->statusBar);
    connect(m_deviceStatusBar, SIGNAL(serverButtionClicked()), this, SLOT(clientListRequest()));
    ui->statusBar->addPermanentWidget(m_deviceStatusBar);

    ui->statusBar->setSizeGripEnabled(false);
}

/*!
 * MainWindow::initToolBar()
 * \brief   初始化工具栏
 * \author  zhangjun
 * \date    2016-10-26
 */
void MainWindow::initToolBar()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

//调试工具栏
    QWidget *wgt1 = new QWidget(ui->toolBar_guide);
    QVBoxLayout *layout = new QVBoxLayout(wgt1);
    layout->addStretch(20);
    wgt1->setLayout(layout);
    QWidget *wgt2 = new QWidget(ui->toolBar_guide);
    wgt2->setFixedHeight(ui->dockWidget_project->titleBarWidget()->sizeHint().height());
    QWidget *wgt3 = new QWidget(ui->toolBar_guide);
    ui->toolBar_guide->insertWidget(ui->action_welcome, wgt2);
    ui->toolBar_guide->insertWidget(ui->action_Run, wgt1);

    QToolButton *toolButton = new QToolButton(ui->toolBar_guide);
//        toolButton->setArrowType(Qt::RightArrow);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setText(tr(u8"模式"));
    QActionGroup *actionGroup = new QActionGroup(toolButton);
    actionGroup->addAction(ui->action_simMode);
    actionGroup->addAction(ui->action_jointDebugMode);
    QMenu *menu = new QMenu(toolButton);
    menu->setTitle(tr(u8"运行模式"));
    menu->addAction(ui->action_simMode);
    menu->addAction(ui->action_jointDebugMode);
    toolButton->setMenu(menu);

    //    toolButton->setIconSize();
    //    toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->toolBar_guide->insertWidget(ui->action_Run, toolButton);
    ui->toolBar_guide->insertSeparator(ui->action_Run);
    ui->toolBar_guide->addWidget(wgt3);

    ui->action_Run->setData("run");
    ui->action_Step->setData("debug");
    connect(ui->action_Run, SIGNAL(triggered()), this, SLOT(runOrDebugTriggered()));
    connect(ui->action_Step, SIGNAL(triggered()), this, SLOT(runOrDebugTriggered()));

    //编辑工具栏
    QMenu *alignmentMenu = new QMenu(tr(u8"对齐"), this);
    foreach (QAction *action, QList<QAction*>()
             << ui->action_alignTopEdge << ui->action_alignBottomEdge
             << ui->action_alignLeftEdge << ui->action_alignRightEdge)
        alignmentMenu->addAction(action);
    ui->action_align->setMenu(alignmentMenu);
    foreach (QAction *action, QList<QAction*>()
             << ui->action_align
             << ui->action_alignTopEdge << ui->action_alignBottomEdge
             << ui->action_alignLeftEdge << ui->action_alignRightEdge)
        connect(action, SIGNAL(triggered()), this, SLOT(editAlign()));

    ui->action_align->setData(Qt::AlignTop);
    ui->action_alignTopEdge->setData(Qt::AlignTop);
    ui->action_alignBottomEdge->setData(Qt::AlignBottom);
    ui->action_alignLeftEdge->setData(Qt::AlignLeft);
    ui->action_alignRightEdge->setData(Qt::AlignRight);
}

/*!
 * MainWindow::initRecentMenu
 * \brief   初始化最近打开文档列表
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::initRecentMenu()
{
    m_recentFileActions = /*(QAction *(*)[MaxRecentFiles])*/new QAction*[m_recentFileCount];
    for (int i = 0; i < m_recentFileCount; ++i) {
        m_recentFileActions[i] = new QAction(ui->menu_F_recentFiles);
        m_recentFileActions[i]->setVisible(false);
        ui->menu_F_recentFiles->addAction(m_recentFileActions[i]);
        connect(m_recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    m_recentProActions = new QAction*[m_recentProCount];
    for (int i = 0; i < m_recentProCount; ++i) {
        m_recentProActions[i] = new QAction(ui->menu_P_recentPros);
        m_recentProActions[i]->setVisible(false);
        ui->menu_P_recentPros->addAction(m_recentProActions[i]);
        connect(m_recentProActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentProject()));
    }

    ui->menu_F_recentFiles->addSeparator();
    QAction *clearRecentFiles = new QAction(tr(u8"清除记录"), ui->menu_F_recentFiles);
    clearRecentFiles->setData("clearRecentFiles");
    connect(clearRecentFiles, SIGNAL(triggered()), this, SLOT(clearRecentMenu()));
    ui->menu_F_recentFiles->addAction(clearRecentFiles);

    ui->menu_P_recentPros->addSeparator();
    QAction *clearRecentPros = new QAction(tr(u8"清除记录"), ui->menu_P_recentPros);
    clearRecentPros->setData("clearRecentPros");
    connect(clearRecentPros, SIGNAL(triggered()), this, SLOT(clearRecentMenu()));
    ui->menu_P_recentPros->addAction(clearRecentPros);

    updateRecentFileActions();
    updateRecentProjectActions();
}

/*!
 * MainWindow::initSysTray
 * \brief   初始化系统托盘
 * \author  zhangjun
 * \date    2016-10-18
 */
void MainWindow::initSysTray()
{
    m_sysTray = new QSystemTrayIcon(this);
    m_sysTray->setIcon(QIcon(CAppEnv::getImageDirPath() + "/rpu.png"));
    m_sysTray->setToolTip(qApp->applicationName());
    m_sysTray->show();
//    m_sysTray->showMessage(qApp->applicationName(), tr(u8"欢迎使用%0！").arg(qApp->applicationName()),
//                           QSystemTrayIcon::Information, 1000);
    connect(m_sysTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(sysTrayActivated(QSystemTrayIcon::ActivationReason)));

    m_sysTrayMenu = new QMenu(this);
    QAction *actionShow = m_sysTrayMenu->addAction(tr(u8"显示主界面"));
    actionShow->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/monitor128.png"));
    connect(actionShow, SIGNAL(triggered()), this, SLOT(actionShowTriggered()));
    m_sysTrayMenu->addSeparator();
    m_sysTrayMenu->addAction(ui->action_Help);
    m_sysTrayMenu->addAction(ui->action_Log);
    m_sysTrayMenu->addAction(ui->action_Bug);
    m_sysTrayMenu->addAction(ui->action_About);
    m_sysTrayMenu->addSeparator();
    QAction *actionQuit = m_sysTrayMenu->addAction(tr(u8"退出"));
    actionQuit->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/close128.png"));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(actionQuitTriggered()));
    m_sysTrayMenu->addSeparator();
    m_sysTrayMenu->addAction(actionQuit);
    m_sysTray->setContextMenu(m_sysTrayMenu);
}

void MainWindow::initRunResult()
{
    m_runResultContextMenu = new QMenu(ui->treeWidget_runResult);
    QAction *action = new QAction(tr(u8"保存到文件"), m_runResultContextMenu);
    connect(action, SIGNAL(triggered()), this, SLOT(runResultSaveToFile()));
    m_runResultContextMenu->addAction(action);
    ui->treeWidget_runResult->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget_runResult, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showRunResultContextMenu(QPoint)));
}


void MainWindow::initXmlEditorMdiArea()
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(xmlEditorAutoSave()));  //定时保存

    m_windowActionGroup = new QActionGroup(this);

    m_mdiAreaTabBarContextMenu = new QMenu(ui->mdiArea);

    QAction *explore = new QAction(m_mdiAreaTabBarContextMenu);
    explore->setText(tr(u8"在Explorer中显示"));
    explore->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/file-explorer128.png")); //icon
    connect(explore, SIGNAL(triggered()), this, SLOT(exploreXmlEditorFileTriggered()));

    QAction *close = new QAction(m_mdiAreaTabBarContextMenu);
    close->setText(tr(u8"关闭"));
    close->setIcon(QIcon(""));
    connect(close, SIGNAL(triggered()), this, SLOT(closeXmlEditorTriggered()));

    QAction *closeOther = new QAction(m_mdiAreaTabBarContextMenu);
    closeOther->setText(tr(u8"关闭其他"));
    closeOther->setIcon(QIcon(""));
    connect(closeOther, SIGNAL(triggered()), this, SLOT(closeOtherXmlEditorTriggered()));

    QAction *closeAll = new QAction(m_mdiAreaTabBarContextMenu);
    closeAll->setText(tr(u8"全部关闭"));
    closeAll->setIcon(QIcon(""));
    connect(closeAll, SIGNAL(triggered()), this, SLOT(on_action_CloseAll_triggered()));

    m_mdiAreaTabBarContextMenu->addAction(explore);
    m_mdiAreaTabBarContextMenu->addSeparator();
    //    m_mdiAreaTabBarContextMenu->addAction(addToPro);
    m_mdiAreaTabBarContextMenu->addAction(close);
    m_mdiAreaTabBarContextMenu->addAction(closeOther);
    m_mdiAreaTabBarContextMenu->addAction(closeAll);

    QTabBar* tabBar = ui->mdiArea->findChild<QTabBar *>();
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showMdiAreaTabBarContextMenu(QPoint)));
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(activeXmlEditorChanged(QMdiSubWindow*)));


}

void MainWindow::initWaveWidget()
{
    if(loadPlugin())
    {
        ui->splitter_2->addWidget(m_rpuWave->getWidget());
        m_rpuWave->getWidget()->setVisible(false);
        ui->menu_D->insertAction(ui->action_restoreDebugStatus, ui->action_openWaveFile);
        ui->menu_D->insertSeparator(ui->action_restoreDebugStatus);
    }
}

bool MainWindow::loadPlugin()
{
    QDir pluginsDir(CAppEnv::getPluginDirPath());
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();

        if (plugin) {
            m_rpuWave = qobject_cast<RpuWaveInterface *>(plugin);
            connect(m_rpuWave, SIGNAL(pullWave()), this, SLOT(pullWaveToShow()));
            if (m_rpuWave)
            {
                qInfo() << tr("装载插件：“%0”成功").arg(m_rpuWave->name());
                return true;
            }
        }
        else
        {
            DBG << pluginLoader.errorString();
            qInfo() << tr("装载插件：“%0”失败").arg(fileName);
        }
    }
    return false;
}

void MainWindow::initServer()
{
    m_clientDialog = new CClientDialog(this);
    m_tcpClient = new CTcpClient(qApp->applicationVersion());
    connect(m_tcpClient, SIGNAL(finished()), m_tcpClient, SLOT(deleteLater()), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(connectStatusChanged(bool)),
            m_deviceStatusBar, SLOT(serverConnectStatusChanged(bool)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(connectStatusChanged(bool)),
            ui->action_community, SLOT(setEnabled(bool)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(connectStatusChanged(bool)),
            m_clientDialog, SLOT(serverConnectStatusChanged(bool)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(showNotify(QString)),
            this, SLOT(receiveNotifyFromServer(QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(showMessage(QString)),
            this, SLOT(receiveMessageFromServer(QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(versionAck(bool, QString)),
            this, SLOT(versionAck(bool, QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(serverCloseProgram(QString)),
            this, SLOT(serverCloseProgram(QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(transferFinished()), this, SLOT(transferFinished()), Qt::QueuedConnection);
    connect(this, SIGNAL(versionRequest()), m_tcpClient, SLOT(versionRequest()), Qt::QueuedConnection);
    connect(this, SIGNAL(licenseChanged()), m_tcpClient, SLOT(licenseChanged()), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(sendClientDialogError(QString)), m_clientDialog, SLOT(showError(QString)), Qt::QueuedConnection);
//    connect(m_clientDialog, SIGNAL(clientRequest()), m_tcpClient, SLOT(clientRequest()), Qt::QueuedConnection);
    connect(m_clientDialog, SIGNAL(sharedFileListRequest()), m_tcpClient, SLOT(sharedFileListRequest()), Qt::QueuedConnection);
    connect(m_clientDialog, SIGNAL(sharedFileDownloadRequest(QString, QString)), m_tcpClient, SLOT(sharedFileDownloadRequest(QString, QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(addSharedFile(QString, QString, QString)), m_clientDialog, SLOT(addSharedFile(QString, QString, QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(sharedFileListIsOld()), m_clientDialog, SLOT(sharedFileListIsOld()), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(serverRejectDownloadSharedFile(QString)), m_clientDialog, SLOT(serverRejectDownloadSharedFile(QString)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(downloadSharedFileFinish(QString, bool)), m_clientDialog, SLOT(downloadSharedFileFinish(QString, bool)), Qt::QueuedConnection);
    connect(m_tcpClient, SIGNAL(downloadSharedFilePercent(int)), m_clientDialog, SLOT(downloadSharedFilePercent(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(shareSolutionRequest(QString)), m_tcpClient, SLOT(shareSolutionRequest(QString)), Qt::QueuedConnection);
}

void MainWindow::initSession()
{
    m_debugSession = new CDebugSession(this);

    m_sessionManager = new CSessionManager(&m_setting, this);
    connect(m_sessionManager, SIGNAL(addSessionAction(QAction *)), this, SLOT(addSessionAction(QAction *)));
    connect(m_sessionManager, SIGNAL(updateCurrentSession()), this, SLOT(updateCurrentSession()));
    connect(m_sessionManager, SIGNAL(aboutToLoadSession()), this, SLOT(loadSessionToUI()));

    m_sessionActionGroup = new QActionGroup(this);
    m_sessionActionGroup->addAction(m_sessionManager->getDefaultSession()->action());
    ui->menu_session->addAction(m_sessionManager->getDefaultSession()->action());
    const QList<CSession*> &sessionList = m_sessionManager->getSessionList();
    foreach (CSession * session, sessionList) {
        m_sessionActionGroup->addAction(session->action());
        ui->menu_session->addAction(session->action());
    }
}

void MainWindow::initAnimation()
{
//    QMovie *mv = new QMovie(CAppEnv::getImageDirPath() + "/animation.gif", "", ui->label_animation);
//    ui->label_animation->setMovie(mv);
//    ui->label_animation->setToolTip(tr("hi, %0").arg(QFileInfo(QDir::homePath()).completeBaseName()));
//    mv->start();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}


void MainWindow::connectToServer() const
{
//    m_tcpClient->start(); // 停用此功能 20210421
}

void MainWindow::showMdiAreaTabBarContextMenu(const QPoint &point)
{
    QTabBar* tabBar = ui->mdiArea->findChild<QTabBar *>();
    m_mdiAreaIndex = tabBar->tabAt(point);
    if(-1 == m_mdiAreaIndex)
        return;
    m_mdiAreaTabBarContextMenu->exec(QCursor::pos());
}

/*!
 * MainWindow::on_stackedWidget_currentChanged
 * \brief   当前页发生改变
 * \param   arg1
 * \author  zhangjun
 * \date    2016-11-04
 */
void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    updateActions();
#if defined (Q_OS_WIN)
    QString str = tr(u8"QToolBar {background-color: #404244;border-right: 1px solid #222222;border-left: 0px;}"
                     "QToolButton {color: #D2D2D2;width: 4.5em;height: 4em;margin:0px;border: 0px;padding-top: 0.5em;padding-bottom: 0.1em;}"
                     "QToolButton:hover {background-color: #595B5D;}"
                     "QToolButton[text=\"模式\"] {height: 1.2em;}");
#else
    QString str = tr(u8"QToolBar {background-color: #404244;border-right: 1px solid #222222;border-left: 0px;}"
                     "QToolButton {color: #D2D2D2;width: 3.5em;height: 3em;margin:0px;border: 0px;padding-top: 0.5em;padding-bottom: 0.1em;}"
                     "QToolButton:hover {background-color: #595B5D;}"
                     "QToolButton[text=\"模式\"] {height: 1.2em;}");
#endif
    switch (arg1) {
    case 0:
        str += tr(u8"QToolButton[text=\"首页\"] {background-color: #262829;}");
        ui->toolBar_guide->setStyleSheet(str);
        break;
    case 1:
        str += tr(u8"QToolButton[text=\"编辑\"] {background-color: #262829;}");
        ui->toolBar_guide->setStyleSheet(str);
        break;
    case 2:
        str += tr(u8"QToolButton[text=\"资源\"] {background-color: #262829;}");
        ui->toolBar_guide->setStyleSheet(str);
        break;
    case 3:
        str += tr(u8"QToolButton[text=\"项目\"] {background-color: #262829;}");
        ui->toolBar_guide->setStyleSheet(str);
        break;
    case 5:
        str += tr(u8"QToolButton[text=\"调试窗\"] {background-color: #262829;}");
        ui->toolBar_guide->setStyleSheet(str);
        break;
    default:
        ui->toolBar_guide->setStyleSheet(str);
        break;
    }
//    ui->dockWidget_attribute->hide(); //隐藏元素属性停靠窗
    ui->label_forecastClock->clear(); //清空预估周期
}

/*!
 * MainWindow::on_action_fullScreen_triggered
 * \brief   全屏显示
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-24
 */
void MainWindow::on_action_fullScreen_triggered(bool checked)
{
    qInfo() << tr(u8"全屏显示");
    if(checked)
    {
        ui->action_fullScreen->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/arrows2128.png"));
        if(this->isMaximized())
        {
            ui->action_fullScreen->setData("max");
        }
        else
        {
            ui->action_fullScreen->setData("normal");
        }
        this->menuBar()->setVisible(false);
        this->showFullScreen();
    }
    else
    {
        ui->action_fullScreen->setIcon(QIcon(CAppEnv::getImageDirPath() + "/full.png"));
        if(ui->action_fullScreen->data() == "max")
        {
            this->showMaximized();
        }
        else
        {
            this->showNormal();
        }
        this->menuBar()->setVisible(true);
    }
    ui->action_showMenuBar->setChecked(ui->menuBar->isVisible());
}

/*!
 * MainWindow::sysTrayActivated
 * \brief   点击系统托盘图标
 * \param   reason
 * \author  zhangjun
 * \date    2016-10-18
 */
void MainWindow::sysTrayActivated(const QSystemTrayIcon::ActivationReason &reason)
{
    qInfo() << tr(u8"点击系统托盘图标");
    switch(reason)
    {
    case QSystemTrayIcon::Trigger://单击
    case QSystemTrayIcon::DoubleClick://双击
    case QSystemTrayIcon::MiddleClick://滚轮
        this->show();
        if(windowState() & Qt::WindowMinimized)
        {
            setWindowState(Qt::WindowNoState);
            restoreGeometry(m_geometry);
        }
        this->activateWindow();
        this->raise();
        break;
    default:
        break;
    }
}

/*!
 * MainWindow::actionShowTriggered
 * \brief   显示程序主界面
 * \author  zhangjun
 * \date    2016-10-18
 */
void MainWindow::actionShowTriggered()
{
    qInfo() << tr(u8"托盘上下文菜单：显示程序主界面");
    sysTrayActivated(QSystemTrayIcon::DoubleClick);
}

/*!
 * MainWindow::actionQuitTriggered
 * \brief   关闭程序
 * \author  zhangjun
 * \date    2016-10-18
 */
void MainWindow::actionQuitTriggered()
{
    qInfo() << tr(u8"关闭程序");
    this->hide();
    on_action_Quit_triggered();
}

void MainWindow::restoreSession()
{
    if(m_setting.restoreSessionFlag)
    {
        m_sessionManager->restoreSession(m_setting.restoreSession);
    }
}

/*!
 * MainWindow::clearRecentMenu
 * \brief   清除最近打开文档列表
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::clearRecentMenu()
{
    qInfo() << tr(u8"清除最近打开文档列表");
    QAction *action = qobject_cast<QAction *>(sender());
    if (action && action->data().toString() == "clearRecentFiles")
    {
        m_recentFiles.clear();
        updateRecentFileActions();
    }
    else if(action && action->data().toString() == "clearRecentPros")
    {
        m_recentPros.clear();
        updateRecentProjectActions();
    }
}

/*!
 * MainWindow::openRecentFile
 * \brief   槽-打开最近打开过的文档
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QFile file(action->data().toString());
        if (!file.exists()) {
            QMessageBox::warning(this, qApp->applicationName(),
                                 tr(u8"文件“%0”不存在。")
                                 .arg(file.fileName()));
            updateRecentFileActions();
            return;
        }
        openXmlFile(action->data().toString());
    }
}

/*!
 * MainWindow::openRecentPro
 * \brief   槽-打开最近打开过的解决方案
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::openRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QFile file(action->data().toString());
        if (!file.exists()) {
            QMessageBox::warning(this, qApp->applicationName(),
                                 tr(u8"文件“%0”不存在。")
                                 .arg(file.fileName()));
            updateRecentProjectActions();
            return;
        }
        loadSolution(QStringList(action->data().toString()));
    }
}

void MainWindow::runProjectTriggered()
{
    setCurProject();
    runOrDebugTriggered("run");
}

void MainWindow::debugProjectTriggered()
{
    setCurProject();
    runOrDebugTriggered("debug");
}

void MainWindow::dropSrcToProject(const QString &fileName)
{
    if(QFileInfo(fileName).suffix().compare("sbox", Qt::CaseInsensitive) != 0
            && QFileInfo(fileName).suffix().compare("fifo", Qt::CaseInsensitive) != 0
            && QFileInfo(fileName).suffix().compare("benes", Qt::CaseInsensitive) != 0
            && QFileInfo(fileName).suffix().compare("imd", Qt::CaseInsensitive) != 0
            && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0)
    {
        int &&ret = QMessageBox::warning(this, tr(u8"提示"),
                                         tr(u8"文件“%0”可能不是正确的资源文件，是否确定添加？")
                                         .arg(fileName),
                                         QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            return;
        }
    }
    m_projectTreeModel->activeProjectAddSrc(fileName);
}

void MainWindow::dropProjectToProjectTree(const QStringList &algList, const QStringList &unSupportList)
{
    loadRpuSln(algList);
    if(!unSupportList.isEmpty())
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"<P>不支持以下文件："
                                                                "%0").arg(unSupportList.join("、")));
    }
}

void MainWindow::dropXmlToXmlEditor(const QStringList &xmlList, const QStringList &unSupportList)
{
    loadFiles(xmlList);
    if(!unSupportList.isEmpty())
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"<P>不支持以下文件："
                                                                "%0").arg(unSupportList.join("、")));
    }
}

/*!
 * MainWindow::setCurProject
 * \brief   设置当前项目
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::setCurProject()
{
    qInfo() << tr(u8"设置当前项目");
    m_projectTreeModel->setActiveProject(ui->treeView_project->currentIndex());
}

/*!
 * MainWindow::newXmlToProject
 * \brief   项目中新建XML
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::newXmlToProject()
{
    qInfo() << tr(u8"新建XML");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString openDir = QFileInfo(m_projectTreeModel->getItemFileName(index)).absolutePath();
    NewForm newForm(openDir, NEWXML, this);
    CAppEnv::animation(&newForm, this);
    if(newForm.exec() == QDialog::Rejected)
        return;
    QString fileName = newForm.getFileName();
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"添加新文件失败！"));
        return;
    }
    file.close();

    m_projectTreeModel->addXml(fileName, index);
    ui->treeView_project->expand(index);
}

/*!
 * MainWindow::addXmlToProject
 * \brief   添加XML到项目中
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::addXmlToProject()
{
    qInfo() << tr(u8"添加XML到项目中");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString openDir = QFileInfo(m_projectTreeModel->getItemFileName(index)).absolutePath();
    QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr(u8"打开"),
                                                             openDir, tr(u8"XML 文件 (*.xml);;RPU Log 文件 (*.rpulog)"));
    foreach (QString str, fileNameList) {
        QFileInfo fileInfo(str);
        if(fileInfo.suffix().toLower() == "rpulog") {
            if(!openXmlFile(str, nullptr, true)) // zhangjun added at 20200902, 添加log文件自动转换成xml
            {
                return;
            }
            str.chop(6);
            str = str + "xml";
        }
        m_projectTreeModel->addXml(str, index);
    }
    ui->treeView_project->expand(index);
}

/*!
 * MainWindow::newSrcToProject
 * \brief   项目中新建SRC
 * \author  zhangjun
 * \date    2016-11-02
 */
void MainWindow::newSrcToProject()
{
    qInfo() << tr(u8"新建资源文件");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString openDir = QFileInfo(m_projectTreeModel->getItemFileName(index)).absolutePath();
    NewSrcForm newSrcForm(openDir, this);
    CAppEnv::animation(&newSrcForm, this);
    if(newSrcForm.exec() == QDialog::Rejected)
        return;
    QString fileName = newSrcForm.getFilePath();
    QString type = newSrcForm.getType();
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"添加新文件失败！"));
        return;
    }
    file.close();
    m_projectTreeModel->addSrc(fileName, index);
    ui->treeView_project->expand(index);
    QString projectFileName = m_projectTreeModel->getProjectFileName(index);
    openSrcFile(projectFileName, fileName, type);
}

/*!
 * MainWindow::addSrcToProject
 * \brief   添加资源到项目
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::addSrcToProject()
{
    qInfo() << tr(u8"添加资源到项目");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString openDir = QFileInfo(m_projectTreeModel->getItemFileName(index)).absolutePath();
    QStringList fileNameList
            = QFileDialog::getOpenFileNames(this, tr(u8"打开"), openDir,
                                            tr(u8"SBOX (*.sbox);;BENES (*.benes);;FIFO (*.fifo);;imd (*.imd);;memory (*.memory);;文本文档 (*.txt);;* (*.*)"));
    foreach (QString fileName, fileNameList) {
        if(QFileInfo(fileName).suffix().compare("sbox", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("fifo", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("benes", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("imd", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0)
        {
            int &&ret = QMessageBox::warning(this, tr(u8"提示"),
                                             tr(u8"文件“%0”可能不是正确的资源文件，是否确定添加？")
                                             .arg(fileName),
                                             QMessageBox::Yes | QMessageBox::No
                                             | QMessageBox::Cancel);
            if (ret == QMessageBox::No) {
                continue;
            } else if (ret == QMessageBox::Cancel) {
                break;
            }

        }
        m_projectTreeModel->addSrc(fileName, index);
    }
    ui->treeView_project->expand(index);

}

void MainWindow::removeProject()
{
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString projectName = m_projectTreeModel->getProjectFileName(index);
    bool active = m_projectTreeModel->isActiveProject(index);
    if(active && m_process && m_process->state() != QProcess::NotRunning)
    {
        QMessageBox::information(this, tr(u8"提示"), tr(u8"不能关闭正在运行或调试的项目！"));
        return;
    }

    //start 工程内xml文件改为非工程文件
    QList<RcaGraphView *> viewList;
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView && rcaGraphView->getProject() && QFileInfo(rcaGraphView->getProject()->getProjectFileName()) == QFileInfo(projectName))
        {
            viewList.append(rcaGraphView);
            rcaGraphView->setProject(nullptr);
        }
    }
    m_projectTreeModel->deleteProject(index);

    foreach (RcaGraphView *view, viewList) {
        view->setProject(m_projectTreeModel->getNearestProjectResolverFromXml(view->getCurFile()));
    }
    //end

    if(!m_projectTreeModel->hasActiveSolution())
    {
        if(ui->stackedWidget->currentIndex() != 1 || ui->mdiArea->subWindowList().count() < 0)
            ui->stackedWidget->setCurrentIndex(0);
        deleteResourceEditorTable();
        resetProjectSettingTable();
        resetResourceSelectTable();
        updateActions();
        m_curProjectResolver = nullptr;
        return;
    }

    //关闭资源编辑器
    if(m_resourceEditorTableModel && QFileInfo(m_resourceEditorTableModel->getProjectName()) == QFileInfo(projectName))
    {
        m_resourceEditorTableModel->setProjectName(m_projectTreeModel->getNearestProjectFileNameFromSrc(m_resourceEditorTableModel->getFileName()));
        if(m_resourceEditorTableModel->getProjectName().isEmpty())
            deleteResourceEditorTable();
    }
}

void MainWindow::newProject()
{
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString openDir = QFileInfo(m_projectTreeModel->getItemFileName(index)).absolutePath();
    NewForm newForm(openDir, NEWPROJECT, this);
    CAppEnv::animation(&newForm, this);
    if(newForm.exec() == QDialog::Rejected)
        return;

    QString fileName = newForm.getFileName();
    ECipherType cipherType = newForm.getCipherType();
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"新建项目失败，未能写入到文件！"));
        return;
    }
    file.close();
    m_projectTreeModel->addProject(fileName, cipherType, index);
    ui->treeView_project->expand(index);
    updateActions();
}

void MainWindow::addProject()
{
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString openDir = QFileInfo(m_projectTreeModel->getItemFileName(index)).absolutePath();
    QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr(u8"打开"),
                                                             openDir, tr(u8"RPU项目文件 (*.rpupro)"));
    foreach (QString fileName, fileNameList) {
        m_projectTreeModel->appendProject(fileName, index);
    }
    ui->treeView_project->expand(index);
    updateActions();
}

void MainWindow::shareSolution()
{
    if(!m_tcpClient->getIsRegister()) {
        QMessageBox::information(this, qApp->applicationName(), tr("共享解决方案失败，未连接到服务器！"));
        return;
    }
    QString solutionFileName = m_projectTreeModel->getSolutionFileName(ui->treeView_project->currentIndex());
    if(solutionFileName.isEmpty()) {
        return;
    }

    if(QMessageBox::Yes != QMessageBox::information(this, qApp->applicationName(),
                                         tr(u8"<p>确认是否共享解决方案？"
                                            "<p>解决方案文件夹内的文件将共享，引用的外部文件不会共享！"
                                            "<p>共享后不能撤销，任何人都能获得共享的解决方案！"),
//                                            "<p>解决方案共享后将在24小时后自动取消共享，在此期间不能撤销共享！"
                                         QMessageBox::Yes | QMessageBox::No))
        return;

    QString solutionPath = QFileInfo(solutionFileName).absolutePath();
    QString compressedFileName = CAppEnv::getTmpDirPath() + QDir::separator() +
            QFileInfo(solutionPath).fileName() + "_" +
            QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".zip" ;

#if defined(Q_OS_WIN)
    QString cmd = CAppEnv::getBinDirPath() + tr(u8"/compress.exe");
#elif defined(Q_OS_UNIX)
    QString cmd = CAppEnv::getBinDirPath() + tr(u8"/compress");
#endif
    QStringList argumentList;
    argumentList << solutionPath << compressedFileName;
    QProcess process;
    process.setProgram(cmd);
    process.setArguments(argumentList);
    process.setProcessChannelMode(QProcess::SeparateChannels);
    process.start();
    if(!process.waitForFinished()) {
        qInfo() << tr("共享解决方案，打包文件失败！");
        CWaitDialog::getInstance()->finishShowing();
        QMessageBox::information(this, qApp->applicationName(), tr("共享解决方案失败！"));
        return;
    }
    else {
        if(process.exitCode() == 0)
        {
            DBG << "compress succeed";
            emit shareSolutionRequest(compressedFileName);
        }
        else
        {
            qInfo() << tr("共享解决方案，打包文件失败！");
            CWaitDialog::getInstance()->finishShowing();
            QMessageBox::information(this, qApp->applicationName(), tr("共享解决方案失败！"));
            return;
        }
    }
}

/*!
 * MainWindow::deleteFile
 * \brief   文件系统中删除文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::deleteFile()
{
    qInfo() << tr(u8"文件系统中删除文件");
    if(QFile::remove(m_deleteFile))
        m_deleteFile.clear();
}


/*!
 * MainWindow::openXmlTriggered
 * \brief   项目中上下文菜单打开xml
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::openXmlTriggered()
{
    on_treeView_project_doubleClicked(ui->treeView_project->currentIndex());
}

void MainWindow::projectSettingTriggered()
{
    const QModelIndex &index = ui->treeView_project->currentIndex();
    ui->treeView_project->setCurrentIndex(ui->treeView_project->currentIndex().parent());
    setCurProject();
    ui->treeView_project->setCurrentIndex(index);
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::removeSrcInContextMenu()
{
    QListWidgetItem *item = ui->listWidget_resource->currentItem();
    QString fileName = item->data(Qt::UserRole).toString();

    DeleteXmlForm delForm(fileName, this);
    CAppEnv::animation(&delForm, this);
    if(delForm.exec() == QDialog::Accepted)
    {
        if(m_resourceEditorTableModel)
        {
            QString currentSrcfileName = m_resourceEditorTableModel->getFileName();
            if(QFileInfo(currentSrcfileName) == QFileInfo(fileName))
            {
                if(delForm.getChecked())
                {
                    m_resourceEditorTableModel->save();
                }
                if(ui->stackedWidget->currentIndex() == 2)
                    ui->stackedWidget->setCurrentIndex(0);
                deleteResourceEditorTable();
            }
        }
        m_projectTreeModel->activeProjectDeleteSrc(fileName);

        if(delForm.getChecked())
        {
            if(!QFile::remove(fileName))
            {
                m_deleteFile = fileName;
                QTimer::singleShot(5000, this, SLOT(deleteFile()));
            }
        }
    }
}

void MainWindow::projectParameterSaved(const QString &fileName)
{
    ui->pushButton_proSettingSave->setEnabled(false);
    ui->label_projectSettingTitle->setText(tr(u8"项目设置：%0").arg(fileName));
}


/*!
 * MainWindow::deleteFileTriggered
 * \brief   项目中移除xml或资源
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::deleteFileTriggered()
{
    qInfo() << tr(u8"项目中移除xml或资源");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    QString fileName = m_projectTreeModel->getItemFileName(index);
    EItemType type = m_projectTreeModel->getItemType(index);
    DeleteXmlForm delForm(fileName, this);
    CAppEnv::animation(&delForm, this);
    if(delForm.exec() == QDialog::Accepted)
    {
        //判断是否已打开
        if(type == ITEMXMLFILE)
        {
            QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
            RcaGraphView *rcaGraphView = nullptr;
            for(int i = 0; i < winList.count(); ++i)
            {
                rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
                if(QFileInfo(rcaGraphView->getCurFile()) == QFileInfo(fileName))
                {
                    if(delForm.getChecked())
                    {
                        //                    rcaGraphView->setProject(nullptr);
                        rcaGraphView->save();
                        winList.at(i)->close();
                        rcaGraphView = nullptr;
                        break;
                    }
                }
            }
            if(delForm.getChecked())
            {
                m_projectTreeModel->deleteXml(fileName);
            }
            else
            {
                m_projectTreeModel->deleteXml(index);
                if(rcaGraphView)
                    rcaGraphView->setProject(m_projectTreeModel->getNearestProjectResolverFromXml(rcaGraphView->getCurFile()));
            }
        }
        else if(type == ITEMSRCFILE)
        {
            if(delForm.getChecked())
            {
                m_projectTreeModel->deleteSrc(fileName);
                if(m_resourceEditorTableModel)
                {
                    QString currentSrcfileName = m_resourceEditorTableModel->getFileName();
                    if(QFileInfo(currentSrcfileName) == QFileInfo(fileName))
                    {
                        m_resourceEditorTableModel->save();
                        if(ui->stackedWidget->currentIndex() == 2)
                            ui->stackedWidget->setCurrentIndex(0);
                        delete m_resourceEditorTableModel;
                        m_resourceEditorTableModel = nullptr;
                        ui->widget_resourceEdit->setEnabled(false);
                        ui->widget_benesEditor->setVisible(false);
                    }
                }
            }
            else
            {
                m_projectTreeModel->deleteSrc(index);
                if(m_resourceEditorTableModel)
                {
                    QString neareastProjectName = m_projectTreeModel->getNearestProjectFileNameFromSrc(fileName);
                    QString currentSrcfileName = m_resourceEditorTableModel->getFileName();
                    if(QFileInfo(currentSrcfileName) == QFileInfo(fileName)
                            && neareastProjectName.isEmpty())
                    {
                        if(ui->stackedWidget->currentIndex() == 2)
                            ui->stackedWidget->setCurrentIndex(0);
                        delete m_resourceEditorTableModel;
                        m_resourceEditorTableModel = nullptr;
                        ui->widget_resourceEdit->setEnabled(false);
                        ui->widget_benesEditor->setVisible(false);
                    }
                    else
                    {
                        m_resourceEditorTableModel->setProjectName(neareastProjectName);
                    }
                }
            }
        }

        if(delForm.getChecked())
        {
            if(!QFile::remove(fileName))
            {
                m_deleteFile = fileName;
                QTimer::singleShot(5000, this, SLOT(deleteFile()));
            }
        }
    }
}

/*!
 * MainWindow::renameXmlTriggered
 * \brief   项目中重命名xml
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::renameFileTriggered()
{
    qInfo() << tr(u8"重命名XML");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    EItemType type = m_projectTreeModel->getItemType(index);
    if(type != ITEMSOLUTIONFILE && type != ITEMXMLFILE && type != ITEMSRCFILE && type != ITEMPROJECTFILE)
        return;
    m_renameFlag = true;
    ui->treeView_project->edit(index);
}

void MainWindow::copyPathTriggered()
{
    const QModelIndex &index = ui->treeView_project->currentIndex();
    EItemType type = m_projectTreeModel->getItemType(index);
    if(type != ITEMXMLFILE && type != ITEMSRCFILE && type != ITEMPROJECTFILE && type!= ITEMSOLUTIONFILE)
        return;
    qApp->clipboard()->setText(m_projectTreeModel->getItemFileName(index));
}

/*!
 * MainWindow::exploreFileTriggered
 * \brief   文件系统中浏览XML文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::exploreProjectFileTriggered()
{
    qInfo() << tr(u8"文件系统中浏览XML文件");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    if(!index.isValid())
        return;

    QString filePath = m_projectTreeModel->getItemFileName(index);
#if defined(Q_OS_WIN)
    filePath.replace("/", "\\");
    filePath = " /select," + filePath;
    LPCSTR file = filePath.toLocal8Bit();
    ShellExecuteA(0, "open", "explorer.exe", file, nullptr, SW_SHOWNORMAL);
#elif defined(Q_OS_UNIX)
    filePath = QFileInfo(filePath).absolutePath();
    QDesktopServices::openUrl(QUrl("file:///" + filePath));
#endif
}

void MainWindow::exploreOpenedFileTriggered()
{
    int row = ui->listView_openedFile->currentIndex().row();
    QString filePath = m_openedFiles.at(row);
#if defined(Q_OS_WIN)
    filePath.replace("/", "\\");
    filePath = " /select," + filePath;
    LPCSTR file = filePath.toLocal8Bit();
    ShellExecuteA(0, "open", "explorer.exe", file, nullptr, SW_SHOWNORMAL);
#elif defined(Q_OS_UNIX)
    filePath = QFileInfo(filePath).absolutePath();
    QDesktopServices::openUrl(QUrl("file:///" + filePath));
#endif
}

void MainWindow::exploreXmlEditorFileTriggered()
{
    QMdiSubWindow *subWindow = ui->mdiArea->subWindowList().at(m_mdiAreaIndex);
    if(!subWindow)
        return;
    RcaGraphView *view = activeXmlEditor(subWindow);
    if(!view)
        return;
    QString filePath = view->getCurFile();
#if defined(Q_OS_WIN)
    filePath.replace("/", "\\");
    filePath = " /select," + filePath;
    LPCSTR file = filePath.toLocal8Bit();
    ShellExecuteA(0, "open", "explorer.exe", file, nullptr, SW_SHOWNORMAL);
#elif defined(Q_OS_UNIX)
    filePath = QFileInfo(filePath).absolutePath();
    QDesktopServices::openUrl(QUrl("file:///" + filePath));
#endif
}

void MainWindow::closeXmlEditorTriggered()
{
    QMdiSubWindow *subWindow = ui->mdiArea->subWindowList().at(m_mdiAreaIndex);
    if(subWindow)
        subWindow->close();
}

void MainWindow::closeOtherXmlEditorTriggered()
{
    QMdiSubWindow *subWindow = ui->mdiArea->subWindowList().at(m_mdiAreaIndex);
    if(subWindow)
    {
        for(int i = ui->mdiArea->subWindowList().count() - 1; i >= 0 ; --i)
        {
            if(i != m_mdiAreaIndex)
            {
                ui->mdiArea->subWindowList().at(i)->close();
            }
        }
    }
}

void MainWindow::closeSolutionTriggered()
{
    qInfo() << tr(u8"关闭解决方案");
    const QModelIndex &index = ui->treeView_project->currentIndex();
    closeSolution(index);
}

void MainWindow::closeOtherSolutionTriggered()
{
    //    QStandardItem *curStdItem = m_projectTreeModel->itemFromIndex(ui->treeView_project->currentIndex());
    QModelIndex currentIndex = ui->treeView_project->currentIndex();
    for(int i = m_projectTreeModel->rowCount() - 1; i >= 0 ; --i)
    {
        const QModelIndex &index = m_projectTreeModel->index(i, 0);
        if(index != currentIndex)
            closeSolution(index);
    }
}


void MainWindow::on_action_closeAllSolution_triggered()
{
    for(int i = m_projectTreeModel->rowCount() - 1; i >= 0 ; --i)
    {
        closeSolution(m_projectTreeModel->index(i, 0));
    }
}

void MainWindow::on_action_closeAllSlnAndEditor_triggered()
{
    on_action_CloseAll_triggered();
    on_action_closeAllSolution_triggered();
}

/*!
 * MainWindow::closeProject
 * \brief   关闭项目
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::closeSolution(const QModelIndex &index)
{
    bool active = m_projectTreeModel->isActiveSolution(index);
    QStringList projectList = m_projectTreeModel->getProjectList(index);
    if(active && m_process && m_process->state() != QProcess::NotRunning)
    {
        QMessageBox::information(this, qApp->applicationName(), tr(u8"不能关闭有正在运行或调试的解决方案！"));
        return;
    }

    //    工程内xml文件改为非工程文件，使用rcaGraphView内计数方法
    QList<RcaGraphView *> viewList;
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        bool continueFlag = false;
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        foreach (QString projectName, projectList) {
            if(rcaGraphView && rcaGraphView->getProject() && QFileInfo(rcaGraphView->getProject()->getProjectFileName()) == QFileInfo(projectName))
            {
                viewList.append(rcaGraphView);
                rcaGraphView->setProject(nullptr);
                continueFlag = true;
                break;
            }
        }
        if(continueFlag)
            continue;
    }

    m_projectTreeModel->closeSolution(index);
    foreach (RcaGraphView *view, viewList) {
        view->setProject(m_projectTreeModel->getNearestProjectResolverFromXml(view->getCurFile()));
    }
    if(!m_projectTreeModel->hasActiveSolution())
    {
        if(ui->stackedWidget->currentIndex() != 1 || ui->mdiArea->subWindowList().count() < 0)
            ui->stackedWidget->setCurrentIndex(0);
        deleteResourceEditorTable();
        resetProjectSettingTable();
        resetResourceSelectTable();
        updateActions();
        m_curProjectResolver = nullptr;
        return;
    }
    //关闭资源编辑器，使用m_resourceEditorTableModel内计数方法
    foreach (QString projectName, projectList) {
        if(m_resourceEditorTableModel && QFileInfo(m_resourceEditorTableModel->getProjectName()) == QFileInfo(projectName))
        {
            m_resourceEditorTableModel->setProjectName(m_projectTreeModel->getNearestProjectFileNameFromSrc(m_resourceEditorTableModel->getFileName()));
            if(m_resourceEditorTableModel->getProjectName().isEmpty())
                deleteResourceEditorTable();
        }
    }
    updateActions();
}

/*!
 * MainWindow::showProjectContextMenu
 * \brief   项目树上下文菜单
 * \param   point
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::showProjectContextMenu(const QPoint &point)
{
    qInfo() << tr(u8"解决方案资源管理器上下文菜单");
    const QModelIndex &index = ui->treeView_project->indexAt(point);
    if(!index.isValid())
        return;

    EItemType itemType = m_projectTreeModel->getItemType(index);
    CProjectResolver *projectResovler = m_projectTreeModel->getProjectResolver(index);
    if(itemType == ITEMSOLUTION)
    {
        m_solutionFolderContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMSOLUTIONFILE)
    {
        m_solutionFileContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMPROJECT)
    {
        m_lastRunResult->setEnabled(projectResovler->getHasResult());
        m_projectFolderContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMPROJECTFILE)
    {
        m_projectFileContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMXMLFOLDER)
    {
        m_projectXmlFolderContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMXMLFILE)
    {
        m_projectXmlFileContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMSRCFOLDER)
    {
        m_projectSrcFolderContextMenu->exec(QCursor::pos());
    }
    else if(itemType == ITEMSRCFILE)
    {
        m_projectXmlFileContextMenu->exec(QCursor::pos());
    }
}

/*!
 * MainWindow::showOpenedFileContextMenu
 * \brief   已打开文件上下文菜单
 * \param   point
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::showOpenedFileContextMenu(const QPoint &point)
{
    qInfo() << tr(u8"已打开文件上下文菜单");
    if(!ui->listView_openedFile->indexAt(point).isValid())
        return;

    m_openedFileListContextMenu->exec(QCursor::pos());
}

/*!
 * MainWindow::showResourceChoiceContextMenu
 * \brief   资源选择文件上下文菜单
 * \param   point
 * \author  zhangjun
 * \date    2016-11-3
 */
void MainWindow::showResourceChoiceContextMenu(const QPoint &point)
{
    if(!ui->listWidget_resource->indexAt(point).isValid())
        return;

    m_resourceChoiceContextMenu->exec(QCursor::pos());
}

/*!
 * MainWindow::showResourceChoiceContextMenu
 * \brief   文件系统中选择文件上下文菜单
 * \param   point
 * \author  zhangjun
 * \date    2016-11-3
 */
void MainWindow::showExplorerChoiceContextMenu(const QPoint &point)
{
    const QModelIndex &index = ui->treeView_explorer->indexAt(point);
    if(!index.isValid())
        return;

    QFileSystemModel *model = static_cast<QFileSystemModel*>(ui->treeView_explorer->model());
    if(!model)
        return;
    QFileInfo fileInfo(model->fileInfo(index));
    if(!fileInfo.isFile())
        return;

    m_explorerChoiceContextMenu->exec(QCursor::pos());
}


void MainWindow::showRunResultContextMenu(const QPoint &point)
{
    QTreeWidgetItem *item = ui->treeWidget_runResult->itemAt(point);
    if(item == nullptr)
    {
        return;
    }
    QTreeWidgetItem *parent = item->parent();
    if(parent != nullptr)
    {
        return;
    }

    m_runResultContextMenu->exec(QCursor::pos());
}

/*!
 * MainWindow::updateRecentFileActions
 * \brief   刷新最近打开文件列表
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(m_recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    if(m_recentFiles.count() < 1)
    {
        ui->menu_F_recentFiles->setEnabled(false);
    }
    else
    {
        ui->menu_F_recentFiles->setEnabled(true);
    }

    for (int j = 0; j < m_recentFileCount; ++j) {
        if (j < m_recentFiles.count()) {
            QString text = tr(u8"&%0 %1")
                    .arg(j + 1)
                    .arg(CAppEnv::stpToNm(m_recentFiles[j]));
            m_recentFileActions[j]->setText(text);
            m_recentFileActions[j]->setData(m_recentFiles[j]);
            m_recentFileActions[j]->setVisible(true);
        } else {
            m_recentFileActions[j]->setVisible(false);
        }
    }

    ui->listWidget_files->clear();
    ui->listWidget_files->addItems(m_recentFiles);

    for(int i = 0; i < ui->listWidget_files->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_files->item(i);
        item->setToolTip(item->text());
        item->setText(QFileInfo(item->text()).baseName() + " - " + item->text());
        item->setSizeHint(QSize(0, 40));
        //        item->setTextColor(Qt::white);
        item->setTextColor(QColor("#404244"));
        item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Xml-tool128.png"));
    }
}

/*!
 * MainWindow::updateRecentProActions
 * \brief   刷新最近打开项目列表
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::updateRecentProjectActions()
{
    QMutableStringListIterator i(m_recentPros);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    if(m_recentPros.count() < 1)
    {
        ui->menu_P_recentPros->setEnabled(false);
    }
    else
    {
        ui->menu_P_recentPros->setEnabled(true);
    }

    for (int j = 0; j < m_recentProCount; ++j) {
        if (j < m_recentPros.count()) {
            QString text = tr(u8"&%0 %1").arg(j + 1)
                    .arg(CAppEnv::stpToNm(m_recentPros[j]));
            m_recentProActions[j]->setText(text);
            m_recentProActions[j]->setData(m_recentPros[j]);
            m_recentProActions[j]->setVisible(true);
        } else {
            m_recentProActions[j]->setVisible(false);
        }
    }

    ui->listWidget_pros->clear();
    ui->listWidget_pros->addItems(m_recentPros);

    for(int i = 0; i < ui->listWidget_pros->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_pros->item(i);
        item->setToolTip(item->text());
        item->setText(QFileInfo(item->text()).baseName() + " - " + item->text());
        item->setSizeHint(QSize(0, 40));
        //        item->setTextColor(Qt::white);
        item->setTextColor(QColor("#404244"));
        item->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Inventory-maintenance128.png"));
    }
}

/*!
 * MainWindow::on_action_newSolution_triggered
 * \brief   新建项目
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_newSolution_triggered()
{
    qInfo() << tr(u8"新建解决方案");
    NewForm newForm(m_setting.defaultProPath, NEWSOLUTION, this);
    CAppEnv::animation(&newForm, this);
    if(newForm.exec() == QDialog::Rejected)
        return;

    QString fileName = newForm.getFileName();
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"创建解决方案失败！"));
        return;
    }
    file.close();
    loadSolution(QStringList(fileName));
}

/*!
 * MainWindow::on_action_openSolution_triggered
 * \brief   打开项目
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_openSolution_triggered()
{
    qInfo() << tr(u8"打开解决方案");
    QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr(u8"打开"),
                                                             m_setting.defaultProPath,
                                                             tr(u8"加解密解决方案 (*.rpusln)"));
    if (fileNameList.isEmpty())
        return;

    loadRpuSln(fileNameList);
}

/*!
 * MainWindow::loadAlgs
 * \brief   解析解决方案文件sln
 * \param   algList
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::loadRpuSln(const QStringList &rpuSlnList)
{
    qInfo() << tr(u8"解析解决方案文件sln");
    QStringList list;
    foreach(QString fileName, rpuSlnList)
    {
        fileName.replace("\\", "/");
        if(!QFileInfo(fileName).suffix().endsWith("rpusln", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"打开加解密解决方案“%0”失败，不支持的文件格式！").arg(fileName));
            continue;
        }
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"打开加解密解决方案“%0”失败！").arg(fileName));
            continue;
        }
        QTextStream in(&file);
        bool isSln = true;
        while(!in.atEnd())
        {
            QString str = in.readLine().trimmed();
            str.replace("\\", "/");
            if(QFileInfo(str).suffix().endsWith("rpusln", Qt::CaseInsensitive)
                    && QFileInfo(CAppEnv::relToAbs(fileName, str)).exists())
            {
                if(isSln)
                    isSln = false;
                list.append(CAppEnv::relToAbs(fileName, str));
            }
        }
        file.close();
        if(isSln)
            list.append(fileName);
    }
    loadSolution(list);
}


/*!
 * MainWindow::loadProjects
 * \brief   载入项目文件ALG
 * \param   fileNameList
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::loadSolution(const QStringList &fileNameList)
{ 
    qInfo() << tr(u8"载入解决方案文件");
    foreach(QString fileName, fileNameList)
    {
        if(!QFileInfo(fileName).fileName().endsWith(".rpusln", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"打开加解密解决方案“%0”失败，不支持的文件格式！").arg(fileName));
            continue;
        }
        m_projectTreeModel->appendSolution(fileName);
        updateRecentProjectList(fileName);
    }
    updateActions();
}

void MainWindow::resetProjectSettingTable()
{
    qInfo() << tr(u8"更新项目设置");
    disconnect(ui->comboBox_projectSettingSetSolution, SIGNAL(currentIndexChanged(int)),
               this, SLOT(setSolutionTypeChanged()));
    disconnect(ui->comboBox_lfsrMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSolutionTypeChanged()));
    disconnect(ui->comboBox_lfsrWidthDeep, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSolutionTypeChanged()));
    CProjectResolver *projectResolver = m_projectTreeModel->getActiveProjectResolver();
    if(projectResolver)
    {
        if(m_projectSettingModel)
        {
            m_projectSettingModel->resetData(projectResolver->getProjectParameter()->xmlParameterList,
                                             &projectResolver->getProjectParameter()->resourceMap);
            ui->tableView_projectSetting->resizeColumnsToContents();
        }
        else
        {
            m_projectSettingModel = new CProjectSettingModel(
                        projectResolver->getProjectParameter()->xmlParameterList,
                        &projectResolver->getProjectParameter()->resourceMap, this);
            connect(m_projectSettingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                    this, SLOT(projectParameterSetDirty()));
            connect(m_projectSettingModel, SIGNAL(currentIndex(QModelIndex)),
                    ui->tableView_projectSetting, SLOT(setCurrentIndex(QModelIndex)));
            ui->tableView_projectSetting->setModel(m_projectSettingModel);
            ui->tableView_projectSetting->resizeColumnsToContents();
        }
        ui->comboBox_projectSettingSetSolution->setCurrentIndex(projectResolver->getProjectParameter()->cipherType);
        ui->comboBox_lfsrMode->setCurrentIndex(projectResolver->getProjectParameter()->lfsrMode);
        ui->comboBox_lfsrWidthDeep->setCurrentIndex(projectResolver->getProjectParameter()->lfsrWidthDeep);
        ui->label_projectSettingTitle->setText(tr(u8"项目设置：%0").arg(projectResolver->getProjectFileName()));

        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInFifoPath0, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInFifoPath1, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInFifoPath2, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInFifoPath3, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInMemoryPath0, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInMemoryPath1, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInMemoryPath2, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInMemoryPath3, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingInMemoryPath4, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingOnlyRsm, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingRMode0, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingRMode1, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingGap, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingSource, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingDest, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingBurstNum, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingLoopNum, true);
        ui->tableView_projectSetting->setColumnHidden(ProjectSettingRepeatNum, true);
        ui->tableView_projectSetting->setColumnHidden(projectSettingLFSRGroup, true);
    }
    else
    {
        delete m_projectSettingModel;
        m_projectSettingModel = nullptr;
        ui->comboBox_projectSettingSetSolution->setCurrentIndex(0);
        ui->label_projectSettingTitle->setText(tr(u8"项目设置："));
    }
    ui->pushButton_proSettingSave->setEnabled(false);
    connect(ui->comboBox_projectSettingSetSolution, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSolutionTypeChanged()));
    connect(ui->comboBox_lfsrMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSolutionTypeChanged()));
    connect(ui->comboBox_lfsrWidthDeep, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setSolutionTypeChanged()));
}

void MainWindow::refreshProjectSettingTable()
{
    if(m_projectSettingModel)
    {
        m_projectSettingModel->refreshModel();
        ui->tableView_projectSetting->resizeColumnsToContents();
    }
}

void MainWindow::deleteResourceEditorTable()
{
    if(m_resourceEditorTableModel)
        delete m_resourceEditorTableModel;
    m_resourceEditorTableModel = nullptr;
    ui->widget_resourceEdit->setEnabled(false);
    ui->widget_benesEditor->setVisible(false);
}

void MainWindow::currentActiveProjectChanged()
{
    m_curProjectResolver = m_projectTreeModel->getActiveProjectResolver();
    //    updateActions();
}

void MainWindow::xmlFileRenamed(const QString &oldName, const QString &newName)
{
    for(int i = 0; i < m_recentFiles.count(); ++i)  //更新最近打开列表名称
    {
        if(m_recentFiles.at(i) == oldName)
        {
            m_recentFiles.replace(i, newName);
            updateRecentFileActions();
            return;
        }
    }
}

void MainWindow::srcFileRenamed(const QString &oldName, const QString &newName)
{
    if(m_resourceEditorTableModel && m_resourceEditorTableModel->rename(oldName, newName))
    {
        ui->label_resourceEditTitle->setText(tr(u8"资源编辑：%0").arg(newName));
    }
}

void MainWindow::projectFileRenamed(const QString &oldName, const QString &newName)
{
    if(ui->label_runResultProjectName->text() == tr(u8"项目设置：%0").arg(oldName))
        ui->label_runResultProjectName->setText(tr(u8"项目设置：%0").arg(newName));
}

void MainWindow::solutionFileRenamed(const QString &oldName, const QString &newName)
{
    for(int i = 0; i < m_recentPros.count(); ++i)  //更新最近打开列表名称
    {
        if(m_recentPros.at(i) == oldName)
        {
            m_recentPros.replace(i, newName);
            updateRecentProjectActions();
            return;
        }
    }
}

void MainWindow::renameFileFinished(const QModelIndex &index)
{
    m_renameFlag = false;
    if(index.isValid())
        ui->treeView_project->expand(index);
}

void MainWindow::resetResourceSelectTable()
{
    CProjectResolver *projectResolver = m_projectTreeModel->getActiveProjectResolver();
    ui->listWidget_resource->clear();
    if(!projectResolver)
    {
        return;
    }
    QMapIterator<QString, QString> i(projectResolver->getProjectParameter()->resourceMap);
    while(i.hasNext())
    {
        i.next();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_resource);
        QString showName = i.key().mid(2);
        item->setText(showName);
        item->setData(Qt::UserRole, i.value());
        item->setSizeHint(QSize(80, 80) * CAppEnv::m_dpiFactor);
        QIcon icon;
        QString suffix = QFileInfo(i.value()).suffix();
        if(QFileInfo(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower()).exists())
            icon = QIcon(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower());
        else
        {
            QFile file(i.value());
            QFileIconProvider fileIcon;
            icon = fileIcon.icon(file);
        }
        item->setIcon(icon);
        item->setToolTip(i.value());
    }
}

void MainWindow::projectParameterSetDirty()
{
    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    if(m_curProjectResolver)
        m_curProjectResolver->setDirty();
    ui->pushButton_proSettingSave->setEnabled(true);
    if(!ui->label_projectSettingTitle->text().endsWith("*"))
        ui->label_projectSettingTitle->setText(ui->label_projectSettingTitle->text() + "*");
    ui->label_forecastClock->clear(); //清空预估周期
}

void MainWindow::resourceEditorSetDirty()
{
    ui->pushButton_saveResource->setEnabled(true);
    if(!ui->label_resourceEditTitle->text().endsWith("*"))
        ui->label_resourceEditTitle->setText(ui->label_resourceEditTitle->text() + "*");
}

void MainWindow::resourceEditorSaved(const QString &fileName)
{
    ui->pushButton_saveResource->setEnabled(false);
    ui->label_resourceEditTitle->setText(tr(u8"资源编辑：%0").arg(fileName));
}

void MainWindow::resourceEditorClosed()
{
    ui->label_resourceEditTitle->setText(tr(u8"资源编辑："));
}

void MainWindow::viewLastRunResult()
{
    CProjectResolver *projectResolver = m_projectTreeModel->getProjectResolver(ui->treeView_project->currentIndex());
    if(!projectResolver)
        return;
    updateRunResult(projectResolver);
}

void MainWindow::receiveMessageFromServer(const QString &msg)
{
    QMessageBox::information(this, tr(u8"RPU Server"), msg);
}

void MainWindow::receiveNotifyFromServer(const QString &msg)
{
    m_sysTray->showMessage(qApp->applicationName(), msg, QSystemTrayIcon::Information, 1000);
}

void MainWindow::on_action_Update_triggered()
{
    if(m_tcpClient->getIsRegister())
    {
        emit versionRequest();
    }
    else
    {
        QMessageBox::information(this, qApp->applicationName(), tr(u8"更新应用程序失败，未能连接到服务器。"));
    }
}

void MainWindow::checkLicense()
{
#ifdef EVALUATION_VERSION
    // zhangjun modified at 20210512，不再校验序列号，改成日期控制
    const QDate buildDate = QLocale( QLocale::English ).toDate( QString( __DATE__ ).replace( "  ", " 0" ), "MMM dd yyyy");
    if(QDate::currentDate().daysTo(buildDate) > 30)
    {
        QMessageBox::information(this, tr(u8"错误"),
                                 tr(u8"<p>RPU Designer v%1 评估版已过评估期限！").arg(qApp->applicationVersion()));
        QTimer::singleShot(0, qApp, SLOT(quit()));
    }
#else
    if(!CAppEnv::checkLicense())
    {
        LicenseForm form(CAppEnv::m_license.toUpper(), this);
        CAppEnv::animation(&form, this);
        if(QDialog::Rejected == form.exec())
        {
            QMessageBox::information(this, tr(u8"RPU Server"),
                                     tr(u8"<p>产品密钥错误或试用已过期！"));
            QTimer::singleShot(0, qApp, SLOT(quit()));
        }
        else
        {
            emit licenseChanged();
        }
    }
#endif
}

void MainWindow::versionAck(const bool &oldVersion, const QString &msg)
{
    if(oldVersion)
    {
        bool update = CSingleUpdateMessageBox::readyToShow(QMessageBox::Information,
                                                           tr(u8"检测到新版本：v%0，是否升级？").arg(msg), this);
        //        int ret = QMessageBox::information(this, qApp->applicationName(),
        //                                           tr(u8"检测到新版本：v%0，是否升级？").arg(msg),
        //                                           QMessageBox::Yes | QMessageBox::No);
        if (update)
        {
            //升级
            UpdateProgressForm form(this);
            if(QDialog::Accepted == form.exec())
            {
                const QString &fileName = form.getFileName();
#if defined(Q_OS_WIN)
                QDesktopServices::openUrl(QUrl(tr(u8"file:///%0").arg(fileName)));
#elif defined(Q_OS_UNIX)
                QFile file(fileName);
                if(!file.setPermissions(QFileDevice::ReadOwner | QFileDevice::ExeOwner)
                        || !QProcess::startDetached(fileName))
                {
                    const int &ret = QMessageBox::information(this, qApp->applicationName(),
                                                              tr(u8"新版本程序已下载至“%0”，点“Yes”键查看。").arg(fileName),
                                                              QMessageBox::Yes | QMessageBox::No);
                    if (ret == QMessageBox::Yes)
                    {
                        QString filePath = QFileInfo(fileName).absolutePath();
                        QDesktopServices::openUrl(QUrl("file:///" + filePath));
                    }
                }
#endif
            }
        }
    }

    else
    {
        QMessageBox::information(this, qApp->applicationName(), msg);
    }
}

void MainWindow::serverCloseProgram(const QString &msg)
{
    QMessageBox::information(this, tr(u8"RPU Server"),
                             tr(u8"<p>%0"
                                "<p>120秒后程序将退出！"
                                ).arg(msg));
    QTimer::singleShot(120000, qApp, SLOT(quit()));
}

void MainWindow::on_action_community_triggered()
{
    clientListRequest();
}

void MainWindow::clientListRequest()
{
    m_clientDialog->on_pushButton_refreshSharedFile_clicked();
    m_clientDialog->exec();
}

void MainWindow::transferFinished()
{
    CAppEnv::showHintMessage(tr("解决方案共享成功！"), this);
}

void MainWindow::handleMessage(const QString &msg)
{
    QStringList algList;
    algList.append(msg);
    if(msg.endsWith(".rpusln", Qt::CaseInsensitive))
    {
        loadRpuSln(algList);
    }
    else if(msg.endsWith(".xml", Qt::CaseInsensitive))
    {
        loadFiles(algList);
    }
    //    else
    //    {
    //        QMessageBox::critical(this, qApp->applicationName(), tr(u8"不支持此文件：%0").arg(msg));
    //    }
}

//void MainWindow::fileChanged(QString fileName)
//{
//    int &&ret = QMessageBox::information(this, qApp->applicationName(),
//                                       tr(u8"文件“%0”在%1以外被改写，是否重新载入？")
//                                       .arg(fileName).arg(qApp->applicationName()),
//                                       QMessageBox::Yes | QMessageBox::No);
//    if (ret == QMessageBox::Yes)
//    {
//        QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
//        for(int i = 0; i < winList.count(); ++i)
//        {
//            RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
//            CProjectResolver *projectResolver = rcaGraphView->getProject();
//            if(rcaGraphView->getCurFile() == fileName)
//            {
//                winList.at(i)->close();
//                openFile(fileName, projectResolver);
//                break;
//            }
//        }
//    }
//}

/*!
 * MainWindow::loadFiles
 * \brief   载入XML文件
 * \param   list
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::loadFiles(const QStringList &list)
{
    qInfo() << tr(u8"载入XML文件");
    if (!list.isEmpty())
    {
        foreach (QString str, list)
        {
            if(ui->mdiArea->subWindowList().count() >= m_setting.maxSubWin)
            {
                qInfo() << tr(u8"载入XML文件：打开窗口数已达上限");
                QMessageBox::information(this, tr(u8"打开"), tr(u8"打开窗口数已达上限！"));
                return;
            }
            str.replace("\\", "/");
            openXmlFile(str);
        }
    }
}

/*!
 * MainWindow::on_action_NewFile_triggered
 * \brief   新建XML文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_NewFile_triggered()
{
    qInfo() << tr(u8"新建XML文件");
    if(ui->mdiArea->subWindowList().count() >= m_setting.maxSubWin)
    {
        QMessageBox::information(this, tr(u8"打开"), tr(u8"打开窗口数已达上限！"));
        return;
    }

    m_curRcaGraphView = new RcaGraphView(&m_customPEConfigHash, this);
    m_curRcaGraphView->newFile();
    m_curFileName.clear();
    connect(m_curRcaGraphView, SIGNAL(statusChanged(rca_space::RcaSceneStates)),
            this, SLOT(xmlEditorEditStatusChanged(rca_space::RcaSceneStates)));
    connect(m_curRcaGraphView, SIGNAL(RcaGraphViewClose(QString)),
            this, SLOT(xmlEditorClosed(QString)));
//    connect(m_curRcaGraphView, SIGNAL(shownItemBeUndo(BaseItem *)),
//            this, SLOT(refreshItemAttribute(BaseItem *)));
//    connect(m_curRcaGraphView, SIGNAL(viewItemBeDoubleClicked(BaseItem *)),
//            this, SLOT(showItemAttribute(BaseItem *)));  //FIXME
//    connect(m_curRcaGraphView, SIGNAL(displayedItemBeDelected(bool)),
//            ui->dockWidget_attribute, SLOT(setVisible(bool)));
    connect(m_curRcaGraphView, SIGNAL(mousePosChanged(QPoint)),
            m_viewInfoStatuBar, SLOT(setMousePos(QPoint)));
    connect(m_curRcaGraphView, SIGNAL(percentChanged(int)),
            m_viewInfoStatuBar, SLOT(setZoomValue(int)));
    connect(m_curRcaGraphView, SIGNAL(selectedItemsAttrChanged(rca_space::SelectedItemsAttr)),
            this, SLOT(updateActions(rca_space::SelectedItemsAttr)));
    connect(m_curRcaGraphView, SIGNAL(dropXml(QStringList,QStringList)),
            this, SLOT(dropXmlToXmlEditor(QStringList,QStringList)));
    connect(m_curRcaGraphView, SIGNAL(addBreakPoint(const rpu_esl_model::SBreak &)),
            this, SLOT(simDebugBreakAdd(const rpu_esl_model::SBreak &)));
    connect(m_curRcaGraphView, SIGNAL(removeBreakPoint(const rpu_esl_model::SBreak &)),
            this, SLOT(simDebugBreakRemove(const rpu_esl_model::SBreak &)));
    m_curRcaGraphView->setGridViewVisable(m_setting.showGrid);
    m_curRcaGraphView->setGridViewStep(m_setting.gridPoint);
    xmlEditorEditStatusChanged(m_curRcaGraphView->getStatus());
    addXmlEditor(m_curRcaGraphView);
}

/*!
 * MainWindow::openFile
 * \brief
 * \param   fileName
 * \param   isProject
 * \author  zhangjun
 * \date    2016-10-12
 */
bool MainWindow::openXmlFile(const QString &fileName, CProjectResolver *proResolver, bool convertOnly)
{
    qInfo() << tr(u8"打开XML文件：%0").arg(fileName);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if(fileName.isEmpty() || !QFileInfo(fileName).exists())
    {
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"文件“%0”不存在！").arg(fileName));
        QApplication::restoreOverrideCursor();
        return false;
    }

    //是否已打开同名文件
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView->getCurFile() == fileName)
        {
            rcaGraphView->getAction()->triggered();
            ui->stackedWidget->setCurrentIndex(1);
            if(proResolver)
            {
                rcaGraphView->setProject(proResolver);
            }
            QApplication::restoreOverrideCursor();
            return false;
        }
    }

    if(ui->mdiArea->subWindowList().count() >= m_setting.maxSubWin)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, tr(u8"打开"), tr(u8"打开窗口数已达上限！"));
        return false;
    }

    //打开校验
    if(!CAppEnv::m_adminUser && m_setting.xmlMd5Check)
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QApplication::restoreOverrideCursor();
            return false;
        }
        QTextStream in(&file);
        QString fileContent = in.readAll();
        if(!fileContent.trimmed().isEmpty())
        {
            QString md5InFile = fileContent.mid(fileContent.indexOf("MD5=\"") + 5);
            md5InFile = md5InFile.left(md5InFile.indexOf("\""));
            fileContent.remove(md5InFile);
            QByteArray bty;
            bty.append(fileContent/* + "zhangjun"*/);
            QByteArray md5Code = QCryptographicHash::hash(bty, QCryptographicHash::Md5);
            QString md5 = md5Code.toHex();
            if(md5 != md5InFile)
            {
                qInfo() << tr(u8"打开XML文件：在试图打开文件时遇到错误，文件已被损坏");
                QApplication::restoreOverrideCursor();
                QMessageBox::warning(this, qApp->applicationName(),
                                     tr(u8"在试图打开文件时遇到错误，文件“%0”已被损坏！").arg(fileName));
                return false;
            }
        }
    }
    else if(!CAppEnv::m_adminUser)
    {
        m_setting.xmlMd5Check = true;
    }

    // zhangjun added at 20200810, for testing code editor
    if(QFileInfo(fileName).suffix().toLower() == "c")
    {
        QApplication::restoreOverrideCursor();
        QDialog *dlg = new QDialog(this);
        QHBoxLayout *layout = new QHBoxLayout(dlg);
        CFileEditor *edit = new CFileEditor(dlg);
        edit->open(fileName);
        layout->addWidget(edit);
        dlg->setLayout(layout);
        dlg->resize(800, 600);
        dlg->exec();
        return true;
    }

    m_curRcaGraphView = new RcaGraphView(&m_customPEConfigHash, this);

    //    是否为项目内的XML
    if(proResolver)
    {
        m_curRcaGraphView->setProject(proResolver);
    }
    else
    {
        m_curRcaGraphView->setProject(m_projectTreeModel->getNearestProjectResolverFromXml(fileName));
    }

    if(!m_curRcaGraphView->open(fileName))
    {
        delete m_curRcaGraphView;
        m_curRcaGraphView = nullptr;
        qInfo() << tr(u8"打开XML文件：在试图打开文件时遇到错误");
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, qApp->applicationName(),
                             tr(u8"在试图打开文件“%0”时遇到错误！").arg(fileName));
        return false;
    }

    //log文件载入到xml zhangjun add at 20200902
    if(convertOnly){
        QFileInfo fileInfo(fileName);
        if(fileInfo.suffix().toLower() == "rpulog") {
            m_curRcaGraphView->close();
            delete m_curRcaGraphView;
            m_curRcaGraphView = nullptr;
            QApplication::restoreOverrideCursor();
            return true;
        }
    }

    // 是否在调试中的文件，进入调试状态
    const auto icend =  m_curDebugViewMap.constEnd();
    for(auto it = m_curDebugViewMap.constBegin(); it != icend; ++it) {
        if(QFileInfo(it.value().first) == QFileInfo(fileName))
        {
            m_curRcaGraphView->startDebugMode();
            break;
        }
    }

    connect(m_curRcaGraphView, SIGNAL(statusChanged(rca_space::RcaSceneStates)),
            this, SLOT(xmlEditorEditStatusChanged(rca_space::RcaSceneStates)));
    connect(m_curRcaGraphView, SIGNAL(RcaGraphViewClose(QString)),
            this, SLOT(xmlEditorClosed(QString)));
//    connect(m_curRcaGraphView, SIGNAL(shownItemBeUndo(BaseItem *)),
//            this, SLOT(refreshItemAttribute(BaseItem *)));
//    connect(m_curRcaGraphView, SIGNAL(viewItemBeDoubleClicked(BaseItem *)),
//            this, SLOT(showItemAttribute(BaseItem *))); //FIXME
//    connect(m_curRcaGraphView, SIGNAL(displayedItemBeDelected(bool)),
//            ui->dockWidget_attribute, SLOT(setVisible(bool)));
    connect(m_curRcaGraphView, SIGNAL(mousePosChanged(QPoint)),
            m_viewInfoStatuBar, SLOT(setMousePos(QPoint)));
    connect(m_curRcaGraphView, SIGNAL(percentChanged(int)),
            m_viewInfoStatuBar, SLOT(setZoomValue(int)));
    connect(m_curRcaGraphView, SIGNAL(selectedItemsAttrChanged(rca_space::SelectedItemsAttr)),
            this, SLOT(updateActions(rca_space::SelectedItemsAttr)));
    connect(m_curRcaGraphView, SIGNAL(dropXml(QStringList,QStringList)),
            this, SLOT(dropXmlToXmlEditor(QStringList,QStringList)));
    m_curRcaGraphView->setGridViewVisable(m_setting.showGrid);
    m_curRcaGraphView->setGridViewStep(m_setting.gridPoint);
    m_curRcaGraphView->showAllWire();
    xmlEditorEditStatusChanged(m_curRcaGraphView->getStatus());
    addXmlEditor(m_curRcaGraphView);
    updateRecentFileList(m_curRcaGraphView->getCurFile());
    QApplication::restoreOverrideCursor();
    return true;
}


/*!
 * MainWindow::openSrcFile
 * \brief   打开资源文件
 * \param   fileName
 * \author  zhangjun
 * \date    2016-11-1
 */
void MainWindow::openSrcFile(const QString &projectName, const QString &fileName, QString type)
{
    qInfo() << tr(u8"打开资源文件：%0").arg(fileName);
    if(fileName.isEmpty())
        return;

    if(!m_resourceEditorTableModel)
    {
        m_resourceEditorTableModel = new CResourceEditorTableModel(this);
        connect(m_resourceEditorTableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                this, SLOT(resourceEditorSetDirty()));
        connect(m_resourceEditorTableModel, SIGNAL(resourceSaved(QString)),
                this, SLOT(resourceEditorSaved(QString)));
        connect(m_resourceEditorTableModel, SIGNAL(editorClosed()),
                this, SLOT(resourceEditorClosed()));
        ui->tableView_resourceEditor->setModel(m_resourceEditorTableModel);

        CBenesEditorSpinBoxDelegate *delegate = new CBenesEditorSpinBoxDelegate(this);
        ui->tableView_benesEditor->setItemDelegate(delegate);
        ui->tableView_benesEditor->setModel(m_resourceEditorTableModel->getBenesEditorTableModel());
    }
    Q_ASSERT(m_resourceEditorTableModel);

    if(m_resourceEditorTableModel->getFileName() == fileName)
    {
        ui->stackedWidget->setCurrentIndex(2);
        return;
    }
    bool ret = m_resourceEditorTableModel->open(projectName, fileName, m_setting.srcMd5Check,
                                                ui->spinBox_maxColumn->value(),
                                                ui->comboBox_addressInterval->currentIndex(), type);
    if(!CAppEnv::m_adminUser && !m_setting.srcMd5Check)
    {
        m_setting.srcMd5Check = true;
    }
    if(!ret)
    {
        return;
    }

    ui->tableView_resourceEditor->resizeColumnsToContents();
    ui->tableView_benesEditor->resizeColumnsToContents();
    ui->stackedWidget->setCurrentIndex(2);

    type = m_resourceEditorTableModel->getType();
    ui->widget_resourceEdit->setEnabled(true);
    if(type == "fifo" || type == "sbox")
    {
        if(type == "sbox")
        {
            ui->checkBox_sboxShowInOneLine->setEnabled(true);
            ui->checkBox_sboxShowInOneLine->setChecked(true);
        }
        else
        {
            ui->checkBox_sboxShowInOneLine->setEnabled(false);
            ui->checkBox_sboxShowInOneLine->setChecked(false);
        }
        if(type == "fifo")
        {
            ui->pushButton_addFifoResourceRowBefore->setEnabled(true);
            ui->pushButton_addFifoResourceRowAfter->setEnabled(true);
            ui->pushButton_deleteFifoResourceRow->setEnabled(true);
            ui->comboBox_addressInterval->setEnabled(false);
            ui->label_3->setEnabled(false);
            ui->spinBox_maxColumn->setValue(4);
        }
        else
        {
            ui->pushButton_addFifoResourceRowBefore->setEnabled(false);
            ui->pushButton_addFifoResourceRowAfter->setEnabled(false);
            ui->pushButton_deleteFifoResourceRow->setEnabled(false);
            ui->comboBox_addressInterval->setEnabled(true);
            ui->label_3->setEnabled(true);
        }
        ui->widget_benesEditor->setVisible(false);
        ui->spinBox_maxColumn->setEnabled(false);
        ui->label_5->setEnabled(false);
    }
    else
    {
        if(type == "benes")
        {
            ui->widget_benesEditor->setVisible(true);
        }
        else
        {
            ui->widget_benesEditor->setVisible(false);
        }
        ui->checkBox_sboxShowInOneLine->setEnabled(false);
        ui->checkBox_sboxShowInOneLine->setChecked(false);
        ui->pushButton_addFifoResourceRowBefore->setEnabled(false);
        ui->pushButton_addFifoResourceRowAfter->setEnabled(false);
        ui->pushButton_deleteFifoResourceRow->setEnabled(false);
        ui->spinBox_maxColumn->setEnabled(true);
        ui->label_5->setEnabled(true);
        ui->comboBox_addressInterval->setEnabled(true);
        ui->label_3->setEnabled(true);
    }

//    on_pushButton_benesEditorLoad_clicked();
    ui->pushButton_saveResource->setEnabled(false);
    ui->label_resourceEditTitle->setText(tr(u8"资源编辑：%0").arg(fileName));
}

void MainWindow::on_spinBox_maxColumn_valueChanged(int arg1)
{
    if(!m_resourceEditorTableModel)
    {
        return;
    }
    m_resourceEditorTableModel->setColumn(arg1);
    ui->tableView_resourceEditor->resizeColumnsToContents();
}

void MainWindow::on_comboBox_addressInterval_currentIndexChanged(int index)
{
    if(!m_resourceEditorTableModel)
    {
        return;
    }
    m_resourceEditorTableModel->setAddressInterval(index);
    ui->tableView_resourceEditor->resizeColumnsToContents();
}

void MainWindow::on_pushButton_addFifoResourceRowBefore_clicked()
{
    if(!m_resourceEditorTableModel)
    {
        return;
    }
    int rowCount = m_resourceEditorTableModel->rowCount(QModelIndex());
    int row = 0;
    const QModelIndex &index = ui->tableView_resourceEditor->currentIndex();
    if(index.isValid() && rowCount > 0)
    {
        row = index.row();
    }
    m_resourceEditorTableModel->insertRows(row, 1, QModelIndex());
    ui->tableView_resourceEditor->resizeColumnsToContents();
    ui->tableView_resourceEditor->setFocus();
    QModelIndex &&newIndex = m_resourceEditorTableModel->index(row, 0);
    ui->tableView_resourceEditor->setCurrentIndex(newIndex);
    ui->tableView_resourceEditor->edit(newIndex);
}

void MainWindow::on_pushButton_addFifoResourceRowAfter_clicked()
{
    if(!m_resourceEditorTableModel)
    {
        return;
    }
    int rowCount = m_resourceEditorTableModel->rowCount(QModelIndex());
    int row = 0;
    const QModelIndex &index = ui->tableView_resourceEditor->currentIndex();
    if(index.isValid() && rowCount > 0)
    {
        row = index.row() + 1;
    }
    m_resourceEditorTableModel->insertRows(row, 1, QModelIndex());
    ui->tableView_resourceEditor->resizeColumnsToContents();
    ui->tableView_resourceEditor->setFocus();
    QModelIndex &&newIndex = m_resourceEditorTableModel->index(row, 0);
    ui->tableView_resourceEditor->setCurrentIndex(newIndex);
    ui->tableView_resourceEditor->edit(newIndex);
}

void MainWindow::on_pushButton_deleteFifoResourceRow_clicked()
{
    QItemSelectionModel *selectionModel = ui->tableView_resourceEditor->selectionModel();
    if(!selectionModel)
    {
        return;
    }
    QModelIndexList &&indexList = selectionModel->selectedIndexes();
    if(indexList.count() < 1)
    {
        return;
    }
    QList<int> rowList;
    //    foreach (QModelIndex index, indexList)
    for(int i = 0; i < indexList.count(); ++i)
    {
        const QModelIndex &index = indexList.at(i);
        bool isExist = false;
        foreach (int row, rowList)
        {
            if(row == index.row())
            {
                isExist = true;
                break;
            }
        }
        if(!isExist)
        {
            rowList.append(index.row());
        }
    }
    qSort(rowList);
    int row = rowList.at(0);
    int count = rowList.count();
    m_resourceEditorTableModel->removeRows(row, count, QModelIndex());
}

void MainWindow::on_pushButton_saveResource_clicked()
{
    if(m_resourceEditorTableModel)
    {
        m_resourceEditorTableModel->save();
    }
}

void MainWindow::on_checkBox_sboxShowInOneLine_clicked(bool checked)
{
    if(!m_resourceEditorTableModel)
    {
        return;
    }
    if(checked)
    {
        ui->spinBox_maxColumn->setValue(4);
    }
    ui->spinBox_maxColumn->setEnabled(!checked);
    ui->label_5->setEnabled(!checked);
    m_resourceEditorTableModel->setShowInLine(checked);
    ui->tableView_resourceEditor->resizeColumnsToContents();
}

void MainWindow::on_pushButton_benesEditorGenerate_clicked()
{
    if(m_resourceEditorTableModel)
    {
        m_resourceEditorTableModel->generateBenes();
    }
}


void MainWindow::on_pushButton_benesEditorLoad_clicked()
{
    if(m_resourceEditorTableModel)
    {
        m_resourceEditorTableModel->loadBenesConfig();
    }
}

void MainWindow::on_pushButton_benesEditorReset_clicked()
{
    if(m_resourceEditorTableModel)
    {
        m_resourceEditorTableModel->getBenesEditorTableModel()->resetValue();
    }
}

/*!
 * MainWindow::on_action_Open_triggered
 * \brief   打开XML文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Open_triggered()
{
    qInfo() << tr(u8"打开XML文件……");
    QStringList fileNameList = QFileDialog::getOpenFileNames(this, tr(u8"打开"),
                                                             /*CAppEnv::getXmlOutDirPath()*/"",
                                                             tr(u8"XML文件 (*.xml);;C文件 (*.c);;RPU Log 文件 (*.rpulog)"));
    if (fileNameList.isEmpty())
        return;

    loadFiles(fileNameList);
}

/*!
 * MainWindow::RcaGraphViewClose
 * \brief   关闭RcaGraphView
 * \param   str
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::xmlEditorClosed(const QString &str)
{
    qInfo() << tr(u8"XML窗口关闭");
    if(m_openedFiles.removeOne(str))
    {
        m_openedFileListModel->setStringList(m_openedFiles);
    }
}

/*!
 * MainWindow::closeSubWindow
 * \brief
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::closeXmlEditorSubWindow()
{
    qInfo() << tr(u8"关闭子窗口");
    int row = ui->listView_openedFile->currentIndex().row();
    QString fileName = m_openedFiles.at(row);

    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView->getCurFile() == fileName)
        {
            winList.at(i)->close();
            break;
        }
    }
}


/*!
 * MainWindow::setCurrentFile
 * \brief   设置当前XML文件
 * \param   fileName
 * \param   saveAs
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::updateRecentFileList(const QString &fileName, bool saveAs)
{
    if (!fileName.isEmpty()) {
        m_recentFiles.removeAll(fileName);
        m_recentFiles.prepend(fileName);

        updateRecentFileActions();

        if(m_openedFiles.contains(fileName))
        {
            m_openedFileListModel->setStringList(m_openedFiles);
            return;
        }
        if(saveAs)
            m_openedFiles.removeAll(m_curFileName);
        m_openedFiles.append(fileName);
        m_openedFileListModel->setStringList(m_openedFiles);
        m_curFileName = fileName;
        ui->listView_openedFile->reset();
    }
}

/*!
 * MainWindow::setCurrentPro
 * \brief   更新最近打开项目列表
 * \param   proName
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::updateRecentProjectList(const QString &projectName)
{
    if (!projectName.isEmpty()) {
        m_recentPros.removeAll(projectName);
        m_recentPros.prepend(projectName);
        updateRecentProjectActions();
    }
}

void MainWindow::on_pushButton_sessionManager_clicked()
{
    on_action_sessionManager_triggered();
}


void MainWindow::on_action_sessionManager_triggered()
{
    SessionManagerForm form(m_sessionManager, this);
    CAppEnv::animation(&form, this);
    form.exec();
}

void MainWindow::updateCurrentSession()
{
    RcaGraphView *view = currentXmlEditor();
    QByteArray bty;
    QDataStream out(&bty, QIODevice::WriteOnly);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    out.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    out.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    out.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    out.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    out.setVersion(QDataStream::Qt_5_6);
#endif
    out << m_sessionManager->getCurrentSessionName()
        << (view ? view->getCurFile() : QString())
        << QDateTime::currentDateTime()
        << (view ? view->horizontalScrollBar()->sliderPosition() : 0)
        << (view ? view->verticalScrollBar()->sliderPosition() : 0) << m_openedFiles
        << (m_projectTreeModel ? m_projectTreeModel->getActiveSolutionName() : QString())
        << (m_projectTreeModel ? m_projectTreeModel->getActiveProjectFileName() : QString())
        << (m_projectTreeModel ? m_projectTreeModel->getSolutionList() : QStringList())
        << (m_resourceEditorTableModel ? m_resourceEditorTableModel->getProjectName() : QString())
        << (m_resourceEditorTableModel ? m_resourceEditorTableModel->getFileName() : QString())
        << ui->stackedWidget->currentIndex();
    m_sessionManager->updateSession(bty);

    //test
    //    QDataStream in(&bty, QIODevice::ReadOnly);
    //#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    //    in.setVersion(QDataStream::Qt_5_10);
    //#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    //    in.setVersion(QDataStream::Qt_5_9);
    //#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    //    in.setVersion(QDataStream::Qt_5_8);
    //#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    //    in.setVersion(QDataStream::Qt_5_7);
    //#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    //    in.setVersion(QDataStream::Qt_5_6);
    //#endif
    //    CSession session;
    //    in >> session;
    //    DBG << session.m_sessionName << session.m_curXml << session.m_lastModify
    //        << session.m_horizontalScrollBarPos << session.m_verticalScrollBarPos
    //        << session.m_openedXmlList << session.m_curSln << session.m_curProject
    //        << session.m_openedSlnList << session.m_curSrcProject
    //        << session.m_curSrc << session.m_curStackedWidgetIndex;
}

void MainWindow::addSessionAction(QAction *action)
{
    m_sessionActionGroup->addAction(action);
    ui->menu_session->addAction(action);
}

void MainWindow::loadSessionToUI()
{
    CSession *s = m_sessionManager->getCurrentSession();
    loadSessionToUI(s);
}

void MainWindow::loadSessionToUI(CSession *s)
{
    on_action_closeAllSlnAndEditor_triggered();
    loadRpuSln(s->getOpenedSlnList());
    m_projectTreeModel->setActiveProject(s->getCurSln(), s->getCurProject());
    loadFiles(s->getOpenedXmlList());
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView->getCurFile() == s->getCurXml())
        {
            ui->mdiArea->setActiveSubWindow(winList.at(i));
            rcaGraphView->horizontalScrollBar()->setSliderPosition(s->getHorizontalScrollBarPos());
            rcaGraphView->verticalScrollBar()->setSliderPosition(s->getVerticalScrollBarPos());
            break;
        }
    }
    openSrcFile(s->getCurSrcProject(), s->getCurSrc());
    if(s->getCurStackedWidgetIndex() < 0 || s->getCurStackedWidgetIndex() > 3
            || (!currentXmlEditor() && s->getCurStackedWidgetIndex() == 1))
        s->setCurStackedWidgetIndex(0);
    ui->stackedWidget->setCurrentIndex(s->getCurStackedWidgetIndex());
}

void MainWindow::loadDebugSessionToUI(const CDebugSession &debugSession)
{
    on_action_connectToDevice_triggered();
    ui->action_jointDebugMode->setChecked(true);
    runOrDebugTriggered("debug");
    if(!CChipDebug::getInstance(this)->getIsDebugging())
    {
        QMessageBox::critical(this, tr("错误"), tr("恢复硬件调试状态失败：启动硬件调试错误！"));
        return;
    }
    if(m_curProjectResolver->getProjectFileName() != m_debugSession->getCurrentProjectName())
    {
        QMessageBox::critical(this, tr("错误"), tr("需恢复的调试项目不是当前打开的项目！"));
        return;
    }

    m_debugLog->getDebugLogView()->horizontalScrollBar()->setSliderPosition(m_debugSession->getHorizontalScrollBarPos());
    m_debugLog->getDebugLogView()->verticalScrollBar()->setSliderPosition(m_debugSession->getVerticalScrollBarPos());
//    if(m_debugSession->stackedWidgetIndex() < 0 || m_debugSession->stackedWidgetIndex() > 3)
//        m_debugSession->setStackedWidgetIndex(0);
    ui->stackedWidget->setCurrentIndex(m_debugSession->getStackedWidgetIndex());
    CChipDebug::getInstance(this)->restoreDebugSession(debugSession);
}

/*!
 * MainWindow::autoSave
 * \brief   定时保存
 * \author  zhangjun
 * \date    2016-10-18
 */
void MainWindow::xmlEditorAutoSave()
{
    qInfo() << tr(u8"定时保存");
    RcaGraphView *gui = currentXmlEditor();
    if (gui && gui->getIsModified() && !gui->getIsUntitled())
    {
        if(gui->save())
            updateRecentFileList(gui->getCurFile());
    }
}

void MainWindow::on_action_changeLicense_triggered()
{
    LicenseForm form(CAppEnv::m_license, this);
    CAppEnv::animation(&form, this);
    if(QDialog::Accepted == form.exec())
    {
        emit licenseChanged();
    }
}

/*!
 * MainWindow::on_action_Save_triggered
 * \brief   保存XML
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Save_triggered()
{
    qInfo() << tr(u8"保存XML");
    RcaGraphView *rcaGraphView = activeXmlEditor();
    if (rcaGraphView)
    {
        if(rcaGraphView->save())
        {
            updateRecentFileList(rcaGraphView->getCurFile());
        }
        else
        {
            QMessageBox::critical(this, qApp->applicationName(), tr(u8"保存到文件失败！"));
        }
    }
}

/*!
 * MainWindow::on_action_SaveAs_triggered
 * \brief   另存为
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_SaveAs_triggered()
{
    qInfo() << tr(u8"XML另存为……");
    RcaGraphView *gui = activeXmlEditor();
    if (gui)
    {
        if(gui->saveAs())
            updateRecentFileList(gui->getCurFile(), true);
    }
}

/*!
 * MainWindow::on_action_SaveAll_triggered
 * \brief   保存所有已打开XML
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool MainWindow::on_action_SaveAll_triggered()
{
    qInfo() << tr(u8"保存所有已打开的XML");
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();

    bool isSavedALL = true;
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(!rcaGraphView->save())
            isSavedALL = false;
        else
            updateRecentFileList(rcaGraphView->getCurFile());
    }
    return isSavedALL;
}


/*!
 * MainWindow::activeWindowChanged
 * \brief   当前激活窗口改变
 * \param   subWindow
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::activeXmlEditorChanged(QMdiSubWindow* subWindow)
{
    //    qInfo() << tr(u8"当前激活窗口改变");
    if(ui->stackedWidget->currentIndex() != 0 && ui->mdiArea->subWindowList().count() < 1)
    {
        disconnect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
                   this, SLOT(activeXmlEditorChanged(QMdiSubWindow*)));
        if(!m_rpuWave || !m_rpuWave->isVisible())
            ui->stackedWidget->setCurrentIndex(0);
        ui->dockWidget_scaleView->setWidget(ui->frame_4);
        ui->dockWidget_attribute->setWidget(ui->frame_5);
        connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
                this, SLOT(activeXmlEditorChanged(QMdiSubWindow*)));
    }

    DBG << tr(u8"当前激活窗口发生改变：") << m_curRcaGraphView;

//    if(m_curRcaGraphView && subWindow && m_curRcaGraphView != subWindow->widget())
//    {
//        ui->dockWidget_attribute->hide();
//    }

    m_curRcaGraphView = activeXmlEditor(subWindow);
    if(!m_curRcaGraphView)
    {
        DBG << tr(u8"下一激活窗口为无效窗口") << m_curRcaGraphView;
        updateActions();
        setWindowFilePath("");
        return;
    }
    xmlEditorEditStatusChanged(m_curRcaGraphView->getStatus());
    m_curFileName = m_curRcaGraphView->getCurFile();
    ui->dockWidget_scaleView->setWidget(m_curRcaGraphView->getRcaScaleView());
//    ui->dockWidget_attribute->setWidget(m_curRcaGraphView->getCurItemSetting());
    ui->dockWidget_attribute->setWidget(m_curRcaGraphView->getCurPropertyBrowser());//TODO:XUFEI 设置属性界面widget
    ui->dockWidget_element->setEnabled(!m_curRcaGraphView->getIsDebugMode());
    updateActions();
    setWindowFilePath(m_curFileName + "[*] - " + QApplication::applicationName());
}

/*!
 * MainWindow::updateActions
 * \brief   更新action
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::updateActions(rca_space::SelectedItemsAttr attr)
{
    bool hasEditor;
    if(ui->mdiArea->subWindowList().count() > 0)
        hasEditor = true;
    else
        hasEditor = false;
    bool hasProject = m_projectTreeModel->hasActiveSolution();
    bool hasSolution = m_projectTreeModel->rowCount() > 0;

    RcaGraphView *xmlEditor = activeXmlEditor();
    ui->dockWidget_element->setEnabled(xmlEditor && !xmlEditor->getIsDebugMode());
    ui->dockWidget_attribute->setEnabled(xmlEditor && !xmlEditor->getIsDebugMode());
    if(xmlEditor/* && xmlEditor->hasFocus()*/)
    {
        activeXmlEditor()->getAction()->setChecked(true);
        ui->action_ShowLine->setChecked(m_curRcaGraphView->getShowAllWire());
        ui->action_ShowLine->setEnabled(true);
        ui->action_Save->setEnabled(true);
        ui->action_SaveAs->setEnabled(true);
        ui->action_SaveAll->setEnabled(true);
        ui->action_exportSvg->setEnabled(true);
        ui->action_print->setEnabled(true);
        ui->action_printPreview->setEnabled(true);
        ui->action_printScreen->setEnabled(true);
        ui->action_autoSetPosOfAllItem->setEnabled(true);
        ui->action_autoMakeAllItem->setEnabled(true);
        ui->action_moveUp->setEnabled(true);
        ui->action_moveDown->setEnabled(true);
        ui->action_moveLeft->setEnabled(true);
        ui->action_moveRight->setEnabled(true);
        ui->action_zoomOut->setEnabled(true);
        ui->action_zoomIn->setEnabled(true);
        ui->action_zoomNormal->setEnabled(true);
        ui->action_showAll->setEnabled(true);
        ui->action_autoMakeAllItem->setEnabled(true);
        ui->action_autoSetPosOfAllItem->setEnabled(true);
        ui->action_selectAll->setEnabled(true);
        ui->action_selectSame->setEnabled(true);
        ui->action_find->setEnabled(true);
        ui->action_grid->setEnabled(true);
        //        ui->action_grid->setChecked(!m_curRcaGraphView->isGridViewHidden());
        ui->listWidget_element->setEnabled(true);
        m_viewInfoStatuBar->setEnabled(true);
        //以下需要选中
        ui->action_rotate->setEnabled(true);
        ui->action_flipHorizintal->setEnabled(true);
        ui->action_flipVertical->setEnabled(true);
        ui->action_bringToFront->setEnabled(true);
        ui->action_SendToBackup->setEnabled(true);
        if(!attr)
            attr = activeXmlEditor()->getCurrentSelectedAttr();
        ui->action_align->setEnabled(attr & isEnabledAlign);
        ui->action_alignTopEdge->setEnabled(attr & isEnabledAlign);
        ui->action_alignBottomEdge->setEnabled(attr & isEnabledAlign);
        ui->action_alignLeftEdge->setEnabled(attr & isEnabledAlign);
        ui->action_alignRightEdge->setEnabled(attr & isEnabledAlign);
        ui->action_sameHeight->setEnabled(attr & isEnabledAlign);
        ui->action_sameWidth->setEnabled(attr & isEnabledAlign);
        ui->action_sameSize->setEnabled(attr & isEnabledAlign);
        ui->action_Cut->setEnabled(attr & isEnabledCut);
        ui->action_delete->setEnabled(attr & isEnabledDel);
        ui->action_Copy->setEnabled(attr & isEnabledCopy);
        ui->action_copyWithLine->setEnabled(attr & isEnabledCopy);
        ui->action_horizintalEquidistant->setEnabled(attr & isEnabledEquidistance);
        ui->action_VerticalEquidistant->setEnabled(attr & isEnabledEquidistance);
        ui->action_Redo->setEnabled(attr & isEnabledRedo);
        ui->action_Undo->setEnabled(attr & isEnabledUndo);
        ui->action_Paste->setEnabled(attr & isEnabledPaste);
    }
    else
    {
        ui->action_ShowLine->setEnabled(false);
        ui->action_Save->setEnabled(false);
        ui->action_SaveAs->setEnabled(false);
        ui->action_SaveAll->setEnabled(false);
        ui->action_exportSvg->setEnabled(false);
        ui->action_print->setEnabled(false);
        ui->action_printPreview->setEnabled(false);
        ui->action_printScreen->setEnabled(false);
        ui->action_autoSetPosOfAllItem->setEnabled(false);
        ui->action_autoMakeAllItem->setEnabled(false);
        ui->action_moveUp->setEnabled(false);
        ui->action_moveDown->setEnabled(false);
        ui->action_moveLeft->setEnabled(false);
        ui->action_moveRight->setEnabled(false);
        ui->action_zoomOut->setEnabled(false);
        ui->action_zoomIn->setEnabled(false);
        ui->action_zoomNormal->setEnabled(false);
        ui->action_showAll->setEnabled(false);
        ui->action_autoMakeAllItem->setEnabled(false);
        ui->action_autoSetPosOfAllItem->setEnabled(false);
        ui->action_Redo->setEnabled(false);
        ui->action_Undo->setEnabled(false);
        ui->action_Paste->setEnabled(false);
        ui->action_copyWithLine->setEnabled(false);
        ui->action_selectAll->setEnabled(false);
        ui->action_selectSame->setEnabled(false);
        ui->action_find->setEnabled(false);
        ui->action_grid->setEnabled(false);
        ui->action_align->setEnabled(false);
        ui->action_alignTopEdge->setEnabled(false);
        ui->action_alignBottomEdge->setEnabled(false);
        ui->action_alignLeftEdge->setEnabled(false);
        ui->action_alignRightEdge->setEnabled(false);
        ui->action_sameHeight->setEnabled(false);
        ui->action_sameWidth->setEnabled(false);
        ui->action_sameSize->setEnabled(false);
        ui->action_delete->setEnabled(false);
        ui->action_rotate->setEnabled(false);
        ui->action_Copy->setEnabled(false);
        ui->action_Cut->setEnabled(false);
        ui->action_flipHorizintal->setEnabled(false);
        ui->action_flipVertical->setEnabled(false);
        ui->action_horizintalEquidistant->setEnabled(false);
        ui->action_VerticalEquidistant->setEnabled(false);
        ui->action_bringToFront->setEnabled(false);
        ui->action_SendToBackup->setEnabled(false);
        ui->listWidget_element->setEnabled(false);
        m_viewInfoStatuBar->setEnabled(false);
    }

    bool simIsBusy = m_simCtrl && m_simCtrl->getIsBusy();
    bool simIsIdle = !m_simCtrl || !m_simCtrl->getIsBusy();
    bool simIsRunMode = m_simCtrl && m_simCtrl->getIsRunMode();
    ui->action_Tile->setEnabled(hasEditor);
    ui->action_Cascade->setEnabled(hasEditor);
    ui->action_xmlEditorShowAsTool->setEnabled(hasEditor);
    ui->action_Run->setEnabled(simIsIdle && hasProject);
    m_runAction->setEnabled(simIsIdle && hasProject);
//    ui->action_Cmd->setEnabled(!simIsBusy);
    ui->action_simMode->setEnabled(simIsIdle);
    ui->action_Code->setEnabled(hasProject);
    ui->action_Step->setEnabled(simIsIdle && hasProject);
    ui->action_debugWindow->setEnabled(simIsBusy && !simIsRunMode);
    m_debugAction->setEnabled(simIsIdle && hasProject);
    ui->action_Close->setEnabled(hasEditor);
    ui->action_CloseAll->setEnabled(hasEditor);
    ui->action_NextWindow->setEnabled(hasEditor);
    ui->action_PreviousWindow->setEnabled(hasEditor);
    ui->action_edit->setEnabled(hasEditor || (m_rpuWave && m_rpuWave->isVisible()));
    ui->action_resource->setEnabled(hasProject);
    ui->action_proSetting->setEnabled(hasProject);
    ui->action_analyze->setEnabled(hasProject);
    ui->action_restoreDebugStatus->setEnabled(simIsIdle);
    ui->action_saveDebugStatus->setEnabled(simIsBusy && !simIsRunMode && !m_isSimDebug);
    ui->action_closeAllSolution->setEnabled(hasSolution);
    ui->action_closeAllSlnAndEditor->setEnabled(hasSolution || hasEditor);
    bool isConnectToDevice = (CHardwareDebug::hasInstance() ? CHardwareDebug::getInstance(this)->getIsConnected() : false);
    ui->action_connectToDevice->setEnabled(!isConnectToDevice);
    ui->action_disconnectToDevice->setEnabled(isConnectToDevice);
    ui->action_deviceReset->setEnabled(isConnectToDevice);
    ui->action_updateFirmware->setEnabled(isConnectToDevice);
    ui->action_OTP->setEnabled(isConnectToDevice);
    ui->action_jointDebugMode->setEnabled(isConnectToDevice && simIsIdle);
    if(!isConnectToDevice)
        ui->action_simMode->setChecked(true);
}

void MainWindow::updateDebugToolBar()
{
    ui->action_Next->setEnabled(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode() && !m_simCtrl->getIsFinish());
    ui->action_Continue->setEnabled(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode() && !m_simCtrl->getIsFinish());
    ui->action_LastClock->setEnabled(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode() && !m_simCtrl->getIsFinish());
    ui->action_Stop->setEnabled(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode());
    m_simDebugToolBar->getSpinBox()->setEnabled(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode() && !m_simCtrl->getIsFinish());
}

RcaGraphView *MainWindow::currentXmlEditor() const
{
    QMdiSubWindow *subWindow = ui->mdiArea->currentSubWindow();
    if (subWindow)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(subWindow->widget());
        if(rcaGraphView)
            return rcaGraphView;
    }
    return nullptr;
}

/*!
 * MainWindow::activeEditor
 * \brief   当前激活编辑器，重载
 * \return  GuiViewer *
 * \author  zhangjun
 * \date    2016-10-12
 */
RcaGraphView *MainWindow::activeXmlEditor() const
{
    QMdiSubWindow *subWindow = ui->mdiArea->activeSubWindow();
    if (subWindow)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(subWindow->widget());
        if(rcaGraphView)
            return rcaGraphView;
    }
    return nullptr;
}

/*!
 * MainWindow::activeEditor
 * \brief   当前激活编辑器，重载
 * \param   subWindow
 * \return  RcaGraphView *
 * \author  zhangjun
 * \date    2016-10-12
 */
RcaGraphView *MainWindow::activeXmlEditor(QMdiSubWindow *subWindow) const
{
    if (subWindow)
    {
        return qobject_cast<RcaGraphView *>(subWindow->widget());
    }
    return nullptr;
}


/*!
 * MainWindow::addXmlEditor
 * \brief   增加编辑器
 * \param   scrollArea
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::addXmlEditor(RcaGraphView *rcaView)
{
    if(ui->stackedWidget->currentIndex() != 1)
    {
        disconnect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
                   this, SLOT(activeXmlEditorChanged(QMdiSubWindow*)));
        ui->stackedWidget->setCurrentIndex(1);
        connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
                this, SLOT(activeXmlEditorChanged(QMdiSubWindow*)));
    }

    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(rcaView);

    ui->menu_W->addAction(rcaView->getAction());
    m_windowActionGroup->addAction(rcaView->getAction());
    subWindow->showMaximized();

    ui->dockWidget_scaleView->setWidget(m_curRcaGraphView->getRcaScaleView());
//    ui->dockWidget_attribute->setWidget(m_curRcaGraphView->getCurItemSetting());
    ui->dockWidget_attribute->setWidget(m_curRcaGraphView->getCurPropertyBrowser());//TODO:XUFEI 设置属性界面widget
    connect(m_curRcaGraphView, SIGNAL(setStatus(QString)), this, SLOT(setStatusBarMessage(QString)));
    connect(m_curRcaGraphView, SIGNAL(displaySelectedItemsWave(QStringList)), this, SLOT(addWaveToShow(QStringList)));
    updateActions();
}

/*!
 * MainWindow::setStatus
 * \brief   设置状态栏内容
 * \param   str
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::setStatusBarMessage(const QString &str)
{
    ui->statusBar->showMessage(str, 2000);
}

/*!
 * MainWindow::searchInCmd
 * \brief   cmd命令行窗口搜索
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::searchInCmd()
{
    qInfo() << tr(u8"cmd命令行窗口搜索");
    bool isFind = ui->textEdit_log->find(ui->lineEdit_searchCmd->text().trimmed(),
                                          QTextDocument::FindBackward);
    if(isFind)
    {
        DBG << tr(u8"行号：%0 列号：%1").arg(ui->textEdit_log->textCursor().blockNumber())
                    .arg(ui->textEdit_log->textCursor().blockNumber());
    }
    else
    {
        QTextCursor textCursor = ui->textEdit_log->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->textEdit_log->setTextCursor(textCursor);
    }
}


/*!
 * MainWindow::on_action_Tile_triggered
 * \brief   平铺子窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Tile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

/*!
 * MainWindow::on_action_Cascade_triggered
 * \brief   层叠子窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Cascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

/*!
 * MainWindow::on_action_About_triggered
 * \brief   关于RPU Designer
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_About_triggered()
{
    auto randomSortAuthorName = []() -> QString {
        std::vector<QString> authorVector = {QStringLiteral("、徐飞"), QStringLiteral("、杨小梅"), QStringLiteral("、池伟伟"), QStringLiteral("、高菁"), QStringLiteral("、张伟")};
        std::random_shuffle(authorVector.begin(), authorVector.end());
        QString author(QStringLiteral("章俊"));
        for(auto it = authorVector.begin(); it != authorVector.end(); ++it) {
            author.append(*it);
        }
        return author;
    };

    qInfo() << tr(u8"关于RPU Designer");
    const QDate buildDate = QLocale( QLocale::English ).toDate( QString( __DATE__ ).replace( "  ", " 0" ), "MMM dd yyyy");
    const QTime buildTime = QTime::fromString( __TIME__, "hh:mm:ss" );
    QMessageBox::about(this, tr(u8"关于 %0").arg(qApp->applicationName()),
                       tr(u8"<h2> %0</h2>"
#ifdef EVALUATION_VERSION
                          "<p>Version: v%1 evaluation"
#else
                          "<p>Version: v%1 alpha"
#endif
//                          "<p>Author: zj, lk"
                      #if defined(Q_OS_WIN)
                          "<p>Based on Qt %2 (MinGW 32 bit)"
                      #elif defined(Q_OS_UNIX)
                          "<p>Based on Qt %2 (GCC 5.3.1 20160406 (Red Hat 5.3.1-6), 64 bit)"
                      #endif
                          "<p>Built on %3 %4"
                          "<p>%5 %6")
                           .arg(qApp->applicationName()).arg(qApp->applicationVersion())
                           .arg(QT_VERSION_STR)
                           //                       .arg(QString::number(QT_VERSION_MAJOR) + "." + QString::number(QT_VERSION_MINOR) + "." + QString::number(QT_VERSION_PATCH))
                           .arg(buildDate.toString("yyyy/MM/dd")).arg(buildTime.toString("hh:mm:ss"))
                           .arg(QString(VER_LEGALCOPYRIGHT_STR)).arg(QString(VER_LEGALTRADEMARKS1_STR)));
}

/*!
 * MainWindow::on_dockWidget_project_visibilityChanged
 * \brief
 * \param   visible
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_dockWidget_project_visibilityChanged(bool visible)
{
    ui->action_solution->setChecked(visible);
}

/*!
 * MainWindow::on_action_solution_triggered
 * \brief
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_solution_triggered(bool checked)
{
    ui->dockWidget_project->setVisible(checked);
    if(checked)
        ui->dockWidget_project->raise();
}

/*!
 * MainWindow::on_dockWidget_openedFile_visibilityChanged
 * \brief
 * \param   visible
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_dockWidget_openedFile_visibilityChanged(bool visible)
{
    ui->action_openedFile->setChecked(visible);
}

/*!
 * MainWindow::on_action_openedFile_triggered
 * \brief
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_openedFile_triggered(bool checked)
{
    ui->dockWidget_openedFile->setVisible(checked);
    if(checked)
        ui->dockWidget_openedFile->raise();
}

void MainWindow::on_dockWidget_attribute_visibilityChanged(bool visible)
{
    ui->action_attribute->setChecked(visible);
}

/*!
 * MainWindow::on_action_attribute_triggered
 * \brief
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_attribute_triggered(bool checked)
{
    ui->dockWidget_attribute->setVisible(checked);
    if(checked)
        ui->dockWidget_scaleView->raise();
}

void MainWindow::on_dockWidget_scaleView_visibilityChanged(bool visible)
{
    ui->action_scale->setChecked(visible);
}

void MainWindow::on_action_scale_triggered(bool checked)
{
    ui->dockWidget_scaleView->setVisible(checked);
    if(checked)
        ui->dockWidget_scaleView->raise();
}

/*!
 * MainWindow::on_dockWidget_element_visibilityChanged
 * \brief
 * \param   visible
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_dockWidget_element_visibilityChanged(bool visible)
{
    ui->action_element->setChecked(visible);
}

/*!
 * MainWindow::on_action_element_triggered
 * \brief
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_element_triggered(bool checked)
{
    ui->dockWidget_element->setVisible(checked);
    if(checked)
        ui->dockWidget_element->raise();
}

void MainWindow::on_action_showMenuBar_triggered(bool checked)
{
    ui->menuBar->setVisible(checked);
}

/*!
 * MainWindow::on_action_fileToolBar_triggered
 * \brief
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_fileToolBar_triggered(bool checked)
{
    ui->toolBar_file->setVisible(checked);
}

/*!
 * MainWindow::on_action_editToolBar_triggered
 * \brief
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_editToolBar_triggered(bool checked)
{
    ui->toolBar_edit->setVisible(checked);
}


void MainWindow::on_action_viewToolBar_triggered(bool checked)
{
    ui->toolBar_view->setVisible(checked);
}

/*!
 * MainWindow::on_action_Quit_triggered
 * \brief   退出RPU Designer
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Quit_triggered()
{
    qInfo() << tr(u8"退出程序");
    this->close();
}

/*!
 * MainWindow::on_action_Close_triggered
 * \brief   关闭当前激活子窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Close_triggered()
{
    qInfo() << tr(u8"关闭当前子窗口");
    ui->mdiArea->closeActiveSubWindow();
}

/*!
 * MainWindow::on_action_CloseAll_triggered
 * \brief   关闭所有子窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_CloseAll_triggered()
{
    qInfo() << tr(u8"关闭所有子窗口");
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_action_xmlEditorShowAsTool_triggered()
{
    ui->widget_xmlEditor->showAsTool();
}
/*!
 * MainWindow::on_action_NextWindow_triggered
 * \brief   显示下一个子窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_NextWindow_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}

/*!
 * MainWindow::on_action_PreviousWindow_triggered
 * \brief   显示上一个子窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_PreviousWindow_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

/*!
 * MainWindow::on_action_Bug_triggered
 * \brief   报告bug
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Bug_triggered()
{
    qInfo() << tr(u8"反馈bug");
    QString mail = tr(u8"mailto:zhangjun@tsinghua-wx.org?subject=%0 bug")
            .arg(qApp->applicationName());
    QDesktopServices::openUrl(QUrl(mail));
}

/*!
 * MainWindow::on_action_welcome_triggered()
 * \brief   显示首页
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_welcome_triggered()
{
    qInfo() << tr(u8"显示首页");
    ui->stackedWidget->setCurrentIndex(0);
}

/*!
 * MainWindow::on_action_edit_triggered
 * \brief   显示编辑界面
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::on_action_edit_triggered()
{
    qInfo() << tr(u8"显示编辑界面");
    ui->stackedWidget->setCurrentIndex(1);
}

/*!
 * MainWindow::on_action_resource_triggered
 * \brief   显示资源信息界面
 * \author  zhangjun
 * \date    2016-10-31
 */
void MainWindow::on_action_resource_triggered()
{
    qInfo() << tr(u8"显示资源信息界面");
    ui->stackedWidget->setCurrentIndex(2);
    ui->tableView_resourceEditor->setFocus();
}

void MainWindow::on_action_debugWindow_triggered()
{
    ui->stackedWidget->setCurrentIndex(5);
}

/*!
 * MainWindow::on_action_proSetting_triggered()
 * \brief   显示项目配置界面
 * \author  zhangjun
 * \date    2016-10-26
 */
void MainWindow::on_action_proSetting_triggered()
{
    qInfo() << tr(u8"显示项目配置界面");
    ui->stackedWidget->setCurrentIndex(3);
}

/*!
 * MainWindow::on_action_Log_triggered
 * \brief   显示程序版本日志
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Log_triggered()
{
    qInfo() << tr(u8"显示程序版本日志");
    VersionForm form(this);
    CAppEnv::animation(&form, this);
    form.exec();
}

/*!
 * MainWindow::on_action_lockToolBar_triggered
 * \brief   锁定工具栏
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-18
 */
void MainWindow::on_action_lockToolBar_triggered(bool checked)
{
    qInfo() << tr(u8"锁定工具栏");
    m_setting.toolBarLocked = checked;
    ui->toolBar_edit->setMovable(!checked);
    ui->toolBar_file->setMovable(!checked);
    ui->toolBar_view->setMovable(!checked);
    if(checked)
    {
        ui->action_lockToolBar->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/locked128.png"));
    }
    else
    {
        ui->action_lockToolBar->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/unlocked128.png"));
    }
}

/*!
 * MainWindow::on_action_Setting_triggered
 * \brief   程序设置
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Setting_triggered()
{
    qInfo() << tr(u8"程序设置");
    SettingForm settingForm(m_setting, this);
    CAppEnv::animation(&settingForm, this);
    int ret = settingForm.exec();
    if(ret == QDialog::Accepted)
    {
#ifdef RELEASE
//        if(m_setting.logOut)
//        {
//            try {
//                CAppEnv::openLog();
//            } catch (CError &error) {
//                QMessageBox::warning(this, tr(u8"错误"), tr(u8"程序遇到问题：%0")
//                                     .arg(QString::fromUtf8(error.what())));
//            }
//            qInfo() << tr(u8"程序设置：打开日志记录");
//        }
//        else
//        {
//            qInfo() << tr(u8"程序设置：关闭日志记录");
//            CAppEnv::closeLog();
//        }
#endif
        settingChanged();
        writeSettings();
    }
}

/*!
 * MainWindow::on_listView_openedFile_clicked
 * \brief   单击已打开的文件列表内容
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_listView_openedFile_clicked(const QModelIndex &index)
{
    DBG << tr(u8"单击已打开的文件列表内容");
    int row = index.row();
    openXmlFile(m_openedFiles.at(row));
}

/*!
 * MainWindow::on_treeView_project_doubleClicked
 * \brief   解决方案资源管理器，双击项目中文件
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_treeView_project_doubleClicked(const QModelIndex &index)
{
    DBG << tr(u8"解决方案资源管理器，双击项目中文件");
    if(!index.isValid())
        return;

    EItemType type = m_projectTreeModel->getItemType(index);
    CProjectResolver *projectResolver = m_projectTreeModel->getProjectResolver(index);
    QString projectName = m_projectTreeModel->getProjectFileName(index);
    QString fileName = m_projectTreeModel->getItemFileName(index);

    if(type == ITEMXMLFILE)
    {
        if(!QFileInfo(fileName).exists())
        {
            QMessageBox::critical(this, qApp->applicationName(), tr(u8"文件“%0”不存在！").arg(fileName));
            m_projectTreeModel->deleteXml(index);
            return;
        }
        openXmlFile(fileName, projectResolver);
    }
    else if(type == ITEMSRCFILE)
    {
        if(!QFileInfo(fileName).exists())
        {
            QMessageBox::critical(this, qApp->applicationName(), tr(u8"文件“%0”不存在！").arg(fileName));
            m_projectTreeModel->deleteSrc(index);
            return;
        }
        openSrcFile(projectName, fileName);
    }
    else if(type == ITEMPROJECTFILE)
    {
        projectSettingTriggered();
    }
}

void MainWindow::on_listWidget_element_itemClicked(QListWidgetItem *item)
{
    if(m_curRcaGraphView)
    {
        m_curRcaGraphView->changeStatus(RcaSceneStates(item->data(Qt::UserRole).toInt()));
    }
}

/*!
 * MainWindow::on_pushButton_newFile_clicked
 * \brief   导航窗新建XML文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_pushButton_newFile_clicked()
{
    qInfo() << tr(u8"新建XML文件……");
    on_action_NewFile_triggered();
}

/*!
 * MainWindow::on_pushButton_openFile_clicked
 * \brief   导航窗打开XML文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_pushButton_openFile_clicked()
{
    qInfo() << tr(u8"打开XML文件……");
    on_action_Open_triggered();
}

/*!
 * MainWindow::on_pushButton_newPro_clicked
 * \brief   导航窗新建项目文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_pushButton_newPro_clicked()
{
    qInfo() << tr(u8"新建解决方案……");
    on_action_newSolution_triggered();
}

/*!
 * MainWindow::on_pushButton_openPro_clicked
 * \brief   导航窗打开项目文件
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_pushButton_openPro_clicked()
{
    qInfo() << tr(u8"打开解决方案……");
    on_action_openSolution_triggered();
}

/*!
 * MainWindow::on_listWidget_files_itemDoubleClicked
 * \brief   导航窗打开最近打开文件列表中文件
 * \param   item
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_listWidget_files_itemDoubleClicked(QListWidgetItem *item)
{
    qInfo() << tr(u8"打开最近打开的文件");
    QFile file(item->toolTip());
    if (!file.exists()) {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr(u8"文件“%0”不存在。")
                             .arg(file.fileName()));
        updateRecentFileActions();
        return;
    }
    openXmlFile(item->toolTip());
}

/*!
 * MainWindow::on_listWidget_pros_itemDoubleClicked
 * \brief   导航窗最近打开的解决方法列表中打开解决方案
 * \param   item
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_listWidget_pros_itemDoubleClicked(QListWidgetItem *item)
{
    qInfo() << tr(u8"打开最近打开的解决方案");
    QFile file(item->toolTip());
    if (!file.exists()) {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr(u8"文件“%0”不存在。")
                             .arg(file.fileName()));
        updateRecentProjectActions();
        return;
    }
    loadSolution(QStringList(item->toolTip()));
}

void MainWindow::on_listWidget_demos_itemDoubleClicked(QListWidgetItem *item)
{
    qInfo() << tr(u8"打开示例解决方案");
    QFile file(item->toolTip());
    if (!file.exists()) {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr(u8"文件“%0”不存在。")
                             .arg(file.fileName()));
        updateRecentProjectActions();
        return;
    }
    loadSolution(QStringList(item->toolTip()));
}

void MainWindow::on_pushButton_proSettingSave_clicked()
{
    if(!m_projectSettingModel || m_projectSettingModel->rowCount(QModelIndex()) < 0)
    {
        return;
    }
    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    m_curProjectResolver->paraChanged();
    m_curProjectResolver->save();
}

void MainWindow::on_pushButton_proSettingSelectAll_clicked()
{
    m_projectSettingModel->selectAll();
}

void MainWindow::on_pushButton_proSettingUp_clicked()
{
    ui->tableView_projectSetting->setCurrentIndex(
                m_projectSettingModel->moveUp(ui->tableView_projectSetting->currentIndex()));
}

void MainWindow::on_pushButton_proSettingDown_clicked()
{
    ui->tableView_projectSetting->setCurrentIndex(
                m_projectSettingModel->moveDown(ui->tableView_projectSetting->currentIndex()));
}

void MainWindow::setSolutionTypeChanged()
{
    //    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    if(m_curProjectResolver && m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver())
    {
        m_curProjectResolver->getProjectParameter()->cipherType = (ECipherType)ui->comboBox_projectSettingSetSolution->currentIndex();
        m_curProjectResolver->getProjectParameter()->lfsrMode = ui->comboBox_lfsrMode->currentIndex();
        m_curProjectResolver->getProjectParameter()->lfsrWidthDeep = ui->comboBox_lfsrWidthDeep->currentIndex();
        projectParameterSetDirty();
    }
}

void MainWindow::on_listWidget_resource_itemDoubleClicked(QListWidgetItem *item)
{
    if(!ui->tableView_projectSetting->currentIndex().isValid())
        return;
    int column = ui->tableView_projectSetting->currentIndex().column();
    if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
        return;

    QString str = ":/" + item->text();
    m_projectSettingModel->setData(ui->tableView_projectSetting->currentIndex(), str, Qt::EditRole);
}

void MainWindow::on_treeView_explorer_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    if(!ui->tableView_projectSetting->currentIndex().isValid())
        return;
    int column = ui->tableView_projectSetting->currentIndex().column();
    if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
        return;

    QFileSystemModel *model = static_cast<QFileSystemModel*>(ui->treeView_explorer->model());
    if(!model)
        return;
    QFileInfo fileInfo(model->fileInfo(index));
    if(!fileInfo.isFile())
        return;
    m_projectSettingModel->setData(ui->tableView_projectSetting->currentIndex(),
                                   fileInfo.absoluteFilePath(), Qt::EditRole);
}

/*!
 * MainWindow::on_action_Help_triggered
 * \brief   打开帮助
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Help_triggered()
{
    qInfo() << tr(u8"打开使用帮助");
    if(!QDesktopServices::openUrl(QUrl("file:///" + CAppEnv::getDocDirPath() + "/manual.pdf")))
    {
        QString filePath = CAppEnv::getDocDirPath() + "/manual.pdf";
#if defined(Q_OS_WIN)
        filePath.replace("/", "\\");
        filePath = " /select," + filePath;
        LPCSTR file = filePath.toLocal8Bit();
        ShellExecuteA(0, "open", "explorer.exe", file, nullptr, SW_SHOWNORMAL);
#elif defined(Q_OS_UNIX)
        filePath = QFileInfo(filePath).absolutePath();
        QDesktopServices::openUrl(QUrl("file:///" + filePath));
#endif
    }
//    HelpForm helpForm(this);
//    CAppEnv::animation(&helpForm, this);
//    helpForm.exec();
}

/*!
 * MainWindow::writeSettings
 * \brief   保存设置
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::writeSettings() const
{
    qInfo() << tr(u8"保存程序设置");
    QSettings settings(ENVIRONMENT_KEY, "rpudesigner");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("recentFiles", m_recentFiles);
    settings.setValue("recentPros", m_recentPros);
    settings.setValue("logOut", m_setting.logOut);
    settings.setValue("logSize", m_setting.logSize);
    settings.setValue("logDelete", m_setting.logDelete);
    settings.setValue("logDeleteDate", m_setting.logDeleteDate);
    settings.setValue("logArchive", m_setting.logArchive);
    settings.setValue("maxSubWin", m_setting.maxSubWin);
    settings.setValue("showTab", m_setting.showTab);
    settings.setValue("tabType", m_setting.tabType);
    settings.setValue("defaultProPath", m_setting.defaultProPath);
    settings.setValue("cycleOut", m_setting.cycleOut);
#if defined(Q_OS_WIN)
    settings.setValue("autoSave", m_setting.autoSave);
#endif
    settings.setValue("saveInterval", m_setting.saveInterval);
    settings.setValue("sysTray", m_setting.sysTray);
    settings.setValue("toolBarLocked", m_setting.toolBarLocked);
    settings.setValue("tabMovable", m_setting.tabMovable);
    settings.setValue("tabClosable", m_setting.tabClosable);
    settings.setValue("tabPosition", m_setting.tabPosition);
    //    settings.setValue("xmlMd5Check", m_setting.xmlMd5Check);
    settings.setValue("xmlMd5Check", true);
    settings.setValue("iconSize", m_setting.iconSize);
    settings.setValue("menuBar", ui->menuBar->isVisible());
    settings.setValue("editToolBar", ui->toolBar_edit->isVisible());
    settings.setValue("fileToolBar", ui->toolBar_file->isVisible());
    settings.setValue("viewToolBar", ui->toolBar_view->isVisible());
    //    settings.setValue("srcMd5Check", m_setting.srcMd5Check);
    settings.setValue("srcMd5Check", true);
    settings.setValue("maxCycle", m_setting.maxCycle);
    settings.setValue("recentFileCount", m_setting.recentFileCount);
    settings.setValue("recentProCount", m_setting.recentProCount);
    //    settings.setValue("showGrid", showGridAction->isChecked());
    //    settings.setValue("autoRecalc", autoRecalcAction->isChecked());
    //    settings.setValue("sortType", m_setting.sortType);
    settings.setValue("showGrid", m_setting.showGrid);
    settings.setValue("gridPoint", m_setting.gridPoint);
    settings.setValue("networkProxy", m_setting.networkPorxy);
    settings.setValue("networkPorxyAddress", m_setting.networkPorxyAddress);
    settings.setValue("networkPorxyPort", m_setting.networkPorxyPort);
    settings.setValue("networkPorxyUserName", m_setting.networkPorxyUserName);
    settings.setValue("networkPorxyPassword", m_setting.networkPorxyPassword);
    settings.setValue("openCmdGenDir", m_setting.openCmdGenDir);
    settings.setValue("customCmdGenDirFlag", m_setting.customCmdGenDirFlag);
    settings.setValue("customCmdGenDir", m_setting.customCmdGenDir);
    settings.setValue("cmdFont", m_setting.cmdFont);
    settings.setValue("cmdFontSize", m_setting.cmdFontSize);
    settings.setValue("cmdGenOutSideCmd", m_setting.cmdGenOutSideCommand);
    settings.setValue("sessions", m_setting.sessions);
    settings.setValue("sessionCount", m_setting.sessionCount);
    settings.setValue("restoreSession", m_setting.restoreSession);
    settings.setValue("restoreSessionFlag", m_setting.restoreSessionFlag);
    settings.setValue("saveWave", m_setting.saveWave);
    settings.setValue("pythonLocate", m_setting.pythonLocate);
}

/*!
 * MainWindow::readSettings
 * \brief   读取设置
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::readSettings()
{
    qInfo() << tr(u8"载入程序设置");
    QSettings settings(ENVIRONMENT_KEY, "rpudesigner");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    m_recentFiles = settings.value("recentFiles").toStringList();
    m_recentPros = settings.value("recentPros").toStringList();
    m_setting.logOut = settings.value("logOut", true).toBool();
    m_setting.logSize = settings.value("logSize", 10).toInt();
    m_setting.logDelete = settings.value("logDelete", true).toBool();
    m_setting.logDeleteDate = settings.value("logDeleteDate", 99).toInt();
    m_setting.logArchive = settings.value("logArchive", true).toBool();
    m_setting.maxSubWin = settings.value("maxSubWin", 20).toInt();
    m_setting.showTab = settings.value("showTab", true).toBool();
    m_setting.tabType = settings.value("tabType", 0).toInt();
    m_setting.defaultProPath = settings.value("defaultProPath", CAppEnv::getProOutDirPath()).toString();
#if defined(Q_OS_WIN)
    m_setting.defaultProPath.replace("\\", "/");
    while(m_setting.defaultProPath.endsWith("/"))
    {
        m_setting.defaultProPath.chop(1);
    }
#endif
    m_setting.cycleOut = settings.value("cycleOut", false).toBool();
    m_setting.autoSave = settings.value("autoSave", false).toBool();
    m_setting.saveInterval = settings.value("saveInterval", 5).toInt();
    m_setting.sysTray = settings.value("sysTray", false).toBool();
    m_setting.toolBarLocked = settings.value("toolBarLocked", true).toBool();
    m_setting.tabMovable = settings.value("tabMovable", true).toBool();
    m_setting.tabClosable = settings.value("tabClosable", true).toBool();
    m_setting.tabPosition = settings.value("tabPosition", 0).toInt();
    m_setting.xmlMd5Check = settings.value("xmlMd5Check", true).toBool();
    m_setting.iconSize = settings.value("iconSize", 1).toInt();
    m_setting.menuBar = settings.value("menuBar", true).toBool();
    m_setting.editToolBar = settings.value("editToolBar", true).toBool();
    m_setting.fileToolBar = settings.value("fileToolBar", true).toBool();
    m_setting.viewToolBar = settings.value("viewToolBar", true).toBool();
    m_setting.srcMd5Check = settings.value("srcMd5Check", true).toBool();
    m_setting.maxCycle = settings.value("maxCycle", 10000).toInt();
    //    m_setting.sortType = settings.value("sortType", 1).toInt();
    m_recentFileCount = m_setting.recentFileCount = settings.value("recentFileCount", 20).toInt();
    m_recentProCount = m_setting.recentProCount = settings.value("recentProCount", 20).toInt();
    m_setting.showGrid = settings.value("showGrid", true).toBool();
    m_setting.gridPoint = settings.value("gridPoint", QPoint(20, 20)).toPoint();
    m_setting.networkPorxy = settings.value("networkProxy", 0).toInt();
    m_setting.networkPorxyAddress = settings.value("networkPorxyAddress").toString();
    m_setting.networkPorxyPort = settings.value("networkPorxyPort").toInt();
    m_setting.networkPorxyUserName = settings.value("networkPorxyUserName").toString();
    m_setting.networkPorxyPassword = settings.value("networkPorxyPassword").toString();
    m_setting.openCmdGenDir = settings.value("openCmdGenDir", true).toBool();
    m_setting.customCmdGenDirFlag = settings.value("customCmdGenDirFlag", false).toBool();
    m_setting.customCmdGenDir = settings.value("customCmdGenDir", "").toString();
    m_setting.cmdFont = settings.value("cmdFont", QFont(u8"宋体,9,-1,5,50,0,0,0,0,0")).value<QFont>();
    m_setting.cmdFontSize = settings.value("cmdFontSize", 9).toDouble();
    m_setting.cmdGenOutSideCommand = settings.value("cmdGenOutSideCmd", "").toString();
    m_setting.sessions = settings.value("sessions").toByteArray();
    m_setting.sessionCount = settings.value("sessionCount", 0).toInt();
    m_setting.restoreSession = settings.value("restoreSession", "default").toString();
    m_setting.restoreSessionFlag = settings.value("restoreSessionFlag", false).toBool();
    m_setting.saveWave = settings.value("saveWave", false).toBool();
    m_setting.pythonLocate = settings.value("pythonLocate", "").toString();

    ui->action_showMenuBar->setChecked(m_setting.menuBar);
    ui->action_editToolBar->setChecked(m_setting.editToolBar);
    ui->action_fileToolBar->setChecked(m_setting.fileToolBar);
    ui->action_viewToolBar->setChecked(m_setting.viewToolBar);
    ui->toolBar_edit->setMovable(!m_setting.toolBarLocked);
    ui->toolBar_file->setMovable(!m_setting.toolBarLocked);
    ui->toolBar_view->setMovable(!m_setting.toolBarLocked);
    ui->action_lockToolBar->setChecked(m_setting.toolBarLocked);
    if(m_setting.toolBarLocked)
    {
        ui->action_lockToolBar->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/locked128.png"));
    }
    else
    {
        ui->action_lockToolBar->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/unlocked128.png"));
    }

#if defined(Q_OS_WIN)
    QScopedPointer<QSettings> regSuffix(new QSettings("HKEY_CLASSES_ROOT\\.rpusln", QSettings::NativeFormat));
    //    QSettings *regSuffix = new QSettings("HKEY_CLASSES_ROOT\\.rpusln", QSettings::NativeFormat);
    if(regSuffix->value("Default").toString().isEmpty())
    {
        qInfo() << tr(u8"读取程序配置：注册表项文件关联未设置，开始设置");
        regSuffix->setValue("Default", "RPUDesigner.rpusln");

        QSettings *iconReg = new QSettings(REG_SUFFIX, QSettings::NativeFormat);
        QString iconFilePath = CAppEnv::getImageDirPath();
        iconFilePath.replace("/", "\\");
        iconFilePath = "\"" + iconFilePath + "\\chip.ico" + "\"";
        iconReg->setValue("Default", iconFilePath);
        delete iconReg;

        QSettings *programReg = new QSettings(REG_CMD, QSettings::NativeFormat);
        QString appFilePath = CAppEnv::getAppFilePath();
        appFilePath.replace("/", "\\");
        appFilePath = "\"" + appFilePath + "\"" + " \"%0\"";
        programReg->setValue("Default", appFilePath);
        delete programReg;
        SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST|SHCNF_FLUSH,0,0);
    }
    //    delete regSuffix;

    QScopedPointer<QSettings> regAutoRun(new QSettings(REG_RUN, QSettings::NativeFormat));
    if(regAutoRun->value(CAppEnv::getAppName()).isNull())
    {
        m_setting.autoRun = false;
    }
    else
    {
        m_setting.autoRun = true;
    }
#endif
}

/*!
 * MainWindow::settingChanged
 * \brief   设置改变
 * \author  zhangjun
 * \date    2016-10-17
 */
void MainWindow::settingChanged()
{
    if(m_setting.showTab)
    {
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    }
    else
    {
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
        if(ui->mdiArea->activeSubWindow())
            ui->mdiArea->activeSubWindow()->showMaximized();
    }
    switch (m_setting.tabType) {
    case 0:
        ui->mdiArea->setTabShape(QTabWidget::Rounded);
        break;
    case 1:
        ui->mdiArea->setTabShape(QTabWidget::Triangular);
        break;
    default:
        ui->mdiArea->setTabShape(QTabWidget::Rounded);
        break;
    }
    if(m_setting.autoSave)
    {
        m_timer.start(m_setting.saveInterval * 60000);
    }
    else
    {
        m_timer.stop();
    }

    ui->mdiArea->setTabsMovable(m_setting.tabMovable);
    ui->mdiArea->setTabsClosable(m_setting.tabClosable);
    switch (m_setting.tabPosition) {
    case 0:
        ui->mdiArea->setTabPosition(QTabWidget::North);
        break;
    case 1:
        ui->mdiArea->setTabPosition(QTabWidget::South);
        break;
    case 2:
        ui->mdiArea->setTabPosition(QTabWidget::West);
        break;
    case 3:
        ui->mdiArea->setTabPosition(QTabWidget::East);
        break;
    default:
        ui->mdiArea->setTabPosition(QTabWidget::North);
        break;
    }

    ui->action_grid->setChecked(m_setting.showGrid);
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView)
        {
            rcaGraphView->setGridViewVisable(m_setting.showGrid);
            rcaGraphView->setGridViewStep(m_setting.gridPoint);
        }
    }

    ui->toolBar_edit->setIconSize(iconSize()*2/3);
    ui->toolBar_file->setIconSize(iconSize()*2/3);
    ui->toolBar_view->setIconSize(iconSize()*2/3);

    m_setting.cmdFont.setPointSizeF((qreal)m_setting.cmdFontSize);
    ui->textEdit_log->setFont(m_setting.cmdFont);
    //    ui->plainTextEdit->setCurrentFont(m_setting.cmdFont);
    //    ui->plainTextEdit->setFontPointSize((qreal)m_setting.cmdFontSize);


    switch (m_setting.networkPorxy) {
    case 0:
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
        break;
    case 1:
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(m_setting.networkPorxyAddress);
        proxy.setPort(m_setting.networkPorxyPort);
        proxy.setUser(m_setting.networkPorxyUserName);
        proxy.setPassword(m_setting.networkPorxyPassword);
        QNetworkProxy::setApplicationProxy(proxy);
    }
        break;
    case 2:
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(m_setting.networkPorxyAddress);
        proxy.setPort(m_setting.networkPorxyPort);
        proxy.setUser(m_setting.networkPorxyUserName);
        proxy.setPassword(m_setting.networkPorxyPassword);
        QNetworkProxy::setApplicationProxy(proxy);
    }
        break;
    default:
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
        break;
    }

    //    switch (m_setting.iconSize)
    //    {
    //    case 0:
    //        this->setIconSize(QSize(16, 16));
    //        break;
    //    case 1:
    //        this->setIconSize(QSize(28, 28));
    //        break;
    //    case 2:
    //        this->setIconSize(QSize(48, 48));
    //        break;
    //    case 3:
    //        this->setIconSize(QSize(72, 72));
    //        break;
    //    default:
    //        this->setIconSize(QSize(16, 16));
    //        break;
    //    }
}

/*!
 * MainWindow::closeEvent
 * \brief   关闭事件
 * \param   event
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_setting.sysTray && isVisible())
    {
        hide();
        m_sysTray->showMessage(qApp->applicationName(),tr(u8"最小化到系统托盘，要关闭请右击此图标选择“退出”。"),
                               QSystemTrayIcon::Information, 1000);
        event->ignore();
        return;
    }

    //关闭调试
    if(m_debugLog != nullptr)
        on_action_Stop_triggered();

    if(m_process)
    {
        if(m_process->state() != QProcess::NotRunning)
        {
            sendCmd("quit");
            if(m_process && !m_process->waitForFinished(3000))
            {
                //            m_process->kill();
                m_process->terminate();
                while(m_process && !m_process->waitForFinished())
                {
                    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                }
            }
        }
        delete m_process;
    }

    //保存会话
    updateCurrentSession();
    m_sessionManager->saveAllSession();
    m_setting.restoreSession = m_sessionManager->getCurrentSessionName();

    //    ui->mdiArea->closeAllSubWindows();
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        if(!winList.at(i)->close())
            break;
    }

    if (!ui->mdiArea->subWindowList().isEmpty())
    {
        event->ignore();
        return;
    }
    else
    {
        if(m_resourceEditorTableModel)
        {
            QString fileName = m_resourceEditorTableModel->getFileName();
            if(m_resourceEditorTableModel->getIsModified())
            {
                ui->stackedWidget->setCurrentIndex(2);
                int ret = QMessageBox::warning(this, qApp->applicationName(),
                                                 tr(u8"是否保存对资源 “%0” 的修改？")
                                                 .arg(CAppEnv::stpToNm(fileName)),
                                                 QMessageBox::Save | QMessageBox::Discard |
                                                 QMessageBox::Cancel);
                if (ret == QMessageBox::Save)
                {
                    on_pushButton_saveResource_clicked();
                }
                else if (ret == QMessageBox::Cancel)
                {
                    event->ignore();
                    return;
                }
                else if(ret == QMessageBox::Discard)
                {
                    m_resourceEditorTableModel->setIsModified(false);
                }
            }
        }

        on_action_closeAllSolution_triggered();
        if(m_projectTreeModel->rowCount() > 0)
        {
            event->ignore();
            return;
        }

        qInfo() << tr("程序即将关闭");
        writeSettings();
        this->hide();
        m_sysTray->hide();

        //关闭线程
        if(m_tcpClient && m_tcpClient->isRunning())
        {
            m_tcpClient->quit();
            while(!m_tcpClient->wait(100))
            {
                //            DBG << tr(u8"等待线程退出！");
            }
            delete m_tcpClient;
        }

        event->accept();
        qApp->quit();
    }
}

/*!
 * MainWindow::keyPressEvent
 * \brief   键盘事件
 * \param   event
 * \author  zhangjun
 * \date    2016-10-25
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt && !ui->menuBar->isVisible())
    {
        ui->menuBar->setVisible(true);
        ui->action_showMenuBar->setChecked(true);
        return;
    }
//    else if(event->modifiers() == Qt::ALT && event->key() == Qt::Key_F10)
//    {
//        ui->action_fullScreen->setChecked(!ui->action_fullScreen->isChecked());
//        on_action_fullScreen_triggered(ui->action_fullScreen->isChecked());
//        return;
//    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
        if(this->windowState() & Qt::WindowMinimized)
        {
            m_geometry = saveGeometry();
        }
    }
    QMainWindow::changeEvent(event);
}

/*!
 * MainWindow::eventFilter
 * \brief   进入事件过滤器
 * \param   watched
 * \param   event
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::KeyRelease && watched == ui->treeView_project
            && ui->treeView_project->currentIndex().isValid())
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Delete && !m_renameFlag)
        {
            EItemType type = m_projectTreeModel->getItemType(ui->treeView_project->currentIndex());
            if(type == ITEMXMLFILE || type == ITEMSRCFILE)
            {
                deleteFileTriggered();
                return true;
            }
        }
    }
    return false;
}


//【开始】调试、运行相关
/*!
 * MainWindow::actionRunOrSteptriggered
 * \brief   运行或调试开始
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::runOrDebugTriggered(QString mode)
{
    qInfo() << tr(u8"开始运行或调试");
    m_isSimDebug = ui->action_simMode->isChecked();
    //仿真调试或运行
    if(mode == "none")
    {
        QAction *action = qobject_cast<QAction *>(sender());
        mode = action->data().toString();
        if((action->data() == "debug" && m_process && m_process->state() != QProcess::NotRunning)
                || (action->data() == "debug" && CChipDebug::getIsDebugging()))
        {
            ui->stackedWidget->setCurrentIndex(5);
            return;
        }
    }

    if (!m_projectTreeModel->hasActiveSolution())
    {
        qInfo() << tr(u8"运行或调试：没有打开项目，退出运行或调试");
        return;
    }

    //已支持多个项目进行硬件联调
//    if(!m_isSimDebug && mode == "debug" && m_projectTreeModel->getAllProjectList().count() > 1)
//    {
//        qInfo() << tr(u8"运行或调试：不支持多个项目进行硬件联调，退出联调");
//        QMessageBox::critical(this, qApp->applicationName(), tr("只能打开一个项目进行硬件联调，请先关闭其他项目！"));
//        return;
//    }

    if(m_resourceEditorTableModel && m_resourceEditorTableModel->getIsModified())
    {
        QStringList srcList = m_projectTreeModel->getActiveSrcList();
        QString fileName = m_resourceEditorTableModel->getFileName();
        foreach (QString srcFileName, srcList) {
            if(QFileInfo(srcFileName) == QFileInfo(fileName))
            {
                ui->stackedWidget->setCurrentIndex(2);
                int &&ret = QMessageBox::warning(this, qApp->applicationName(),
                                                 tr(u8"是否保存对资源 “%0” 的修改？")
                                                 .arg(CAppEnv::stpToNm(fileName)),
                                                 QMessageBox::Save | QMessageBox::Discard |
                                                 QMessageBox::Cancel);
                if (ret == QMessageBox::Save)
                {
                    on_pushButton_saveResource_clicked();
                }
                else if (ret == QMessageBox::Cancel)
                {
                    return;
                }
                else if (ret == QMessageBox::Discard)
                {
                    //                                    openSrcFile(rootItem->getProjectResolver()->getProjectFileName(),
                    //                                                fileName, "none", false);
                    //                                    m_resourceEditorTableModel->reload();
                }
            }
        }
    }

    //xml是否保存
    QStringList activeXmlList = m_projectTreeModel->getActiveXmlList();
    foreach (QString xmlFileName, activeXmlList) {
        QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
        for(int i = 0; i < winList.count(); ++i)
        {
            RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
            if(QFileInfo(xmlFileName) == QFileInfo(rcaGraphView->getXml()->m_curFile))
            {
                if(!rcaGraphView->okToContinue())
                    return;
            }
        }
    }

    //项目信息是否保存
    m_curShowResultProjectResolver = m_projectTreeModel->getActiveProjectResolver();
    if(m_curShowResultProjectResolver->okToContinue())
    {
        SProjectParameter *projectParameter = m_curShowResultProjectResolver->getProjectParameter();
        bool warningFlag = false;
        for(int i = 0; i < projectParameter->xmlParameterList->count(); ++i)
        {
            if(m_projectSettingModel->index(i, ProjectSettingXmlName).data(Qt::CheckStateRole) == Qt::Unchecked)
            {
                continue;
            }
            if(!QFileInfo(m_projectSettingModel->index(i, ProjectSettingXmlName)
                          .data(Qt::EditRole).toString().trimmed()).exists())
            {
                QMessageBox::critical(this, tr(u8"错误"), tr(u8"存在无效参数，请核对！"));
                ui->stackedWidget->setCurrentIndex(3);
                return;
            }
            for(int j = ProjectSettingInFifoPath0; j <= ProjectSettingInMemoryPath4; ++j)
            {
                QString fileName = m_projectSettingModel->index(i, j).data(Qt::EditRole).toString().trimmed();
                if(!fileName.isEmpty() && !checkFileExists(projectParameter, fileName))
                {
                    QMessageBox::critical(this, tr(u8"错误"), tr(u8"存在无效参数，请核对！"));
                    ui->stackedWidget->setCurrentIndex(3);
                    return;
                }
            }

            for(int j = ProjectSettingInFifoPath0; j <= ProjectSettingInMemoryPath4; ++j)
            {
                QString fileName = m_projectSettingModel->index(i, j).data(Qt::EditRole).toString().trimmed();
                if((j > ProjectSettingXmlName && j < ProjectSettingInMemoryPath0)
                        && !fileName.isEmpty()
                        && QFileInfo(fileName).suffix().compare("fifo", Qt::CaseInsensitive) != 0)
                {
                    warningFlag = true;
                }
                if((j > ProjectSettingInFifoPath3 && j < ProjectSettingOnlyRsm)
                        && !fileName.isEmpty()
//                        && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0
                        && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0)
                {
                    warningFlag = true;
                }
            }
        }
        if(warningFlag)
        {
            int &&ret = QMessageBox::warning(this, qApp->applicationName(),
                                             tr(u8"项目配置存在告警，是否忽略并继续？"),
                                             QMessageBox::Yes | QMessageBox::No
                                             /*| QMessageBox::Cancel*/);
            if (ret == QMessageBox::No)
            {
                return;
            }
        }
    }
    else
    {
        return;
    }

    if(m_process && m_process->state() != QProcess::NotRunning)
    {
        int &&ret = QMessageBox::warning(this, tr(u8"错误"),
                                         tr(u8"正在进行软件仿真调试，是否关闭仿真调试？"),
                                         QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes)
        {
            qInfo() << tr(u8"运行或调试：正在进行软件仿真调试，即将关闭软件仿真调试");
            stopSimDebug();
        }
        else
        {
            qInfo() << tr(u8"运行或调试：正在进行软件仿真调试，选择保留当前软件仿真调试");
            return;
        }
        m_process->waitForFinished();
    }
    if(CChipDebug::getIsDebugging())
    {
        int &&ret = QMessageBox::warning(this, tr(u8"错误"),
                                         tr(u8"正在进行硬件联调，是否停止联调？"),
                                         QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes)
        {
            qInfo() << tr(u8"运行或调试：正在进行硬件联调，即将停止硬件联调");
            stopJointDebug();
        }
        else
        {
            qInfo() << tr(u8"运行或调试：正在进行硬件联调，选择保留现有联调");
            return;
        }
    }

    m_selectedXmlParameterList.clear();
    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    for(int i = 0; i < m_curProjectResolver->getProjectParameter()->xmlParameterList->count(); ++i)
    {
        if(m_curProjectResolver->getProjectParameter()->xmlParameterList->at(i).selected == Qt::Checked)
        {
            m_selectedXmlParameterList.append(m_curProjectResolver->getProjectParameter()->xmlParameterList->at(i));
        }
    }
    if(m_selectedXmlParameterList.count() < 1)
    {
        qInfo() << tr(u8"运行或调试：没有打开项目或打开的项目内没有可运行或调试的文件，退出运行或调试");
        //        QMessageBox::information(this, tr(u8"提示"), tr(u8"<p>没有可用的项目或xml文件"
        //                                                    "<p>注：从项目中打开xml，关闭此项目后，打开的xml不可调试！"));
        QMessageBox::information(this, tr(u8"提示"), tr(u8"<p>没有可用的项目来运行或调试！"));
        return;
    }

    if(!CAppEnv::clcCycleOutDir())
    {
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"Cycle OUT目录未能清空！"));
        qWarning() << tr(u8"运行或调试：Cycle OUT目录未能清空！");
        return;
    }
    try {
        CAppEnv::checkDirEnv();
    } catch (CError &error) {
        qCritical() << tr(u8"运行或调试：程序环境校验未通过");
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"程序遇到问题：%0！")
                              .arg(QString::fromUtf8(error.what())));
    }

    if(m_isSimDebug)
    {
        if(mode == "debug")
        {
            qInfo() << tr(u8"运行或调试：进入软件仿真调试模式");

            auto* projectResolver = m_curProjectResolver;
            m_simCtrl = rpu_esl_model::CSimulatorControl::getInstance(CAppEnv::getCustomEtcSimulatorConfigFilePath().toStdString());
            if(m_simCtrl == nullptr)
            {
                QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器异常！"));
                return;
            }
//            m_simCtrl->stopSim();

            ESIM_RET ret = m_simCtrl->loadProject(projectResolver->getProjectFileName().toStdString());
            if(ESIM_RET::RET_SUCCESS != ret)
            {
                QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(QString::fromStdString(m_simCtrl->getRetMsg())));
                return;
            }
//            simCtrl->setDebugMode();
//            m_simCtrl->startSimDebug();
            m_simThread = std::thread(&CSimulatorControl::startSimDebug, m_simCtrl);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            m_lastIndex = ui->stackedWidget->currentIndex();
            startSimDebug();
        }
        else if(mode == "run")
        {
            qInfo() << tr(u8"运行或调试：进入软件仿真运行模式");

            auto* projectResolver = m_curProjectResolver;
            SimCtrlRAII simRAII(&m_simCtrl);
//            m_simCtrl = rpu_esl_model::CSimulatorControl::getInstance(CAppEnv::getEtcSimulatorConfigFilePath().toStdString());
//            m_simCtrl->stopSim();
            ESIM_RET ret = m_simCtrl->loadProject(projectResolver->getProjectFileName().toStdString());
            if(ESIM_RET::RET_SUCCESS != ret)
            {
                QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(QString::fromStdString(m_simCtrl->getRetMsg())));
                return;
            }

            m_progressStatusBar->setProgressBarValue(0);
            m_progressStatusBar->readyToStart(CAppEnv::getImageDirPath() + "/128/start128.png");
            CWaitDialog::getInstance(this)->readyToShowDlg();
            CWaitDialog::getInstance(this)->setText(tr(u8"仿真运行中"));
            QTime runTime;
            runTime.start();

            QFuture<void> future = QtConcurrent::run(m_simCtrl, &rpu_esl_model::CSimulatorControl::startSim);
        #if defined(Q_OS_WIN)
            Sleep(50);
        #else
            usleep(500);
        #endif
//            future.waitForFinished();
            while(!future.isFinished()) {
                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            }
            qInfo() << tr(u8"仿真运行消逝：%1毫秒").arg(runTime.elapsed());
            m_simCtrl->stopSim();

            CWaitDialog::getInstance(this)->finishShowing();
            m_progressStatusBar->finished();

            for(auto i = 0; i < m_selectedXmlParameterList.count(); ++i)
            {
                bool error = false;
                std::shared_ptr<std::vector<int>> outFifo;
                if (ESIM_RET::RET_SUCCESS != m_simCtrl->getOutFifoValue(i, outFifo))  //FIXME ONE CORE ONLY
                {
                    error &= true;
                    QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(QString::fromStdString(m_simCtrl->getRetMsg())));
                }
                std::shared_ptr<std::vector<int>> memory = nullptr;
                if (ESIM_RET::RET_SUCCESS != m_simCtrl->getMemoryValue(i, memory))  //FIXME ONE CORE ONLY
                {
                    error &= true;
                    QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(QString::fromStdString(m_simCtrl->getRetMsg())));
                }

                if(!error)
                {
                    projectResolver->setOutfifoRunResult(i, outFifo);
                    projectResolver->setMemoryRunResult(i, memory);
                }
            }
            projectResolver->m_lastRunXmlParameterList.clear();
            projectResolver->m_lastRunXmlParameterList = m_selectedXmlParameterList;
            projectResolver->setHasResult(true);
            projectResolver->setDateTime();
            updateRunResult(projectResolver);
        }
    }
    else
    {
        qInfo() << tr(u8"运行或调试：进入硬件联调模式");
        QMessageBox::information(this, qApp->applicationName(), tr(u8"联调功能即将上线，敬请期待！"));
        return;

        qInfo() << tr(u8"运行或调试：进入硬件联调模式");
        m_lastIndex = ui->stackedWidget->currentIndex();
        startJointDebug();
    }
    updateActions();
}

/*!
* MainWindow::sendCmd
* \brief   发送命令
* \param   cmd
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::sendCmd(const QString &cmd)
{
//    qInfo() << tr(u8"软件仿真，发送命令：%0").arg(cmd);
//    if(cmd.trimmed().isEmpty())
//    {
//        if(m_isGui && m_cmdStatus != rcsStop && m_cmdStatus != rcsWaitCmd)
//            return;
//        ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        return;
//    }
//    if(cmd.trimmed() == "about" || cmd.trimmed() == "version")
//    {
//        ui->action_About->triggered();
//        ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        return;
//    }
//    if(cmd.trimmed() == "clc")
//    {
//        ui->plainTextEdit->clear();
//        ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        return;
//    }
//    if(cmd.trimmed() == "history")
//    {
//        for(int i = 0; i < ui->plainTextEdit->m_historyList.count(); ++i)
//        {
//            ui->plainTextEdit->append(tr(u8"<font color=#FF6A00>%0:</font> %2").arg(i + 1)
//                                      .arg(ui->plainTextEdit->m_historyList.at(i)));
//        }
//        ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        return;
//    }
//    if(cmd.trimmed() == "rcs start")
//    {
//        if(m_process && m_process->state() != QProcess::NotRunning)
//        {
//            ui->plainTextEdit->append("RCS is already running!");
//            ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//            return;
//        }
//        if(m_process == nullptr)
//        {
//            DBG << tr(u8"初始化process");
//            ui->plainTextEdit->append("");
//            m_process = new QProcess(this);
//            connect(m_process, SIGNAL(started()), this, SLOT(rcsProcessStarted()));
//            connect(m_process, SIGNAL(readyReadStandardOutput()),
//                    this, SLOT(rcsReadyReadStandardOutput()));
//            connect(m_process, SIGNAL(readyReadStandardError()),
//                    this, SLOT(rcsReadyReadStandardError()));
//            connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
//                    this, SLOT(rcsProcessFinished(int,QProcess::ExitStatus)));
//        }
//        m_isGui = false;
//        QString cmd;
//#if defined(Q_OS_WIN)
//        if(QFileInfo(qApp->applicationDirPath() + "/rcs.exe").exists())
//            cmd = qApp->applicationDirPath() + "/rcs.exe";
//#elif defined(Q_OS_UNIX)
//        if(QFileInfo(qApp->applicationDirPath() + "/rcs").exists())
//            cmd = qApp->applicationDirPath() + "/rcs";
//#endif
//        else
//        {
//            appendToTextEdit("rcs not exists!");
//            delete m_process;
//            //            m_process = nullptr;
//            m_cmdStatus = rcsStop;
//            if(m_debugLog)
//                on_action_Stop_triggered();
//            m_isRunning = false;
//            m_progressStatusBar->finished();
//            updateActions();
//            QMessageBox::critical(this, tr(u8"错误"), tr(u8"未找到仿真软件RCS！"));
//            qCritical() << tr(u8"未找到仿真软件RCS！");
//        }
//        m_process->setProgram(cmd);
//        m_process->start();
//        return;
//    }
//    if(cmd.trimmed() == "rcs status")
//    {
//        int status = -1;
//        if(m_process)
//            status = m_process->state();
//        switch (status) {
//        case -1:
//        {
//            ui->plainTextEdit->append("process not init!");
//            ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        }
//            break;
//        case 0:
//        {
//            ui->plainTextEdit->append("rcs not Running!");
//            ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        }
//            break;
//        case 1:
//        {
//            ui->plainTextEdit->append("rcs Starting!");
//            ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        }
//            break;
//        case 2:
//        {
//            ui->plainTextEdit->append("rcs Running!");
//            ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        }
//            break;
//        default:
//            break;
//        }
//        return;
//    }
//    if(cmd.trimmed() == "rcs stop" || cmd.trimmed() == "exit" || cmd.trimmed() == "bye")
//    {
//        if(m_process && m_process->state() != QProcess::NotRunning)
//        {
//            sendCmd("quit");
//        }
//        return;
//    }
//    if(!m_process)
//    {
//        ui->plainTextEdit->append("rcs not running!");
//        ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        return;
//    }
//    ui->plainTextEdit->append("");
//    qInfo() << tr(u8"发送RCS命令：%0").arg(cmd);
//    m_process->write(cmd.toLocal8Bit() + "\n");
}

/*!
* MainWindow::processReadyReadStandardOutput
* \brief   收到RCS标准输出
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::rcsReadyReadStandardOutput()
{
//    QByteArray btyArry = m_process->readAllStandardOutput();
//    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
//    QString str = gbkCodec->toUnicode(btyArry);

//    qInfo() << tr(u8"收到仿真器RCS输出：%0").arg(str);
//    if(!str.endsWith("$rcs>"))
//    {
//        qInfo() << tr(u8"收到仿真器RCS输出：未知消息");
//        return;
//    }
//    str.remove(str.length() - 5, 5);
//    str = str.trimmed();
//    //    QTextCodec *codec = QTextCodec::codecForName("GBK");
//    //    if (codec)
//    //    {
//    //        str = QString::fromLatin1(codec->fromUnicode(str));
//    //    }
//    if(str.isEmpty())
//    {
//        qInfo() << tr(u8"收到仿真器RCS输出：空消息");
//        return;
//    }

//    if(!m_isGui)
//    {
//        appendToTextEdit(str);
//        qInfo() << tr(u8"收到仿真器RCS输出：当前为命令行模式，控制台打印消息：%0").arg(str);
//        return;
//    }

//    if(m_isRunMode)
//    {
//        qInfo() << tr(u8"收到仿真器RCS输出：当前为运行模式，解析消息并显示");
//        switch (m_cmdStatus) {
//        case rcsAtpStart:
//        {
//            if(!str.endsWith("start RCS for debug"))
//            {
//                break;
//            }
//            //else goto next case;
//        }
//        case rcsStart:
//        {
//            appendToTextEdit(str);
//            m_paraInfoIndex = 0;
//            m_cmdStatus = rcsLoad;
//            sendCmd(tr(u8"load %0").arg(CAppEnv::addQtaMrk(m_selectedXmlParameterList.at(m_paraInfoIndex).xmlPath).trimmed()));
//        }
//            break;
//        case rcsLoad:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsPara;
//            m_curClock = 0;
//            QString para = " infifo=" + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath0.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath1.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath2.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath3.trimmed()));
//            para += " mem=" + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath0.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath1.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath2.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath3.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath4.trimmed()));
//            para += " onlyrsm=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).onlyRsm);
//            para += " rmode0=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).rMode0);
//            para += " rmode1=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).rMode1);
//            para += " gap=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).gap);
//            para += " cycle=" + QString::number(m_setting.maxCycle);

//            QString source = m_selectedXmlParameterList.at(m_paraInfoIndex).source;
//            if(source.startsWith("M", Qt::CaseInsensitive))
//            {
//                int num = QString(source.at(source.length() - 1)).toInt();
//                int ten = QString(source.at(source.length() - 2)).toInt();
//                QString str;
//                if(ten)
//                    str = QString::number(ten * 10 + num);
//                else
//                    str = QString::number(num);
//                para += " source=" + str;
//            }
//            else if(source.startsWith("I", Qt::CaseInsensitive))
//            {
//                int num = QString(source.at(source.length() - 1)).toInt() + 16;
//                para += " source=" + QString::number(num);
//            }

//            QString dest = m_selectedXmlParameterList.at(m_paraInfoIndex).dest;
//            if(dest.startsWith("M", Qt::CaseInsensitive))
//            {
//                int num = QString(dest.at(dest.length() - 1)).toInt();
//                int ten = QString(dest.at(dest.length() - 2)).toInt();
//                QString str;
//                if(ten)
//                    str = QString::number(ten * 10 + num);
//                else
//                    str = QString::number(num);
//                para += " dest=" + str;
//            }
//            else if(dest.startsWith("O", Qt::CaseInsensitive))
//            {
//                int num = QString(dest.at(dest.length() - 1)).toInt() + 16;
//                para += " dest=" + QString::number(num);
//            }

//            para += " burst=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).burstNum);
//            para += " loop=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).loopNum);
//            para += " repeat=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).repeatNum);

//            if(m_setting.cycleOut)
//            {
//                QString cycleout = CAppEnv::getCycleOutDirPath() + "/"
//                        + QFileInfo(m_selectedXmlParameterList.at(m_paraInfoIndex).xmlPath).baseName() + ".log";
//                para += " log=" + CAppEnv::addQtaMrk(cycleout);
//            }
//            sendCmd(tr(u8"para%0").arg(para));
//        }
//            break;
//        case rcsPara:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsFinish;
//            sendCmd("finish");
//        }
//            break;
//        case rcsFinish:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsPrintOutFifo;
//            sendCmd("print outfifo");
//        }
//            break;
//        case rcsLoadXml:
//        {
//            appendToTextEdit(str);
//            m_memoryList.append(str);
//            ++m_paraInfoIndex;
//            if(m_paraInfoIndex < m_selectedXmlParameterList.count())
//            {
//                m_cmdStatus = rcsLoad;
//                sendCmd(tr(u8"load %0").arg(CAppEnv::addQtaMrk(m_selectedXmlParameterList.at(m_paraInfoIndex).xmlPath).trimmed()));
//            }
//            else
//            {
//                m_curShowResultProjectResolver->setMemoryList(m_memoryList);
//                m_curShowResultProjectResolver->setOutfifoList(m_outfifoList);
//                m_curShowResultProjectResolver->m_lastRunXmlParameterList.clear();
//                m_curShowResultProjectResolver->m_lastRunXmlParameterList = m_selectedXmlParameterList;
//                m_curShowResultProjectResolver->setHasResult(true);
//                m_curShowResultProjectResolver->setDateTime();
//                updateRunResult(m_projectTreeModel->getActiveProjectResolver());
//                m_cmdStatus = rcsStop;
//                sendCmd("quit");
//                m_progressStatusBar->setProgressBarValue(100);
//                if(m_process && m_process->state() != QProcess::NotRunning)
//                {
//                    m_process->waitForFinished();
//                    //                    while(!m_process->waitForFinished());
//                    //                    m_process->close();
//                    //                        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
//                }
//                qInfo() << tr(u8"仿真运行消逝：%1毫秒").arg(m_runTime.elapsed());
//            }
//        }
//            break;
//        case rcsPrintOutFifo:
//        {
//            appendToTextEdit(str);
//            m_outfifoList.append(str);
//            m_cmdStatus = rcsLoadXml;
//            sendCmd("print mem");
//        }
//            break;
//            //        case rcsPrintMemory:
//            //        {
//            //            appendToTextEdit(str);

//            //            ResultForm form(&m_paraInfoList, m_outfifoList, m_memoryList, this);
//            //            form.exec();
//            //            m_cmdStatus = rcsStop;
//            //            sendCmd("quit");
//            //            if(m_process && m_process->state() != QProcess::NotRunning)
//            //            {
//            //                while(!m_process->waitForFinished())
//            //                    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
//            //            }
//            //        }
//            //            break;
//        case rcsStop:
//            appendToTextEdit(str);
//            break;
//        case rcsWaitCmd:
//            appendToTextEdit(str);
//            break;
//        default:
//            break;
//        }
//    }
//    else
//    {
//        qInfo() << tr(u8"收到仿真器RCS输出：当前为调试模式");
//        switch (m_cmdStatus) {
//        case rcsAtpStart:
//        {
//            if(!str.endsWith("start RCS for debug"))
//            {
//                break;
//            }
//            //else goto next case;
//        }
//        case rcsStart:
//        {
//            appendToTextEdit(str);
//            m_paraInfoIndex = 0;
//            m_cmdStatus = rcsLoad;
//            sendCmd(tr(u8"load %0").arg(CAppEnv::addQtaMrk(m_selectedXmlParameterList.at(m_paraInfoIndex).xmlPath).trimmed()));
//        }
//            break;
//        case rcsLoad:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsPara;
//            m_curClock = 0;
//            m_hasNextClock = false;
//            m_justLoad = true;
//            QString para = " infifo=" + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath0.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath1.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath2.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inFifoPath3.trimmed()));
//            para += " mem=" + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath0.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath1.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath2.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath3.trimmed()))
//                    + "," + CAppEnv::addQtaMrk(sourceToAbsolute(m_selectedXmlParameterList.at(m_paraInfoIndex).inMemPath4.trimmed()));
//            para += " onlyrsm=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).onlyRsm);
//            para += " rmode0=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).rMode0);
//            para += " rmode1=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).rMode1);
//            para += " gap=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).gap);
//            para += " cycle=" + QString::number(m_setting.maxCycle);

//            QString source = m_selectedXmlParameterList.at(m_paraInfoIndex).source;
//            if(source.startsWith("M", Qt::CaseInsensitive))
//            {
//                int num = QString(source.at(source.length() - 1)).toInt();
//                int ten = QString(source.at(source.length() - 2)).toInt();
//                QString str;
//                if(ten)
//                    str = QString::number(ten * 10 + num);
//                else
//                    str = QString::number(num);
//                para += " source=" + str;
//            }
//            else if(source.startsWith("I", Qt::CaseInsensitive))
//            {
//                int num = QString(source.at(source.length() - 1)).toInt() + 16;
//                para += " source=" + QString::number(num);
//            }

//            QString dest = m_selectedXmlParameterList.at(m_paraInfoIndex).dest;
//            if(dest.startsWith("M", Qt::CaseInsensitive))
//            {
//                int num = QString(dest.at(dest.length() - 1)).toInt();
//                int ten = QString(dest.at(dest.length() - 2)).toInt();
//                QString str;
//                if(ten)
//                    str = QString::number(ten * 10 + num);
//                else
//                    str = QString::number(num);
//                para += " dest=" + str;
//            }
//            else if(dest.startsWith("O", Qt::CaseInsensitive))
//            {
//                int num = QString(dest.at(dest.length() - 1)).toInt() + 16;
//                para += " dest=" + QString::number(num);
//            }

//            para += " burst=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).burstNum);
//            para += " loop=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).loopNum);
//            para += " repeat=" + QString::number(m_selectedXmlParameterList.at(m_paraInfoIndex).repeatNum);

//            if(m_setting.cycleOut)
//            {
//                QString cycleout = CAppEnv::getCycleOutDirPath() + "/" + QFileInfo(m_selectedXmlParameterList.at(m_paraInfoIndex).xmlPath).baseName() + ".log";
//                para += " log=" + CAppEnv::addQtaMrk(cycleout);
//            }
//            sendCmd(tr(u8"para%0").arg(para));
//        }
//            break;
//        case rcsPara:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsInfo;
//            sendCmd("info");
//        }
//            break;
//        case rcsInfo:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsPrintAll;
//#if defined(Q_OS_WIN)
//            QStringList strList = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//            QStringList strList = str.split("\n");
//#endif
//            foreach(QString string, strList)
//            {
//                if(string.startsWith("next", Qt::CaseInsensitive))
//                {
//                    QString hasNext = string.split(":").at(1);
//                    if(QString::compare("true", hasNext.trimmed(), Qt::CaseInsensitive) == 0)
//                    {
//                        m_hasNextClock = true;
//                    }
//                    else
//                    {
//                        m_hasNextClock = false;
//                    }
//                }
//            }

//            //        sendCmd("print bcu rcu bfu infifo outfifo mem sbox benes");
//            sendCmd("print bcu rcu bfu sbox benes");
//        }
//            break;
//        case rcsPrintAll:
//        {
//            DBG << "in rcsPrintAll" << m_paraInfoIndex << m_curIndexOfAll;
//            if(m_paraInfoIndex == 0 || m_paraInfoIndex == m_curIndexOfAll)
//            {
//                appendToTextEdit(str);
//                m_cmdStatus = rcsWaitCmd;
//#if defined(Q_OS_WIN)
//                QStringList infoList = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//                QStringList infoList = str.split("\n");
//#endif
//                infoList.removeAt(0);

//                if(m_paraInfoIndex == 0 && m_justLoad)
//                {
//                    //首张图
//                    startSimDebug(infoList);
//                    return;
//                }
//                if(m_justLoad)
//                {
//                    //下张图
//                    setNextDebugXml(infoList);
//                }
//                else
//                {
//                    //下一周期
//                    m_simDebugToolBar->getSpinBox()->setMinimum(m_curClock);
//                    m_simDebugToolBar->getSpinBox()->setValue(m_curClock);

//                    ui->action_Next->setEnabled(m_hasNextClock);
//                    ui->action_Continue->setEnabled(m_hasNextClock);
//                    ui->action_LastClock->setEnabled(m_hasNextClock);
//                    m_simDebugToolBar->getSpinBox()->setEnabled(m_hasNextClock);
//                    m_debugLog->setAllElementInfo(infoList);
//                }
//            }
//            else
//            {
//                m_cmdStatus = rcsLoadXml;
//                sendCmd("finish");
//            }
//        }
//            break;
//        case rcsLoadXml:
//        {
//            appendToTextEdit(str);
//            ++m_paraInfoIndex;
//            m_cmdStatus = rcsLoad;
//            sendCmd(tr(u8"load %0").arg(CAppEnv::addQtaMrk(m_selectedXmlParameterList.at(m_paraInfoIndex).xmlPath).trimmed()));
//        }
//            break;
//        case rcsNextClock:
//        {
//            appendToTextEdit(str);
//#if defined(Q_OS_WIN)
//            QStringList strList = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//            QStringList strList = str.split("\n");
//#endif
//            foreach(QString string, strList)
//            {
//                if(string.startsWith("NEXT", Qt::CaseInsensitive))
//                {
//                    QString hasNext = string.split(":").at(1);
//                    if(QString::compare("TRUE", hasNext.trimmed(), Qt::CaseInsensitive) == 0)
//                    {
//                        m_hasNextClock = true;
//                    }
//                    else
//                    {
//                        m_hasNextClock = false;
//                    }
//                }
//            }
//            ++m_curClock;
//            m_justLoad = false;
//            m_cmdStatus = rcsPrintAll;
//            sendCmd("print bcu rcu bfu sbox benes");
//        }
//            break;
//        case rcsJumpToClock:
//        {
//            appendToTextEdit(str);
//#if defined(Q_OS_WIN)
//            QStringList strList1 = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//            QStringList strList1 = str.split("\n");
//#endif
//            foreach(QString string, strList1)
//            {
//                if(string.startsWith("NEXT", Qt::CaseInsensitive))
//                {
//                    QString hasNext = string.split(":").at(1);
//                    if(QString::compare("TRUE", hasNext.trimmed(), Qt::CaseInsensitive) == 0)
//                    {
//                        m_hasNextClock = true;
//                    }
//                    else
//                    {
//                        m_hasNextClock = false;
//                    }
//                }
//                if(string.startsWith("CLK", Qt::CaseInsensitive))
//                {
//                    QString hasNext = string.split(":").at(1);
//                    bool ok;
//                    int clock = hasNext.trimmed().toInt(&ok);
//                    if(ok)
//                    {
//                        m_curClock = clock;
//                        m_simDebugToolBar->getSpinBox()->setValue(clock);
//                    }
//                }
//            }
//            m_justLoad = false;
//            m_cmdStatus = rcsPrintAll;
//            sendCmd("print bcu rcu bfu sbox benes");
//        }
//            break;
//        case rcsFinish:
//        {
//            appendToTextEdit(str);
//            //            m_curClock = m_paraInfoList.at(m_paraInfoIndex).cycle;
//            m_justLoad = false;
//            m_hasNextClock = false;
//            m_cmdStatus = rcsPrintAll;

//#if defined(Q_OS_WIN)
//            QStringList strList = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//            QStringList strList = str.split("\n");
//#endif

//            foreach(QString string, strList)
//            {
//                if(string.startsWith("CLK", Qt::CaseInsensitive))
//                {
//                    QString hasNext = string.split(":").at(1);
//                    m_curClock = hasNext.trimmed().toInt();
//                }
//            }

//            sendCmd("print bcu rcu bfu sbox benes");
//        }
//            break;
//        case rcsContinue:
//        {
//            appendToTextEdit(str);
//#if defined(Q_OS_WIN)
//            QStringList stringList = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//            QStringList stringList = str.split("\n");
//#endif
//            foreach(QString string, stringList)
//            {
//                if(string.startsWith("NEXT", Qt::CaseInsensitive))
//                {
//                    QString hasNext = string.split(":").at(1);
//                    if(QString::compare("TRUE", hasNext.trimmed(), Qt::CaseInsensitive) == 0)
//                    {
//                        m_hasNextClock = true;
//                    }
//                    else
//                    {
//                        m_hasNextClock = false;
//                    }
//                }
//            }
//            int clock = 0;
//#if defined(Q_OS_WIN)
//            QStringList strList = str.split("\r\n");
//#elif defined(Q_OS_UNIX)
//            QStringList strList = str.split("\n");
//#endif
//            if(strList.count() > 0)
//                strList = QString(strList.at(0)).split(":");
//            if(strList.count() == 2)
//                clock = QString(strList.at(1)).toInt();
//            if(clock == 0)
//                return;
//            m_curClock = clock;
//            m_justLoad = false;
//            m_cmdStatus = rcsPrintAll;
//            sendCmd("print bcu rcu bfu sbox benes");
//        }
//            break;
//        case rcsEditValue:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsWaitCmd;
//        }
//            break;
//        case rcsSetBreak:
//        {
//            appendToTextEdit(str);
//            m_cmdStatus = rcsWaitCmd;
//        }
//            break;
//        case rcsWaitCmd:
//            appendToTextEdit(str);
//            break;
//        default:
//            break;
//        }
//    }
}

/*!
* MainWindow::processReadyReadStandardError
* \brief   收到RCS错误输出
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::rcsReadyReadStandardError()
{
//    m_cmdStatus = rcsWaitCmd;
//    QString str = m_process->readAllStandardError();
//    qInfo() << tr(u8"收到仿真器RCS错误输出：").arg(str);
//    //    if(!str.endsWith("$rcs>"))
//    //        return;
//    //    str.remove(str.length() - 5, 5);
//    str = str.trimmed();
//    if(!str.isEmpty())
//    {
//        QTextCursor textCursor = ui->plainTextEdit->textCursor();
//        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
//        ui->plainTextEdit->setTextCursor(textCursor);
//        ui->plainTextEdit->insertPlainText(str);
//        ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//        //        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
//        //        ui->plainTextEdit->setTextCursor(textCursor);
//    }

//    if(m_isGui)
//    {
//        sendCmd("quit");
//        qWarning() << tr(u8"收到仿真器RCS错误输出：当前为图形界面模式，退出RCS，错误信息：%0").arg(str);
//        if(str.length() > 100)
//            str = str.left(100) + "...";
//        //        QMessageBox::critical(this, tr(u8"错误"), tr(u8"RCS报告错误：%0！").arg(str));
//        CSingleRcsErrorMessageBox::readyToShow(QMessageBox::Critical, tr(u8"仿真器RCS遇到错误：%0！").arg(str), this);
//        //        CSingleRcsErrorMessageBox::releaseInstance();
//    }
}

/*!
* MainWindow::appendToTextEdit
* \brief   收到RCS输出，显示到控制台
* \param   msg
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::appendToTextEdit(const QString &msg)
{
    qInfo() << tr(u8"控制台打印消息：%0").arg(msg);
    //    qInfo() << tr(u8"收到SIM输出，显示到控制台“%1”").arg(msg);
    //    QTextCursor textCursor = ui->plainTextEdit->textCursor();
    //    textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    //    ui->plainTextEdit->setTextCursor(textCursor);
    //    ui->plainTextEdit->insertPlainText(msg);
    //    ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
    //    textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    //    ui->plainTextEdit->setTextCursor(textCursor);

    ui->widget_cmd->setVisible(true);
    ui->textEdit_log->append(msg);
    QTextDocument *doc =  ui->textEdit_log->document();
    QTextCursor textcursor = ui->textEdit_log->textCursor();
    for(QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextBlockFormat tbf = it.blockFormat();
        tbf.setLineHeight(130,QTextBlockFormat::ProportionalHeight);
        textcursor.setPosition(it.position());
        textcursor.setBlockFormat(tbf);
        ui->textEdit_log->setTextCursor(textcursor);
    }
}

void MainWindow::appendToTextEdit(EMsgLevel level, const QString &msg, bool showMsgBox)
{
    switch (level) {
    case MSG_INFOMATION:
        appendToTextEdit(QString("<a><font color=#000000>Information: </font></a>").arg(msg));
        break;
    case MSG_WARNING:
        appendToTextEdit(QString("<a><font color=#D87300>Warning: %1</font></a>").arg(msg));
        break;
    case MSG_CRITICAL:
        appendToTextEdit(QString("<a><font color=#DA0000>Critical: %1</font></a>").arg(msg));
        if(showMsgBox)
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(msg));
        break;
    case MSG_FATAL:
        appendToTextEdit(QString("<a><font color=#DA0000>Fatal: %1</font></a>").arg(msg));
        if(showMsgBox)
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(msg));
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void MainWindow::simDebugBreakAdd(const rpu_esl_model::SBreak &breakHandle)
{
    RcaGraphView *view = qobject_cast<RcaGraphView *>(sender());
    int coreIndex = m_curDebugViewMap.key({view->getXml()->getCurFile(), view}, -1);
    if(coreIndex >= 0 && coreIndex <= 15)
    {
        if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode())
            m_simCtrl->debug_breakAdd(coreIndex, breakHandle);
    }
}

void MainWindow::simDebugBreakRemove(const rpu_esl_model::SBreak &breakHandle)
{
    RcaGraphView *view = qobject_cast<RcaGraphView *>(sender());
    int coreIndex = m_curDebugViewMap.key({view->getXml()->getCurFile(), view}, -1);
    if(coreIndex >= 0 && coreIndex <= 15)
    {
        if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode())
            m_simCtrl->debug_breakRemove(coreIndex, breakHandle);
    }
}

/*!
* MainWindow::processStarted
* \brief   检测到RCS运行
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::rcsProcessStarted()
{
//    qInfo() << tr(u8"仿真软件RCS运行");
//    //    ui->plainTextEdit->append("rcs started!");
//    //    ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//    ui->widget_cmd->setVisible(true);
//    m_cmdStatus = rcsStart;
}

/*!
* MainWindow::processFinished
* \brief   检测到RCS停止
* \param   code
* \param   exitStatus
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::rcsProcessFinished(const int &code, const QProcess::ExitStatus &exitStatus)
{
//    qInfo() << tr(u8"仿真软件RCS停止：退出码“%0”，退出原因“%1”").arg(code).arg(exitStatus);
//    delete m_process;
//    //    m_process = nullptr;
//    //    ui->plainTextEdit->append("quit now!");
//    //    ui->plainTextEdit->append("<a><font color=#5aff5a>DSN></font> </a>");
//    appendToTextEdit("quit now!");
//    m_cmdStatus = rcsStop;
//    if(m_debugLog)
//        on_action_Stop_triggered();

//    m_isRunning = false;
//    m_progressStatusBar->finished();
//    updateActions();
}

void MainWindow::updateRunResult(CProjectResolver *projectResolver)
{
    qInfo() << tr(u8"查看运行结果");
    m_curShowResultProjectResolver = projectResolver;
    if(!m_curShowResultProjectResolver->getHasResult())
    {
        QMessageBox::information(this, tr("错误"), tr("没有可用的运行结果！"));
        return;
    }
    disconnect(ui->treeWidget_runResult,
               SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this,
               SLOT(on_treeWidget_runResult_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    ui->treeWidget_runResult->clear();
    ui->tableWidget_runResult->clearContents();
    ui->tableWidget_runResult->setRowCount(0);
    ui->label_runResultProjectName->setText(tr(u8"项目名称：%0").arg(m_curShowResultProjectResolver->getProjectFileName()));
    ui->label_runResultTime->setText(tr(u8"运行于：%0").arg(m_curShowResultProjectResolver->getDateTime()));

    for(int i = 0; i < m_curShowResultProjectResolver->m_lastRunXmlParameterList.count(); ++i)
    {
        SXmlParameter xmlParameter = m_curShowResultProjectResolver->m_lastRunXmlParameterList.at(i);
        if(xmlParameter.selected == Qt::Unchecked)
            continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_runResult);
        item->setIcon(0, QIcon(CAppEnv::getImageDirPath() + "/128/Xml-tool128.png"));
        item->setText(0, CAppEnv::stpToNm(xmlParameter.xmlPath));
        item->setToolTip(0, xmlParameter.xmlPath);
        item->setData(0, Qt::UserRole, xmlParameter.xmlPath);
        item->setText(1, tr(u8"XML文件"));
        item->setData(1, Qt::UserRole, i);

        QTreeWidgetItem *fifoOut = new QTreeWidgetItem(item);
        fifoOut->setText(0, tr(u8"OutFIFO"));
        fifoOut->setText(1, tr(u8"OutFIFO输出"));
//        fifoOut = new QTreeWidgetItem(item);
//        fifoOut->setText(0, tr(u8"FIFO1"));
//        fifoOut->setText(1, tr(u8"FIFO输出"));
//        fifoOut = new QTreeWidgetItem(item);
//        fifoOut->setText(0, tr(u8"FIFO2"));
//        fifoOut->setText(1, tr(u8"FIFO输出"));
//        fifoOut = new QTreeWidgetItem(item);
//        fifoOut->setText(0, tr(u8"FIFO3"));
//        fifoOut->setText(1, tr(u8"FIFO输出"));

        QTreeWidgetItem *memOut = new QTreeWidgetItem(item);
        memOut->setText(0, tr(u8"Memory"));
        memOut->setText(1, tr(u8"Memory输出"));
//        memOut = new QTreeWidgetItem(item);
//        memOut->setText(0, tr(u8"Memory1"));
//        memOut->setText(1, tr(u8"Memory输出"));
//        memOut = new QTreeWidgetItem(item);
//        memOut->setText(0, tr(u8"Memory2"));
//        memOut->setText(1, tr(u8"Memory输出"));
//        memOut = new QTreeWidgetItem(item);
//        memOut->setText(0, tr(u8"Memory3"));
//        memOut->setText(1, tr(u8"Memory输出"));
//        memOut = new QTreeWidgetItem(item);
//        memOut->setText(0, tr(u8"Memory4"));
//        memOut->setText(1, tr(u8"Memory输出"));
    }

    ui->treeWidget_runResult->expandAll();
    ui->treeWidget_runResult->resizeColumnToContents(0);

    ui->stackedWidget->setCurrentIndex(4);
    connect(ui->treeWidget_runResult, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(on_treeWidget_runResult_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

void MainWindow::on_treeWidget_runResult_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    QTreeWidgetItem *parent = current->parent();
    if(parent == nullptr)
        return;

    int row = parent->data(1, Qt::UserRole).toInt();
    int at = parent->indexOfChild(current); //item在父项中的节点行号(从0开始)
    if(at == 0)
    {
        showFifo(row);
    }
    else if(at == 1)
    {
        showMem(row);
    }
    else
    {
        Q_ASSERT(false);
    }
}

/**
* ResultForm::showFifo
* \brief
* \param   row
* \param   index
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::showFifo(const int row)
{
    ui->tableWidget_runResult->clearContents();
    ui->tableWidget_runResult->setRowCount(0);

    // 支持多核，row索引
    const auto &outFifo = m_curShowResultProjectResolver->getOutfifoRunResult(row);
    for(int i = 0; i < outFifo.size(); ++i)
    {
        int row = ui->tableWidget_runResult->rowCount();
        if(i % 4 == 0)
        {
            ui->tableWidget_runResult->insertRow(row);
        }
        row = ui->tableWidget_runResult->rowCount() - 1;
        QTableWidgetItem *item = new QTableWidgetItem();
        auto value = outFifo.at(i);
        item->setText(QByteArray((const char *)(&value), sizeof(value)).toHex());
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_runResult->setItem(row, i % 4, item);
    }
}

/**
* MainWindow::showMem
* \brief
* \param   row
* \param   index
* \author  zhangjun
* \date    2016-10-12
*/
void MainWindow::showMem(const int row)
{
    ui->tableWidget_runResult->clearContents();
    ui->tableWidget_runResult->setRowCount(0);

    // 支持多核，row索引
    const auto &memory = m_curShowResultProjectResolver->getMemoryRunResult(row);
    for(int i = 0; i < memory.size(); ++i)
    {
        int row = ui->tableWidget_runResult->rowCount();
        if(i % 4 == 0)
        {
            ui->tableWidget_runResult->insertRow(row);
        }
        row = ui->tableWidget_runResult->rowCount() - 1;
        QTableWidgetItem *item = new QTableWidgetItem();
        auto value = memory.at(i);
        item->setText(QByteArray((const char *)(&value), sizeof(value)).toHex());
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_runResult->setItem(row, i % 4, item);
    }
}

void MainWindow::runResultSaveToFile()
{
    QTreeWidgetItem *item = ui->treeWidget_runResult->currentItem();
    if(item->parent() != nullptr)
        return;

    int row = item->data(1, Qt::UserRole).toInt();
    QString fifo = CAppEnv::getRunResultOutDirPath() + "/" + item->text(0) + "." + "fifo.txt";
    QFile fifoFile(fifo);
    if(!fifoFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr(u8"保存到文件“%0”失败：\n%1.")
                             .arg(fifoFile.fileName())
                             .arg(fifoFile.errorString()));
        return;
    }
    QTextStream fifoOut(&fifoFile);
#if defined(Q_OS_WIN)
    QStringList fifoStrList = QString(m_curShowResultProjectResolver->getOutfifoList().at(row)).split("\r\n");
#elif defined(Q_OS_UNIX)
    QStringList fifoStrList = QString(m_curShowResultProjectResolver->getOutfifoList().at(row)).split("\n");
#endif
    if(fifoStrList.count() > 0)
    {
        fifoStrList.removeAt(0);
        foreach (QString str, fifoStrList)
        {
            fifoOut << str << endl;
        }
    }
    fifoOut.flush();
    fifoFile.close();


    QString memory = CAppEnv::getRunResultOutDirPath() + "/" + item->text(0) + "." + "memory.txt";
    QFile memFile(memory);
    if(!memFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr(u8"保存到文件“%0”失败：\n%1.")
                             .arg(memFile.fileName())
                             .arg(memFile.errorString()));
        return;
    }
    QTextStream memOut(&memFile);
#if defined(Q_OS_WIN)
    QStringList memStrList = QString(m_curShowResultProjectResolver->getMemoryList().at(row)).split("\r\n");
#elif defined(Q_OS_UNIX)
    QStringList memStrList = QString(m_curShowResultProjectResolver->getMemoryList().at(row)).split("\n");
#endif
    if(memStrList.count() > 0)
    {
        memStrList.removeAt(0);
        foreach (QString str, memStrList)
        {
            memOut << str << endl;
        }
    }
    memOut.flush();
    memFile.close();

    QDesktopServices::openUrl(QUrl("file:///" + CAppEnv::getRunResultOutDirPath()));
}

/*!
 * MainWindow::sourceToAbsolute
 * \brief   资源文件转为文件系统文件路径
 * \param   fileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString MainWindow::sourceToAbsolute(const QString &fileName, CProjectResolver *projectResolver) const
{
    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    if(!projectResolver)
        projectResolver = m_curProjectResolver;
    QString str;
    if(fileName.startsWith(":"))
    {
        str = projectResolver->getProjectParameter()->resourceMap.value(fileName, fileName);
    }
    else
    {
        str = fileName;
    }
    return str;
}

/*!
 * MainWindow::on_lineEdit_searchCmd_returnPressed
 * \brief   搜索命令行控制台中内容
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_lineEdit_searchCmd_returnPressed()
{
    qInfo() << tr(u8"搜索命令行控制台中内容");
    searchInCmd();
}

/*!
 * MainWindow::on_pushButton_hideDebugLogWidget_clicked
 * \brief   关闭命令行调试，并隐藏命令行窗口
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_pushButton_hideDebugLogWidget_clicked()
{
    qInfo() << tr(u8"关闭命令行调试，并隐藏命令行窗口");
    if(m_process && m_process->state() != QProcess::NotRunning)
    {
        sendCmd("quit");
    }
    ui->widget_cmd->setVisible(false);
}


/*!
 * MainWindow::on_action_Cmd_triggered
 * \brief   开启命令行控制台
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Cmd_triggered()
{
    qInfo() << tr(u8"开启命令行控制台");
    if(ui->widget_cmd->isVisible())
    {
        return;
    }
    ui->widget_cmd->setVisible(true);
    ui->lineEdit_cmd->setFocus();
}

bool MainWindow::checkFileExists(SProjectParameter *projectParameter, const QString &fileName) const
{
    if(fileName.startsWith(":"))
    {
        return QFileInfo(projectParameter->resourceMap.value(fileName)).exists();
    }
    else
    {
        return QFileInfo(fileName).exists();
    }
}

/*!
 * MainWindow::comboBoxCurrentIndexChanged
 * \brief   调试悬浮窗，切换当前调试XML
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::currentDebugXmlChanged(const int &index)
{
    qInfo() << tr(u8"调试悬浮窗，切换当前调试XML");
    const auto& fileName = m_curDebugViewMap.value(index, {QString(), nullptr}).first;
    if(!fileName.isEmpty())
        openXmlFile(fileName);

//    m_cmdStatus = rcsLoadXml;
//    sendCmd("finish");
}

/*!
 * MainWindow::setNextDebugLog
 * \brief   调试下一个XML，更新debuglog信息
 * \param   infoList
 * \param   totalClock
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::setNextDebugXml(const QStringList &infoList)
{
    qInfo() << tr(u8"调试下一个XML，更新debuglog信息");
//    m_debugLog->endDebugMode();
//    m_debugLog->startDebugMode(infoList);
//    m_debugLog->getDebugLogView()->setWindowTitle(m_simDebugToolBar->getComboBox()->currentText());

//    m_simDebugToolBar->getSpinBox()->setMinimum(0);
//    m_simDebugToolBar->getSpinBox()->setValue(0);

//    ui->action_Next->setEnabled(m_hasNextClock);
//    ui->action_Continue->setEnabled(m_hasNextClock);
//    ui->action_LastClock->setEnabled(m_hasNextClock);
//    m_simDebugToolBar->getSpinBox()->setEnabled(m_hasNextClock);

//    disconnect(m_simDebugToolBar->getComboBox(), SIGNAL(currentIndexChanged(int)),
//               this, SLOT(currentDebugXmlChanged(int)));
//    int currentNo = m_simDebugToolBar->getComboBox()->currentIndex();
//    for(int i = 0; i < currentNo; ++i)
//    {
//        m_simDebugToolBar->getComboBox()->removeItem(0);
//    }
//    connect(m_simDebugToolBar->getComboBox(), SIGNAL(currentIndexChanged(int)),
//            this, SLOT(currentDebugXmlChanged(int)));
}

void MainWindow::on_lineEdit_cmd_returnPressed()
{

//        m_debugAlg->sendCmd(ui->lineEdit_cmd->text());
        ui->lineEdit_cmd->appendCmd(ui->lineEdit_cmd->text());
        ui->lineEdit_cmd->clear();


}

/*!
 * MainWindow::startDebug
 * \brief   开始调试，初始化debuglog信息
 * \param   infoList
 * \param   totalClock
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::startSimDebug()
{
    qInfo() << tr(u8"开始调试，初始化调试模式");
//    QDesktopWidget *desktopWidget = QApplication::desktop();
//    QRect screenRect = desktopWidget->availableGeometry();
    if(m_simDebugToolBar == nullptr)
    {
        m_simDebugToolBar = new ToolBar(this);
        connect(m_simDebugToolBar->getSpinBox()->getAction(), SIGNAL(triggered(bool)),
                this, SLOT(jumpToCycle()));
        m_simDebugToolBar->addAction(ui->action_Next);
        m_simDebugToolBar->addAction(ui->action_Continue);
        m_simDebugToolBar->addAction(ui->action_LastClock);
        m_simDebugToolBar->addAction(ui->action_Stop);
        //        m_simDebugToolBar->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint
        //                                       | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
//        int marginLeft = 0.65 * screenRect.width();
//        int marginTop = 0.12 * screenRect.height();
//        m_simDebugToolBar->move(marginLeft, marginTop);
//        m_simDebugToolBar->setFloatable(true);
        //        m_debugToolBar->setGeometry(marginLeft, marginTop, marginLeft ,
        //                                    0.8 * screenRect.height());
//        ui->verticalLayout_13->addWidget(m_simDebugToolBar);
        ui->verticalLayout_10->addWidget(m_simDebugToolBar);
    }

    m_simDebugToolBar->getComboBox()->clear();
    for(int i = 0; i < m_selectedXmlParameterList.count(); ++i)
    {
        SXmlParameter xmlParameter = m_selectedXmlParameterList.at(i);
        if(xmlParameter.selected == Qt::Checked)
        {
            m_simDebugToolBar->getComboBox()->addItem(QIcon(CAppEnv::getImageDirPath() +
                                                            "/128/Xml-tool128.png"),
                                                      QFileInfo(xmlParameter.xmlPath).baseName());
            m_simDebugToolBar->getComboBox()->setFont(QFont("Arial", 9));
        }
    }

    if(m_simDebugToolBar->getComboBox()->count() > 0)
    {
        m_simDebugToolBar->getComboBox()->setCurrentIndex(0);
    }
    connect(m_simDebugToolBar->getComboBox(), SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentDebugXmlChanged(int)));


    m_curDebugViewMap.clear();
    auto* xmlParameterList = m_curProjectResolver->getProjectParameter()->xmlParameterList;
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for (int xmlIndex = 0, selectIndex = 0; xmlIndex < xmlParameterList->count(); ++xmlIndex)
    {
        const auto &xmlParameter = xmlParameterList->at(xmlIndex);
        if(xmlParameter.selected != Qt::Checked)
            continue;

        bool opened = false;
        RcaGraphView *rcaGraphView = nullptr;
        for(int i = 0; i < winList.count(); ++i)
        {
            rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
            if(QFileInfo(xmlParameter.xmlPath) == QFileInfo(rcaGraphView->getXml()->m_curFile))
            {
                opened = true;
                break;
            }
        }
        if(!opened)
        {
            if(openXmlFile(xmlParameter.xmlPath, m_curProjectResolver)) {
                rcaGraphView = m_curRcaGraphView;
            }
        }
        rcaGraphView->startDebugMode();
        m_curDebugViewMap.insert(selectIndex++, {xmlParameter.xmlPath, rcaGraphView});
    }

//    if(m_debugLog == nullptr)
//    {
//        QString fileName = m_simDebugToolBar->getComboBox()->itemData(0).toString();
//        DBG << tr(u8"当前调试文件：%0").arg(fileName);
//        m_debugLog = new CfgDebugLog(/*fileName*/);
//        m_debugLog->getDebugLogView()->setWindowTitle(m_simDebugToolBar->getComboBox()->currentText());
//        connect(m_debugLog->getDebugLogView(), SIGNAL(debugLogViewClose()),
//                this, SLOT(on_action_Stop_triggered()));
//        connect(m_debugLog, SIGNAL(sendChangedElementInfo(QString, QStringList, QStringList)),
//                this, SLOT(elementValueEdited(QString, QStringList, QStringList)));
//        connect(m_debugLog, SIGNAL(addBreakPoint(QString,QString)),
//                this, SLOT(elementSetBreak(QString, QString)));
//        connect(m_debugLog, SIGNAL(deleteBreakPoint(QString,QString)),
//                this, SLOT(elementRemoveBreak(QString, QString)));

//    }
//    m_debugLog->getDebugLogView()->setGeometry(0.1 * screenRect.width(), 0.15 * screenRect.height(),
//                                               0.8 * screenRect.width() , 0.8 * screenRect.height());
//    m_debugLog->startDebugMode();
//    ui->verticalLayout_13->insertWidget(0, m_debugLog->getDebugLogView());

    ui->stackedWidget->setCurrentIndex(1);
    m_simDebugToolBar->getSpinBox()->setMinimum(0);
    m_simDebugToolBar->getSpinBox()->setValue(0);
    updateDebugToolBar();
    m_simDebugToolBar->show();
    on_action_Cmd_triggered();
    showDebugValue();
}

/*!
 * MainWindow::on_action_Stop_triggered
 * \brief   退出调试
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Stop_triggered()
{
    if(m_isSimDebug)
        stopSimDebug();
    else
        stopJointDebug();
}

void MainWindow::stopSimDebug()
{
    qInfo() << tr(u8"退出仿真调试");

    ui->widget_cmd->setVisible(false);

    if(m_simCtrl) {
        m_simCtrl->stopSim();
        m_simCtrl = nullptr;
    }
    m_simThread.join();
    rpu_esl_model::CSimulatorControl::releaseInstance();
//    m_debugLog->endDebugMode();
//    disconnect(m_debugLog->getDebugLogView(), SIGNAL(debugLogViewClose()),
//               this, SLOT(on_action_Stop_triggered()));
//    m_debugLog->getDebugLogView()->close();


    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView && rcaGraphView->getIsDebugMode())
        {
            rcaGraphView->endDebugMode();
        }
    }

    m_simDebugToolBar->hide();
//    delete m_debugLog;
//    m_debugLog = nullptr;

    qApp->restoreOverrideCursor();

    m_simDebugToolBar->getSpinBox()->setMinimum(0);
    m_simDebugToolBar->getSpinBox()->setValue(0);
    disconnect(m_simDebugToolBar->getComboBox(), SIGNAL(currentIndexChanged(int)),
               this, SLOT(currentDebugXmlChanged(int)));
    updateDebugToolBar();
    ui->stackedWidget->setCurrentIndex(m_lastIndex);

    updateActions();
}

void MainWindow::stopJointDebug()
{
    qInfo() << tr(u8"退出硬件联调");
    if(!CChipDebug::hasInstance() || CChipDebug::getInstance(this)->getRunIsBusy())
        goto updateUi;
    CChipDebug::getInstance(this)->stopDebug();
//    ui->widget_cmd->setVisible(false);

    m_debugLog->endDebugMode();
    disconnect(m_debugLog->getDebugLogView(), SIGNAL(debugLogViewClose()),
               this, SLOT(on_action_Stop_triggered()));
    m_debugLog->getDebugLogView()->close();
    m_jointDebugToolBar->hide();
    delete m_debugLog;
    m_debugLog = nullptr;


updateUi:
    qApp->restoreOverrideCursor();
    m_jointDebugToolBar->getSpinBox()->setMinimum(0);
    m_jointDebugToolBar->getSpinBox()->setValue(0);
    ui->action_Next->setEnabled(false);
    ui->action_Continue->setEnabled(false);
    ui->action_LastClock->setEnabled(false);
    ui->action_PauseRun->setEnabled(false);
    m_jointDebugToolBar->getSpinBox()->setEnabled(false);
    m_jointDebugToolBar->getComboBox()->setEnabled(false);
    ui->action_Stop->setEnabled(false);
    updateActions();
    ui->stackedWidget->setCurrentIndex(m_lastIndex);
    CChipDebug::releaseInstance();
}

bool MainWindow::checkSimRet(rpu_esl_model::ESIM_RET ret)
{
    switch (ret) {
    case ESIM_RET::RET_SUCCESS:
        return true;
        break;
    case ESIM_RET::RET_INFOMATION:
        emit sendMsg("<a><font color=#000000>Information: </font></a>");
        emit sendMsg(QString::fromStdString(m_simCtrl ? m_simCtrl->getRetMsg() : ""));
        return true;
        break;
    case ESIM_RET::RET_WARNING:
        emit sendMsg(QString("<a><font color=#D87300>Warning: %1</font></a>").arg(QString::fromStdString(m_simCtrl ? m_simCtrl->getRetMsg() : "")));
        return true;
        break;
    case ESIM_RET::RET_CRITICAL:
        emit sendMsg(QString("<a><font color=#DA0000>Critical: %1</font></a>").arg(QString::fromStdString(m_simCtrl ? m_simCtrl->getRetMsg() : "")));
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(QString::fromStdString(m_simCtrl ? m_simCtrl->getRetMsg() : "")));
        return false;
        break;
    case ESIM_RET::RET_FATAL:
        emit sendMsg(QString("<a><font color=#DA0000>Fatal: %1</font></a>").arg(QString::fromStdString(m_simCtrl ? m_simCtrl->getRetMsg() : "")));
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"仿真器错误：%1！").arg(QString::fromStdString(m_simCtrl ? m_simCtrl->getRetMsg() : "")));
        return false;
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    return false;
}

void MainWindow::showDebugValue()
{
    if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode()) {
        vector<SPEValue> peValueList;
        ESIM_RET ret = m_simCtrl->getAllPEValue(0, peValueList); // FIXME core 0 only
        if(!checkSimRet(ret))
        {
            on_action_Stop_triggered();
            return;
        }
#if 0
        peValueList.clear();
        SPEValue peValue;
        peValue.index.rcuIndex = 0;
        peValue.inPort[0] = 0x11111111;
        peValue.outPort[0] = 0x22222222;

        for(auto i = 12; i >= 0; --i)
        {
            peValue.index.peIndex = i;
            peValue.inPort[0] = i;
            peValue.outPort[0] = i + 1;
            peValueList.push_back(peValue);
        }
#endif

        m_curRcaGraphView->setAllElementInfo(peValueList);
        int curCycle = m_simCtrl->getCurCycle();
        m_simDebugToolBar->getSpinBox()->setValue(curCycle);
        m_simDebugToolBar->getSpinBox()->setMinimum(curCycle);
    }
}

/*!
 * MainWindow::on_action_LastClock_triggered
 * \brief   调试模式，运行到最后一周期
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_LastClock_triggered()
{
    qInfo() << tr(u8"调试模式，运行到最后一周期");
    if(m_isSimDebug)
    {
        if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode()) {
            ESIM_RET ret = m_simCtrl->debug_run();
            if(!checkSimRet(ret))
            {
                on_action_Stop_triggered();
                return;
            }
            showDebugValue();
        }
    }
    else if(CChipDebug::getIsDebugging())
    {
//        if(!CChipDebug::getInstance(this)->runCycle(1))
//        {
//            on_action_Stop_triggered();
//        }
    }
    updateDebugToolBar();
}

/*!
 * MainWindow::on_action_Next_triggered
 * \brief   调试模式，下一周期
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Next_triggered()
{
    qInfo() << tr(u8"调试模式，下一周期");
    if(m_isSimDebug)
    {
        if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode()) {
            ESIM_RET ret = m_simCtrl->debug_next();
            if(!checkSimRet(ret))
            {
                on_action_Stop_triggered();
                return;
            }
            showDebugValue();
        }
    }
    else if(CChipDebug::getIsDebugging())
    {
        if(!CChipDebug::getInstance(this)->runCycle(1))
        {
            on_action_Stop_triggered();
        }
    }
    updateDebugToolBar();
}

/*!
 * MainWindow::on_action_Continue_triggered
 * \brief   调试模式，继续运行，遇到断点则停止
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Continue_triggered()
{
    qInfo() << tr(u8"调试模式，带断点运行");
    if(m_isSimDebug)
    {
        if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode()) {
            ESIM_RET ret = m_simCtrl->debug_continue();
            if(!checkSimRet(ret))
            {
                on_action_Stop_triggered();
                return;
            }
            showDebugValue();
        }
    }
    else if(CChipDebug::getIsDebugging())
    {
        if(!CChipDebug::getInstance(this)->runWithBreak())
            on_action_Stop_triggered();
    }
    updateDebugToolBar();
}


/*!
 * MainWindow::jumpToClock
 * \brief   调试模式，跳转到某周期
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::jumpToCycle()
{
    qInfo() << tr(u8"调试模式，跳转到某周期");
    if(m_isSimDebug)
    {
        if(m_simCtrl && m_simCtrl->getIsBusy() && m_simCtrl->getIsDebugMode()) {
            if(m_simDebugToolBar->getSpinBox()->value() <= m_simCtrl->getCurCycle())
            {
                return;
            }
            ESIM_RET ret = m_simCtrl->debug_jump(m_simDebugToolBar->getSpinBox()->value());
            if(!checkSimRet(ret))
            {
                on_action_Stop_triggered();
                return;
            }
            showDebugValue();
        }
    }
    else if(CChipDebug::getIsDebugging())
    {
        if(!CChipDebug::getInstance(this)->runCycle(m_jointDebugToolBar->getSpinBox()->value() - CChipDebug::getInstance(this)->getCurCycle()))
            on_action_Stop_triggered();
    }
    updateDebugToolBar();
}

/*!
 *
 * MainWindow::elementValueEdited
 * \brief   调试模式，元素当前值被编辑
 * \param   element
 * \param   input
 * \param   output
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::elementValueEdited(const QString &element, const QStringList &input, const QStringList &output)
{
    qInfo() << tr(u8"调试模式，修改算子值");
    QStringList elementIdList = element.split(",");
    if(elementIdList.count() != 4)
        return;

    QString cmd = "set ";
    cmd += tr(u8"bcu=%0 ").arg(elementIdList.at(1));
    cmd += tr(u8"rcu=%0 ").arg(elementIdList.at(2));
    switch (QString(elementIdList.at(0)).toInt()) {
    case 0:
    {
        if(input.count() != 3 || output.count() != 2)
            return;
        cmd += tr(u8"bfu=%0 ").arg(elementIdList.at(3));
    }
        break;
    case 1:
    {
        if(input.count() != 1 || output.count() != 4)
            return;
        cmd += tr(u8"sbox=%0 ").arg(elementIdList.at(3));
    }
        break;
    case 2:
    {
        if(input.count() != 4 || output.count() != 4)
            return;
        cmd += tr(u8"benes=%0 ").arg(elementIdList.at(3));
    }
        break;
    default:
        return;
        break;
    }

    cmd += tr(u8"value=%0,%1").arg(input.join(",")).arg(output.join(","));
    sendCmd(cmd);
}

/*!
 * MainWindow::elementSetBreak
 * \brief   调试模式，设置调试断点
 * \param   element
 * \param   portInfo
 * \author  zhangjun
 * \date    2016-10-12
 */
//void MainWindow::elementSetBreak(const QString &element, const QString &portInfo)
//{
//    qInfo() << tr(u8"调试模式，增加调试断点");
//    QStringList elementIdList = element.split(",");
//    if(elementIdList.count() != 4)
//        return;

//    QString cmd = "monitor ";
//    cmd += tr(u8"bcu=%0 ").arg(elementIdList.at(1));
//    cmd += tr(u8"rcu=%0 ").arg(elementIdList.at(2));
//    switch (QString(elementIdList.at(0)).toInt()) {
//    case 0:
//    {
//        cmd += tr(u8"bfu=%0 ").arg(elementIdList.at(3));
//    }
//        break;
//    case 1:
//    {
//        cmd += tr(u8"sbox=%0 ").arg(elementIdList.at(3));
//    }
//        break;
//    case 2:
//    {
//        cmd += tr(u8"benes=%0 ").arg(elementIdList.at(3));
//    }
//        break;
//    default:
//        return;
//        break;
//    }

//    cmd += portInfo;

//    sendCmd(cmd);
//}

/*!
 * MainWindow::elementRemoveBreak
 * \brief   调试模式，移除调试断点
 * \param   element
 * \param   portInfo
 * \author  zhangjun
 * \date    2016-10-12
 */
//void MainWindow::elementRemoveBreak(const QString &element, const QString &portInfo)
//{
//    qInfo() << tr(u8"调试模式，移除调试断点");
//    QStringList elementIdList = element.split(",");
//    if(elementIdList.count() != 4)
//        return;

//    QString cmd = "monitor -del ";
//    cmd += tr(u8"bcu=%0 ").arg(elementIdList.at(1));
//    cmd += tr(u8"rcu=%0 ").arg(elementIdList.at(2));
//    switch (QString(elementIdList.at(0)).toInt()) {
//    case 0:
//    {
//        cmd += tr(u8"bfu=%0 ").arg(elementIdList.at(3));
//    }
//        break;
//    case 1:
//    {
//        cmd += tr(u8"sbox=%0 ").arg(elementIdList.at(3));
//    }
//        break;
//    case 2:
//    {
//        cmd += tr(u8"benes=%0 ").arg(elementIdList.at(3));
//    }
//        break;
//    default:
//        return;
//        break;
//    }

//    cmd += portInfo;

////    m_cmdStatus = rcsSetBreak;
//    sendCmd(cmd);
//}

void MainWindow::on_pushButton_forecastClock_clicked()
{
    qInfo() << tr(u8"预估需要周期");
    analyseAlgorithm();
}

void MainWindow::on_pushButton_statistics_clicked()
{
    on_action_analyze_triggered();
}


void MainWindow::on_action_analyze_triggered()
{
    qInfo() << tr(u8"算法分析");
    analyseAlgorithm(true);
}


void MainWindow::analyseAlgorithm(const bool &showPerfom)
{
    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    if(!m_curProjectResolver->okToContinue())
        return;

#if defined(Q_OS_WIN)
    QString rcsCmd = CAppEnv::getBinDirPath() + tr(u8"/rcs.exe");
#elif defined(Q_OS_UNIX)
    QString rcsCmd = CAppEnv::getBinDirPath() + tr(u8"/rcs");
#endif
    QStringList rcsArgumentList;
    rcsArgumentList << "-cost" << m_curProjectResolver->getProjectFileName();
    QProcess process;
    process.setProgram(rcsCmd);
    process.setArguments(rcsArgumentList);
    process.setProcessChannelMode(QProcess::SeparateChannels);
    process.start();
    if(!process.waitForFinished())
        ui->label_forecastClock->setText(process.errorString());
    else
    {
        QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
        QByteArray errorBtyArry = process.readAllStandardError();
        QString errorMsg = gbkCodec->toUnicode(errorBtyArry);
        if(!errorMsg.isEmpty())
        {
            if(showPerfom)
            {
                ui->label_forecastClock->setText(tr(u8"算法分析失败"));
                QMessageBox::critical(this, tr("算法分析"), tr(u8"算法分析失败"));
            }
            else
            {
                ui->label_forecastClock->setText(tr(u8"预估需周期：无法计算"));
            }
            return;
        }
        QByteArray btyArry = process.readAllStandardOutput();
        QString str = gbkCodec->toUnicode(btyArry);
#if defined(Q_OS_WIN)
        QStringList strList = str.split("\r\n");
#elif defined(Q_OS_UNIX)
        QStringList strList = str.split("\n");
#endif
        QString last = strList.at(strList.count() - 2);
        QString clock = last.split(" ").at(2);
        ui->label_forecastClock->setText(tr(u8"预估需周期：%0").arg(clock));

        if(showPerfom)
        {
            SPerformanceStatistics perform;
            perform.totalCycle = clock.toInt();
            SXMLPerformanceStatistics xmlPerform;
            foreach (QString line, strList) {
                if(line.startsWith("file"))
                {
                    xmlPerform.xmlName = line.right(line.length() - line.indexOf(":") -  2);
                }
                else if(line.startsWith("au"))
                {
                    xmlPerform.auUsedCount = QString(line.split(" ").last()).toInt();
                }
                else if(line.startsWith("lu"))
                {
                    xmlPerform.luUsedCount = QString(line.split(" ").last()).toInt();
                }
                else if(line.startsWith("su"))
                {
                    xmlPerform.suUsedCount = QString(line.split(" ").last()).toInt();
                }
                else if(line.startsWith("sbox"))
                {
                    xmlPerform.tuUsedCount = QString(line.split(" ").last()).toInt();
                }
                else if(line.startsWith("benes"))
                {
                    xmlPerform.puUsedCount = QString(line.split(" ").last()).toInt();
                }
                else if(line.startsWith("=============="))
                {
                    perform.xmlPerformList.append(xmlPerform);
                }
            }

            PerformanceStatisticsForm form(&perform, this);
            form.exec();
        }
    }
}


void MainWindow::on_action_saveDebugStatus_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr(u8"保存调试状态"), "", tr(u8"rpudebug 文件 (*.rpudebug)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, qApp->applicationName(), tr("保存调试状态失败，文件打开失败：%0").arg(file.errorString()));
        return;
    }

    QDataStream out(&file);
    out << g_configMagicNumber << g_configVersionNumber;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    out.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    out.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    out.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    out.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    out.setVersion(QDataStream::Qt_5_6);
#endif

    updateCurrentSession();
    out << *m_sessionManager->getCurrentSession();

    CChipDebug::getInstance(this)->saveToDebugSession();
    if(m_debugLog)
    {
        m_debugSession->setHorizontalScrollBarPos(m_debugLog->getDebugLogView()->horizontalScrollBar()->sliderPosition());
        m_debugSession->setVerticalScrollBarPos(m_debugLog->getDebugLogView()->verticalScrollBar()->sliderPosition());
    }
    m_debugSession->setStackedWidgetIndex(ui->stackedWidget->currentIndex());
    out << *m_debugSession;
    file.close();
}

void MainWindow::on_action_restoreDebugStatus_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr(u8"恢复调试状态"), "", tr(u8"rpudebug 文件 (*.rpudebug)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, qApp->applicationName(), tr("恢复调试状态失败，文件打开失败：%0").arg(file.errorString()));
        return;
    }

    QDataStream in(&file);
    qint32 magicNumber;
    in >> magicNumber;
    if (magicNumber != g_configMagicNumber) {
        QMessageBox::warning(this, tr("错误"),
                tr("文件“%0”不是一个调试文件！").arg(file.fileName()));
        return;
    }
    qint16 versionNumber;
    in >> versionNumber;
    if (versionNumber > g_configVersionNumber) {
        QMessageBox::warning(this, tr("错误"),
                tr("%0 needs a more recent version of %1")
                .arg(file.fileName())
                .arg(QApplication::applicationName()));
        return;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    in.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    in.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    in.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    in.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    in.setVersion(QDataStream::Qt_5_6);
#endif

    CSession session;
    in >> session;
    CDebugSession debugSession;
    in >> debugSession;
    file.close();

    loadSessionToUI(&session);
    loadDebugSessionToUI(debugSession);
}


void MainWindow::on_action_saveWave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr(u8"保存"), "", tr(u8"RPU Curve 文件 (*.rpucurve)"));
    if(fileName.isEmpty())
        return;
    if(!fileName.endsWith(".rpucurve", Qt::CaseInsensitive))
        fileName.append(".rpucurve");
    CChipDebug::getInstance(this)->saveWaveData(fileName);
}


/*!
 * MainWindow::on_action_Code_triggered
 * \brief
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_Code_triggered()
{
    if(generateCmdWord())
    {
        if(!m_setting.cmdGenOutSideCommand.isEmpty())
        {
            if(QProcess::startDetached(m_setting.cmdGenOutSideCommand))
                appendToTextEdit(tr(u8"调用外部程序成功！（本程序仅启动外部程序，不对外部程序运行行为及结果负责！）"));
            else
                appendToTextEdit(tr(u8"调用外部程序失败！"));
        }
    }
}

bool MainWindow::generateCmdWord(CProjectResolver *projectResolver, bool askSaveXml, bool askSaveResrc)
{
    qInfo() << tr(u8"生成命令字");

    if(!m_projectTreeModel || m_projectTreeModel->rowCount() < 1)
    {
        qInfo() << tr(u8"生成命令字：没有可用的项目来生成命令字");
        QMessageBox::information(this, tr(u8"提示"), tr(u8"<p>没有可用的项目文件来生成命令字！"));
        return false;
    }

    //xml是否保存
    if(askSaveXml)
    {
        QStringList xmlList = m_projectTreeModel->getAllXmlList();
        foreach (QString xmlFileName, xmlList) {
            QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
            for(int i = 0; i < winList.count(); ++i)
            {
                RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
                if(!rcaGraphView)
                    continue;
                if(QFileInfo(xmlFileName) == QFileInfo(rcaGraphView->getXml()->m_curFile))
                {
                    if(!rcaGraphView->okToContinue())
                    {
                        return false;
                    }
                }
            }
        }
    }

    if(!CAppEnv::clcCmdWordDir())
    {
        qWarning() << tr(u8"生成命令字：Cmd Word输出目录未能清空，退出生成命令字");
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"Cmd Word输出目录未能清空！"));
        return false;
    }
    try {
        CAppEnv::checkDirEnv();
    } catch (CError &error) {
        qCritical() << tr(u8"运行or调试：程序环境校验未通过");
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"程序遇到问题：%0")
                              .arg(QString::fromUtf8(error.what())));
    }

    if(askSaveResrc && m_resourceEditorTableModel && m_resourceEditorTableModel->getIsModified())
    {
        ui->stackedWidget->setCurrentIndex(2);
        QString fileName = m_resourceEditorTableModel->getFileName();
        int ret = QMessageBox::warning(this, qApp->applicationName(),
                                         tr(u8"是否保存对资源 “%0” 的修改？")
                                         .arg(CAppEnv::stpToNm(fileName)),
                                         QMessageBox::Save | QMessageBox::Discard |
                                         QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            on_pushButton_saveResource_clicked();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
        else if (ret == QMessageBox::Discard)
        {
            //            m_resourceEditorTableModel->reload();
        }
    }

    QString cmdWordOutDirPath;
    if(m_setting.customCmdGenDirFlag)
        cmdWordOutDirPath = m_setting.customCmdGenDir;
    else
        cmdWordOutDirPath = CAppEnv::getCmdwordOutDirPath();
    
    
    QList<CProjectResolver *> projectResolverList;
    if(projectResolver)
        projectResolverList.append(projectResolver);
    else
        projectResolverList = m_projectTreeModel->getAllProjectResolver();
    
    CCode code(cmdWordOutDirPath);
    for(int i = 0; i < projectResolverList.count(); ++i)
    {
        CProjectResolver *projectResolver = projectResolverList.at(i);
        if(!projectResolver || !projectResolver->okToContinue())
        {
            return false;
        }

        if(!code.addProject(projectResolver))
        {
            qCritical() << tr(u8"生成命令字：失败：%1").arg(code.lastError());
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"生成配置码失败：%1！").arg(code.lastError()));
            m_progressStatusBar->finished();
            return false;
        }
        m_progressStatusBar->setProgressBarValue(((double)i / projectResolverList.count()) * 100);

    }
    code.finish();

    m_progressStatusBar->setProgressBarValue(100);
    m_progressStatusBar->finished();
    //    int &&ret = QMessageBox::information(this, tr(u8"生成命令字"),
    //                                       tr(u8"生成命令字成功，点击“Yes”浏览生成的文件并退出！"),
    //                                       QMessageBox::Yes | QMessageBox::No);
    if(/*ret == QMessageBox::Yes && */m_setting.openCmdGenDir && !projectResolver)
    {
        QDesktopServices::openUrl(QUrl("file:///" + cmdWordOutDirPath));
    }
    return true;
}
//【结束】调试、运行相关

//【开始】硬件调试相关
void MainWindow::startJointDebug()
{
    qInfo() << tr("进入硬件联调");
    Q_ASSERT(m_curProjectResolver == m_projectTreeModel->getActiveProjectResolver());
    if(!generateCmdWord(m_curProjectResolver, false, false))
        return;

    QString cmdWordOutDirPath;
    if(m_setting.customCmdGenDirFlag)
        cmdWordOutDirPath = m_setting.customCmdGenDir;
    else
        cmdWordOutDirPath = CAppEnv::getCmdwordOutDirPath();

    //转化配置码
    QString msg, errorMsg;
    QString cmd = "python";
    if(!m_setting.pythonLocate.isEmpty())
        cmd = m_setting.pythonLocate;
    if(!CAppEnv::executeCmd(cmd, QStringList("--version"), msg, errorMsg, 3000))
    {
        qInfo() << tr("Python没有找到，请在[工具]->[调试与运行]中设置Python路径，或在环境变量中设置Pythond路径！");
        QMessageBox::critical(this, qApp->applicationName(), tr("Python没有找到，请在[工具]->[调试与运行]中设置Python路径，或在环境变量中设置Pythond路径！"));
        return;
    }

    if(!CAppEnv::clcFirmwareOutDir())
    {
        qWarning() << tr("硬件联调：firmware输出目录未能清空，退出硬件调试");
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"firmware输出目录未能清空！"));
        return;
    }

    QStringList argvList;
    argvList.append(CAppEnv::getScriptDirPath() + "/CopyDataToFirmware.py");
    argvList.append(cmdWordOutDirPath);
    argvList.append(CAppEnv::getFirmwareOutDirPath());

    if(!CAppEnv::executeCmd(cmd, argvList, msg, errorMsg, 5000))
    {
        qInfo() << tr("转换配置码文件失败：%0").arg(errorMsg);
        QMessageBox::critical(this, qApp->applicationName(), tr("转换配置码文件失败：%0").arg(errorMsg));
        return;
    }
//    cmd = CAppEnv::getScriptDirPath() + "/CopyDataToGitFirmware.py";
    argvList.replace(0, CAppEnv::getScriptDirPath() + "/CopyDataToGitFirmware.py");
    if(!CAppEnv::executeCmd(cmd, argvList, msg, errorMsg, 5000))
    {
        qInfo() << tr("转换配置码文件失败：%0").arg(errorMsg);
        QMessageBox::critical(this, qApp->applicationName(), tr("转换配置码文件失败：%0").arg(errorMsg));
        return;
    }

    //硬件调试类CChipDebug
    CChipDebug *chipDebug = CChipDebug::getInstance(this);
    m_debugSession->setCurrentProjectName(m_curProjectResolver->getProjectFileName());
    chipDebug->setDebugSession(m_debugSession);
    chipDebug->setSaveWave(m_setting.saveWave);
    ui->action_saveWave->setEnabled(m_setting.saveWave);
    connect(chipDebug, SIGNAL(sendMessage(QString)), this, SLOT(appendToTextEdit(QString)));
    connect(chipDebug, SIGNAL(sendErrorMessage(QString)), this, SLOT(jointDebugError(QString)));
    connect(chipDebug, SIGNAL(sendTimeOut()), this, SLOT(jointDebugSdkTimeOut()));

//    QDesktopWidget *desktopWidget = QApplication::desktop();
//    QRect screenRect = desktopWidget->availableGeometry();
    if(m_jointDebugToolBar == nullptr)
    {
        m_jointDebugToolBar = new ToolBar(this);
        connect(m_jointDebugToolBar->getSpinBox()->getAction(), SIGNAL(triggered(bool)),
                this, SLOT(jumpToCycle()));

        m_jointDebugToolBar->addAction(ui->action_Next);
        m_jointDebugToolBar->addAction(ui->action_Continue);
        //        m_jointDebugToolBar->addAction(ui->action_LastClock);
        m_jointDebugToolBar->addAction(ui->action_PauseRun);
        m_jointDebugToolBar->addAction(ui->action_Stop);
        m_jointDebugToolBar->addSeparator();
        m_jointDebugToolBar->addAction(ui->action_saveWave);
        //        m_jointDebugToolBar->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint
        //                                       | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
//        int marginLeft = 0.65 * screenRect.width();
//        int marginTop = 0.12 * screenRect.height();
//        m_jointDebugToolBar->move(marginLeft, marginTop);
        //        m_debugToolBar->setGeometry(marginLeft, marginTop, marginLeft ,
        //                                    0.8 * screenRect.height());
        ui->verticalLayout_13->addWidget(m_jointDebugToolBar);
    }
    connect(chipDebug, SIGNAL(sendCurCycle(int)),
            m_jointDebugToolBar->getSpinBox(), SLOT(updateValue(int)));
    disconnect(m_jointDebugToolBar->getComboBox(), SIGNAL(currentIndexChanged(int)),
               chipDebug, SLOT(parseModelChanged(int)));

    QStringList selectedXmlFileNameList;
    m_jointDebugToolBar->getComboBox()->clear();
    for(int i = 0; i < m_selectedXmlParameterList.count(); ++i)
    {
        SXmlParameter xmlParameter = m_selectedXmlParameterList.at(i);
        if(xmlParameter.selected == Qt::Checked)
        {
            selectedXmlFileNameList.append(xmlParameter.xmlPath);
            m_jointDebugToolBar->getComboBox()->addItem(QIcon(CAppEnv::getImageDirPath() +
                                                              "/128/Xml-tool128.png"),
                                                        QFileInfo(xmlParameter.xmlPath).baseName());
            m_jointDebugToolBar->getComboBox()->setFont(QFont("Arial", 9));
        }
    }
    //    m_debugToolBar->adjustSize();
    //    m_debugToolBar->getComboBox()->adjustSize();
    if(m_jointDebugToolBar->getComboBox()->count() > 0)
    {
        m_jointDebugToolBar->getComboBox()->setCurrentIndex(0);
    }
    connect(m_jointDebugToolBar->getComboBox(), SIGNAL(currentIndexChanged(int)),
            chipDebug, SLOT(parseModelChanged(int)));
    connect(ui->action_PauseRun, SIGNAL(triggered()), chipDebug, SLOT(setPause()));
    connect(chipDebug, SIGNAL(sendEnableRunPause(bool)), this, SLOT(jointDebugRun(bool)));


    if(m_debugLog == nullptr)
    {
        m_debugLog = new CfgDebugLog();
        m_debugLog->getDebugLogView()->setWindowTitle(m_jointDebugToolBar->getComboBox()->currentText());
        connect(m_debugLog->getDebugLogView(), SIGNAL(debugLogViewClose()),
                this, SLOT(on_action_Stop_triggered()));
        connect(m_debugLog, SIGNAL(sendChangedElementInfo(QString, QStringList, QStringList)),
                chipDebug, SLOT(modifyPEValue(QString, QStringList, QStringList)));
        connect(chipDebug, SIGNAL(sendPeValue(QStringList)), m_debugLog, SLOT(setAllElementInfo(QStringList)));
    }
//    m_debugLog->getDebugLogView()->setGeometry(0.1 * screenRect.width(), 0.15 * screenRect.height(),
//                                               0.8 * screenRect.width() , 0.8 * screenRect.height());

    if(!chipDebug->enterDebug(CAppEnv::getFirmwareOutDirPath(), selectedXmlFileNameList, m_debugLog))
    {
        on_action_Stop_triggered();
        return;
    }
    QStringList memoryFileList, inFifoFileList;
    memoryFileList << m_selectedXmlParameterList.at(0).inMemPath0 << m_selectedXmlParameterList.at(0).inMemPath1
                   << m_selectedXmlParameterList.at(0).inMemPath2 << m_selectedXmlParameterList.at(0).inMemPath3
                   << m_selectedXmlParameterList.at(0).inMemPath4;
    inFifoFileList << m_selectedXmlParameterList.at(0).inFifoPath0 << m_selectedXmlParameterList.at(0).inFifoPath1
                   << m_selectedXmlParameterList.at(0).inFifoPath2 << m_selectedXmlParameterList.at(0).inFifoPath3;

    CResourceEditorTableModel model;
    for(int i = 0; i < memoryFileList.count(); ++i)
    {
        QString fileName = sourceToAbsolute(memoryFileList.at(i), m_curProjectResolver);
        if(fileName.isEmpty())
            continue;
        if(!model.open(m_curProjectResolver->getProjectFileName(), fileName, true,
                       ui->spinBox_maxColumn->value(), ui->comboBox_addressInterval->currentIndex()))
            continue;
        uint *data = nullptr;
        uint len = 0;
        model.getSrcForHardware(data, len);
        if(!chipDebug->writeGprf(i, 0, data, len*4))
        {
            if(len > 0)
                delete[] data;
            on_action_Stop_triggered();
            return;
        }
        if(len > 0)
            delete[] data;
    }
    for(int i = 0; i < inFifoFileList.count(); ++i)
    {
        QString fileName = sourceToAbsolute(inFifoFileList.at(i), m_curProjectResolver);
        if(fileName.isEmpty())
            continue;
        if(!model.open(m_curProjectResolver->getProjectFileName(), fileName, true,
                       ui->spinBox_maxColumn->value(), ui->comboBox_addressInterval->currentIndex()))
            continue;
        uint *data = nullptr;
        uint len = 0;
        model.getSrcForHardware(data, len);
        if(!chipDebug->writeInFifo(i, data, len*4))
        {
            if(len > 0)
                delete[] data;
            on_action_Stop_triggered();
            return;
        }
        if(len > 0)
            delete[] data;
    }
    if(!chipDebug->startDebug(m_setting.maxCycle, CAppEnv::getFirmwareOutDirPath()))
    {
        on_action_Stop_triggered();
        return;
    }

    //初始化硬件联调界面
    m_debugLog->startDebugMode(chipDebug->getAllPEValue());
//    ui->verticalLayout_13->addWidget(m_debugLog->getDebugLogView());
    ui->verticalLayout_13->insertWidget(0, m_debugLog->getDebugLogView());
    ui->stackedWidget->setCurrentIndex(5);
    m_jointDebugToolBar->getSpinBox()->setMinimum(0);
    m_jointDebugToolBar->getSpinBox()->setValue(0);
    ui->action_Next->setEnabled(true);
    ui->action_Continue->setEnabled(true);
    ui->action_LastClock->setEnabled(true);
    m_jointDebugToolBar->getSpinBox()->setEnabled(true);
    m_jointDebugToolBar->getComboBox()->setEnabled(true);
    ui->action_Stop->setEnabled(true);
    m_jointDebugToolBar->show();
    updateActions();
}

void MainWindow::jointDebugError(const QString &msg)
{
    CWaitDialog::getInstance()->finishShowing();
    qInfo() << tr("硬件联合调试出现问题：%0").arg(msg);
    appendToTextEdit(tr("硬件联合调试出现问题：%0").arg(msg));
    QMessageBox::critical(this, qApp->applicationName(), tr("硬件联合调试出现问题：%0").arg(msg));
}

void MainWindow::jointDebugSdkTimeOut()
{
    qInfo() << tr("硬件联合调试SDK超时，退出调试");
    appendToTextEdit(tr("硬件联合调试SDK超时，退出调试"));
    on_action_Stop_triggered();
}

void MainWindow::jointDebugRun(const bool &flag)
{
    qInfo() << tr("硬件联合调试run with break, update ui");
    ui->action_PauseRun->setEnabled(flag);
    ui->action_Next->setEnabled(!flag);
    ui->action_Continue->setEnabled(!flag);
    ui->action_LastClock->setEnabled(!flag);
    m_jointDebugToolBar->getSpinBox()->setEnabled(!flag);
    m_jointDebugToolBar->getComboBox()->setEnabled(!flag);
    ui->action_Stop->setEnabled(!flag);
}


void MainWindow::on_action_maker_triggered()
{
    qInfo() << tr("固件签名工具");
    CMaker maker(this);
    CAppEnv::animation(&maker, this);
    maker.exec();
}

void MainWindow::on_action_updateFirmware_triggered()
{
    qInfo() << tr("更新固件");
    CFirmwareUpdateDialog firmwareUpdateDialog(this);
    CAppEnv::animation(&firmwareUpdateDialog, this);
    firmwareUpdateDialog.exec();
}

void MainWindow::on_action_OTP_triggered()
{
    qInfo() << tr("更新OTP");
    COTPDialog otpDialog(this);
    CAppEnv::animation(&otpDialog, this);
    otpDialog.exec();
}
//【结束】硬件调试相关

//【开始】xml编辑器相关
void MainWindow::on_action_print_triggered()
{
    qInfo() << tr("打印XML图纸");
    if(m_curRcaGraphView)
        m_curRcaGraphView->printView();
}

void MainWindow::on_action_printPreview_triggered()
{
    qInfo() << tr("打印预览XML图纸");
    if(m_curRcaGraphView)
        m_curRcaGraphView->printPreview();
}

void MainWindow::on_action_printScreen_triggered()
{
    qInfo() << tr("XML图纸拷屏");
    if(!activeXmlEditor())
        return;
    QPixmap pixmap = QPixmap::grabWidget(activeXmlEditor());
    QList<QByteArray> list = QImageReader::supportedImageFormats();
    QString format;
    foreach (QByteArray array, list)
    {
        format.append(tr(u8"%0 (*.%0);;").arg(QString(array)));
    }
    format.chop(2);
    QString fileName = QFileDialog::getSaveFileName(this, qApp->applicationName()
                                                    , QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).at(0)
                                                    , format);

    if(!fileName.trimmed().isEmpty())
        pixmap.save(fileName);
}

void MainWindow::setProjectResolverToXmlEditor(const QString &fileName, CProjectResolver *projectResolver)
{
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(QFileInfo(rcaGraphView->getCurFile()) == QFileInfo(fileName))
        {
            DBG << tr(u8"已打开的非项目xml改为项目内文件");
            rcaGraphView->setProject(projectResolver);
        }
    }
}

/*!
 * MainWindow::rcaGraphViewEditStatusChanged
 * \brief   编辑窗口，状态有改变
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::xmlEditorEditStatusChanged(const rca_space::RcaSceneStates &status)
{
    qInfo() << tr(u8"编辑窗口，状态有改变");
    for(int i = 0; i < ui->listWidget_element->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_element->item(i);
        if(item->data(Qt::UserRole).toInt() == status)
        {
            ui->listWidget_element->setCurrentItem(item);
            break;
        }
    }
    if(m_curRcaGraphView)
        m_viewInfoStatuBar->setZoomValue(m_curRcaGraphView->getCurrentScalePercent());
}

void MainWindow::on_action_autoMakeAllItem_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，自动生成所有算子");
    if(activeXmlEditor())
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        activeXmlEditor()->autoAddNewRcuCfg();
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::on_action_autoSetPosOfAllItem_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，自动排布所有算子");
    if(activeXmlEditor())
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        activeXmlEditor()->autoSetPosOfAllItem(m_bcuCom);
        QApplication::restoreOverrideCursor();
        m_bcuCom = EBcuComposing(((m_bcuCom +1)%BcuEnd));
    }
}

void MainWindow::on_action_grid_triggered(bool checked)
{
    qInfo() << tr(u8"XML编辑窗口，显示网格");
    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
    for(int i = 0; i < winList.count(); ++i)
    {
        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
        if(rcaGraphView)
        {
            m_setting.showGrid = checked;
            rcaGraphView->setGridViewVisable(checked);
        }
    }
}

/*!
 * MainWindow::on_action_ShowLine_triggered
 * \brief   显示连接线
 * \param   checked
 * \author  zhangjun
 * \date    2016-10-12
 */
void MainWindow::on_action_ShowLine_triggered(bool checked)
{
    qInfo() << tr(u8"XML编辑窗口，显示连接线");
    if(m_curRcaGraphView == nullptr)
    {
        return;
    }
    if(checked)
    {
        m_curRcaGraphView->showAllWire();
    }
    else
    {
        m_curRcaGraphView->hideAllWire();
    }
}

void MainWindow::on_action_zoomNormal_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，正常比例显示");
    if(m_curRcaGraphView)
        m_curRcaGraphView->restoreViewToNormal();
}

void MainWindow::on_action_showAll_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，适合屏幕");
    if(m_curRcaGraphView)
        m_curRcaGraphView->showSecenAllItem();
}

void MainWindow::on_action_zoomIn_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，放大");
    if(m_curRcaGraphView)
        m_curRcaGraphView->zoomIn();
}

void MainWindow::on_action_zoomOut_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，缩小");
    if(m_curRcaGraphView)
        m_curRcaGraphView->zoomOut();
}

void MainWindow::on_action_moveLeft_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，左移画布");
    if(m_curRcaGraphView)
        m_curRcaGraphView->moveLeft();
}

void MainWindow::on_action_moveRight_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，右移画布");
    if(m_curRcaGraphView)
        m_curRcaGraphView->moveRight();
}

void MainWindow::on_action_moveUp_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，上移画布");
    if(m_curRcaGraphView)
        m_curRcaGraphView->moveUp();
}

void MainWindow::on_action_moveDown_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，下移画布");
    if(m_curRcaGraphView)
        m_curRcaGraphView->moveDown();
}

void MainWindow::on_action_delete_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，删除图元");
    if(m_curRcaGraphView)
        m_curRcaGraphView->deleteSelectedItems();
}

void MainWindow::on_action_selectAll_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，选择所有图元");
    if(m_curRcaGraphView)
        m_curRcaGraphView->selectAllItems();
}

void MainWindow::xmlEditorZoomTo(const int &value)
{
    qInfo() << tr(u8"XML编辑窗口，设定显示比例");
    if(m_curRcaGraphView)
        m_curRcaGraphView->zoomOneValue(value);
}

void MainWindow::editAlign()
{
    qInfo() << tr(u8"XML编辑窗口， 对齐");
    RcaGraphView *view = m_curRcaGraphView;
    if(!view)
        return;

    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    Qt::Alignment alignment = static_cast<Qt::Alignment>(
                action->data().toInt());
    if (action != ui->action_align) {
        ui->action_align->setData(action->data());
        ui->action_align->setIcon(action->icon());
    }

    if (alignment == Qt::AlignTop)
        view->alignSelectedItems(ItemsAlignTop);
    else if(alignment == Qt::AlignBottom)
        view->alignSelectedItems(ItemsAlignBottom);
    else if(alignment == Qt::AlignLeft)
        view->alignSelectedItems(ItemsAlignLeft);
    else if(alignment == Qt::AlignRight)
        view->alignSelectedItems(ItemsAlignRight);

}

void MainWindow::on_action_horizintalEquidistant_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，等距");
    if(m_curRcaGraphView)
        m_curRcaGraphView->equiSelectedItems(true);
}

void MainWindow::on_action_VerticalEquidistant_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，等距");
    if(m_curRcaGraphView)
        m_curRcaGraphView->equiSelectedItems(false);
}

void MainWindow::on_action_Cut_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，剪切");
    DBG << activeXmlEditor();
    if(m_curRcaGraphView)
        m_curRcaGraphView->cutSelectedItems();
}

void MainWindow::on_action_Copy_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，复制");
    if(m_curRcaGraphView)
        m_curRcaGraphView->copySelectedItems(false);
}

void MainWindow::on_action_Paste_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，黏贴");
    if(m_curRcaGraphView)
        m_curRcaGraphView->pasteSelectedItems();
}

void MainWindow::on_action_copyWithLine_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，带连线复制");
    if(m_curRcaGraphView)
        m_curRcaGraphView->copySelectedItems(true);
}

void MainWindow::on_action_Undo_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，撤销");
    if(m_curRcaGraphView)
        m_curRcaGraphView->actionUndo();
}

void MainWindow::on_action_Redo_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，重做");
    if(m_curRcaGraphView)
        m_curRcaGraphView->actionRedo();
}

void MainWindow::on_action_exportSvg_triggered()
{
    qInfo() << tr(u8"XML编辑窗口，导出SVG");
    if(m_curRcaGraphView)
    {
        RcaGraphView *view = m_curRcaGraphView;
        QString fileName = QFileDialog::getSaveFileName(this, tr(u8"导出"), ""/*str*/, tr(u8"SVG 文件 (*.svg)"));
        if (fileName.isEmpty())
        {
            return;
        }
        bool grid = ui->action_grid->isChecked();
        bool wire = ui->action_ShowLine->isChecked();
        if(grid)
            view->setGridViewVisable(false);
        if(!wire)
            view->showAllWire();
        view->exportSvg(fileName);
        if(grid)
            view->setGridViewVisable(true);
        if(!wire)
            view->hideAllWire();
    }
}
//【结束】xml编辑器相关

void MainWindow::on_pushButton_cmdClear_clicked()
{
    sendCmd("clc");
}

void MainWindow::on_pushButton_cmdFontSizeZoomOut_clicked()
{
    QFont font = ui->textEdit_log->font();
    if(font.pointSize() > 6)
    {
        font.setPointSize(font.pointSize() - 1);
        ui->textEdit_log->setFont(font);
    }
}

void MainWindow::on_pushButton_cmdFontSizeZoomIn_clicked()
{
    QFont font = ui->textEdit_log->font();
    if(font.pointSize() < 280)
    {
        font.setPointSize(font.pointSize() + 1);
        ui->textEdit_log->setFont(font);
    }
}

void MainWindow::on_pushButton_cmdFontSizeDefault_clicked()
{
    //    QFont font = ui->textEdit_log->font();
    //    font.setPointSize(9);
    //    ui->textEdit_log->setFontPointSize(m_setting.cmdFontSize);
    ui->textEdit_log->setFont(m_setting.cmdFont);

}

void MainWindow::on_action_connectToDevice_triggered()
{
    qInfo() << tr(u8"SDK连接设备");

    CHardwareDebug *hardwareDebug = CHardwareDebug::getInstance(this);
    connect(hardwareDebug, SIGNAL(deviceDisconnected()), m_deviceStatusBar, SLOT(deviceDisconnected()));
    connect(hardwareDebug, SIGNAL(deviceConnected(QString, QString)), m_deviceStatusBar, SLOT(deviceConnected(QString, QString)));
    connect(hardwareDebug, SIGNAL(deviceDisconnected()), this, SLOT(updateActions()));
    connect(hardwareDebug, SIGNAL(deviceDisconnected()), this, SLOT(hardwareDeviceDisconnected()));
    connect(hardwareDebug, SIGNAL(deviceConnected(QString, QString)), this, SLOT(updateActions()));
    if(!hardwareDebug->deviceOpen())
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"未能成功连接到设备！"));
    }
}

void MainWindow::hardwareDeviceDisconnected()
{
    qInfo() << tr(u8"SDK设备断开");
    if(CChipDebug::getIsDebugging() && !m_isSimDebug)
        on_action_Stop_triggered();
}

void MainWindow::on_action_disconnectToDevice_triggered()
{
    qInfo() << tr(u8"SDK断开设备");
    if(!CHardwareDebug::hasInstance())
        return;
    if(!CHardwareDebug::getInstance(this)->deviceClose())
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"未能成功断开连接的设备！"));
    }
    hardwareDeviceDisconnected();
}

void MainWindow::on_action_deviceReset_triggered()
{
    qInfo() << tr(u8"SDK重置设备");
    if(!CHardwareDebug::hasInstance())
        return;
    if(CHardwareDebug::getInstance(this)->deviceReset())
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"连接的设备已复位！"));
    }
    else
    {
        QMessageBox::critical(this, qApp->applicationName(), tr(u8"未能成功复位连接的设备！"));
    }
}


void MainWindow::on_action_openWaveFile_triggered()
{
    if(m_rpuWave)
    {
        if(!m_rpuWave->getWidget()->isVisible())
        {
            m_rpuWave->getWidget()->setVisible(true);
//            if(ui->mdiArea->subWindowList().count() > 0)
//            {
                ui->splitter_2->setStretchFactor(0,1);
                ui->splitter_2->setStretchFactor(1,2);
//            }
//            else
//            {
//                ui->mdiArea->setVisible(false);
//            }
        }
        ui->stackedWidget->setCurrentIndex(1);
//        m_rpuWave->openWave();
    }
}

void MainWindow::pullWaveToShow()
{
    RcaGraphView *view = currentXmlEditor();
    if(view)
    {
        addWaveToShow(view->getSelectedItemsInfo());
    }
}

void MainWindow::addWaveToShow(const QStringList &list)
{
    if(m_rpuWave && m_rpuWave->getWidget()->isVisible())
    {
        m_rpuWave->addWave(list);
    }
}


void MainWindow::on_actiondebug_triggered()
{

    CWaitDialog::getInstance(this)->readyToShowDlg();
    CWaitDialog::getInstance(this)->setText(tr(u8"仿真运行中"));

//    loadFiles(QStringList(QString("C:/Users/Jun/Desktop/log.txt-0708-aes-all-3.txt-0708-aes-all-3.rpulog")));
//m_curRcaGraphView->horizontalScrollBar()->setSliderPosition(100);

//硬件调试
//    on_action_connectToDevice_triggered();
//    ui->action_jointDebugMode->setChecked(true);
//    runOrDebugTriggered("debug");


//    m_debugLog->getDebugLogView()->setWindowFlags(Qt::Dialog);
//    m_debugLog->getDebugLogView()->show();
//    m_debugLog->getDebugLogView()->showFullScreen();
}

void MainWindow::on_actiondebug_2_triggered()
{
//    m_debugLog->getDebugLogView()->setWindowFlags(Qt::SubWindow);
//    m_debugLog->getDebugLogView()->showNormal();

    //    CWaitDialog::getInstance()->readyToShowDlg();
}


