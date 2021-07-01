#ifndef CCLIENTDIALOG_H
#define CCLIENTDIALOG_H

#include <QDialog>
#include "datafile.h"

namespace Ui {
class CClientDialog;
}

class CClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CClientDialog(QWidget *parent = 0);
    ~CClientDialog();

    QList<SSharedFile> *getShareFileList() const;

public slots:
    void serverConnectStatusChanged(bool flag);
    void addSharedFile(QString fileName, QString userName, QString dateTime);
    void showError(QString errorMesg);
    void sharedFileListIsOld();
    void serverRejectDownloadSharedFile(QString mesg);
    void downloadSharedFileFinish(QString fileName, bool flag);
    void downloadSharedFilePercent(int percent);

    void on_pushButton_refreshSharedFile_clicked();

signals:
    void clientRequest();
    void sharedFileListRequest();
    void sharedFileDownloadRequest(QString, QString);

private slots:

    void on_pushButton_download_clicked();

private:
    Ui::CClientDialog *ui;
    bool m_serverConnected = false;
    bool m_sharedFileListIsOld = true;
    bool m_canRefresh = true;

    QList<SSharedFile> *m_shareFileList;
};

#endif // CCLIENTDIALOG_H
