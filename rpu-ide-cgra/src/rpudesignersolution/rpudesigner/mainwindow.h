#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QModelIndex>
#include <QTimer>
#include <QProcess>
#include <QPointer>
#include <memory>
#include <thread>
#include "datafile.h"
#include "RcaGraphDef.h"
#include "export_typedef.h"

class QMdiSubWindow;
class QActionGroup;
class QStandardItemModel;
class QStandardItem;
class QListWidgetItem;
class QLabel;
class QComboBox;
class QTreeWidgetItem;
class QFileSystemWatcher;
class QWinTaskbarProgress;
class QWinTaskbarButton;
class CProgressStatusBar;
class CViewInfoStatusBar;
class CDeviceStatusBar;
class CProjectResolver;
class CProjectSettingModel;
class CResourceEditorTableModel;
class CProjectTreeItemModel;
class COpenedFileListModel;
class CfgDebugLog;
class CProjectTreeItem;
class CTcpClient;
class CSessionManager;
class CDebugSession;
class CSession;
class CClientDialog;
class RpuWaveInterface;
class ToolBar;
class RcaGraphView;
class BaseItem;
class ElementBfu;
class CDebugAlg;

namespace rpu_esl_model {
class CSimulatorControl;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum EMsgLevel {
        MSG_INFOMATION,
        MSG_WARNING,
        MSG_CRITICAL,
        MSG_FATAL
    };

    void restoreSession();
    void loadFiles(const QStringList &list);
    void loadRpuSln(const QStringList &rpuSlnList);
    void connectToServer() const;
    void checkLicense();

signals:
    void sendMsg(const QString&);
    void versionRequest();
    void licenseChanged();
    void shareSolutionRequest(QString);

private:
    Ui::MainWindow *ui;

    QMenu *m_solutionFolderContextMenu = nullptr;
    QMenu *m_solutionFileContextMenu = nullptr;
    QMenu *m_projectFolderContextMenu = nullptr;
    QMenu *m_projectFileContextMenu = nullptr;
    QMenu *m_projectXmlFolderContextMenu = nullptr;
    QMenu *m_projectXmlFileContextMenu = nullptr;
    QMenu *m_projectSrcFolderContextMenu = nullptr;
    QMenu *m_openedFileListContextMenu = nullptr;                       //打开的XML上下文
    QMenu *m_resourceChoiceContextMenu = nullptr;                       //资源选择上下文
    QMenu *m_explorerChoiceContextMenu = nullptr;                       //文件选择上下文
    QMenu *m_runResultContextMenu = nullptr;                            //运行结果上下文
    QMenu *m_mdiAreaTabBarContextMenu = nullptr;                        //mdiarea标签上下文
    QMenu *m_sysTrayMenu = nullptr;                                     //托盘上下文
    QAction **m_recentFileActions;                                      //最近打开文件
    QAction **m_recentProActions;                                       //最近打开项目
    QAction *m_runAction = nullptr;                                     //项目树上下文中Action
    QAction *m_debugAction = nullptr;
    QAction *m_lastRunResult = nullptr;
    QActionGroup *m_windowActionGroup = nullptr;                        //xml子窗口Action组
    QActionGroup *m_sessionActionGroup = nullptr;                       //会话Action组
    CProgressStatusBar *m_progressStatusBar = nullptr;
    CViewInfoStatusBar *m_viewInfoStatuBar = nullptr;
    CDeviceStatusBar *m_deviceStatusBar = nullptr;
    ToolBar *m_simDebugToolBar = nullptr;                               //调试工具条
    ToolBar *m_jointDebugToolBar = nullptr;
    COpenedFileListModel *m_openedFileListModel = nullptr;              //已打开XML文件model
    CProjectTreeItemModel *m_projectTreeModel = nullptr;                //项目树model
    CProjectSettingModel *m_projectSettingModel = nullptr;              //项目设置model
    CResourceEditorTableModel *m_resourceEditorTableModel = nullptr;    //资源编辑model
    QSystemTrayIcon *m_sysTray = nullptr;
    CSessionManager *m_sessionManager = nullptr;                        //会话管理
    CDebugSession *m_debugSession = nullptr;
    CClientDialog *m_clientDialog = nullptr;
    QHash<int, SCustomPEConfig> m_customPEConfigHash;

    RpuWaveInterface *m_rpuWave = nullptr;
    QByteArray m_geometry;                                              //记录窗口geometry
    QTimer m_timer;                                                     //定时器
    SSetting m_setting;                                                 //设置结构体
    bool m_renameFlag = false;                                          //重命名标志
    int m_lastIndex = 0;                                                //记录运行调试前窗口，运行调试结束后切回去
    int m_mdiAreaIndex = -1;                                            //mdiArea上下文菜单时的标签index
    QString m_deleteFile;                                               //待删除的文件
    QPointer<CTcpClient> m_tcpClient;                                   //连接服务器
    int m_recentFileCount = 10;                                         //最近打开列表显示个数
    int m_recentProCount = 10;
    QStringList m_recentFiles;                                          //最近打开文件列表
    QStringList m_recentPros;                                           //最近打开项目列表
    QStringList m_openedFiles;                                          //打开的XML文件列表
    QString m_curFileName; //当前文件，用于重命名xml文件时更新最近打开列表，不能用来识别当前打开xml文件
    CProjectResolver *m_curProjectResolver = nullptr;                   //当前项目
    RcaGraphView *m_curRcaGraphView = nullptr;                          //当前guiviewer
    rca_space::EBcuComposing m_bcuCom = rca_space::BcuVCom;

    //运行调试相关
    std::thread m_simThread;
    bool m_isSimDebug = false;                                          //仿真调试还是硬件调试
    bool m_isRunMode = true;
    bool m_isRunning = false;
    bool m_hasNextClock = false;                                        //是否有下一周期
    bool m_justLoad = false;
    int m_paraInfoIndex = 0;                                            //m_paraInfoList的当前索引，用于调试时记录当前时第几个xml图
    int m_curIndexOfAll = 0;
//    std::weak_ptr<rpu_esl_model::CSimulatorControl> m_simCtrl;
    rpu_esl_model::CSimulatorControl* m_simCtrl = nullptr;
    QPointer<QProcess> m_process;                                       //调用外部程序rcs
    CfgDebugLog *m_debugLog = nullptr;                                  //单步调试窗
    CProjectResolver *m_curShowResultProjectResolver = nullptr;         //当前正在运行或者调试的项目解析器
    QList<SXmlParameter> m_selectedXmlParameterList;                    //生成配置码选择的xml文件列表
    QHash<int, QPair<QString, RcaGraphView *>> m_curDebugViewMap;

    //初始化项，构造函数中调用
    void disableUnavaliable();
    void initDockWidget();
    void initStatusBar();
    void initToolBar();
    void initRecentMenu();
    void initProjectTree();
    void initProjectContextMenu();
    void initOpenedFileList();
    void initDemoList();
    void initXmlEditorElement();
    void initWelcomeWidget();
    void initIcon();
    void initCmdWidget();
    void initProjectSetting();
    void initResourceEditer();
    void initSysTray();
    void initRunResult();
    void initXmlEditorMdiArea();
    void initWaveWidget();
    void initServer();
    void initAnimation();
    void initSession();
#if defined(Q_OS_WIN)
    QWinTaskbarButton *initTaskButton();
#endif
    bool loadPlugin();
    void writeSettings() const;                                          //程序配置保存
    void readSettings();                                                 //程序配置读取
    void settingChanged();                                               //应用程序配置更改

    void loadSolution(const QStringList &fileNameList);
    void closeSolution(const QModelIndex &index);
    bool openXmlFile(const QString &fileName, CProjectResolver *proResolver = nullptr, bool convertOnly = false);
    void addXmlEditor(RcaGraphView *rcaView);
    void openSrcFile(const QString &projectName, const QString &fileName, QString type = "none");
    void updateRecentProjectActions();                                   //更新最近打开的项目列表
    void updateRecentProjectList(const QString &projectName);
    void updateRecentFileActions();                                      //更新最近打开的xml列表
    void updateRecentFileList(const QString &fileName, bool saveAs = false);
    RcaGraphView *currentXmlEditor() const;
    RcaGraphView *activeXmlEditor() const;
    RcaGraphView *activeXmlEditor(QMdiSubWindow *subWindow) const;
    QString sourceToAbsolute(const QString &fileName, CProjectResolver *projectResolver = nullptr) const; //资源文件路径转换
    bool checkFileExists(SProjectParameter *projectParameter, const QString &fileName) const;       //资源文件校验文件是否存在

    //仿真调试
    void startSimDebug();
    void stopSimDebug();
    void setNextDebugXml(const QStringList &infoList);                   //调试下一个XML
    void updateRunResult(CProjectResolver *projectResolver);             //更新运行结果
    void showFifo(const int row);                     //显示运行结果fifo
    void showMem(const int row);                      //显示运行结果memory
    //硬件调试
    void startJointDebug();
    void stopJointDebug();
    //生成配置码
    bool generateCmdWord(CProjectResolver *projectResolver = nullptr, bool askSaveXml = true, bool askSaveResrc = true);
    //算法分析
    void analyseAlgorithm(const bool &showPerfom = false);
    bool checkSimRet(rpu_esl_model::ESIM_RET ret);
    void showDebugValue();

private slots:
    void xmlEditorAutoSave();
    void clearRecentMenu();
    void searchInCmd();
    void handleMessage(const QString& msg);
    void openRecentFile();                                //打开最近打开文档
    void openRecentProject();                             //打开最近打开项目
    void updateActions(rca_space::SelectedItemsAttr attr = rca_space::SelectedAttrNone);
    void updateDebugToolBar();
    void updateCurrentSession();                          //更新当前会话
    void addSessionAction(QAction *action);
    void loadSessionToUI();
    void loadSessionToUI(CSession *s);
    void loadDebugSessionToUI(const CDebugSession &debugSession);
    //    void fileChanged(QString fileName);
    //项目树上下文菜单
    void deleteFile();                                    //文件系统中删除文件
    void openXmlTriggered();
    void newXmlToProject();                               //新建xml文件到项目
    void addXmlToProject();                               //增加xml文件到项目
    void newSrcToProject();                               //新建src文件到项目
    void addSrcToProject();                               //增加src文件到项目
    void removeProject();
    void projectSettingTriggered();
    void deleteFileTriggered();
    void renameFileTriggered();
    void newProject();
    void addProject();
    void shareSolution();                                 //打包并共享解决方案
    void setCurProject();                                 //设置为当前活动项目
    void copyPathTriggered();
    void closeSolutionTriggered();
    void closeOtherSolutionTriggered();
    void closeXmlEditorSubWindow();
    void exploreProjectFileTriggered();
    void exploreOpenedFileTriggered();
    void dropProjectToProjectTree(const QStringList &algList, const QStringList &unSupportList);
    void xmlFileRenamed(const QString &oldName, const QString &newName);
    void srcFileRenamed(const QString &oldName, const QString &newName);
    void projectFileRenamed(const QString &oldName, const QString &newName);
    void solutionFileRenamed(const QString &oldName, const QString &newName);
    void renameFileFinished(const QModelIndex &index);
    void setSolutionTypeChanged();
    //显示上下文菜单
    void showProjectContextMenu(const QPoint &point);
    void showOpenedFileContextMenu(const QPoint &point);
    void showResourceChoiceContextMenu(const QPoint &point);
    void showExplorerChoiceContextMenu(const QPoint &point);
    void showMdiAreaTabBarContextMenu(const QPoint &point);
    void showRunResultContextMenu(const QPoint &point);
    //XML编辑
    void setProjectResolverToXmlEditor(const QString &fileName, CProjectResolver *projectResolver);
    void setStatusBarMessage(const QString &str);                              //设置状态栏消息
    void xmlEditorEditStatusChanged(const rca_space::RcaSceneStates &status = rca_space::Normal); //从RcaGraphView获取当前状态并更新至界面
    void activeXmlEditorChanged(QMdiSubWindow *subWindow);     //当前激活窗口变化
    void exploreXmlEditorFileTriggered();
    void closeXmlEditorTriggered();
    void closeOtherXmlEditorTriggered();
    void xmlEditorClosed(const QString &str);
    void dropXmlToXmlEditor(const QStringList &xmlList, const QStringList &unSupportList);
    void xmlEditorZoomTo(const int &value);
    void editAlign();
    //资源编辑
    void fileSystemTreeActionTriggled();
    void resourceListActionTriggled();
    void resourceEditorSetDirty();
    void resourceEditorSaved(const QString &fileName);
    void resourceEditorClosed();
    //项目配置
    void currentActiveProjectChanged();
    void projectParameterSaved(const QString &fileName);
    void projectParameterSetDirty();
    void resetProjectSettingTable();  //重置项目配置
    void refreshProjectSettingTable();//刷新项目配置
    //项目配置界面资源选择
    void dropSrcToProject(const QString &fileName);
    void removeSrcInContextMenu();
    void resetResourceSelectTable();//重置资源选择列表
    void deleteResourceEditorTable();//删除资源编辑列表
    //托盘菜单
    void sysTrayActivated(const QSystemTrayIcon::ActivationReason &reason);
    void actionShowTriggered();
    void actionQuitTriggered();
    //void addToProject();                                //添加打开的xml文件到项目
    //调试运行相关
    void runOrDebugTriggered(QString mode = "none");
    void currentDebugXmlChanged(const int &index);
    void rcsReadyReadStandardOutput();
    void rcsReadyReadStandardError();
    void rcsProcessStarted();
    void rcsProcessFinished(const int &code, const QProcess::ExitStatus &exitStatus);
    void sendCmd(const QString &cmd);
    void jumpToCycle();                                   //运行到第N个周期
    void elementValueEdited(const QString &element, const QStringList &input, const QStringList &output);
//    void elementSetBreak(const QString &element, const QString &portInfo);
//    void elementRemoveBreak(const QString &element, const QString &portInfo);
    void viewLastRunResult();
    void runProjectTriggered();
    void debugProjectTriggered();
    void runResultSaveToFile();
    void appendToTextEdit(const QString &msg);            //RCS消息打印到cmd窗口
    void appendToTextEdit(EMsgLevel level, const QString &msg, bool showMsgBox = false);            //RCS消息打印到cmd窗口
    void simDebugBreakAdd(const rpu_esl_model::SBreak &breakHandle);               // 增加断点，并返回断点ID
    void simDebugBreakRemove(const rpu_esl_model::SBreak &breakHandle);            // 移除断点
    //硬件调试与设备
    void jointDebugError(const QString &msg);
    void jointDebugSdkTimeOut();
    void jointDebugRun(const bool &flag);
    void hardwareDeviceDisconnected();
    //服务器
    void receiveMessageFromServer(const QString &msg);
    void receiveNotifyFromServer(const QString &msg);
    void versionAck(const bool &, const QString &);
    void serverCloseProgram(const QString &msg);
    void clientListRequest();
    void transferFinished();
    //波形
    void pullWaveToShow();
    void addWaveToShow(const QStringList &list);

    void on_action_NewFile_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_SaveAs_triggered();
    void on_action_Tile_triggered();
    void on_action_Cascade_triggered();
    void on_action_About_triggered();
    void on_action_Quit_triggered();
    bool on_action_SaveAll_triggered();
    void on_action_ShowLine_triggered(bool checked);
    void on_action_Close_triggered();
    void on_action_CloseAll_triggered();
    void on_action_NextWindow_triggered();
    void on_action_PreviousWindow_triggered();
    void on_action_Bug_triggered();
    void on_action_Log_triggered();
    void on_action_Setting_triggered();
    void on_action_openSolution_triggered();
    void on_action_newSolution_triggered();
    void on_action_Code_triggered();
    void on_action_Stop_triggered();
    void on_action_LastClock_triggered();
    void on_action_Next_triggered();
    void on_action_Continue_triggered();
    void on_listView_openedFile_clicked(const QModelIndex &index);
    void on_treeView_project_doubleClicked(const QModelIndex &index);
    void on_pushButton_hideDebugLogWidget_clicked();
    void on_action_Cmd_triggered();
    void on_pushButton_newFile_clicked();
    void on_pushButton_openFile_clicked();
    void on_pushButton_newPro_clicked();
    void on_pushButton_openPro_clicked();
    void on_listWidget_files_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_pros_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_demos_itemDoubleClicked(QListWidgetItem *item);
    void on_action_Help_triggered();
    void on_lineEdit_searchCmd_returnPressed();
    void on_action_lockToolBar_triggered(bool checked);
    void on_action_fullScreen_triggered(bool checked);
    void on_action_welcome_triggered();
    void on_action_edit_triggered();
    void on_dockWidget_project_visibilityChanged(bool visible);
    void on_action_solution_triggered(bool checked);
    void on_dockWidget_openedFile_visibilityChanged(bool visible);
    void on_action_openedFile_triggered(bool checked);
    void on_action_attribute_triggered(bool checked);
    void on_dockWidget_element_visibilityChanged(bool visible);
    void on_action_element_triggered(bool checked);
    void on_action_fileToolBar_triggered(bool checked);
    void on_action_editToolBar_triggered(bool checked);
    void on_action_proSetting_triggered();
    void on_pushButton_proSettingSave_clicked();
    void on_pushButton_proSettingSelectAll_clicked();
    void on_pushButton_proSettingUp_clicked();
    void on_pushButton_proSettingDown_clicked();
    void on_action_resource_triggered();
    void on_spinBox_maxColumn_valueChanged(int arg1);
    void on_checkBox_sboxShowInOneLine_clicked(bool checked);
    void on_pushButton_addFifoResourceRowBefore_clicked();
    void on_pushButton_addFifoResourceRowAfter_clicked();
    void on_pushButton_deleteFifoResourceRow_clicked();
    void on_pushButton_saveResource_clicked();
    void on_treeWidget_runResult_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_stackedWidget_currentChanged(int arg1);
    void on_listWidget_resource_itemDoubleClicked(QListWidgetItem *item);
    void on_treeView_explorer_doubleClicked(const QModelIndex &index);
    void on_action_scale_triggered(bool checked);
    void on_dockWidget_scaleView_visibilityChanged(bool visible);
    void on_action_autoMakeAllItem_triggered();
    void on_action_autoSetPosOfAllItem_triggered();
    void on_action_zoomNormal_triggered();
    void on_action_showAll_triggered();
    void on_action_zoomIn_triggered();
    void on_action_zoomOut_triggered();
    void on_action_moveLeft_triggered();
    void on_action_moveRight_triggered();
    void on_action_moveUp_triggered();
    void on_action_moveDown_triggered();
    void on_action_maker_triggered();
    void on_action_showMenuBar_triggered(bool checked);
    void on_action_viewToolBar_triggered(bool checked);
    void on_action_delete_triggered();
    void on_action_selectAll_triggered();
    void on_listWidget_element_itemClicked(QListWidgetItem *item);
//    void on_action_alignTopEdge_triggered();
//    void on_action_alignBottomEdge_triggered();
//    void on_action_alignLeftEdge_triggered();
//    void on_action_alignRightEdge_triggered();
    void on_pushButton_forecastClock_clicked();
    void on_action_closeAllSolution_triggered();
    void on_action_horizintalEquidistant_triggered();
    void on_action_VerticalEquidistant_triggered();
    void on_action_Update_triggered();
    void on_action_grid_triggered(bool checked);
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_action_exportSvg_triggered();
    void on_action_changeLicense_triggered();
    void on_action_updateFirmware_triggered();
    void on_comboBox_addressInterval_currentIndexChanged(int index);
    void on_pushButton_benesEditorGenerate_clicked();
    void on_pushButton_benesEditorLoad_clicked();
    void on_pushButton_benesEditorReset_clicked();
    void on_action_print_triggered();
    void on_action_printPreview_triggered();
    void on_action_printScreen_triggered();
    void on_action_Copy_triggered();
    void on_action_Cut_triggered();
    void on_action_Paste_triggered();
    void on_action_copyWithLine_triggered();
    void on_pushButton_cmdClear_clicked();
    void on_pushButton_cmdFontSizeZoomOut_clicked();
    void on_pushButton_cmdFontSizeZoomIn_clicked();
    void on_pushButton_cmdFontSizeDefault_clicked();
    void on_action_connectToDevice_triggered();
    void on_action_disconnectToDevice_triggered();
    void on_action_deviceReset_triggered();
    void on_pushButton_statistics_clicked();
    void on_pushButton_sessionManager_clicked();
    void on_action_sessionManager_triggered();
    void on_action_closeAllSlnAndEditor_triggered();
    void on_action_analyze_triggered();
    void on_action_saveDebugStatus_triggered();
    void on_action_restoreDebugStatus_triggered();
    void on_action_debugWindow_triggered();
    void on_action_saveWave_triggered();
    void on_action_openWaveFile_triggered();
    void on_action_OTP_triggered();
    void on_action_community_triggered();
    void on_dockWidget_attribute_visibilityChanged(bool visible);
    void on_action_xmlEditorShowAsTool_triggered();
    void on_lineEdit_cmd_returnPressed();

    void on_actiondebug_triggered();
    void on_actiondebug_2_triggered();

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);
};

#endif // MAINWINDOW_H
