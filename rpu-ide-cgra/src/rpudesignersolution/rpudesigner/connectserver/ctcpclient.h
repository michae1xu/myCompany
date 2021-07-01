#ifndef CTCPCLIENT_H
#define CTCPCLIENT_H

class QSslSocket;
class QFile;
class QTimer;

#include <QThread>
#include <QSslSocket>
#include "datafile.h"

class CTcpClient : public QThread
{
    Q_OBJECT
public:
    CTcpClient(const QString &curVersion, QObject *parent = 0);
    ~CTcpClient();
    void setIpPort(const QString &destIp, const quint16 &destPort);
    void sendData(ESendReason reason, const QByteArray &bty);
    bool getIsRegister() const {return m_isRegisted;}

public slots:
    void versionRequest();
    void licenseChanged();
    void clientRequest();
    void shareSolutionRequest(QString fileName);
    void sharedFileListRequest();
    void sharedFileDownloadRequest(QString fileName, QString userName);

signals:
    void connectStatusChanged(bool);
    void showNotify(QString);
    void showMessage(QString);
    void versionAck(bool, QString);
    void serverCloseProgram(QString);
    void transferFinished();
    void addSharedFile(QString, QString, QString);
    void sendClientDialogError(QString);
    void sharedFileListIsOld();
    void serverRejectDownloadSharedFile(QString);
    void downloadSharedFileFinish(QString, bool);
    void downloadSharedFilePercent(int);

private slots:
    void connectToServer();
    void registToServer();
    void connectionClosedByServer();
    void inCommingData();
    void error();
    void deleteSocket();

    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketEncrypted();
    void socketError(QAbstractSocket::SocketError error);
    void sslErrors(const QList<QSslError> &errors);
    void updateProgress(qint64 numBytes);
    void finishShareSolution();

private:
    void run();
    void closeConnection();
    ESendReason checkVersion(const QString &lastVersion) const;
    void startShareSolution();
    void generateHeader(ESendReason reason, QDataStream &stream);


    QSslSocket *m_sslSocket;
    quint16 m_nextBlockSize = 0;
    QString m_destIp;
    quint16 m_destPort;
    bool m_isRegisted;
    QTimer *m_registTimer;
    QTimer *m_connectTimer;
    QString m_curVersion;
    QString m_userName;

    //发送共享文件
    QFile *m_send_localFile = nullptr;  // 要发送的文件
    qint64 m_send_totalBytes = 0;  // 发送数据的总大小
    qint64 m_send_bytesWritten = 0;  // 已经发送数据大小
    qint64 m_send_bytesToWrite = 0;   // 剩余数据大小
    qint64 m_send_payloadSize = 64*1024;   // 每次发送数据的大小
    QByteArray m_send_outBlock;  // 数据缓冲区，即存放每次要发送的数据块
    QString m_send_fileName;
    bool m_send_startTransfer = false;

    //下载共享文件
    qint64 m_receive_totalBytes = 0;     // 存放总大小信息
    qint64 m_receive_bytesReceived = 0;  // 已收到数据的大小
    qint64 m_receive_fileNameSize = 0;   // 文件名的大小信息
    QByteArray m_receive_inBlock;    // 数据缓冲区
    QString m_receive_fileName;      // 存放文件名
    QFile *m_receive_localFile = nullptr;      // 本地文件
    QString m_receive_shareFileMD5;
    bool m_receive_startDownloading = false;
    QTime m_time;
};

#endif // CTCPCLIENT_H
