#ifndef CAPPENV_H
#define CAPPENV_H

#define SIZE_MB 1048576;

#include <exception>
#include <QString>
#include <QMessageBox>
#include <QPointer>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDirModel>

class QDir;

class CHintMessage : public QDialog
{
    Q_OBJECT
public:
    explicit CHintMessage(const QString &text, QWidget *parent = 0) :
        CHintMessage(parent) {
        setText(text);
    }
    explicit CHintMessage(QWidget *parent = 0);

    void setText(const QString &text);

private:
    QLabel *m_label;

private slots:
    void playCloseAnimation();
};

class CError : public std::exception
{
public:
    explicit CError(const QString &message) noexcept
        : message(message.toUtf8()) {}
    ~CError() throw() {}

    const char *what() const noexcept { return message; }

private:
    const char *message;
};

#ifdef USE_CUSTOM_DIR_MODEL
class CDirModel : public QDirModel
{
public:
    explicit CDirModel(QObject *parent=0) : QDirModel(parent) {}

    QVariant data(const QModelIndex &index,
                  int role=Qt::DisplayRole) const
    {
        if (role == Qt::DisplayRole && index.column() == 0) {
            QString path = QDir::toNativeSeparators(filePath(index));
            if (path.endsWith(QDir::separator()))
                path.chop(1);
            return path;
        }
        return QDirModel::data(index, role);
    }
};
#endif


class CSingleRcsErrorMessageBox : public QObject
{
    Q_OBJECT
public:
    static CSingleRcsErrorMessageBox *getInstance(QWidget *parent = 0);
    static void releaseInstance();
    static void readyToShow(QMessageBox::Icon icon, QString text, QWidget *parent = 0);

    static CSingleRcsErrorMessageBox *m_instance;
    static QMutex m_mutex;

private:
    explicit CSingleRcsErrorMessageBox(QWidget *parent = 0);
    ~CSingleRcsErrorMessageBox();

    QPointer<QMessageBox> m_messageBox;
};


class CSingleUpdateMessageBox : public QObject
{
    Q_OBJECT
public:
    static CSingleUpdateMessageBox *getInstance(QWidget *parent = 0);
    static void releaseInstance();
    static bool readyToShow(QMessageBox::Icon icon, QString text, QWidget *parent = 0);

    static CSingleUpdateMessageBox *m_instance;
    static QMutex m_mutex;

private:
    explicit CSingleUpdateMessageBox(QWidget *parent = 0);
    ~CSingleUpdateMessageBox();

    QPointer<QMessageBox> m_messageBox;
    QAbstractButton *m_yesButton;
};


class CAppEnv
{
    CAppEnv() = default;
    ~CAppEnv() = default;

    CAppEnv(const CAppEnv &) = delete;
    CAppEnv& operator = (const CAppEnv &) = delete;

public:
    static bool associateFileTypes();
    static bool checkLicense(QString license = QString(), bool probationAvaliable = true);
    static void checkDirEnv();
    static bool copyDir(const QString &source, const QString &destination, bool override = false);
    static QString getAppName();
    static QString getAppFilePath();
    static QDir getAppDir();
    static QString getAppDirPath();
    static QString getBinDirPath();
    static QString getDocDirPath();

    static QString getEtcDirPath();
    static QString getEtcCustomPeDirPath();
    static QString getEtcSimulatorDirPath();
    static QString getEtcSimulatorConfigFilePath();

    static QString getCustomEtcDirPath();
    static QString getCustomEtcCustomPeDirPath();
    static QString getCustomEtcSimulatorDirPath();
    static QString getCustomEtcSimulatorConfigFilePath();

    static QString getImageDirPath();
    static QString getScriptDirPath();
    static QString getPluginDirPath();
#if defined(Q_OS_UNIX)
    static QString getLibDirPath();
#endif
    static QDir getRPUDesignerDir();
    static QString getRPUDesignerDirPath();
    static QDir getLogDir();
    static QString getLogDirPath();
    static void readSetting();
    static void initLog();
    static QString getTmpDirPath();
    static void clcTmpDir();
    static QDir getVarDir();
    static QString getVarDirPath();

    static QString getBinOutDirPath();
    static QString getCmdwordOutDirPath();
    static bool clcCmdWordDir();
    static QString getFirmwareOutDirPath();
    static bool clcFirmwareOutDir();
    static QString getCycleOutDirPath();
    static bool clcCycleOutDir();
    static QString getRunResultOutDirPath();
    static QString getMakerOutDirPath();
    static QString getProOutDirPath();
    static QString getDemoProOutDirPath();
    static QString getXmlOutDirPath();
    static QString getHeadInDirPath();

    static QString relToAbs(const QString &base, const QString &fileName);
    static QString absToRel(const QString &base, const QString &fileName);

    static QString stpToNm(const QString &fullFileName);
    static QString stpToBs(const QString &fullFileName);
    static QString stpToCpltBs(const QString &fullFileName);
    static QString addQtaMrk(const QString &str);
    static QString toNtvSprt(const QString &fileName);

    static void openLog(bool separator = false);
    static void closeLog();
//    static void writeLog(QString log);

    static QVector<int> chunkSizes(const int size, const int chunkCount);
    static void animation(QWidget *wgt, QWidget *parent);
    static void showHintMessage(const QString &text, QWidget *wgt);
    static bool executeCmd(const QString &cmd, const QStringList &args, QString &msg, QString &errorMsg, int msecs = 30000);
    static QString getMacAddr();
    static void generateDpiFactor();
    static bool findFile(QStringList &fileList, const QString &path, const QString &suffix);

    static QString m_sysLog;
//    static QString m_debugLog;
    static int m_logSize;
    static bool m_logArchive;
    static bool m_logDelete;
    static int m_logDeleteDate;
    static bool m_logOut;
    static QString m_license;
    static QDate m_probation;
    static QFile m_logFile;
    static QTextStream m_textStream;
    static bool m_adminUser;
    static QPointer<QWidget> m_mainWgt;
    static Qt::CaseSensitivity m_caseSensitivity;
    static qreal m_xDpiFactor;
    static qreal m_yDpiFactor;
    static qreal m_dpiFactor;
};
#endif // CAPPENV_H
