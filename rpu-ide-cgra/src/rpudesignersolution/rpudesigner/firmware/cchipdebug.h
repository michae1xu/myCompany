#ifndef CCHIPDEBUG_H
#define CCHIPDEBUG_H

#include <QObject>
#include <QMutex>
#include <QHash>
#include "chardwaredebug.h"

#define BCUMAX 4
#define RCUMAX 4
#define BFUMAX 8
#define SBOXMAX 4
#define BENESMAX 1

class CHardwareDebug;
class CfgDebugLog;
class CPeModel;
class CDebugSession;

using LineDataHash=QHash<QString, QMap<qint64, quint32>* >;

//CHardwareDebug的适配器类，外观类
class CChipDebug : public QObject
{
    Q_OBJECT
public:
    static CChipDebug *getInstance(QObject *parent);
    static void releaseInstance();

    static CChipDebug *m_instance;
    static QMutex m_mutex;
    static bool m_isDebugging;
    static bool getIsDebugging();
    static bool hasInstance();
    uint getCurCycle() const;
    void setSaveWave(bool flag);
    void setDebugSession(CDebugSession *debugSession);
    void saveToDebugSession();
    void restoreDebugSession(const CDebugSession &debugSession);
    void savePeValueToWave(uint32_t *allPeValue);
    void saveWaveData(const QString &fileName);

private:
    explicit CChipDebug(QObject *parent = nullptr);
    ~CChipDebug();

    QWidget *m_parent;
    CDebugSession *m_debugSession = nullptr;
    CHardwareDebug *m_hardwareDebug;
    CfgDebugLog *m_cfgDebugLog;
    QList<CPeModel *> m_peModelList;
    int m_curPeModelIndex;
    uint m_curCycle;
    bool m_runIsBusy;
    bool m_pause;
    bool m_saveWave = false;
    LineDataHash m_dataHash;

public:
    //调试控制
    bool writeGprf(const int &id, const int &offset, uint *data, const uint &len);
    bool writeInFifo(const int &id, uint *data, const int &len);
    bool enterDebug(const QString &firmwareDir, const QStringList &xmlFileNameList, CfgDebugLog *cfgDebugLog);
    bool startDebug(const int &maxCycle, const QString &cmdWordDir);
    bool stopDebug();
    bool runCycle(int cycle);
    bool runWithBreak();

    //调试数据
    bool getPEValue();
    bool getOutFifoValue();
    bool getOutFifoStatus();
    QStringList getAllPEValue();
    bool getRunIsBusy() const;

public slots:
    void modifyPEValue(const QString &element, const QStringList &input, QStringList output);
    void parseModelChanged(int index);
    void setPause();

signals:
    void sendMessage(QString);
    void sendErrorMessage(QString);
    void sendPeValue(QStringList);
    void sendCurCycle(int);
    void sendTimeOut();
    void sendEnableRunPause(bool);
};

#endif // CCHIPDEBUG_H
