#include <QCloseEvent>
#include <QCompleter>
#include <QFileDialog>
#include "cfirmwareupdatedialog.h"
#include "ui_cfirmwareupdatedialog.h"
#include "chardwaredebug.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CFirmwareUpdateDialog::CFirmwareUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFirmwareUpdateDialog),
    m_isUpdating(false)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr(u8"更新"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr(u8"退出"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->pushButton_explorer->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Folder128.png"));
    ui->pushButton_refresh->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/order128.png"));

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_error->setPalette(pe);

//    QRegExp regExp("[^\\\\/:*?\"<>|]+(,[^\\\\/:*?\"<>|]+)*");
//    ui->lineEdit->setValidator(new QRegExpValidator(regExp, this));

    QCompleter *completer = new QCompleter(this);
#ifndef Q_WS_X11
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    completer->setModel(new CDirModel(completer));
#else
    completer->setModel(new QDirModel(completer));
#endif
    ui->lineEdit->setCompleter(completer);

    on_pushButton_refresh_clicked();

    connect(CHardwareDebug::getInstance(this), SIGNAL(sendPercent(int)), this, SLOT(updateProgressPercent(int)));
}

CFirmwareUpdateDialog::~CFirmwareUpdateDialog()
{
//    CHardwareDebug::releaseInstance();
    delete ui;
}

void CFirmwareUpdateDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        if(checkFileName(ui->lineEdit->text()))
        {
            ui->buttonBox->setEnabled(false);
            ui->pushButton_refresh->setEnabled(false);
            ui->progressBar->setValue(0);
            m_isUpdating = true;
            bool ok = CHardwareDebug::getInstance(this)->rcpWriteSpi(0, ui->lineEdit->text());
            if(ok)
            {
                ui->label_error->setText(tr(u8"更新固件成功"));
            }
            else
            {
                ui->label_error->setText(tr(u8"更新固件失败"));
            }
            ui->buttonBox->setEnabled(true);
            ui->pushButton_refresh->setEnabled(true);
            m_isUpdating = false;
        }
    }
    else
        this->reject();
}

void CFirmwareUpdateDialog::on_lineEdit_textChanged(const QString &arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkFileName(arg1));
}

bool CFirmwareUpdateDialog::checkFileName(const QString &str)
{
    if(str.trimmed().isEmpty())
    {
        ui->label_error->setText(tr(u8"文件名不能为空"));
        return false;
    }
    else if(QFileInfo(str).isFile())
    {
        ui->label_error->setText(tr(u8""));
        return true;
    }
    else
    {
        ui->label_error->setText(tr(u8"文件不存在"));
        return false;
    }
}

void CFirmwareUpdateDialog::closeEvent(QCloseEvent *event)
{
    if(m_isUpdating)
    {
        ui->label_error->setText(tr(u8"不能退出"));
        event->ignore();
        return;
    }
    event->accept();
}

void CFirmwareUpdateDialog::on_lineEdit_editingFinished()
{
    QString text = ui->lineEdit->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
    if(text.endsWith(":"))
        text += "/";
#endif
    ui->lineEdit->setText(text);
    ui->lineEdit->setCursorPosition(0);
}

void CFirmwareUpdateDialog::updateProgressPercent(int percent)
{
    ui->progressBar->setValue(percent);
    if(percent < 50)
    {
        ui->label_error->setText(tr("正在更新备用分区"));
    }
    else
    {
        ui->label_error->setText(tr("正在更新主分区"));
    }
//    uint32_t percent = 0;
//    if(CHardwareDebug::getInstance(this)->deviceFirmwareUpdateProgress(&percent))
//    {
//        ui->progressBar->setValue(percent);
//    }
//    if(percent == 100)
//    {
//        m_timer.stop();
//        m_isUpdating = false;
//        ui->buttonBox->setEnabled(true);
//        ui->pushButton_refresh->setEnabled(true);
//        ui->label_error->setText(tr(u8"更新固件成功"));
//    }
//    else if(percent == 101)
//    {
//        m_timer.stop();
//        m_isUpdating = false;
//        ui->buttonBox->setEnabled(true);
//        ui->pushButton_refresh->setEnabled(true);
//        ui->label_error->setText(tr(u8"更新固件失败"));
//    }
}

void CFirmwareUpdateDialog::on_pushButton_explorer_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), "", tr(u8"Bin (*.bin);; IMG (*.img)"));
    if(str.isEmpty())
        return;

    ui->lineEdit->setText(str);
    on_lineEdit_editingFinished();
}

void CFirmwareUpdateDialog::on_pushButton_refresh_clicked()
{
    bool ok1 = CHardwareDebug::getInstance(this)->rcpGetSysInfo();
    if(ok1)
    {
        ui->label_info->setText(CHardwareDebug::getInstance(this)->getBuffer1String());
    }
    bool ok2 = CHardwareDebug::getInstance(this)->deviceFirmwareVersion();
    if(ok2)
    {
        ui->label_version->setText(CHardwareDebug::getInstance(this)->getBuffer1String());
    }
    if(!ok1 || !ok2)
    {
        ui->label_error->setText(tr(u8"设备未连接"));
        ui->lineEdit->setEnabled(false);
        ui->pushButton_explorer->setEnabled(false);
        ui->label_2->setEnabled(false);
        ui->label_3->setEnabled(false);
        ui->progressBar->setEnabled(false);
    }
    else
    {
        ui->label_error->setText(tr(u8""));
    }
    ui->progressBar->setValue(0);
}
