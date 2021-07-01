#include <QTimer>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "cclientdialog.h"
#include "ui_cclientdialog.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#if defined(Q_OS_WIN)
#include "windows.h"
#include "shellapi.h"
#include "shlobj.h"
#endif

CClientDialog::CClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CClientDialog)
{
    ui->setupUi(this);

    m_shareFileList = new QList<SSharedFile>;

    ui->tableWidget->setColumnWidth(0, 300);
    ui->tableWidget->setColumnWidth(1, 50);
    ui->tableWidget->setColumnWidth(2, 160);
    ui->pushButton_refreshSharedFile->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/refresh128.png"));
    ui->pushButton_download->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/down128.png"));
}

CClientDialog::~CClientDialog()
{
    delete ui;
    delete m_shareFileList;
}

void CClientDialog::serverConnectStatusChanged(bool flag)
{
    m_serverConnected = flag;
    if(!m_serverConnected)
        m_sharedFileListIsOld = true;
}

void CClientDialog::addSharedFile(QString fileName, QString userName, QString dateTime)
{
    m_sharedFileListIsOld = false;
    DBG << m_shareFileList->count();
    for(int i = 0; i < m_shareFileList->count(); ++i) {
        const SSharedFile &file = m_shareFileList->at(i);
        DBG << file.fileName << file.userName << file.shareTime.toString("yyyy-MM-dd hh:mm:ss");
    }

    m_canRefresh = true;
    if(fileName == "none" && userName == "none" && dateTime == "none")
        return;

    int row = m_shareFileList->count();
    ui->tableWidget->insertRow(row);
    QTableWidgetItem *item1 = new QTableWidgetItem(fileName);
    ui->tableWidget->setItem(row, 0, item1);
    QTableWidgetItem *item2 = new QTableWidgetItem(userName);
    item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, 1, item2);
    QTableWidgetItem *item3 = new QTableWidgetItem(dateTime);
    item3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, 2, item3);

    SSharedFile sharedFile = {fileName, userName, QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss")};
    m_shareFileList->append(sharedFile);
}

void CClientDialog::showError(QString errorMesg)
{
    DBG << errorMesg;
    QMessageBox::critical(this, qApp->applicationName(), errorMesg);
}

void CClientDialog::sharedFileListIsOld()
{
    m_sharedFileListIsOld = true;
}

void CClientDialog::serverRejectDownloadSharedFile(QString mesg)
{
    QMessageBox::critical(this, qApp->applicationName(), tr("服务器拒绝下载共享文件：%1").arg(mesg));
}

void CClientDialog::downloadSharedFileFinish(QString fileName, bool flag)
{
    if(flag)
    {
        if(QMessageBox::Yes == QMessageBox::information(this, qApp->applicationName(), tr("下载共享文件：“%1”成功，点击“Yes”浏览生成的文件！").arg(fileName),
                                                        QMessageBox::Yes | QMessageBox::No))
        {
#if defined(Q_OS_WIN)
            fileName.replace("/", "\\");
            fileName = " /select," + fileName;
            LPCSTR file = fileName.toLocal8Bit();
            ShellExecuteA(0, "open", "explorer.exe", file, nullptr, SW_SHOWNORMAL);
#elif defined(Q_OS_UNIX)
            QDesktopServices::openUrl(QUrl("file:///" + fileName));
#endif
        }
        ui->progressBar->setValue(0);
    }
    else
    {
        QMessageBox::information(this, qApp->applicationName(), tr("下载共享文件：“%1”失败！").arg(fileName));
        ui->progressBar->setValue(0);
    }
}

void CClientDialog::downloadSharedFilePercent(int percent)
{
    ui->progressBar->setValue(percent);
}

QList<SSharedFile> *CClientDialog::getShareFileList() const
{
    return m_shareFileList;
}

void CClientDialog::on_pushButton_refreshSharedFile_clicked()
{
    if(!m_canRefresh)
        return;
    m_canRefresh = false;
    m_shareFileList->clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    emit sharedFileListRequest();
}

void CClientDialog::on_pushButton_download_clicked()
{
    if(!m_serverConnected)
    {
        QMessageBox::information(this, qApp->applicationName(), tr("服务连接已断开！"));
        return;
    }
    if(m_sharedFileListIsOld)
    {
        QMessageBox::information(this, qApp->applicationName(), tr("数据已过时，请先刷新！"));
        return;
    }

    ui->progressBar->setValue(0);
    if(QTableWidgetItem *item = ui->tableWidget->currentItem())
        emit sharedFileDownloadRequest(ui->tableWidget->item(item->row(), 0)->text(),
                                       ui->tableWidget->item(item->row(), 1)->text());
}
