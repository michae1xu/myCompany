#include <QApplication>
#include "session/cdebugsession.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "cpemodel.h"
#include "cchipdebug.h"
#include "cfgdebuglog/CfgDebugLog.h"
#include "form/waittingform.h"

namespace {
constexpr qint32 g_waveMagicNumber = 0x5A78bBFA;
constexpr qint16 g_waveVersionNumber = 1;
}

CChipDebug* CChipDebug::m_instance = nullptr;
QMutex CChipDebug::m_mutex;
bool CChipDebug::m_isDebugging = false;

CChipDebug::CChipDebug(QObject *parent) :
    QObject(parent),
    m_curPeModelIndex(0),
    m_curCycle(0),
    m_runIsBusy(false),
    m_pause(false)
{
    m_hardwareDebug = CHardwareDebug::getInstance(parent);
    connect(m_hardwareDebug, SIGNAL(sendErrorMessage(QString)), this, SIGNAL(sendErrorMessage(QString)));
    connect(m_hardwareDebug, SIGNAL(sendTimeOut()), this, SIGNAL(sendTimeOut()));
}

CChipDebug::~CChipDebug()
{
    qDeleteAll(m_dataHash);
    m_dataHash.clear();
    qDeleteAll(m_peModelList);
}


void CChipDebug::setDebugSession(CDebugSession *debugSession)
{
    m_debugSession = debugSession;
}

void CChipDebug::saveToDebugSession()
{
    m_debugSession->setLastCycle(m_curCycle);
    const QList<DebugBreakPoint> *list = m_cfgDebugLog->getBreakPointList();
    foreach (DebugBreakPoint breakPoint, *list) {
        m_debugSession->addBreakPoint(breakPoint.pointStr, breakPoint.isEnabled);
    }
    m_debugSession->setCurrentXmlIndex(m_curPeModelIndex);
}

void CChipDebug::restoreDebugSession(const CDebugSession &debugSession)
{
    if(!getIsDebugging())
        return;

    QList<SPeValue> peValueList = debugSession.getModifyPeValueList();
    foreach (const SPeValue &peValue, peValueList) {
        while(m_curCycle < peValue.cycle)
        {
            runCycle(peValue.cycle - m_curCycle);
        }
        Q_ASSERT(m_curCycle == peValue.cycle);
        modifyPEValue(peValue.element, peValue.input, peValue.output);
    }
    Q_ASSERT(m_curCycle <= debugSession.getLastCycle());
    if(m_curCycle < debugSession.getLastCycle())
    {
        runCycle(debugSession.getLastCycle() - m_curCycle);
    }
    Q_ASSERT(m_curCycle == debugSession.getLastCycle());
    QList<DebugBreakPoint> *list = m_cfgDebugLog->getBreakPointList();
    QMapIterator<QString, bool> i(debugSession.getBreakPointMap());
    while(i.hasNext())
    {
        i.next();
        DebugBreakPoint breakPoint;
        breakPoint.isEnabled = i.value();
        breakPoint.pointStr = i.key();
        if(m_cfgDebugLog->setItemBreakInfo(breakPoint.pointStr, breakPoint.isEnabled))
            list->append(breakPoint);
    }
}

void CChipDebug::savePeValueToWave(uint32_t *allPeValue)
{
    uint32_t output[4];
    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX; peid++)
            {
                uint32_t *peValue = allPeValue + (bcuid*(RCUMAX*(BFUMAX+SBOXMAX+BENESMAX)) + rcuid*(BFUMAX+SBOXMAX+BENESMAX) + peid) * 4;
                for(int i = 0; i < 4; ++i){
                    output[i] = peValue[i];
                }
                m_dataHash.value(QString("BFU(%0,%1,%2) output:X").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[1]);
                m_dataHash.value(QString("BFU(%0,%1,%2) output:Y").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[0]);;
            }
            for(int peid = 0; peid < SBOXMAX; peid++)
            {
                uint32_t *peValue = allPeValue + (bcuid*(RCUMAX*(BFUMAX+SBOXMAX+BENESMAX)) + rcuid*(BFUMAX+SBOXMAX+BENESMAX) + BFUMAX + peid) * 4;
                for(int i = 0; i < 4; ++i){
                    output[i] = peValue[i];
                }
                m_dataHash.value(QString("SBOX(%0,%1,%2) output:0").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[0]);
                m_dataHash.value(QString("SBOX(%0,%1,%2) output:1").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[1]);
                m_dataHash.value(QString("SBOX(%0,%1,%2) output:2").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[2]);
                m_dataHash.value(QString("SBOX(%0,%1,%2) output:3").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[3]);
            }
            for(int peid = 0; peid < BENESMAX; peid++)
            {
                uint32_t *peValue = allPeValue + (bcuid*(RCUMAX*(BFUMAX+SBOXMAX+BENESMAX)) + rcuid*(BFUMAX+SBOXMAX+BENESMAX) + BFUMAX + SBOXMAX + peid) * 4;
                for(int i = 0; i < 4; ++i){
                    output[i] = peValue[i];
                }
                m_dataHash.value(QString("BENES(%0,%1,%2) output:0").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[0]);
                m_dataHash.value(QString("BENES(%0,%1,%2) output:1").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[1]);
                m_dataHash.value(QString("BENES(%0,%1,%2) output:2").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[2]);
                m_dataHash.value(QString("BENES(%0,%1,%2) output:3").arg(bcuid).arg(rcuid).arg(peid))->insert(m_curCycle, output[3]);
            }
        }
    }
}

void CChipDebug::saveWaveData(const QString &fileName)
{
    if(!getIsDebugging())
        return;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    QDataStream out(&file);
    out << g_waveMagicNumber << g_waveVersionNumber;
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
    out << m_curCycle << m_dataHash.count();
    QHashIterator<QString, QMap<qint64, quint32>* > hashIter(m_dataHash);
    while(hashIter.hasNext()) {
        hashIter.next();
        out << hashIter.key();
        out << *hashIter.value();
    }
    file.close();
}

bool CChipDebug::getRunIsBusy() const
{
    return m_runIsBusy;
}

void CChipDebug::setPause()
{
    qInfo() << tr("硬件联调：暂停自动运行");
//    if(m_runIsBusy)
        m_pause = true;
//    else
//        m_pause = false;
}

uint CChipDebug::getCurCycle() const
{
    return m_curCycle;
}

void CChipDebug::setSaveWave(bool flag)
{
    m_saveWave = flag;
    qDeleteAll(m_dataHash);
    m_dataHash.clear();
    for(int bcuid = 0; bcuid < BCUMAX; bcuid++)
    {
        for(int rcuid = 0; rcuid < RCUMAX; rcuid++)
        {
            for(int peid = 0; peid < BFUMAX; peid++)
            {
                QMap<qint64, quint32> *mapX = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("BFU(%0,%1,%2) output:X").arg(bcuid).arg(rcuid).arg(peid), mapX);
                QMap<qint64, quint32> *mapY = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("BFU(%0,%1,%2) output:Y").arg(bcuid).arg(rcuid).arg(peid), mapY);
            }
            for(int peid = 0; peid < SBOXMAX; peid++)
            {
                QMap<qint64, quint32> *map0 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("SBOX(%0,%1,%2) output:0").arg(bcuid).arg(rcuid).arg(peid), map0);
                QMap<qint64, quint32> *map1 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("SBOX(%0,%1,%2) output:1").arg(bcuid).arg(rcuid).arg(peid), map1);
                QMap<qint64, quint32> *map2 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("SBOX(%0,%1,%2) output:2").arg(bcuid).arg(rcuid).arg(peid), map2);
                QMap<qint64, quint32> *map3 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("SBOX(%0,%1,%2) output:3").arg(bcuid).arg(rcuid).arg(peid), map3);
            }
            for(int peid = 0; peid < BENESMAX; peid++)
            {
                QMap<qint64, quint32> *map0 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("BENES(%0,%1,%2) output:0").arg(bcuid).arg(rcuid).arg(peid), map0);
                QMap<qint64, quint32> *map1 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("BENES(%0,%1,%2) output:1").arg(bcuid).arg(rcuid).arg(peid), map1);
                QMap<qint64, quint32> *map2 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("BENES(%0,%1,%2) output:2").arg(bcuid).arg(rcuid).arg(peid), map2);
                QMap<qint64, quint32> *map3 = new QMap<qint64, quint32>;
                m_dataHash.insert(QString("BENES(%0,%1,%2) output:3").arg(bcuid).arg(rcuid).arg(peid), map3);
            }
        }
    }
}

CChipDebug *CChipDebug::getInstance(QObject *parent)
{
    if(m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if(m_instance == nullptr)
        {
            m_instance = new CChipDebug(parent);
        }
    }
    return m_instance;
}

void CChipDebug::releaseInstance()
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

bool CChipDebug::getIsDebugging()
{
    return m_isDebugging;
}

bool CChipDebug::hasInstance()
{
    return (m_instance != nullptr);
}

bool CChipDebug::enterDebug(const QString &firmwareDir, const QStringList &xmlFileNameList, CfgDebugLog *cfgDebugLog)
{
    qInfo() << tr("硬件联调：进入调试");
    if(m_runIsBusy)
        return false;
    Q_ASSERT(cfgDebugLog);
    emit sendMessage(tr("正在初始化调试"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在初始化调试"));
    m_cfgDebugLog = cfgDebugLog;
    m_curCycle = 0;

    for(int i = 0; i < xmlFileNameList.count(); ++i)
    {
        CPeModel *peModel = new CPeModel(xmlFileNameList.at(i), i);
        m_peModelList.append(peModel);
    }

    emit sendMessage(tr("正在初始化SDK"));
    if(!m_hardwareDebug->initSdk())
    {
        emit sendMessage(tr("初始化SDK失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }

    emit sendMessage(tr("正在初始化设备"));
    if(!m_hardwareDebug->debugReset())
    {
        emit sendMessage(tr("初始化设备失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }

    emit sendMessage(tr("正在进入调试"));
    if(!m_hardwareDebug->debugEnter())
    {
        emit sendMessage(tr("进入调试失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }

    QFile file(firmwareDir + "/include/renux/renux_const.h");
    if(file.open(QIODevice::ReadOnly))
    {
        uint8_t cmdBitsLen = 0;
        bool ok = false;
        QTextStream in(&file);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            if(line.contains("CMDBITS_LEN"))
            {
                QString str = line.trimmed().split(" ").last();
                cmdBitsLen = str.toUInt();
                ok = true;
            }
        }
        file.close();
        if(!ok)
        {
            CWaitDialog::getInstance()->finishShowing();
            return false;
        }
        emit sendMessage(tr("正在初始化配置数据"));
        if(!m_hardwareDebug->debugInitConfig(cmdBitsLen, firmwareDir + "/drivers/rpu/rpu_cfg.bin"))
        {
            emit sendMessage(tr("初始化配置数据失败"));
            CWaitDialog::getInstance()->finishShowing();
            return false;
        }
    }
    else
    {
        emit sendMessage(tr("初始化配置数据失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }
    emit sendMessage(tr("正在初始化SBOX数据"));
    if(!m_hardwareDebug->debugInitSboxData(firmwareDir + "/drivers/rpu/sboxData.bin", firmwareDir + "/drivers/rpu/renux_const.c"))
    {
        emit sendMessage(tr("初始化SBOX数据失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }
    CWaitDialog::getInstance()->finishShowing();
    return true;
}

bool CChipDebug::writeGprf(const int &id, const int &offset, uint *data, const uint &len)
{
    qInfo() << tr("硬件联调：写入GPRF数据");
    if(m_runIsBusy)
        return false;
    emit sendMessage(tr("正在写入GPRF数据"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在写入GPRF数据"));
    if(!m_hardwareDebug->debugWriteGprf(id, offset, data, len))
    {
        emit sendMessage(tr("写入GPRF数据失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }
    CWaitDialog::getInstance()->finishShowing();
    return true;
}

bool CChipDebug::writeInFifo(const int &id, uint *data, const int &len)
{
    qInfo() << tr("硬件联调：写入FIFO数据");
    if(m_runIsBusy)
        return false;
    emit sendMessage(tr("正在写入FIFO数据"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在写入FIFO数据"));
    if(!m_hardwareDebug->debugWriteFifo(id, data, len))
    {
        emit sendMessage(tr("写入FIFO数据失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }
    CWaitDialog::getInstance()->finishShowing();
    return true;
}

bool CChipDebug::startDebug(const int &maxCycle, const QString &firmwareDir)
{
    qInfo() << tr("硬件联调：开始调试");
    if(m_runIsBusy)
        return false;
    emit sendMessage(tr("正在启动调试"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在启动调试"));
    uint32_t startNum = 0;
    uint32_t endNum = 0;
    QFile file(firmwareDir + "/include/renux/renux_const.h");
    if(file.open(QIODevice::ReadOnly))
    {
        bool ok = false;
        QTextStream in(&file);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            if(line.contains("_CMD_START"))
            {
                QString str = line.trimmed().split(" ").last();
                startNum = str.toUInt();
                ok = true;
            }
            if(line.contains("_CMD_END"))
            {
                QString str = line.trimmed().split(" ").last();
                endNum = str.toUInt();
                ok = true;
            }
        }
        file.close();
        if(!ok)
        {
            emit sendMessage(tr("启动调试失败，文件错误"));
            CWaitDialog::getInstance()->finishShowing();
            return false;
        }
    }
    else
    {
        emit sendMessage(tr("启动调试失败，找不到文件"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }

    if(!m_hardwareDebug->debugStartDebug((uint)maxCycle, startNum, endNum))
    {
        emit sendMessage(tr("启动调试失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }
    m_isDebugging = true;
    CWaitDialog::getInstance()->finishShowing();
    return true;
}

bool CChipDebug::stopDebug()
{
    qInfo() << tr("硬件联调：停止调试");
    if(m_runIsBusy)
        return false;
    emit sendMessage(tr("正在停止硬件联调"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在结束调试"));
    if(!m_hardwareDebug->debugExit())
    {
        emit sendMessage(tr("停止硬件联调失败"));
        CWaitDialog::getInstance()->finishShowing();
        return false;
    }
    if(!m_hardwareDebug->debugReset())
    {
        return false;
    }
    m_isDebugging = false;
    m_runIsBusy = false;
    m_pause = false;
    m_cfgDebugLog = nullptr;
    CWaitDialog::getInstance()->finishShowing();
    return true;
}

bool CChipDebug::runCycle(int cycle)
{
    qInfo() << tr("硬件联调：运行周期：%0").arg(QString::number(cycle));
    if(m_runIsBusy)
        return false;
    m_pause = false;
    if(cycle < 0)
        return false;
    if(cycle == 0)
        return true;
    emit sendMessage(tr("向后运行%0周期").arg(cycle));
    if(m_cfgDebugLog->getBreakPointList()->count() > 0 || m_saveWave)//是否存在断点
    {
        m_runIsBusy = true;
        emit sendEnableRunPause(m_runIsBusy);
        while(cycle)
        {
            if(m_pause)
            {
                m_runIsBusy = false;
                emit sendEnableRunPause(m_runIsBusy);
                return true;
            }
            if(!m_hardwareDebug->debugRunCycle(1))
            {
                m_runIsBusy = false;
                emit sendEnableRunPause(m_runIsBusy);
                return false;
            }
            ++m_curCycle;
            emit sendCurCycle(m_curCycle);
            emit sendPeValue(getAllPEValue());
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            if(m_cfgDebugLog->isIntoBreakpoint())//是否遇到断点
            {
                m_runIsBusy = false;
                emit sendEnableRunPause(m_runIsBusy);
                return true;
            }
            --cycle;
        }
        m_runIsBusy = false;
        emit sendEnableRunPause(m_runIsBusy);
    }
    else
    {
        if(!m_hardwareDebug->debugRunCycle(cycle))
        {
            return false;
        }
        m_curCycle += cycle;
        emit sendCurCycle(m_curCycle);
        emit sendPeValue(getAllPEValue());
    }
    return true;
}

bool CChipDebug::runWithBreak()
{
    qInfo() << tr("硬件联调：运行，遇断点停止");
    if(m_runIsBusy)
        return false;
    m_pause = false;
    emit sendMessage(tr("运行，遇断点停止"));
    emit sendEnableRunPause(true);

    while(true)
    {
        if(m_pause)
        {
            emit sendEnableRunPause(false);
            return true;
        }
        if(!m_hardwareDebug->debugRunCycle(1))
        {
            return false;
        }
        ++m_curCycle;
        emit sendCurCycle(m_curCycle);
        emit sendPeValue(getAllPEValue());
        QApplication::processEvents(QEventLoop::AllEvents);
        if(m_cfgDebugLog->isIntoBreakpoint())//是否遇到断点
        {
            return true;
        }
    }
}

void CChipDebug::parseModelChanged(int index)
{
    qInfo() << tr("硬件联调：切换解析模型");
    if(m_runIsBusy)
        return;
    emit sendMessage(tr("切换解析模型"));
    m_curPeModelIndex = index;
    emit sendPeValue(getAllPEValue());
}

void CChipDebug::modifyPEValue(const QString &element, const QStringList &input, QStringList output)
{
    qInfo() << tr("硬件联调：修改PE数据");
    Q_UNUSED(input);
    if(m_runIsBusy)
        return;
    SPe pe;
    QStringList elementIdList = element.split(",");
    if(elementIdList.count() != 4)
        return;

    //记录到session
    SPeValue peValue = {m_curCycle, element, input, output};
    m_debugSession->addModifyPeValue(peValue);

    pe.bcu_id = QString(elementIdList.at(1)).toUInt();
    pe.rcu_id = QString(elementIdList.at(2)).toUInt();
    pe.pe_id = QString(elementIdList.at(3)).toUInt();
    while(output.count() < 4)
    {
        output.append("0");
    }
    for (int i = 0; i < 4; ++i) {
        QString str = output.at(i);
        bool ok;
        pe.data[i] = str.toUInt(&ok, 16);
    }

    emit sendMessage(tr("修改PE数据"));
    if(m_hardwareDebug->debugWritePe(&pe))
    {
        m_peModelList.at(m_curPeModelIndex)->updatePeValue(pe.bcu_id, pe.rcu_id, pe.pe_id, pe.data);
    }
    else
    {
        emit sendMessage(tr("修改PE数据失败"));
    }
//    emit sendPeValue(m_peModelList.at(m_curPeModelIndex)->getAllValue());
    emit sendPeValue(getAllPEValue());
}

bool CChipDebug::getPEValue()
{
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在获取PE数据"));
    SPe pe;
    pe.bcu_id = 0;
    pe.rcu_id = 0;
    pe.pe_id = 0;
    emit sendMessage(tr("正在获取PE数据"));
    if(m_hardwareDebug->debugReadPe(&pe))
    {
        //TODO do something
        DBG << QString::number(pe.data[0], 16)
                << QString::number(pe.data[1], 16)
                << QString::number(pe.data[2], 16)
                << QString::number(pe.data[3], 16);
        CWaitDialog::getInstance()->finishShowing();
        return true;
    }
    emit sendMessage(tr("获取PE数据失败"));
    CWaitDialog::getInstance()->finishShowing();
    return false;
}

bool CChipDebug::getOutFifoValue()
{
    emit sendMessage(tr("正在获取FIFO数据"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在获取FIFO数据"));
    for(int i = 2; i < 4; ++i)
    {
        uint32_t *fifoValue = nullptr;
        if(m_hardwareDebug->debugReadFifo(i, fifoValue))
        {
                //TODO do something
            printf("%x\n%x\n%x\n%x\n", fifoValue[0], fifoValue[1], fifoValue[2], fifoValue[3]);
            CWaitDialog::getInstance()->finishShowing();
            return true;
        }
        else
        {
            break;
        }
    }
    emit sendMessage(tr("获取FIFO数据失败"));
    CWaitDialog::getInstance()->finishShowing();
    return false;
}

bool CChipDebug::getOutFifoStatus()
{
    emit sendMessage(tr("正在获取FIFO状态"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在获取FIFO状态"));
    uint32_t status;
    if(m_hardwareDebug->debugReadFifoStatus(&status))
    {
        //TODO do something
        CWaitDialog::getInstance()->finishShowing();
        return true;
    }
    emit sendMessage(tr("获取FIFO状态失败"));
    CWaitDialog::getInstance()->finishShowing();
    return false;
}

QStringList CChipDebug::getAllPEValue()
{
    qInfo() << tr("硬件联调：获取所有PE数据");
    emit sendMessage(tr("正在获取所有PE数据"));
    CWaitDialog::getInstance()->readyToShowDlg();
    CWaitDialog::getInstance()->setText(tr(u8"正在获取PE数据"));
    QStringList peList;
    uint32_t *allPeValue = nullptr;
    if(m_hardwareDebug->debugReadAllPe(allPeValue))
    {
        savePeValueToWave(allPeValue);
        m_peModelList.at(m_curPeModelIndex)->refreshAllValue(allPeValue);
        CWaitDialog::getInstance()->finishShowing();
        return m_peModelList.at(m_curPeModelIndex)->getAllValue();
    }
    else
    {
        emit sendMessage(tr("获取所有PE数据失败"));
    }
    CWaitDialog::getInstance()->finishShowing();
    return peList;
}
