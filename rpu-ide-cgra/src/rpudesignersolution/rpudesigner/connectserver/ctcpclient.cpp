#include <QDataStream>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QHostInfo>
#include <QTimer>
#include <QSslCipher>
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "ctcpclient.h"
#include "version.h"

CTcpClient::CTcpClient(const QString &curVersion, QObject *parent)
    : QThread(parent),
      m_curVersion(curVersion)
{
#if defined(Q_OS_WIN)
    m_destIp = "192.168.31.196";
#else
    if(QHostInfo::localHostName().startsWith("wds") || QHostInfo::localHostName().startsWith("sds"))
        m_destIp = "wds020";
    else
        m_destIp = "192.168.21.20";
#endif

    m_destPort = 10202;
    m_isRegisted = false;
    m_nextBlockSize = 0;
    m_userName = QFileInfo(QDir::homePath()).completeBaseName();
    moveToThread(this);
    qInfo() << tr(u8"当前设备核心数：%0").arg(QThread::idealThreadCount());
}

CTcpClient::~CTcpClient()
{
    qInfo() << tr("断开服务器连接");
}

void CTcpClient::setIpPort(const QString &destIp, const quint16 &destPort)
{
    m_destIp = destIp;
    m_destPort = destPort;
}

void CTcpClient::run()
{
    m_connectTimer = new QTimer(this);
    m_registTimer = new QTimer(this);
    m_connectTimer->setInterval(30000);
    m_registTimer->setInterval(180000);
    connect(m_registTimer, SIGNAL(timeout()), this, SLOT(registToServer()));
    connect(m_connectTimer, SIGNAL(timeout()), this, SLOT(connectToServer()));
    m_registTimer->start();

    m_sslSocket = new QSslSocket();
    m_sslSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    m_sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
//    m_sslSocket->addCaCertificates(QSslCertificate::fromPath(":/ca/root.crt"));
//    m_sslSocket->ignoreSslErrors();

    connect(this, SIGNAL(finished()), this, SLOT(deleteSocket()), Qt::DirectConnection);
    connect(m_sslSocket, SIGNAL(connected()), this, SLOT(registToServer()), Qt::DirectConnection);
    connect(m_sslSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()), Qt::DirectConnection);
    connect(m_sslSocket, SIGNAL(readyRead()), this, SLOT(inCommingData()), Qt::DirectConnection);
    connect(m_sslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()), Qt::DirectConnection);
    connect(m_sslSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(m_sslSocket, SIGNAL(encrypted()),
            this, SLOT(socketEncrypted()));
    connect(m_sslSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(m_sslSocket, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(sslErrors(QList<QSslError>)));
    connect(m_sslSocket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(updateProgress(qint64)));

    connectToServer();
    exec();
}

void CTcpClient::connectToServer()
{
    m_sslSocket->setProxy(QNetworkProxy::applicationProxy());
    m_sslSocket->connectToHostEncrypted(m_destIp, m_destPort);
    if(m_sslSocket->waitForConnected(3000))
    {
        qInfo() << tr(u8"连接到服务器成功");
        m_connectTimer->stop();
    }
    else
    {
        qInfo() << tr(u8"连接到服务器失败, 30秒后再次尝试");
        m_connectTimer->start();
    }
}


void CTcpClient::registToServer()
{
    if(m_isRegisted || m_sslSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }
    qInfo() << tr(u8"正在注册到服务器……");
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    generateHeader(srRegistSyn, out);
    //包大小 << 发送原因 << 序列号 << 产品名称 << 版本信息 << 主机名 << 用户名 << MAC地址 <<平台名
    QString license = CAppEnv::m_license;
    if(license.isEmpty())
        license = "tsinghuarpudesigner";
#if defined(Q_OS_WIN)
    out << license << QString(VER_PRODUCTNAME_STR) << qApp->applicationVersion()
        << QHostInfo::localHostName() << m_userName << CAppEnv::getMacAddr() << QString("windows");
#elif defined(Q_OS_UNIX)
    out << license << QString("rpu designer") << qApp->applicationVersion()
        << QHostInfo::localHostName() << m_userName << CAppEnv::getMacAddr() << QString("linux");
#endif
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_sslSocket->write(block);
    m_sslSocket->flush();
    m_nextBlockSize = 0;
}

void CTcpClient::inCommingData()
{
    QDataStream in(m_sslSocket);
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

    if(m_receive_startDownloading)
    {
        float useTime = m_time.elapsed();
        // 如果接收到的数据小于16个字节，保存到来的文件头结构
        if (m_receive_bytesReceived <= (qint64)sizeof(qint64)*2) {
            if((m_sslSocket->bytesAvailable() >= (qint64)sizeof(qint64)*2)
                    && (m_receive_fileNameSize == 0)) {
                // 接收数据总大小信息和文件名大小信息
                in >> m_receive_totalBytes >> m_receive_fileNameSize;
                m_receive_bytesReceived += sizeof(qint64) * 2;
            }
            if((m_sslSocket->bytesAvailable() >= m_receive_fileNameSize)
                    && (m_receive_fileNameSize != 0)) {
                // 接收文件名，并建立文件
                in >> m_receive_fileName;
                m_receive_fileName = CAppEnv::getTmpDirPath() + "/" + m_receive_fileName;
                m_receive_bytesReceived += m_receive_fileNameSize;
                m_receive_localFile = new QFile(m_receive_fileName, this);
                if (!m_receive_localFile->open(QIODevice::WriteOnly)) {
                    m_receive_startDownloading = false;
                    delete m_receive_localFile;
                    m_receive_localFile = nullptr;
                    return;
                }
            } else {
                return;
            }
        }
        // 如果接收的数据小于总数据，那么写入文件
        if (m_receive_bytesReceived < m_receive_totalBytes) {
            m_receive_bytesReceived += m_sslSocket->bytesAvailable();
            m_receive_inBlock = m_sslSocket->readAll();
            m_receive_localFile->write(m_receive_inBlock);
            m_receive_inBlock.resize(0);
        }

        qreal percent = qreal(m_receive_bytesReceived) / m_receive_totalBytes;
        int range = 100;
        percent *= range;
        DBG << "已完成：" << percent;
        emit downloadSharedFilePercent(percent);
        double speed = m_receive_bytesReceived / useTime;
        DBG << tr(u8"已接收 %0MB (%1MB/s) \n共%2MB 已用时:%3秒\n估计剩余时间：%4秒")
                                       .arg(m_receive_bytesReceived / (1024*1024))//已接收
                                       .arg(speed*1000/(1024*1024),0,'f',2)//速度
                                       .arg(m_receive_totalBytes / (1024 * 1024))//总大小
                                       .arg(useTime/1000,0,'f',0)//用时
                                       .arg(m_receive_totalBytes/speed/1000 - useTime/1000,0,'f',0);//剩余时间

        // 接收数据完成时
        if (m_receive_bytesReceived == m_receive_totalBytes) {
            if(m_receive_localFile)
            {
                if(m_receive_localFile->isOpen())
                    m_receive_localFile->close();
                delete m_receive_localFile;
                m_receive_localFile = nullptr;
            }
            m_receive_startDownloading = false;
            DBG << tr(u8"正在校验");
            QFile file(m_receive_fileName);
            if(!file.open(QIODevice::ReadOnly))
            {
                DBG << tr(u8"校验失败，不能打开文件");
                return;
            }
            const QByteArray &allContent = file.readAll();
            file.close();
            const QString &localFileMD5 = QCryptographicHash::hash(allContent, QCryptographicHash::Md5).toHex().toUpper();
            if(!m_receive_shareFileMD5.isEmpty() && m_receive_shareFileMD5 == localFileMD5)
            {
                DBG << tr(u8"收到文件") << m_receive_fileName;
                emit downloadSharedFileFinish(m_receive_fileName, true);
            }
            else
            {
                DBG << tr(u8"校验失败") << m_receive_shareFileMD5 << localFileMD5;
                emit downloadSharedFileFinish(m_receive_fileName, false);
            }
        }
        return;
    }

    if (m_nextBlockSize == 0)
    {
        if (m_sslSocket->bytesAvailable() < (qint64)sizeof(quint16))
            return;
        in >> m_nextBlockSize;
    }

    if (m_sslSocket->bytesAvailable() < m_nextBlockSize)
    {
        return;
    }
    //包大小 << 发送原因
    quint32 sendReason;
    in >> sendReason;

    switch (sendReason) {
    case srRegistAck:
    {
        QString str;
        QString lastVersion;
        in >> str >> lastVersion;
        qInfo() << tr(u8"收到服务器注册反馈：%0").arg(str);
        m_isRegisted = (str == "1");
        emit connectStatusChanged(m_isRegisted);
        if(m_isRegisted)
        {
            if(srUpdateAcceptAck == checkVersion(lastVersion)) {
                emit versionAck(true, lastVersion);
            }
        }
    }
        break;
    case srVersionSyn:

        break;
    case srVersionAck:
    {
        QString lastVersion;
        in >> lastVersion;
        qInfo() << tr(u8"收到服务器版本信息反馈：%0").arg(lastVersion);
        switch (checkVersion(lastVersion)) {
        case srUpdateAcceptAck:
            emit versionAck(true, lastVersion);
            break;
        case srUpdateRejectAck:
            emit versionAck(false, tr(u8"版本信息有误！"));
            break;
        case srNoNeedUpdateAck:
            emit versionAck(false, tr(u8"当前已是最新版本！"));
            break;
        default:
            break;
        }
    }
        break;
    case srUpdateSyn:

        break;
    case srShowMessage:
    {
        QString str;
        in >> str;
        qInfo() << tr(u8"收到服务器弹窗消息：%0").arg(str);
        emit showMessage(tr(u8"服务器消息：%0").arg(str));
    }
        break;
    case srShowNotify:
    {
        QString str; 
        in >> str;
        qInfo() << tr(u8"收到服务器托盘消息：%0").arg(str);
        emit showNotify(tr(u8"服务器消息：%0").arg(str));
    }
        break;
    case srClientUpdate:
    {
        QString lastVersion;
        in >> lastVersion;
        qInfo() << tr(u8"收到服务器要求升级：%0").arg(lastVersion);
        if(srUpdateAcceptAck == checkVersion(lastVersion)) {
            emit versionAck(true, lastVersion);
        }
    }
        break;
    case srCloseClient:
    {
        QString msg;
        in >> msg;
        qInfo() << tr(u8"服务器要求退出程序：%0").arg(msg);
        emit serverCloseProgram(msg);
    }
        break;
    case srKeepAlive:
    {
        QString heartbeat;
        in >> heartbeat;
        qInfo() << tr(u8"服务器心跳帧：%0").arg(heartbeat);
    }
        break;
    case srShareFileAck:
    {
        int ret = -1;
        in >> ret;
        if(ret) {
            qInfo() << tr(u8"服务器分享文件应答：accept");
            m_send_startTransfer = true;
            startShareSolution();
        }
        else {
            qInfo() << tr(u8"服务器分享文件应答：reject");
        }
    }
        break;
    case srShareFileListOld:
    {
        QString ret;
        in >> ret;
        qInfo() << tr(u8"收到服务器消息：分享文件数据已旧");
        emit sharedFileListIsOld();
    }
        break;
    case srShareFileListAck:
    {
        QString fileName;
        QString userName;
        QString dateTime;
        in >> fileName >> userName >> dateTime;
        qInfo() << tr(u8"收到服务器发送的分享文件信息：%1").arg(fileName);
        emit addSharedFile(fileName, userName, dateTime);
    }
        break;
    case srDownloadShareFileAck:
    {
        QString mesg, ret;
        in >> mesg >> ret;
        if("1" == ret)
        {
            qInfo() << tr(u8"服务器响应下载共享文件请求：accept");
            m_receive_totalBytes = 0;     // 存放总大小信息
            m_receive_bytesReceived = 0;  // 已收到数据的大小
            m_receive_fileNameSize = 0;   // 文件名的大小信息
            m_receive_inBlock.clear();    // 数据缓冲区
            m_receive_localFile = nullptr;      // 本地文件
            m_receive_shareFileMD5 = mesg;
            m_receive_startDownloading = true;
            m_time.start();
            sendData(srDownloadShareFileReady, 0);
        }
        else
        {
            qInfo() << tr(u8"服务器响应下载共享文件请求：reject");
            emit serverRejectDownloadSharedFile(mesg);
        }
    }
        break;
    default:
        break;
    }
    m_sslSocket->readAll();
    m_nextBlockSize = 0;
}

void CTcpClient::sendData(ESendReason reason, const QByteArray &array)
{
    qInfo() << tr(u8"发送数据包到服务器");
    QByteArray block;
    QDataStream stream(&block, QIODevice::WriteOnly);
    generateHeader(reason, stream);
    stream << array;
    stream.device()->seek(0);
    stream << quint16(block.size() - sizeof(quint16));
    if(-1 == m_sslSocket->write(block))
    {
        qInfo() << tr(u8"发送数据包到服务器出错");
    }
    m_sslSocket->flush();
}

void CTcpClient::generateHeader(ESendReason reason, QDataStream &stream)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    stream.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    stream.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    stream.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    stream.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    stream.setVersion(QDataStream::Qt_5_6);
#endif
    stream << quint16(0) << quint32(reason);
}

void CTcpClient::shareSolutionRequest(QString fileName)
{
    if(!m_isRegisted || m_sslSocket->state() != QAbstractSocket::ConnectedState)
        return;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return;
    const QByteArray &allContent = file.readAll();
    file.close();
    m_send_fileName = fileName;
    const QString &md5Code = QCryptographicHash::hash(allContent, QCryptographicHash::Md5).toHex().toUpper();

    qInfo() << tr(u8"发送分享文件请求");
    DBG << tr(u8"发送分享文件请求") << md5Code;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    generateHeader(srShareFileSyn, out);
    out << md5Code;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_sslSocket->write(block);
    m_sslSocket->flush();
    m_nextBlockSize = 0;
}

void CTcpClient::versionRequest()
{
    if(!m_isRegisted || m_sslSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }
    qInfo() << tr(u8"检查是否有可用的更新");
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    generateHeader(srVersionSyn, out);
    //包大小 << 发送原因 << 序列号 << 版本信息
    out << 0;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_sslSocket->write(block);
    m_sslSocket->flush();
    m_nextBlockSize = 0;
}

void CTcpClient::licenseChanged()
{
    m_isRegisted = false;
    registToServer();
}

void CTcpClient::clientRequest()
{
    DBG << "TODO";
}

void CTcpClient::sharedFileListRequest()
{
    if(!m_isRegisted || m_sslSocket->state() != QAbstractSocket::ConnectedState)
    {
        sendClientDialogError(tr("未连接到服务器"));
        return;
    }
    qInfo() << tr(u8"获取分享文件列表");
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    generateHeader(srShareFileListSyn, out);
    //包大小 << 发送原因 << 序列号 << 版本信息
    out << 0;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_sslSocket->write(block);
    m_sslSocket->flush();
    m_nextBlockSize = 0;
}

void CTcpClient::sharedFileDownloadRequest(QString fileName, QString userName)
{
    if(!m_isRegisted || m_sslSocket->state() != QAbstractSocket::ConnectedState)
    {
        sendClientDialogError(tr("未连接到服务器"));
        return;
    }
    qInfo() << tr(u8"请求下载文件：%1").arg(fileName);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    generateHeader(srDownloadShareFileSyn, out);
    out << fileName << userName;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_sslSocket->write(block);
    m_sslSocket->flush();
    m_nextBlockSize = 0;
}


void CTcpClient::connectionClosedByServer()
{
    qInfo() << tr(u8"服务器断开了连接");
    closeConnection();
}

void CTcpClient::error()
{
    qInfo() << tr(u8"连接到服务器出错：%0").arg(m_sslSocket->errorString());
    closeConnection();
}

void CTcpClient::closeConnection()
{
    m_sslSocket->close();
    m_isRegisted = false;
    m_connectTimer->start();
    emit connectStatusChanged(m_isRegisted);
}

void CTcpClient::deleteSocket()
{
    m_sslSocket->abort();
    m_sslSocket->close();
    m_sslSocket->deleteLater();
    delete m_registTimer;
    delete m_connectTimer;
}

ESendReason CTcpClient::checkVersion(const QString &lastVersion) const
{
    QStringList strList = lastVersion.split(" ");
    strList = QString(strList.at(0)).split(".");
    if(strList.count() != 3)
        return srUpdateRejectAck;

    bool ok1, ok2, ok3;
    int first = QString(strList.at(0)).toInt(&ok1);
    int second = QString(strList.at(1)).toInt(&ok2);
    int third = QString(strList.at(2)).toInt(&ok3);
    if(!ok1 || !ok2 || !ok3)
    {
        return srUpdateRejectAck;
    }

    QStringList dataList = m_curVersion.trimmed().split(".");
    if(dataList.count() != 3)
        return srUpdateRejectAck;

    int a = QString(dataList.at(0)).toInt(&ok1);
    int b = QString(dataList.at(1)).toInt(&ok2);
    int c = QString(dataList.at(2)).toInt(&ok3);
    if(!ok1 || !ok2 || !ok3)
    {
        return srUpdateRejectAck;
    }

    if(a < first)
    {
        return srUpdateAcceptAck;
    }
    else if(first == a)
    {
        if(b < second)
        {
            return srUpdateAcceptAck;
        }
        else if(second == b && c < third)
        {
            return srUpdateAcceptAck;
        }
    }
    return srNoNeedUpdateAck;
}

void CTcpClient::startShareSolution()
{
    qInfo() << tr("发送共享解决方案到服务器");
    m_send_totalBytes = 0;  // 发送数据的总大小
    m_send_bytesWritten = 0;  // 已经发送数据大小
    m_send_bytesToWrite = 0;   // 剩余数据大小
    m_send_outBlock.resize(0);  // 数据缓冲区，即存放每次要发送的数据块
    m_send_localFile = new QFile(m_send_fileName);
    if (!m_send_localFile->open(QFile::ReadOnly))
    {
        delete m_send_localFile;
        m_send_localFile = nullptr;
        m_send_startTransfer = false;
        return;
    }

    QTimer::singleShot(180000, this, SLOT(finishShareSolution()));

    // 获取文件大小
    m_send_totalBytes = m_send_localFile->size();

    QDataStream sendOut(&m_send_outBlock, QIODevice::WriteOnly);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    sendOut.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    sendOut.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    sendOut.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    sendOut.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    sendOut.setVersion(QDataStream::Qt_5_6);
#endif
    QString currentFileName = QFileInfo(m_send_fileName).fileName();
    // 保留总大小信息空间、文件名大小信息空间，然后输入文件名
    sendOut << qint64(0) << qint64(0) << currentFileName;
    // 这里的总大小是总大小信息、文件名大小信息、文件名和实际文件大小的总和
    m_send_totalBytes += m_send_outBlock.size();
    sendOut.device()->seek(0);
    // 返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut << m_send_totalBytes << qint64(m_send_outBlock.size() - sizeof(qint64)*2);
    // 发送完文件头结构后剩余数据的大小
    m_send_bytesToWrite = m_send_totalBytes - m_sslSocket->write(m_send_outBlock);
    m_send_outBlock.resize(0);
}

void CTcpClient::updateProgress(qint64 numBytes)
{
    if(!m_send_startTransfer)
        return;
    if(m_sslSocket->state() != QAbstractSocket::ConnectedState)
        return;

    // 已经发送数据的大小
    m_send_bytesWritten += (int)numBytes;
    // 如果已经发送了数据
    if (m_send_bytesToWrite > 0) {
        // 每次发送payloadSize大小的数据，这里设置为64KB，如果剩余的数据不足64KB，
        // 就发送剩余数据的大小
        m_send_outBlock = m_send_localFile->read(qMin(m_send_bytesToWrite, m_send_payloadSize));
        // 发送完一次数据后还剩余数据的大小
        m_send_bytesToWrite -= (int)m_sslSocket->write(m_send_outBlock);
        // 清空发送缓冲区
        m_send_outBlock.resize(0);
    }
    else { // 如果没有发送任何数据，则关闭文件
        m_send_localFile->close();
        delete m_send_localFile;
        m_send_localFile = nullptr;
    }
    // 更新进度条
//    int percent = m_bytesWritten / m_totalBytes;
    // 如果发送完毕
    if(m_send_bytesWritten == m_send_totalBytes)
    {
        if(m_send_localFile)
        {
            if(m_send_localFile->isOpen())
                m_send_localFile->close();
            delete m_send_localFile;
            m_send_localFile = nullptr;
        }
        finishShareSolution();
    }
}

void CTcpClient::finishShareSolution()
{
    if(!m_send_startTransfer)
        return;
    m_send_startTransfer = false;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    generateHeader(srShareFileFin, out);
    out << 0;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_sslSocket->write(block);
    m_sslSocket->flush();
    m_nextBlockSize = 0;
    emit transferFinished();
}


void CTcpClient::socketStateChanged(QAbstractSocket::SocketState state)
{
    DBG << "socketStateChanged" << state;
}

void CTcpClient::socketEncrypted()
{
    const QSslCipher cipher = m_sslSocket->sessionCipher();
    const QString cipherInfo = QString("%1, %2 (%3/%4)").arg(cipher.authenticationMethod())
                                       .arg(cipher.name()).arg(cipher.usedBits())
                                       .arg(cipher.supportedBits());;
    DBG << "socketEncrypted " << cipherInfo;
}

void CTcpClient::socketError(QAbstractSocket::SocketError error)
{
    DBG << "socket error " << error << m_sslSocket->errorString();
}

void CTcpClient::sslErrors(const QList<QSslError> &errors)
{
    DBG << "sslErrors";
    for (const auto &error : errors)
        DBG << error.errorString();
}

