#include <QTcpSocket>
#include <QMessageBox>
#include <QHostInfo>
#include <QDir>
#include <QTemporaryFile>
#include <QCloseEvent>
#include <QTimer>
#include <QNetworkProxy>
#include <QCryptographicHash>
#include "updateprogressform.h"
#include "ui_updateprogressform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "version.h"

UpdateProgressForm::UpdateProgressForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateProgressForm)
{
    ui->setupUi(this);

//    Qt::WindowFlags flags = Qt::Dialog;
//    flags |= Qt::WindowMinimizeButtonHint;
//    setWindowFlags(flags);

#if defined(Q_OS_WIN)
    m_destIp = "192.168.31.196";
#else
    if(QHostInfo::localHostName().startsWith("wds") || QHostInfo::localHostName().startsWith("sds"))
        m_destIp = "wds020";
    else
        m_destIp = "192.168.31.196";
#endif
    m_destPort = 10201;

    m_nextBlockSize = 0;
    m_startDownloading = false;
    m_downloadSuccess= false;
    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_fileNameSize = 0;
    m_localFile = nullptr;

    m_tcpSocket = new QTcpSocket(this);
//    connect(m_tcpSocket, SIGNAL(disconnected()), m_tcpSocket, SLOT(deleteLater()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(registToServer()));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(inCommingData()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

    m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    m_tcpSocket->setProxy(QNetworkProxy::applicationProxy());

    connectToServer();

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

UpdateProgressForm::~UpdateProgressForm()
{
    delete ui;
    m_tcpSocket->close();
    if(m_localFile)
    {
        if(m_localFile->isOpen())
            m_localFile->close();
        delete m_localFile;
        m_localFile = nullptr;
    }
    delete m_tcpSocket;
}

void UpdateProgressForm::setIpPort(const QString &destIp, const quint16 &destPort)
{
    m_destIp = destIp;
    m_destPort = destPort;
}

void UpdateProgressForm::connectToServer()
{
    m_tcpSocket->connectToHost(m_destIp, m_destPort);
    if(!m_tcpSocket->waitForConnected())
    {
        m_tcpSocket->close();
        ui->label->setText(tr(u8"更新失败"));
    }
    else
    {
        ui->label->setText(tr(u8"正在下载"));
    }
}


void UpdateProgressForm::registToServer()
{
    if(m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
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
    QString license = CAppEnv::m_license;
    if(license.isEmpty())
        license = "tsinghuarpudesigner";
    QString userName = QFileInfo(QDir::homePath()).completeBaseName();
#if defined(Q_OS_WIN)
    out << quint16(0) << quint32(srUpdateSyn) << license << QString(VER_PRODUCTNAME_STR)
        << qApp->applicationVersion() << QHostInfo::localHostName() << userName << CAppEnv::getMacAddr() << QString("windows");
#elif defined(Q_OS_UNIX)
    out << quint16(0) << quint32(srUpdateSyn) << license << QString(VER_PRODUCTNAME_STR)
        << qApp->applicationVersion() << QHostInfo::localHostName() << userName << CAppEnv::getMacAddr() << QString("linux");
#endif
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_tcpSocket->write(block);
    m_tcpSocket->flush();
    m_nextBlockSize = 0;
}

void UpdateProgressForm::error()
{
    DBG << m_tcpSocket->errorString();
    if(!m_downloadSuccess)
        ui->label->setText(tr(u8"更新失败"));
}

void UpdateProgressForm::inCommingData()
{
    QDataStream in(m_tcpSocket);
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

    if(m_startDownloading)
    {
        float useTime = m_time.elapsed();
        // 如果接收到的数据小于16个字节，保存到来的文件头结构
        if (m_bytesReceived <= (qint64)sizeof(qint64)*2) {
            if((m_tcpSocket->bytesAvailable() >= (qint64)sizeof(qint64)*2)
                    && (m_fileNameSize == 0)) {
                // 接收数据总大小信息和文件名大小信息
                in >> m_totalBytes >> m_fileNameSize;
                m_bytesReceived += sizeof(qint64) * 2;
            }
            if((m_tcpSocket->bytesAvailable() >= m_fileNameSize)
                    && (m_fileNameSize != 0)) {
                // 接收文件名，并建立文件
                in >> m_fileName;
#if defined(Q_OS_WIN)
                m_fileName = QDir::tempPath() + QDir::separator() +
                            m_fileName + "_XXXXXX." + "exe";
#elif defined(Q_OS_UNIX)
                m_fileName = QDir::tempPath() + QDir::separator() +
                        m_fileName + "_XXXXXX." + "run";
#endif
                m_bytesReceived += m_fileNameSize;
                m_localFile = new QTemporaryFile(m_fileName, this);
                m_localFile->setAutoRemove(false);
                if (!m_localFile->open()) {
                    delete m_localFile;
                    m_localFile = nullptr;
                    m_startDownloading = false;
                    m_tcpSocket->close();
                    ui->label->setText(tr(u8"更新失败"));
                    return;
                }
                m_fileName = m_localFile->fileName();
            } else {
                return;
            }
        }
        // 如果接收的数据小于总数据，那么写入文件
        if (m_bytesReceived < m_totalBytes) {
            m_bytesReceived += m_tcpSocket->bytesAvailable();
            m_inBlock = m_tcpSocket->readAll();
            m_localFile->write(m_inBlock);
            m_inBlock.resize(0);
        }

        qreal percent = qreal(m_bytesReceived) / m_totalBytes;
        int range = 100;
        percent *= range;
        ui->progressBar->setValue(qRound(percent));
        double speed = m_bytesReceived / useTime;
        ui->label->setText(tr(u8"已接收 %0MB (%1MB/s) \n共%2MB 已用时:%3秒\n估计剩余时间：%4秒")
                                       .arg(m_bytesReceived / (1024*1024))//已接收
                                       .arg(speed*1000/(1024*1024),0,'f',2)//速度
                                       .arg(m_totalBytes / (1024 * 1024))//总大小
                                       .arg(useTime/1000,0,'f',0)//用时
                                       .arg(m_totalBytes/speed/1000 - useTime/1000,0,'f',0));//剩余时间

        // 接收数据完成时
        if (m_bytesReceived == m_totalBytes) {
            if(m_localFile)
            {
                if(m_localFile->isOpen())
                    m_localFile->close();
                delete m_localFile;
                m_localFile = nullptr;
            }
            m_startDownloading = false;
            m_downloadSuccess = true;
            ui->label->setText(tr(u8"正在校验"));
            QFile file(m_fileName);
            if(!file.open(QIODevice::ReadOnly))
            {
                ui->label->setText(tr(u8"校验失败"));
                QMessageBox::critical(this, qApp->applicationName(), tr("校验失败"));
                return;
            }
            const QByteArray &allContent = file.readAll();
            file.close();
            const QString &localFileMD5 = QCryptographicHash::hash(allContent, QCryptographicHash::Md5).toHex().toUpper();
            if(!m_MD5.isEmpty() && m_MD5 == localFileMD5)
            {
                ui->label->setText(tr(u8"即将安装"));
                QTimer::singleShot(1000, this, SLOT(accept()));
            }
            else
            {
                ui->label->setText(tr(u8"校验失败"));
                QMessageBox::critical(this, qApp->applicationName(), tr("校验失败"));
            }
        }
        return;
    }

    if (m_nextBlockSize == 0) {
        if (m_tcpSocket->bytesAvailable() < (qint64)sizeof(quint16))
            return;
        in >> m_nextBlockSize;
    }

    if (m_tcpSocket->bytesAvailable() < m_nextBlockSize)
        return;

    //包大小 << 发送原因
    quint32 sendReason;
    in >> sendReason;
    QString msg;
    switch (sendReason) {
    case srUpdateAcceptAck:
        in >> m_MD5;
        m_startDownloading = true;
        m_time.start();
        break;
    case srUpdateRejectAck:
        in >> msg;
        QMessageBox::critical(this, qApp->applicationName(), msg);
        m_tcpSocket->close();
        ui->label->setText(tr(u8"更新失败"));
        break;
    case srNoNeedUpdateAck:
        in >> msg;
        QMessageBox::information(this, qApp->applicationName(), msg);
        m_tcpSocket->close();
        ui->label->setText(tr(u8"更新失败"));
        break;
    default:
        break;
    }
}
