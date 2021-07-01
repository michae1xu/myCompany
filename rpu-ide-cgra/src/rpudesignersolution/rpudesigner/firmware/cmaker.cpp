#include <QFileInfo>
#include <QProcess>
#include <QDesktopServices>
#include <QDateTime>
#include <QFileDialog>
#include <QCompleter>
#include "cmaker.h"
#include "ui_cmaker.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CMaker::CMaker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CMaker)
{
    ui->setupUi(this);
    QString imgPth = CAppEnv::getImageDirPath() + "/128/";
    ui->pushButton_mcu1->setIcon(QIcon(imgPth + "Folder128.png"));
    ui->pushButton_mcu2->setIcon(QIcon(imgPth + "Folder128.png"));
    ui->pushButton_mcu3->setIcon(QIcon(imgPth + "Folder128.png"));

    m_redPlt.setColor(QPalette::Text,Qt::red);
    m_blackPlt.setColor(QPalette::Text,Qt::black);
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_info->setPalette(pe);

    QCompleter *completer = new QCompleter(this);
#ifndef Q_WS_X11
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    completer->setModel(new CDirModel(completer));
#else
    completer->setModel(new QDirModel(completer));
#endif
    ui->lineEdit_mcu1->setCompleter(completer);
    ui->lineEdit_mcu2->setCompleter(completer);
    ui->lineEdit_mcu3->setCompleter(completer);

    if(!(QFileInfo(CAppEnv::getEtcDirPath() + "/maker/prikey.sm2").isFile() &&
    QFileInfo(CAppEnv::getEtcDirPath() + "/maker/prikey.rsa").isFile() &&
    QFileInfo(CAppEnv::getEtcDirPath() + "/maker/enckey").isFile() &&
    QFileInfo(CAppEnv::getEtcDirPath() + "/maker/romloader.bin").isFile() &&
    QFileInfo(CAppEnv::getEtcDirPath() + "/maker/vaultip.bin").isFile() &&
    QFileInfo(CAppEnv::getEtcDirPath() + "/maker/otp.preload").isFile()))
    {
        ui->label_info->setText(tr(u8"程序有误"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
#if defined(Q_OS_WIN)
    ui->label_info->setText(tr(u8"不支持此平台"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
#endif
}

CMaker::~CMaker()
{
    delete ui;
}

void CMaker::on_buttonBox_accepted()
{
    if(checkValue())
    {
        ui->label_info->clear();
#if defined(Q_OS_WIN)
        QString makerCmd = CAppEnv::getBinDirPath() + tr(u8"/maker.exe");
#elif defined(Q_OS_UNIX)
        QString makerCmd = CAppEnv::getBinDirPath() + tr(u8"/maker");
#endif
        QStringList makerCmdList;
        if(ui->radioButton_sm2->isChecked())
        {
            makerCmdList << "--sign=sm2";
            makerCmdList << QString("--prikey=%1").arg(CAppEnv::getEtcDirPath() + "/maker/prikey.sm2");
        }
        else
        {
            makerCmdList << "--sign=rsa";
            makerCmdList << QString("--prikey=%1").arg(CAppEnv::getEtcDirPath() + "/maker/prikey.rsa");
        }
        makerCmdList << QString("--enckey=%1").arg(CAppEnv::getEtcDirPath() + "/maker/enckey");
        makerCmdList << QString("--boot=%1").arg(CAppEnv::getEtcDirPath() + "/maker/romloader.bin");
        makerCmdList << QString("--eip=%1").arg(CAppEnv::getEtcDirPath() + "/maker/vaultip.bin");
        makerCmdList << QString("--otppreload=%1").arg(CAppEnv::getEtcDirPath() + "/maker/otp.preload");
        makerCmdList << QString("--mcu1=%1").arg(ui->lineEdit_mcu1->text().trimmed());
        makerCmdList << QString("--mcu2=%1").arg(ui->lineEdit_mcu2->text().trimmed());
        makerCmdList << QString("--mcu3=%1").arg(ui->lineEdit_mcu3->text().trimmed());
        makerCmdList << QString("--output=%1").arg(CAppEnv::getMakerOutDirPath() +
                tr(u8"/rcp_firmware_%0").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz")));

        int exitCode = QProcess::execute(makerCmd, makerCmdList);
        if(exitCode != 0)
        {
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"签名固件失败，点击“Yes”退出，退出码：%0！").arg(exitCode));
            return;
        }
        if(QMessageBox::Yes == QMessageBox::information(this, tr(u8"固件签名"), tr(u8"签名固件成功，点击“Yes”浏览生成的文件并退出！"), QMessageBox::Yes | QMessageBox::No))
        {
            QDesktopServices::openUrl(QUrl("file:///" + CAppEnv::getMakerOutDirPath()));
        }
    }
    else
    {
        ui->label_info->setText(tr(u8"配置错误"));
        return;
    }
    accept();
}

bool CMaker::checkValue()
{
    return (QFileInfo(ui->lineEdit_mcu1->text().trimmed()).isFile() &&
            QFileInfo(ui->lineEdit_mcu2->text().trimmed()).isFile() &&
            QFileInfo(ui->lineEdit_mcu3->text().trimmed()).isFile());
}

QString CMaker::decToHex(int value)
{
    QString str = QString::number(value, 16);
    for(int i = str.length(); i < 8; ++i)
    {
        str.insert(0, "0");
    }
    str = "0x" + str;
    return str;
}


void CMaker::on_lineEdit_mcu1_textChanged(const QString &arg1)
{
    if(QFileInfo(arg1.trimmed()).exists())
    {
        ui->lineEdit_mcu1->setPalette(m_blackPlt);
    }
    else
    {
        ui->lineEdit_mcu1->setPalette(m_redPlt);
    }
}

void CMaker::on_lineEdit_mcu2_textChanged(const QString &arg1)
{
    if(QFileInfo(arg1.trimmed()).exists())
    {
        ui->lineEdit_mcu2->setPalette(m_blackPlt);
    }
    else
    {
        ui->lineEdit_mcu2->setPalette(m_redPlt);
    }
}

void CMaker::on_lineEdit_mcu3_textChanged(const QString &arg1)
{
    if(QFileInfo(arg1.trimmed()).exists())
    {
        ui->lineEdit_mcu3->setPalette(m_blackPlt);
    }
    else
    {
        ui->lineEdit_mcu3->setPalette(m_redPlt);
    }
}

void CMaker::on_pushButton_mcu1_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), "");
    if(str.isEmpty())
        return;
    ui->lineEdit_mcu1->setText(str);
}

void CMaker::on_pushButton_mcu2_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), "");
    if(str.isEmpty())
        return;
    ui->lineEdit_mcu2->setText(str);
}

void CMaker::on_pushButton_mcu3_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), "");
    if(str.isEmpty())
        return;
    ui->lineEdit_mcu3->setText(str);
}
