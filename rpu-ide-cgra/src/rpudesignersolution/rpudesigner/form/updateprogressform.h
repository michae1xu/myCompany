#ifndef UPDATEPROGRESSFORM_H
#define UPDATEPROGRESSFORM_H

#include <QDialog>
#include <QTime>
#include "datafile.h"

class QTcpSocket;
class QTemporaryFile;

namespace Ui {
class UpdateProgressForm;
}

class UpdateProgressForm : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateProgressForm(QWidget *parent = 0);
    ~UpdateProgressForm();

    void setIpPort(const QString &destIp, const quint16 &destPort);
    inline QString getFileName() const {return m_fileName;}

public slots:
    void registToServer();
    void error();
    void inCommingData();

private:
    Ui::UpdateProgressForm *ui;

    QTcpSocket *m_tcpSocket = nullptr;
    QString m_destIp;
    quint16 m_destPort;
    qint16 m_nextBlockSize;

    qint64 m_totalBytes;     // 存放总大小信息
    qint64 m_bytesReceived;  // 已收到数据的大小
    qint64 m_fileNameSize;   // 文件名的大小信息
    QByteArray m_inBlock;    // 数据缓冲区
    QString m_fileName;      // 存放文件名
    QTemporaryFile *m_localFile;      // 本地文件
    QString m_MD5;

    QTime m_time;

    bool m_startDownloading;
    bool m_downloadSuccess;

    void connectToServer();
};

#endif // UPDATEPROGRESSFORM_H
