#ifndef DATAFILE_H
#define DATAFILE_H

#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define REG_CMD "HKEY_CLASSES_ROOT\\RPUDesigner.rpusln\\shell\\open\\command"
#define REG_SUFFIX "HKEY_CLASSES_ROOT\\RPUDesigner.rpusln\\DefaultIcon"

#include <QString>
#include <QMap>
#include <QPoint>
#include <QFont>
#include <QDateTime>

typedef enum _tagCipherType
{
    CIPHER_TYPE_NONE,
    CIPHER_TYPE_ASYMMETRIC_RSA,
    CIPHER_TYPE_ASYMMETRIC_ECC,
    CIPHER_TYPE_SYMMETRIC_BLOCK,
    CIPHER_TYPE_SYMMETRIC_STREAM,
    CIPHER_TYPE_HASH,
    CIPHER_TYPE_TRNG,
} ECipherType;

typedef enum _tagResourceType
{
    RESOURCE_TYPE_NONE,
    RESOURCE_TYPE_SBOX,
    RESOURCE_TYPE_BENES,
    RESOURCE_TYPE_INFIFO,
    RESOURCE_TYPE_IMD,
    RESOURCE_TYPE_MEMORY,
} EResourceType;

typedef struct _tagXMLPerformanceStatistics
{
    _tagXMLPerformanceStatistics(){
        outFifoCount = 0;
        auUsedCount = 0;
        suUsedCount = 0;
        luUsedCount = 0;
        tuUsedCount = 0;
        puUsedCount = 0;
        rcuUsedCount = 0;
        bcuUsedCount = BCUUSED0;
    }
    enum EBCUUSED {
        BCUUSED0 = 0b0000,
        BCUUSED1 = 0b0001,
        BCUUSED2 = 0b0010,
        BCUUSED3 = 0b0100,
        BCUUSED4 = 0b1000
    };

    QString xmlName;
    int outFifoCount = 0;
    int needCycle;
#define TOTALBCUCOUNT 4
    EBCUUSED bcuUsedCount = BCUUSED0;
#define TOTALRCUCOUNT 16
    int rcuUsedCount = 0;
#define TOTALRCCOUNT 208
    int auUsedCount = 0;
    int suUsedCount = 0;
    int luUsedCount = 0;
    int tuUsedCount = 0;
    int puUsedCount = 0;
} SXMLPerformanceStatistics;

typedef struct _tagPerformanceStatistics
{
    QString projectName;
    int totalCycle = 0;
    int latency = 0;
    int dataOutInterval = 0;
#define TOTALMEMORYCOUNT 256
    int memoryUsedCount = 0;
    int memoryOnlyReadedCount = 0;
    int memoryOnlyWrittenCount = 0;
    QList<SXMLPerformanceStatistics> xmlPerformList;
} SPerformanceStatistics;


typedef struct _tagXmlParameter  //XML参数
{
//    ~_tagXmlParameter(){DBG << "~_tagXmlParameter()";}

    Qt::CheckState selected;
    int progress = 0;
    QString status;
    QString projectName;

    QString xmlPath;
    int sort = -1;
    QString inFifoPath0;
    QString inFifoPath1;
    QString inFifoPath2;
    QString inFifoPath3;
    QString inMemPath0;
    QString inMemPath1;
    QString inMemPath2;
    QString inMemPath3;
    QString inMemPath4;
    QString outFifoPath0; //不用使用者指定
    QString outFifoPath1; //不用使用者指定
    QString outFifoPath2; //不用使用者指定
    QString outFifoPath3; //不用使用者指定
    QString outMemPath0; //不用使用者指定
    QString outMemPath1; //不用使用者指定
    QString outMemPath2; //不用使用者指定
    QString outMemPath3; //不用使用者指定
    QString outMemPath4; //不用使用者指定
    QString cycleLogPath; //不用使用者指定
    QString binPath; //不用使用者指定
//    QString indexFile; //不用使用者指定

    int onlyRsm = 0;
    int rMode0 = 0;
    int rMode1 = 0;
    int gap = 0;
    QString source;
    QString dest;
    int burstNum = 0;
    int loopNum = 0;
    int repeatNum = 0;
    QString lfsrGroup = u8"未分组";
} SXmlParameter;


typedef struct _tagResourceParameter  //项目参数
{
    int index;
    QString baseName;
    QString fileName;
    EResourceType resourceType;
} SResourceParameter;

typedef struct _tagProjectParameter  //项目参数
{
    _tagProjectParameter()
    {
        xmlParameterList = new QList<SXmlParameter>;
//        resourceParameterList = new QList<SResourceParameter>;
    }

    ~_tagProjectParameter()
    {
        delete(xmlParameterList);
//        delete(resourceParameterList);
    }
    QString solutionName;
    QString projectName;
    ECipherType cipherType;
    QList<SXmlParameter> *xmlParameterList = nullptr;
    QMap<QString, QString> resourceMap;

    int lfsrWidthDeep = 0;
    int lfsrMode = 0;
//    QList<SResourceParameter> *resourceParameterList = nullptr;

} SProjectParameter;

// 项目信息表枚举
enum
{
    ProjectSettingXmlName,
    ProjectSettingInFifoPath0,
    ProjectSettingInFifoPath1,
    ProjectSettingInFifoPath2,
    ProjectSettingInFifoPath3,
    ProjectSettingInMemoryPath0,
    ProjectSettingInMemoryPath1,
    ProjectSettingInMemoryPath2,
    ProjectSettingInMemoryPath3,
    ProjectSettingInMemoryPath4,
    ProjectSettingOnlyRsm,
    ProjectSettingRMode0,
    ProjectSettingRMode1,
    ProjectSettingGap,
    ProjectSettingSource,
    ProjectSettingDest,
    ProjectSettingBurstNum,
    ProjectSettingLoopNum,
    ProjectSettingRepeatNum,
    projectSettingLFSRGroup,
};

typedef struct _tagSetting
{
//    int priority;
    bool logOut;
#if defined(Q_OS_WIN)
    bool autoRun;
#endif
    int logSize;
    int maxSubWin;
    bool showTab;
    int tabType;
    QString defaultProPath;
    bool cycleOut;
    bool autoSave;
    int saveInterval;
    bool sysTray;
    bool toolBarLocked;
    bool tabMovable;
    bool tabClosable;
    int tabPosition;
    bool xmlMd5Check;
    int iconSize;
    bool menuBar;
    bool editToolBar;
    bool fileToolBar;
    bool viewToolBar;
    bool srcMd5Check;
    int maxCycle;
    int recentFileCount;
    int recentProCount;
    bool logArchive;
    bool logDelete;
    int logDeleteDate;
//    int sortType;
    bool showGrid;
    QPoint gridPoint;
    int networkPorxy;
    QString networkPorxyAddress;
    int networkPorxyPort;
    QString networkPorxyUserName;
    QString networkPorxyPassword;
    //    QMap<QString, QList<int> > resourceEditFormat;
    bool openCmdGenDir;
    bool customCmdGenDirFlag;
    QString customCmdGenDir;
    QString cmdGenOutSideCommand;
    QFont cmdFont;
    double cmdFontSize;
    QByteArray sessions;
    int sessionCount;
    QString restoreSession;
    bool restoreSessionFlag;
    bool saveWave;
    QString pythonLocate;
} SSetting;

// 解决方法树item枚举
typedef enum _tagItemType
{
    ITEMNONE,
    ITEMSOLUTION,
    ITEMSOLUTIONFILE,
    ITEMPROJECT,
    ITEMPROJECTFILE,
    ITEMXMLFOLDER,
    ITEMXMLFILE,
    ITEMSRCFOLDER,
    ITEMSRCFILE,
} EItemType;

typedef enum _tagSendReason
{
    srReserve,         //保留
    srRegistSyn,       //客户端发送 请求注册
    srRegistAck,       //服务器应答 注册应答
    srVersionSyn,      //客户端发送 请求最新版本号
    srVersionAck,      //服务器应答 最新版本号
    srUpdateSyn,       //客户端发送 开始升级，准备接收传输，连接updateServer
    srUpdateAcceptAck, //服务器应答 接收升级
    srUpdateRejectAck, //服务器应答 拒绝升级
    srNoNeedUpdateAck, //服务器应答 不用升级
    srClientUpdate,    //服务器发送 要求客户端升级
    srShowMessage,     //服务器发送 显示弹窗消息
    srShowNotify,      //服务器发送 显示托盘消息
    srCloseClient,     //服务器发送 关闭客户端
    srKeepAlive,       //服务器发送 心跳包
    srShareFileSyn,    //客户端发送 分享文件请求
    srShareFileAck,    //服务器发送 分享文件应答
    srShareFileFin,    //客户端发送 传输分享文件完成
    srClientListOld, //服务器发送 客户端列表数据已旧
    srClientListSyn,   //客户端发送 请求获取客户端列表
    srClientListAck,   //服务器应答 获取客户端列表应答
    srShareFileListOld,   //服务器发送 共享文件列表数据已旧
    srShareFileListSyn,     //客户端发送 请求获取共享文件列表
    srShareFileListAck,     //服务器应答 获取共享文件列表应答
    srDownloadShareFileSyn, //客户端发送 请求下载共享文件
    srDownloadShareFileAck, //服务器应答 下载共享文件应答
    srDownloadShareFileReady,//客户端发送 下载共享文件准备完毕
    srDownloadShareFileFin, //服务器发送 传输共享文件完成
} ESendReason;

typedef struct _tagSharedFile
{
    QString fileName;
    QString userName;
    QDateTime shareTime;
} SSharedFile;

typedef struct _tagCustomPEConfig
{
    QString peName;
    QString iconFile;
} SCustomPEConfig;

#endif // DATAFILE_H
