#include "common/cappenv.hpp"
#include "common/debug.h"
#include "version.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QProcess>
#include <QHBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QNetworkInterface>
#include <QMimeDatabase>
#include <iostream>
#include <memory>
#include <QStandardPaths>

using namespace std;

QString CAppEnv::m_sysLog;
//QString CAppEnv::m_debugLog;
int CAppEnv::m_logSize;
bool CAppEnv::m_logArchive;
bool CAppEnv::m_logDelete;
int CAppEnv::m_logDeleteDate;
bool CAppEnv::m_logOut;
QString CAppEnv::m_license;
QDate CAppEnv::m_probation;
QFile CAppEnv::m_logFile;
QTextStream CAppEnv:: m_textStream;
bool CAppEnv::m_adminUser;
QPointer<QWidget> CAppEnv::m_mainWgt;
Qt::CaseSensitivity CAppEnv::m_caseSensitivity;
qreal CAppEnv::m_xDpiFactor;
qreal CAppEnv::m_yDpiFactor;
qreal CAppEnv::m_dpiFactor;

CSingleRcsErrorMessageBox* CSingleRcsErrorMessageBox::m_instance = nullptr;
QMutex CSingleRcsErrorMessageBox::m_mutex;
CSingleUpdateMessageBox* CSingleUpdateMessageBox::m_instance = nullptr;
QMutex CSingleUpdateMessageBox::m_mutex;


class Message
{
public:
    virtual ~Message() {}

    virtual void foo()
    {

    }
};


class Message1 : public Message
{
public:

    Message1()
    {
        std::cout << "message1" << std::endl;
    }

    Message1(int a)
    {
        std::cout << "message1" << std::endl;
    }

    ~Message1()
    {
    }

    void foo() override
    {
        std::cout << "message1" << std::endl;
    }
};

struct factory
{
    template<typename T>
    struct register_t
    {
        register_t(const std::string& key)
        {
            factory::get().map_.emplace(key, [] { return new T(); });
        }

        template<typename... Args>
        register_t(const std::string& key, Args... args)
        {
            factory::get().map_.emplace(key, [&] { return new T(args...); });
        }
    };

    static Message* produce(const std::string& key)
    {
        if (map_.find(key) == map_.end())
            throw std::invalid_argument("the message key is not exist!");

        return map_[key]();
    }

    static std::unique_ptr<Message> produce_unique(const std::string& key)
    {
        return std::unique_ptr<Message>(produce(key));
    }

    static std::shared_ptr<Message> produce_shared(const std::string& key)
    {
        return std::shared_ptr<Message>(produce(key));
    }

private:
    factory() {};
    factory(const factory&) = delete;
    factory(factory&&) = delete;

    static factory& get()
    {
        static factory instance;
        return instance;
    }

    static std::map<std::string, std::function<Message*()>> map_;
};

std::map<std::string, std::function<Message*()>> factory::map_;

#define REGISTER_MESSAGE_VNAME(T) reg_msg_##T##_
#define REGISTER_MESSAGE(T, key, ...) static factory::register_t<T> REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);
//REGISTER_MESSAGE(Message1, "message1", 2);
REGISTER_MESSAGE(Message1, "message1");


/*!
 * \brief  messageOutput
 * \param  type
 * \param  context
 * \param  msg
 * \note   日志输出函数
 * \author zhangjun
 * \date   2016-10-13
 */
void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    Q_UNUSED(context)

    static QMutex mutex;
    mutex.lock();

    QString info = "[" + QDateTime::currentDateTime().toString("yyyy/MM/dd-hh:mm:ss") + "]";
    switch(type)
    {
    case QtInfoMsg:
        info += QString("Infomation:") + msg;
        break;
    case QtDebugMsg:
        info += QString("Debug:") + msg;
        break;
    case QtWarningMsg:
        info += QString("Warning:") + msg;
        break;
    case QtCriticalMsg:
        info += QString("Critical:") + msg;
        break;
    case QtFatalMsg:
        info += QString("Fatal:") + msg;
        break;
    default:
        info += QString("Unknow:") + msg;
        break;
    }
    //    CAppEnv::writeLog(info);
    CAppEnv::m_textStream << info << endl;
    mutex.unlock();
}

CHintMessage::CHintMessage(QWidget *parent) : QDialog(parent, Qt::FramelessWindowHint),
    m_label(new QLabel)
{
    QFont font = m_label->font();
    font.setPixelSize(14);
    m_label->setFont(font);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_label);
    this->setLayout(layout);
    this->setStyleSheet(u8"QWidget {background-color: #404244; color: #D2D2D2;}");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(windowFlags() | Qt::Tool);
//    this->setWindowModality(Qt::WindowModal);
    this->setWindowOpacity(0.7);
    QTimer::singleShot(1000, this, SLOT(playCloseAnimation()));
}

void CHintMessage::playCloseAnimation()
{
    setMinimumSize(0,0);
    QPropertyAnimation* closeAnimation = new QPropertyAnimation(this,"geometry");
    closeAnimation->setStartValue(geometry());
    closeAnimation->setEndValue(QRect(geometry().x(),geometry().y()+height()/2,width(),0));
    closeAnimation->setDuration(500);
    closeAnimation->start(QAbstractAnimation::DeleteWhenStopped);

//    QGraphicsOpacityEffect *pOpacity = new QGraphicsOpacityEffect(this);
//    pOpacity->setOpacity(0.7);
//    setGraphicsEffect(pOpacity);
//    QPropertyAnimation* closeAnimation = new QPropertyAnimation(this);
//    closeAnimation->setTargetObject(pOpacity);
//    closeAnimation->setPropertyName("opacity");
//    closeAnimation->setDuration(500);
//    closeAnimation->setStartValue(0.7);
//    closeAnimation->setEndValue(0);
//    closeAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(500, this, SLOT(close()));
}

void CHintMessage::setText(const QString &text)
{
    m_label->setText(text);
}


CSingleRcsErrorMessageBox::CSingleRcsErrorMessageBox(QWidget *parent) :
    QObject(parent)
{
    m_messageBox = new QMessageBox(parent);
    if (parent)
        m_messageBox->setWindowModality(Qt::WindowModal);
    m_messageBox->addButton(QMessageBox::Ok);
    m_messageBox->setWindowTitle(qApp->applicationName());
}

CSingleRcsErrorMessageBox::~CSingleRcsErrorMessageBox()
{
    if(!m_messageBox.isNull())
        m_messageBox.data()->close();
}


CSingleRcsErrorMessageBox *CSingleRcsErrorMessageBox::getInstance(QWidget *parent)
{
    if(m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if(m_instance == nullptr)
        {
            m_instance = new CSingleRcsErrorMessageBox(parent);
        }
    }
    return m_instance;
}

void CSingleRcsErrorMessageBox::releaseInstance()
{
    if(m_instance)
    {
        QMutexLocker locker(&m_mutex);
        if(m_instance)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }
}

void CSingleRcsErrorMessageBox::readyToShow(QMessageBox::Icon icon, QString text, QWidget *parent)
{
    if(getInstance(parent) == nullptr)
        return;
    if(getInstance(parent)->m_messageBox.data()->isHidden())
    {
        getInstance(parent)->m_messageBox.data()->setText(text);
        getInstance(parent)->m_messageBox.data()->setIcon(icon);
        getInstance(parent)->m_messageBox.data()->exec();
    }
}


CSingleUpdateMessageBox::CSingleUpdateMessageBox(QWidget *parent) :
    QObject(parent)
{
    m_messageBox = new QMessageBox(parent);
    if (parent)
        m_messageBox->setWindowModality(Qt::WindowModal);
    //    m_messageBox->addButton(QMessageBox::Ok);
    //    m_messageBox->addButton(QMessageBox::No);
    m_messageBox->setWindowTitle(qApp->applicationName());
    m_yesButton = m_messageBox->addButton(tr(u8"升级"), QMessageBox::AcceptRole);
    m_messageBox->addButton(tr(u8"忽略"), QMessageBox::RejectRole);
    m_messageBox->setDefaultButton(qobject_cast<QPushButton*>(m_yesButton));
}

CSingleUpdateMessageBox::~CSingleUpdateMessageBox()
{
    if(!m_messageBox.isNull())
        m_messageBox.data()->close();
}


CSingleUpdateMessageBox *CSingleUpdateMessageBox::getInstance(QWidget *parent)
{
    if(m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if(m_instance == nullptr)
        {
            m_instance = new CSingleUpdateMessageBox(parent);
        }
    }
    return m_instance;
}

void CSingleUpdateMessageBox::releaseInstance()
{
    if(m_instance)
    {
        QMutexLocker locker(&m_mutex);
        if(m_instance)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }
}

bool CSingleUpdateMessageBox::readyToShow(QMessageBox::Icon icon, QString text, QWidget *parent)
{
    //    if(getInstance()->m_currTimerId > 0)
    //        return;
    if(getInstance(parent) == nullptr)
    {
        Q_ASSERT(false);
        return false;
    }
    if(getInstance(parent)->m_messageBox.data()->isHidden())
    {
        getInstance(parent)->m_messageBox.data()->setText(text);
        //    if (!detailedText.isEmpty())
        //        getInstance()->m_messageBox.data()->setInformativeText(detailedText);
        getInstance(parent)->m_messageBox.data()->setIcon(icon);
        getInstance(parent)->m_messageBox.data()->exec();
        QAbstractButton *clickedButton = getInstance(parent)->m_messageBox.data()->clickedButton();
        return clickedButton == getInstance(parent)->m_yesButton;
    }
    return false;
}

bool CAppEnv::associateFileTypes()
{
#if defined(Q_OS_WIN)
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();

    const QString key = QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    if (settings.status() != QSettings::NoError) {
        qWarning() << "Cannot access registry key" << key;
        return false;
    }
//    settings.setValue(QStringLiteral("FriendlyAppName"), displayName);

    settings.beginGroup(QStringLiteral("SupportedTypes"));
    settings.setValue(".rpusln", QString());
    settings.endGroup();

    settings.beginGroup(QStringLiteral("shell"));
    settings.beginGroup(QStringLiteral("open"));
//    settings.setValue(QStringLiteral("FriendlyAppName"), displayName);
    settings.beginGroup(QStringLiteral("Command"));
    settings.setValue(QStringLiteral("."),
                      QLatin1Char('"') + QDir::toNativeSeparators(filePath) + QStringLiteral("\" \"%1\""));
#endif
    return true;
}

bool CAppEnv::checkLicense(QString license, bool probationAvaliable)
{
    if(license.isEmpty())
        license = CAppEnv::m_license;
    QByteArray bty;
    bty.append(license);
    QByteArray md5Code = QCryptographicHash::hash(bty, QCryptographicHash::Md5);
    QString md5 = md5Code.toHex().toUpper();
    QFile file(":sn/sn");
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        if(md5 == in.readLine())
            return true;
    }

    QSettings settingsTry(ENVIRONMENT_KEY, "rpudesignerTry");
    QDate probation = settingsTry.value("probation").toDate();
    if(probationAvaliable && probation.daysTo(QDate::currentDate()) < 0)
    {
        if(!settingsTry.value("last").isNull() &&
                settingsTry.value("last").toDateTime().secsTo(QDateTime::currentDateTime()) < 0)
        {
            settingsTry.setValue("last", QDateTime::currentDateTime());
            return false;
        }
        settingsTry.setValue("last", QDateTime::currentDateTime());
        return true;
    }
    settingsTry.setValue("last", QDateTime::currentDateTime());
    return false;
}

/*!
 * \brief CAppEnv::checkDirEnv
 * \return
 * \note 校验程序目录环境
 */
void CAppEnv::checkDirEnv()
{
    QDir dir;

    dir.setPath(getBinDirPath());
    if(!dir.exists())
    {
        throw CError(QObject::tr(u8"bin文件夹不存在"));
    }

#if defined(Q_OS_UNIX)
    dir.setPath(getLibDirPath());
    if(!dir.exists())
    {
        throw CError(QObject::tr(u8"lib文件夹不存在"));
    }
#endif
    dir.setPath(getEtcDirPath());
    if(!dir.exists())
    {
        throw CError(QObject::tr(u8"etc文件夹不存在"));
    }

    bool flag = true;
#if defined(Q_OS_WIN)
    dir = QDir::home();
    dir.cd("Documents");
    if(!dir.mkpath("RPUDesigner"))
    {
        flag = false;
    }
#endif

    dir = getAppDir();
    if(!dir.mkpath(getDocDirPath()))
    {
        flag = false;
    }

    dir = getRPUDesignerDir();
    if(!dir.mkpath(getCustomEtcDirPath()) || !dir.mkpath(getCustomEtcCustomPeDirPath())
            || !dir.mkpath(getCustomEtcSimulatorDirPath())
            || !dir.mkpath(getLogDirPath()) || !dir.mkpath(getTmpDirPath())
            || !dir.mkpath(getVarDirPath()))
    {
        flag = false;
    }

    dir.setPath(getVarDirPath());
    if(!dir.mkpath(getBinOutDirPath()) /*|| !dir.mkpath(getCmdgenOutDirPath())*/
            || !dir.mkpath(getCmdwordOutDirPath()) || !dir.mkpath(getFirmwareOutDirPath())
            /*|| !dir.mkpath(getFifoOutDirPath())
                    || !dir.mkpath(getMemOutDirPath()) || !dir.mkpath(getMergeOutDirPath())*/
            || !dir.mkpath(getProOutDirPath())
            || !dir.mkpath(getXmlOutDirPath()) || !dir.mkpath(getCycleOutDirPath())
            || !dir.mkpath(getHeadInDirPath()) || !dir.mkpath(getRunResultOutDirPath())
            || !dir.mkpath(getMakerOutDirPath()))
    {
        flag = false;
    }
    if(QFileInfo(getDemoProOutDirPath()).exists())
    {
        QDir _dir;
        _dir.setPath(getDemoProOutDirPath());
        _dir.removeRecursively();
    }
    if(!dir.mkpath(getDemoProOutDirPath())) {
        flag = false;
    } else {
        copyDir(getDocDirPath() + "/projects", getDemoProOutDirPath());
    }

    if(!flag)
    {
        throw CError(QObject::tr(u8"程序文件夹未能创建"));
    }
}

bool CAppEnv::copyDir(const QString &source, const QString &destination, bool override)
{
    QDir directory(source);
    if (!directory.exists())
    {
        return false;
    }

    QString srcPath = QDir::toNativeSeparators(source);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QString dstPath = QDir::toNativeSeparators(destination);
    if (!dstPath.endsWith(QDir::separator()))
        dstPath += QDir::separator();


    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (override)
            {
                QFile::setPermissions(dstFilePath, QFile::WriteOwner);
            }
            QFile::copy(srcFilePath, dstFilePath);
        }
        else if (fileInfo.isDir())
        {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if (!copyDir(srcFilePath, dstFilePath, override))
            {
                error = true;
            }
        }
    }
    return !error;
}

QString CAppEnv::getAppName()
{
    return qApp->applicationName();
}

QString CAppEnv::getAppFilePath()
{
    return qApp->applicationFilePath();
}

QDir CAppEnv::getAppDir()
{
    QDir dir(qApp->applicationDirPath());
    dir.cdUp();
    return dir;
}

QString CAppEnv::getAppDirPath()
{
    return getAppDir().absolutePath();
}

QString CAppEnv::getBinDirPath()
{
    return qApp->applicationDirPath();
}

QString CAppEnv::getDocDirPath()
{
    return getAppDirPath() + "/doc";
}

QString CAppEnv::getEtcDirPath()
{
    return getAppDirPath() + "/etc";
}

QString CAppEnv::getEtcCustomPeDirPath()
{
    return getEtcDirPath() + "/custom_pe";
}

QString CAppEnv::getEtcSimulatorDirPath()
{
    return getEtcDirPath() + "/simulator";
}

QString CAppEnv::getEtcSimulatorConfigFilePath()
{
    return getEtcSimulatorDirPath() + "/simulator_config.xml";
}

QString CAppEnv::getCustomEtcDirPath()
{
    return getRPUDesignerDirPath() + "/etc";
}

QString CAppEnv::getCustomEtcCustomPeDirPath()
{
    return getCustomEtcDirPath() + "/custom_pe";
}

QString CAppEnv::getCustomEtcSimulatorDirPath()
{
    return getCustomEtcDirPath() + "/simulator";
}

QString CAppEnv::getCustomEtcSimulatorConfigFilePath()
{
    return getCustomEtcSimulatorDirPath() + "/simulator_config.xml";
}

QString CAppEnv::getImageDirPath()
{
    return getAppDirPath() + "/image";
}

QString CAppEnv::getScriptDirPath()
{
    return getAppDirPath() + "/script";
}

QString CAppEnv::getPluginDirPath()
{
    return getAppDirPath() + "/plugins";
}

QDir CAppEnv::getRPUDesignerDir()
{
    QDir dir(getRPUDesignerDirPath());
    if(dir.exists())
        return dir;
    else
        return QDir();
}

QString CAppEnv::getRPUDesignerDirPath()
{
//    const QStringList &docDirs =  QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
//    if(docDirs.count() > 0)
//        return docDirs.at(0) + "/RPUDesigner";
#if defined(Q_OS_WIN)
    return QDir::homePath() + "/Documents/RPUDesigner";
#elif defined(Q_OS_UNIX)
    return QDir::homePath() + "/Documents/RPUDesigner";
#endif
}

#if defined(Q_OS_UNIX)
QString CAppEnv::getLibDirPath()
{
    return getAppDirPath() + "/lib";
}
#endif

QDir CAppEnv::getLogDir()
{
    QDir dir = getRPUDesignerDir();
    if(dir.cd("log"))
    {
        return dir;
    }
    else
    {
        return QDir();
    }
}

QString CAppEnv::getLogDirPath()
{
    return getRPUDesignerDirPath() + "/log";
}

void CAppEnv::readSetting()
{
    QSettings settings(ENVIRONMENT_KEY, "rpudesigner");
    int logSize = settings.value("logSize", 10).toInt();
    m_logSize = logSize * SIZE_MB;
    m_logArchive = settings.value("logArchive", true).toBool();
    m_logDelete = settings.value("logDelete", true).toBool();
    m_logDeleteDate = settings.value("logDeleteDate", 365).toInt();
    m_logOut = settings.value("logOut", true).toBool();
    m_license = settings.value("license").toString();
    QSettings settingsTry(ENVIRONMENT_KEY, "rpudesignerTry");
    m_probation = settingsTry.value("probation").toDate();
}

void CAppEnv::initLog()
{
    QDir dir = getLogDir();
    if(!dir.exists())
    {
        throw CError(QObject::tr(u8"日志文件夹不存在"));
    }

    m_sysLog = getLogDirPath() + "/sys.log";

    if(QFileInfo(m_sysLog).exists() && QFileInfo(m_sysLog).size() > m_logSize)
    {
        QString newName = getLogDirPath() + QObject::tr(u8"/sys_%0.log").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
        QFile::rename(m_sysLog, newName);
        if(m_logArchive)
        {
            // do something
        }
    }

    if(m_logDelete)
    {
        QStringList filters;
        filters << "sys_*.log" << "debug_*.log" << "sys_*.log.zip" << "debug_*.log.zip";
        dir.setFilter(QDir::NoSymLinks|QDir::Files|QDir::NoDotAndDotDot);
        dir.setNameFilters(filters);
        QFileInfoList fileInfoList = dir.entryInfoList();
        if(fileInfoList.count() > 0)
        {
            foreach (QFileInfo fileInfo, fileInfoList)
            {
                if(fileInfo.lastModified().date().daysTo(QDate::currentDate()) > m_logDeleteDate)
                {
                    QFile(fileInfo.absoluteFilePath()).remove();
                }
            }
        }
    }

    m_logFile.setFileName(m_sysLog);
    if(m_logOut)
    {
        openLog(true);
    }
    else
    {
        closeLog();
    }
}

void CAppEnv::openLog(bool separator)
{
    if(!m_logFile.isOpen() && !m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        throw CError(QObject::tr(u8"日志文件不可写入"));
    }

    m_textStream.setDevice(&m_logFile);
    if(separator)
        m_textStream << "==========================================================================" << endl;
    qInstallMessageHandler(messageOutput);
}

void CAppEnv::closeLog()
{
    qInstallMessageHandler(0);
    m_textStream.setDevice(0);
    m_logFile.close();
}

//void CAppEnv::writeLog(QString log)
//{
//    m_textStream << log << endl;
//}

QString CAppEnv::getVarDirPath()
{
    return getRPUDesignerDirPath() + "/var";
}

QString CAppEnv::getBinOutDirPath()
{
    return getVarDirPath() + "/bin";
}

QString CAppEnv::getCmdwordOutDirPath()
{
    return getVarDirPath() + "/cmdword";
}

QString CAppEnv::getFirmwareOutDirPath()
{
    return getVarDirPath() + "/firmware";
}

bool CAppEnv::clcFirmwareOutDir()
{
    QDir dir = getVarDir();
    if(dir.cd("firmware"))
    {
       if(dir.removeRecursively())
       {
           dir = getVarDir();
           return dir.mkpath(getFirmwareOutDirPath());
       }
       else
           return false;
    }
    else
        return true;
}

bool CAppEnv::clcCmdWordDir()
{
    QDir dir = getVarDir();
    if(dir.cd("cmdword"))
        return dir.removeRecursively();
    else
        return true;
    //    QDir dir(getVarDirPath());
    //    dir.cd("cmdwordout") ? dir.removeRecursively() : dir.mkdir("cmdwordout");
    //    QDir dir(getCmdwordDirPath());
    //    if(dir.exists())
    //    {
    //        dir.setFilter(QDir::Files);
    //        bool flag = true;
    //        for (uint i = 0; i < dir.count(); i++)
    //        {
    //            if(!dir.remove(dir[i]))
    //            {
    //                flag = false;
    //            }
    //        }
    //        return flag;
    //    }
    //    else
    //    {
    //        return checkDirEnv();
    //    }
}

QString CAppEnv::getCycleOutDirPath()
{
    return getVarDirPath() + "/cycleLog";
}

bool CAppEnv::clcCycleOutDir()
{
    QDir dir = getVarDir();
    if(dir.cd("cycleLog"))
        return dir.removeRecursively();
    else
        return true;
    //    QDir dir(getCycleOutDirPath());
    //    if(dir.exists())
    //    {
    //        dir.setFilter(QDir::Files);
    //        bool flag = true;
    //        for (uint i = 0; i < dir.count(); i++)
    //        {
    //            if(!dir.remove(dir[i]))
    //            {
    //                flag = false;
    //            }
    //        }
    //        return flag;
    //    }
    //    else
    //    {
    //        return checkDirEnv();
    //    }
}

QString CAppEnv::getRunResultOutDirPath()
{
    return getVarDirPath() + "/runResult";
}

QString CAppEnv::getMakerOutDirPath()
{
    return getVarDirPath() + "/maker";
}

//QString CAppEnv::getFifoOutDirPath()
//{
//    return getVarDirPath() + "/fifoOut";
//}

//bool CAppEnv::clcFifoOutDir()
//{
//    QDir dir = getVarDir();
//    if(dir.cd("[ARRAY]fifoOut"))
//        return dir.removeRecursively();
//    else
//        return true;
//    //    QDir dir(getFifoOutDirPath());
//    //    if(dir.exists())
//    //    {
//    //        dir.setFilter(QDir::Files);
//    //        bool flag = true;
//    //        for (uint i = 0; i < dir.count(); i++)
//    //        {
//    //            if(!dir.remove(dir[i]))
//    //            {
//    //                flag = false;
//    //            }
//    //        }
//    //        return flag;
//    //    }
//    //    else
//    //    {
//    //        return checkDirEnv();
//    //    }
//}

//QString CAppEnv::getMemOutDirPath()
//{
//    return getVarDirPath() + "/memoryOut";
//}

//QString CAppEnv::getMergeOutDirPath()
//{
//    return getVarDirPath() + "/merge";
//}

QString CAppEnv::getProOutDirPath()
{
    return getVarDirPath() + "/project";
}

QString CAppEnv::getDemoProOutDirPath()
{
    return getVarDirPath() + "/demoproject";
}

QString CAppEnv::getXmlOutDirPath()
{
    return getVarDirPath() + "/xml";
}

QString CAppEnv::getHeadInDirPath()
{
    return getVarDirPath() + "/head";
}

QString CAppEnv::getTmpDirPath()
{
    return getRPUDesignerDirPath() + "/tmp";
}

void CAppEnv::clcTmpDir()
{
    QDir dir = getRPUDesignerDir();
    if(dir.cd("tmp") && !dir.removeRecursively())
    {
        throw CError(QObject::tr(u8"程序临时文件夹未能清空"));
    }
}

QDir CAppEnv::getVarDir()
{
    QDir dir = getRPUDesignerDir();
    if(dir.cd("var"))
    {
        return dir;
    }
    else
    {
        return QDir();
    }
}

/**
 * CAppEnv::relativeToAbsolute
 * \brief
 * \param   path
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString CAppEnv::relToAbs(const QString &base, const QString &fileName)
{
    if(fileName.isEmpty() || fileName.startsWith(":"))
        return fileName;

    QDir dir = QFileInfo(base).absoluteDir();
    QString str = QDir::cleanPath(dir.absoluteFilePath(fileName));
    //    DBG << QObject::tr(u8"文件“%0”转为绝对路径“%2”").arg(fileName).arg(str);
    return str;
}

/**
 * CAppEnv::absoluteToRelative
 * \brief
 * \param   path
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString CAppEnv::absToRel(const QString &base, const QString &fileName)
{
    if(fileName.isEmpty() || fileName.startsWith(":"))
        return fileName;

    QDir dir = QFileInfo(base).absoluteDir();
    QString str = QDir::cleanPath(dir.relativeFilePath(fileName));
    //    DBG << QObject::tr(u8"文件“%0”转为相对路径“%2”").arg(fileName).arg(str);
    return str;
}

QString CAppEnv::stpToNm(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QString CAppEnv::stpToBs(const QString &fullFileName)
{
    return QFileInfo(fullFileName).baseName();
}

QString CAppEnv::stpToCpltBs(const QString &fullFileName)
{
    return QFileInfo(fullFileName).completeBaseName();
}

/*!
 * CAppEnv::addQtaMrk
 * \brief
 * \param   str
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString CAppEnv::addQtaMrk(const QString &str)
{
    //#if defined(Q_OS_WIN)
    if(str.contains(" "))
        return ("\"" + str + "\"");
    else
        //#endif
        return str;
}

QString CAppEnv::toNtvSprt(const QString &fileName)
{
    return QDir::toNativeSeparators(fileName);
}

QVector<int> CAppEnv::chunkSizes(const int size, const int chunkCount)
{
    Q_ASSERT(size > 0 && chunkCount > 0);
    if (chunkCount == 1)
        return QVector<int>() << size;
    QVector<int> result(chunkCount, size / chunkCount);
    if (int remainder = size % chunkCount) {
        int index = 0;
        for (int i = 0; i < remainder; ++i) {
            ++result[index];
            ++index;
            index %= chunkCount;
        }
    }
    return result;
}

void CAppEnv::animation(QWidget *wgt, QWidget *parent)
{
#ifdef CUSTOM_ANIMATION
#if defined(Q_OS_WIN)
    QPropertyAnimation *animation = new QPropertyAnimation(wgt, "pos");
    animation->setDuration(300);
    QPoint startPoint = QPoint(parent->geometry().center().x() - wgt->normalGeometry().width()/2 - 20, parent->geometry().center().y() - wgt->normalGeometry().height()/2 - 40);
    QPoint endPoint = QPoint(parent->geometry().center().x() - wgt->normalGeometry().width()/2, parent->geometry().center().y() - wgt->normalGeometry().height()/2 - 20);
    if(startPoint.x() < 20)
    {
        startPoint.setX(20);
        endPoint.setX(40);
    }
    if(startPoint.y() < 20)
    {
        startPoint.setY(20);
        endPoint.setY(40);
    }
    animation->setStartValue(startPoint);
    animation->setEndValue(endPoint);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
#elif defined(Q_OS_UNIX)
    Q_UNUSED(wgt)
    Q_UNUSED(parent)
#endif
#else
    Q_UNUSED(wgt)
    Q_UNUSED(parent)
#endif
}

bool CAppEnv::executeCmd(const QString &cmd, const QStringList &args, QString &msg, QString &errorMsg, int msecs)
{
    QProcess process;
    process.setProgram(cmd);
    process.setArguments(args);
    process.setProcessChannelMode(QProcess::SeparateChannels);
    process.start();
    if(!process.waitForFinished(msecs))
    {
        errorMsg = process.errorString();
        return false;
    }
    else
    {
        QByteArray errorBtyArry = process.readAllStandardError();
        errorMsg = QString(errorBtyArry);
        QByteArray btyArray = process.readAllStandardOutput();
        msg = QString(btyArray);

        if(process.exitCode() != 0)
        {
            return false;
        }
    }
    return true;
}

QString CAppEnv::getMacAddr()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString strMacAddr;
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}

void CAppEnv::generateDpiFactor()
{
    QDesktopWidget* desktopWidget = QApplication::desktop();
    //获取可用桌面大小
//    QRect deskRect = desktopWidget->availableGeometry();
    //获取设备屏幕大小
    QRect screenRect = desktopWidget->screenGeometry();

    //获取系统设置的屏幕个数（屏幕拷贝方式该值为1）
//    desktopWidget->screenCount();

    m_xDpiFactor = screenRect.width() / (qreal)1920;
    m_yDpiFactor = screenRect.height() / (qreal)1080;
    m_dpiFactor = qMin(m_xDpiFactor, m_yDpiFactor);
}

bool CAppEnv::findFile(QStringList &fileList, const QString &path, const QString &suffix)
{
    QDir dir(path);
    if (!dir.exists()) {
        return false;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    const QFileInfoList& list = dir.entryInfoList();
    foreach (const auto &fileInfo, list) {
        if (fileInfo.isDir()) {
            findFile(fileList, fileInfo.filePath(), suffix);
        } else {
            if(fileInfo.suffix() == suffix)
                fileList.append(fileInfo.absoluteFilePath());
        }
    }
    return true;
}

void CAppEnv::showHintMessage(const QString &text, QWidget *wgt)
{
    QPointer<CHintMessage> mesg = new CHintMessage(text, wgt);
    mesg->exec();
}
