#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QFileInfo>
#include <QIODevice>
#include <math.h>
#include "chardwaredebug.h"
#if defined(Q_OS_UNIX)
#include <unistd.h>
#endif
#ifdef __cplusplus
extern "C"{
#include "rcpsdk/include/api_device.h"
#include "rcpsdk/include/api_comm.h"
}
#endif

CHardwareDebug* CHardwareDebug::m_instance = nullptr;
QMutex CHardwareDebug::m_mutex;
QTime CHardwareDebug::m_time;

CHardwareDebug::CHardwareDebug(QObject *parent) :
    QObject(parent),
//    m_parent(parent),
    m_initSdk(false),
    m_isConnected(false),
    m_deviceId(0)
{
}

CHardwareDebug *CHardwareDebug::getInstance(QObject *parent)
{
    if(m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if(m_instance == nullptr)
        {
            m_instance = new CHardwareDebug(parent);
        }
    }
    return m_instance;
}

void CHardwareDebug::releaseInstance()
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

QString CHardwareDebug::getBuffer1String()
{
    return QString::fromLatin1((char *)m_buffer1.buffer);
}

QString CHardwareDebug::getBuffer2String()
{
    return QString::fromLatin1((char *)m_buffer2.buffer);
}

QString CHardwareDebug::ucharToQString(uchar *array)
{
    QString str;
    for(size_t i = 0; i < strlen((char *)array); ++i)
    {
        QChar code(array[i]);
        str.append(code);
    }
    return str;
}

bool CHardwareDebug::handleReturn(const int32_t &ret, QString msg)
{
    switch ((DSC_STATUS)ret) {
    case DSC_SUCCESS:
        return true;
//        break;
    case DSC_INTERNAL_ERROR:
        emit sendErrorMessage(tr("%0失败！").arg(msg));
        break;
    case DSC_INVALID_PARAMETER:
        emit sendErrorMessage(tr("%0参数不正确！").arg(msg));
        break;
    case DSC_TIMEOUT_ERROR:
        emit sendErrorMessage(tr("%0超时！").arg(msg));
        break;
    case DSC_DEVICE_NOT_FOUND:
        emit sendErrorMessage(tr("没有找到可执行的设备！"));
        break;
    default:
        emit sendErrorMessage(tr("错误！"));
        break;
    }
    return false;
}

bool CHardwareDebug::handleAsyncReturn(const int32_t &ret, QString msg)
{
    if(handleReturn(ret, msg))
        return handleReturn(waitForFinished(), msg);
    else
        return false;
}

int32_t CHardwareDebug::waitForFinished()
{
    uint32_t status;
    int32_t ret = debugCmdStatus(&status);
    m_time.start();
    while(!status)
    {
        if(m_time.elapsed() > 10000)  //zhangjun modified at 20180817
        {
            ret = DSC_TIMEOUT_ERROR;
            break;
        }
        QApplication::processEvents(/*QEventLoop::ExcludeUserInputEvents*/);
        ret = debugCmdStatus(&status);
    }
    return ret;
}

//#if defined(Q_OS_UNIX)
bool CHardwareDebug::initSdk()
{
#ifdef NODEVICE
    return true;
#else
    int32_t ret = rcp_rpu_dbg_init();
    m_initSdk = handleReturn(ret, tr("初始化SDK"));
    return m_initSdk;
#endif
}

bool CHardwareDebug::rcpGetSysInfo()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    m_buffer1.clearBuffer();
    int32_t ret = rcp_get_sys_info((void *)m_buffer1.buffer);
    return handleReturn(ret, tr("获取设备信息"));
#endif
}

bool CHardwareDebug::rcpReadOtp(uint32_t addr, uint32_t size, uchar *buffer)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    const uint32_t onceSize = 4;
    uint32_t readedSize = 0;
    m_buffer1.clearBuffer();
    while(readedSize < size)
    {
        int32_t ret = dsc_read_otp(addr + readedSize*8, (uint32_t*)(buffer + readedSize), onceSize/4);
        if(!handleReturn(ret, tr("读取OTP数据！")))
            return false;
        readedSize += onceSize;
//        int percent = ceil((float)readedSize/size*100);
//        emit sendPercent(percent > 100 ? 100 : percent);
    }
#endif
    return true;
}

bool CHardwareDebug::rcpWriteOtp(uint32_t addr, uint32_t *data, uint32_t size)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    const uint32_t onceSize = 4;
    uint32_t writtenSize = 0;
    while(writtenSize < size)
    {
        int32_t ret = dsc_write_otp(addr + writtenSize*8, data + writtenSize/sizeof(uint32_t), onceSize/4);
        if(!handleReturn(ret, tr("写入OTP数据！")))
            return false;
        if(!rcpReadOtp(addr + writtenSize*8, onceSize, m_buffer1.buffer))
            return handleReturn(DSC_INTERNAL_ERROR, tr("读取OTP数据错误！"));
        if(0 != memcmp(m_buffer1.buffer, (uchar*)(data + writtenSize/sizeof(uint32_t)), onceSize))
            return handleReturn(DSC_INTERNAL_ERROR, tr("数据比对错误！"));
        writtenSize += onceSize;
        int percent = ceil((float)writtenSize/size*100);
        emit sendPercent(percent > 100 ? 100 : percent);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
#endif
    return true;
}

bool CHardwareDebug::rcpReadSpi(uint32_t addr, uint32_t size)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(size > 100000)
        return handleReturn(DSC_INVALID_PARAMETER, tr("读取SPI数据越限！"));

    const uint32_t onceSize = 16 * 1024;
    uint32_t readedSize = 0;
    m_buffer1.clearBuffer();
    while(readedSize < size)
    {
        int32_t ret = dsc_read_spi(addr + readedSize, m_buffer1.buffer + readedSize, onceSize);
        if(!handleReturn(ret, tr("读取SPI数据！")))
            return false;
        readedSize += onceSize;
//        int percent = ceil((float)readedSize/size*100);
//        emit sendPercent(percent > 100 ? 100 : percent);
    }
#endif
    return true;
}

bool CHardwareDebug::rcpWriteSpi(uint32_t addr, const QString &fileName)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(!QFileInfo(fileName).isFile() || !QFileInfo(fileName).isReadable())
    {
        emit sendErrorMessage(tr("文件不存在或不可读！"));
        return false;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        emit sendErrorMessage(tr("打开文件失败！"));
        return false;
    }
    uint32_t totalSize = (uint32_t)file.size();
    QByteArray array = file.readAll();
    char* data = array.data();

    for(int i = 0; i < 2; ++i)
    {
        const uint32_t onceSize = 16 * 1024;
        uint32_t writtenSize = 0;
        uint32_t offset = 0;
        if(i == 0)
        {
            offset = totalSize / 2;
        }
        totalSize = totalSize / 2;
        while(writtenSize < totalSize)
        {
            int32_t ret = dsc_write_spi(addr + offset + writtenSize, (uchar*)(data + offset + writtenSize), onceSize);
#if defined(Q_OS_UNIX)
            usleep(5000);
#endif
            if(!handleReturn(ret, tr("写入SPI数据！")))
                return false;
            if(!rcpReadSpi(addr + offset + writtenSize, onceSize))
                return handleReturn(DSC_INTERNAL_ERROR, tr("读取SPI数据错误！"));
            if(0 != memcmp(m_buffer1.buffer, (uchar*)(data + offset + writtenSize), onceSize))
                return handleReturn(DSC_INTERNAL_ERROR, tr("数据比对错误！"));
            writtenSize += onceSize;

            int percent = ceil((float)writtenSize/totalSize*100);
            percent /= 2;
            if(i == 1)
                percent += 50;
            qDebug() << QString::number(writtenSize, 16) << QString::number(totalSize, 16)
                     << percent;
            emit sendPercent(percent > 100 ? 100 : percent);
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
#endif
    return true;
}

//device
bool CHardwareDebug::deviceOpen()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(!m_isConnected)
    {
        int32_t ret = rcp_device_open(&m_deviceId);
        m_isConnected = handleReturn(ret, tr("打开设备"));
        if(!m_isConnected)
        {
            return false;
        }
    }

    QString deviceInfo;
    QString firmwareVersion;
    if(devicePoll())
    {
        deviceInfo = getBuffer1String();
    }
    if(deviceFirmwareVersion())
    {
        firmwareVersion = getBuffer1String();
    }
    emit deviceConnected(deviceInfo, firmwareVersion);
    return m_isConnected;
#endif
}

bool CHardwareDebug::deviceClose()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(m_isConnected)
    {
        int32_t ret = rcp_device_close(m_deviceId);
        m_isConnected = !handleReturn(ret, tr("关闭设备"));
        if(m_isConnected)
        {
            return false;
        }
    }
    emit deviceDisconnected();
#endif
    return true;
}

bool CHardwareDebug::deviceReset()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_device_reset(m_deviceId);
    return handleReturn(ret, tr("重置设备"));
#endif
}

bool CHardwareDebug::devicePoll()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    m_buffer1.clearBuffer();
    int32_t ret = rcp_device_poll((uchar *)m_buffer1.buffer, &m_buffer1.curBufferSize);
    return handleReturn(ret, tr("获取设备信息"));
#endif
}

bool CHardwareDebug::deviceFirmwareVersion()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    m_buffer1.clearBuffer();
    int32_t ret = rcp_device_firmware_version((uchar *)m_buffer1.buffer, &m_buffer1.curBufferSize);
    return handleReturn(ret, tr("获取设备版本信息"));
#endif
}

bool CHardwareDebug::deviceFirmwareUpdate(const QString &fileName)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(!QFileInfo(fileName).isFile() || !QFileInfo(fileName).isReadable())
    {
        emit sendErrorMessage(tr("文件不存在或不可读！"));
        return false;
    }
    m_buffer1.clearBuffer();
    memcpy(m_buffer1.buffer, fileName.toLocal8Bit().data(), fileName.toLocal8Bit().size() + 1);
    int32_t ret = rcp_device_update_firmware((char *)m_buffer1.buffer);
    return handleReturn(ret, tr("更新设备固件"));
#endif
}

bool CHardwareDebug::deviceFirmwareUpdateProgress(uint32_t *status)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_device_update_progress_query(status);
    return (DSC_SUCCESS == ret);
#endif
}

//debug
int32_t CHardwareDebug::debugCmdStatus(uint32_t *status)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    return rcp_rpu_dbg_read_cmd_status(status);
#endif
}

bool CHardwareDebug::debugReset()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_reset();
    return handleAsyncReturn(ret, tr("重置设备"));
#endif
}

bool CHardwareDebug::debugEnter()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_enter();
    return handleAsyncReturn(ret, tr("进入调试"));
#endif
}

bool CHardwareDebug::debugExit()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_exit();
    return handleAsyncReturn(ret, tr("退出调试"));
#endif
}

//bool CHardwareDebug::debugPrepareDebug()
//{
//    int32_t ret = debugReset();
//    return handleAsyncReturn(ret, tr("进入调试模式"));
//}

bool CHardwareDebug::debugInitConfig(const uint8_t cmdBitsLen, const QString &fileName)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(!QFileInfo(fileName).isFile() || !QFileInfo(fileName).isReadable())
    {
        emit sendErrorMessage(tr("文件不存在或不可读！"));
        return false;
    }
    m_buffer1.clearBuffer();
    memcpy(m_buffer1.buffer, fileName.toLocal8Bit().data(), fileName.toLocal8Bit().size() + 1);
    int32_t ret = rcp_rpu_dbg_init_config(cmdBitsLen, (char *)m_buffer1.buffer);
    return handleAsyncReturn(ret, tr("调试模式：初始化设备配置"));
#endif
}

bool CHardwareDebug::debugInitSboxData(const QString &sboxFileName, const QString &renuxFileName)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    if(!QFileInfo(sboxFileName).isFile() || !QFileInfo(sboxFileName).isReadable())
    {
        emit sendErrorMessage(tr("%0文件不存在或不可读！").arg(sboxFileName));
        return false;
    }
    if(!QFileInfo(renuxFileName).isFile() || !QFileInfo(renuxFileName).isReadable())
    {
        emit sendErrorMessage(tr("%0文件不存在或不可读！").arg(renuxFileName));
        return false;
    }
    m_buffer1.clearBuffer();
    memcpy(m_buffer1.buffer, sboxFileName.toLocal8Bit().data(), sboxFileName.toLocal8Bit().size() + 1);
    m_buffer2.clearBuffer();
    memcpy(m_buffer2.buffer, renuxFileName.toLocal8Bit().data(), renuxFileName.toLocal8Bit().size() + 1);
    int32_t ret = rcp_rpu_dbg_init_sbox((char *)m_buffer1.buffer, (char *)m_buffer2.buffer);
    return handleAsyncReturn(ret, tr("调试模式：初始化设备SBOX"));
#endif
}

bool CHardwareDebug::debugWriteFifo(const int &fifoId, uint *data, const int &len)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
//    m_buffer1.clearBuffer();
//    memcpy(m_buffer1.buffer, bty.data(), bty.size());
    int32_t ret = rcp_rpu_dbg_write_fifo((uint8_t)fifoId, (uchar *)data, len);
    return handleAsyncReturn(ret, tr(u8"调试模式：写入Fifo数据"));
#endif
}

bool CHardwareDebug::debugWriteGprf(const int &gprfId, const int &offset, uint *data, const uint &len)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_write_gprf((uint8_t)gprfId, (uint16_t)offset, (uchar *)data, len);
    return handleAsyncReturn(ret, tr(u8"调试模式：写入GPRF数据"));
#endif
}

bool CHardwareDebug::debugStartDebug(uint32_t nums, uint32_t startNum, uint32_t endNum)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_start(nums, startNum, endNum);
    return handleAsyncReturn(ret, tr(u8"进入调试模式"));
#endif
}

bool CHardwareDebug::debugRunNext()
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_run(1);
    return handleAsyncReturn(ret, tr(u8"调试模式：单步运行"));
#endif
}

bool CHardwareDebug::debugRunCycle(const uint32_t &cycle)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_run(cycle);
    return handleAsyncReturn(ret, tr(u8"调试模式：运行"));
#endif
}

bool CHardwareDebug::debugWritePe(SPe *pe)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_write_pe(pe);
    return handleAsyncReturn(ret, tr(u8"调试模式：修改PE值"));
#endif
}

bool CHardwareDebug::debugReadPe(SPe *pe)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_read_pe(pe);
    return handleAsyncReturn(ret, tr(u8"调试模式：读取PE值"));
#endif
}

bool CHardwareDebug::debugReadAllPe(uint32_t *&allPeValue)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    m_buffer1.clearBuffer();
    m_buffer1.curBufferSize = 832;
    for(int i = 0; i < 4; ++i)
    {
        int32_t ret = rcp_rpu_dbg_read_bcu_pe((uint32_t)i, (uchar *)(m_buffer1.buffer + m_buffer1.curBufferSize * i), m_buffer1.curBufferSize);
        if(!handleAsyncReturn(ret, tr(u8"调试模式：读取PE值")))
            return false;
    }
    allPeValue = (uint32_t *)m_buffer1.buffer;
    return true;
#endif
}

bool CHardwareDebug::debugReadFifo(const int &fifoId, uint32_t *&fifoValue)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    m_buffer1.clearBuffer();
    m_buffer1.curBufferSize = 16; //fifo max size
    int32_t ret = rcp_rpu_dbg_read_fifo((uint8_t)fifoId, (uchar *)m_buffer1.buffer, m_buffer1.curBufferSize);
    if(!handleAsyncReturn(ret, tr(u8"调试模式：读取Fifo值")))
        return false;
//    *bty = bty->fromRawData((char *)m_buffer1.buffer, m_buffer1.curBufferSize);
    fifoValue = (uint32_t *)m_buffer1.buffer;
    return true;
#endif
}

bool CHardwareDebug::debugReadFifoStatus(uint32_t *status)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_read_fifo_status(status);
    return handleAsyncReturn(ret, tr(u8"调试模式：读取Fifo状态"));
#endif
}

bool CHardwareDebug::debugRcaStatus(uint32_t *status)
{
#if (defined UNIXLIKE) || (defined NODEVICE)
    return true;
#else
    int32_t ret = rcp_rpu_dbg_read_rca_status(status);
    return handleAsyncReturn(ret, tr(u8"调试模式：读取RCA状态"));
#endif
}

//#endif
