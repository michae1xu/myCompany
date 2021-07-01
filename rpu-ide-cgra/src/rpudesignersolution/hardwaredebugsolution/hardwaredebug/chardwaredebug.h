#ifndef CHARDWAREDEBUG_H
#define CHARDWAREDEBUG_H

#include <QObject>
#include <QMutex>
#include <QTime>
#include "hardwaredebug_global.h"
#ifdef __cplusplus
extern "C"{
#include "rcpsdk/include/api_debug.h"
#include "rcpsdk/include/dsc.h"
}
#endif

typedef pe_t SPe;

#if defined(Q_OS_WIN)
#define UNIXLIKEONLY
#endif

class CBuffer
{
public:
    CBuffer() : totalBufferSize(100001) {
        buffer = new uchar[totalBufferSize];
        memset(buffer, 0, totalBufferSize);
    }
    ~CBuffer() {
        delete []buffer;
    }
    void clearBuffer() {
        memset(buffer, 0, totalBufferSize);
        curBufferSize = 0;
    }

    uchar *buffer = nullptr;
    const int totalBufferSize;
    uint32_t curBufferSize = 0;
};

class HARDWAREDEBUGSHARED_EXPORT CHardwareDebug : public QObject
{
    Q_OBJECT
public:
    static CHardwareDebug *getInstance(QObject *parent);
    static void releaseInstance();

    static CHardwareDebug *m_instance;
    static QMutex m_mutex;
    static bool hasInstance() {return m_instance;}
    static QTime m_time;

    bool getIsConnected() const {return m_isConnected;}
    QString getBuffer1String();
    QString getBuffer2String();

signals:
    void deviceDisconnected();
    void deviceConnected(QString, QString);
    void sendMessage(QString);
    void sendErrorMessage(QString);
    void sendTimeOut();
    void sendPercent(int);

private:
//    QObject *m_parent;
    bool m_initSdk;
    bool m_isConnected;
    int32_t m_deviceId;

    CBuffer m_buffer1;
    CBuffer m_buffer2;

    explicit CHardwareDebug(QObject *parent);
    ~CHardwareDebug() = default;

    bool handleReturn(const int32_t &ret, QString msg = "");
    bool handleAsyncReturn(const int32_t &ret, QString msg = "");
    int32_t waitForFinished();
    QString ucharToQString(uchar *array);

public:
    bool initSdk();

    bool rcpGetSysInfo();
    bool rcpReadOtp(uint32_t addr, uint32_t size, uchar *buffer);
    bool rcpWriteOtp(uint32_t addr, uint32_t *data, uint32_t size);
    bool rcpReadSpi(uint32_t addr, uint32_t size);
    bool rcpWriteSpi(uint32_t addr, const QString &fileName);

    bool deviceOpen();
    bool deviceClose();
    bool deviceReset();
    bool devicePoll();
    bool deviceFirmwareVersion();
    bool deviceFirmwareUpdate(const QString &fileName);
    bool deviceFirmwareUpdateProgress(uint32_t *status);

    int32_t debugCmdStatus(uint32_t *status);
    bool debugReset();
    bool debugEnter();
    bool debugExit();
//    bool debugPrepareDebug();
    bool debugInitConfig(uint8_t cmdBitsLen, const QString &fileName);
    bool debugInitSboxData(const QString &sboxFileName, const QString &renuxFileName);
    bool debugWriteFifo(const int &fifoId, uint *data, const int &len);
    bool debugWriteGprf(const int &gprfId, const int &offset, uint *data, const uint &len);
    bool debugStartDebug(uint32_t nums, uint32_t startNum, uint32_t endNum);
    bool debugRunNext();
    bool debugRunCycle(const uint32_t &cycle);
    bool debugWritePe(SPe *pe);
    bool debugReadPe(SPe *pe);
    bool debugReadAllPe(uint32_t *&allPeValue);
    bool debugReadFifo(const int &fifoId, uint32_t *&fifoValue);
    bool debugReadFifoStatus(uint32_t *status);
    bool debugRcaStatus(uint32_t *status);
};

#endif // CHARDWAREDEBUG_H
