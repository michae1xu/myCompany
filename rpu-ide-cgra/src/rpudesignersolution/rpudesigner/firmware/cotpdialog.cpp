#include <QCloseEvent>
#include <QCompleter>
#include <QFileDialog>
#include "cotpdialog.h"
#include "ui_cotpdialog.h"
#include "chardwaredebug.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

COTPDialog::COTPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::COTPDialog),
    m_isUpdating(false)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr(u8"写入"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr(u8"退出"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->pushButton_pubKeyExplorer->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Folder128.png"));
    ui->pushButton_encryptKeyExplorer->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Folder128.png"));
    ui->pushButton_refresh->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/order128.png"));

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_error->setPalette(pe);

//    QRegExp regExp("[^\\\\/:*?\"<>|]+(,[^\\\\/:*?\"<>|]+)*");
//    ui->lineEdit_pubKey->setValidator(new QRegExpValidator(regExp, this));

    QCompleter *completer = new QCompleter(this);
#ifndef Q_WS_X11
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    completer->setModel(new CDirModel(completer));
#else
    completer->setModel(new QDirModel(completer));
#endif
    ui->lineEdit_pubKey->setCompleter(completer);

    on_pushButton_refresh_clicked();

    connect(CHardwareDebug::getInstance(this), SIGNAL(sendPercent(int)), this, SLOT(updateProgressPercent(int)));
}

COTPDialog::~COTPDialog()
{
//    CHardwareDebug::releaseInstance();
    delete ui;
}

void COTPDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        if(checkFileName(ui->lineEdit_pubKey->text()) && checkFileName(ui->lineEdit_encryptKey->text()))
        {
            QFile file(ui->lineEdit_pubKey->text());
            if(!file.open(QIODevice::ReadOnly))
            {
                ui->label_error->setText(tr(u8"文件打开失败"));
                return;
            }

            file.seek(0x10000);
            char *indexData = new char[128];
            file.read(indexData, 128);
            file.close();

            struct SFlashHeader
            {
                uint32_t magic;
                uint32_t image_version;
                uint32_t signature_algorithm;
                uint32_t digest_algorithm;
                uint32_t encrypt_algorithm;
                uint32_t pubkey_hash_address;
                uint32_t encrypt_key_address;
                uint32_t certificate_partition_address;
                uint32_t certificate_partition_size;
                uint32_t mcu1_partition_address;
                uint32_t mcu1_partition_size;
                uint32_t mcu2_partition_address;
                uint32_t mcu2_partition_size;
                uint32_t mcu3_partition_address;
                uint32_t mcu3_partition_size;
                uint32_t vaultip_partition_address;
                uint32_t vaultip_partition_size;
                uint32_t pka_partition_address;
                uint32_t pka_partition_size;
                uint32_t rpu_partition_address;
                uint32_t rpu_partition_size;
                uint32_t debug_mode;
                uint32_t otp_image_address;
                uint32_t sys_clock;
                uint32_t sys_baudrate;
                uint32_t reserved[6];
                uint32_t checksum;
            };
            SFlashHeader *header = (SFlashHeader*)(indexData);

            file.setFileName(ui->lineEdit_encryptKey->text());
            if(!file.open(QIODevice::ReadOnly))
            {
                ui->label_error->setText(tr(u8"文件打开失败"));
                delete[] indexData;
            }

            ui->buttonBox->setEnabled(false);
            ui->pushButton_refresh->setEnabled(false);
            ui->progressBar->setValue(0);
            m_isUpdating = true;
            for(int i = 0; i < 2; ++i)
            {
                uint32_t address;
                if(i == 0)
                    address = header->pubkey_hash_address;
                else
                    address = header->encrypt_key_address;

                file.seek(address/8);
                char *otpData = new char[32];
                file.read(otpData, 32);

                bool ok = CHardwareDebug::getInstance(this)->rcpWriteOtp(address, (uint32_t*)otpData, 32);
                delete[] otpData;
                if(ok)
                {
                    ui->label_error->setText(tr(u8"OTP写入成功"));
                }
                else
                {
                    ui->label_error->setText(tr(u8"OTP写入失败"));
                    break;
                }
            }
            delete[] indexData;
            ui->buttonBox->setEnabled(true);
            ui->pushButton_refresh->setEnabled(true);
            m_isUpdating = false;
        }
    }
    else
        this->reject();
}

void COTPDialog::on_lineEdit_pubKey_textChanged(const QString &arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkFileName(arg1)
                                                            && checkFileName(ui->lineEdit_encryptKey->text()));
}

void COTPDialog::on_lineEdit_encryptKey_textChanged(const QString &arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkFileName(arg1)
                                                            && checkFileName(ui->lineEdit_pubKey->text()));
}

bool COTPDialog::checkFileName(const QString &str)
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

void COTPDialog::closeEvent(QCloseEvent *event)
{
    if(m_isUpdating)
    {
        ui->label_error->setText(tr(u8"不能退出"));
        event->ignore();
        return;
    }
    event->accept();
}

void COTPDialog::on_lineEdit_pubKey_editingFinished()
{
    QString text = ui->lineEdit_pubKey->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
    if(text.endsWith(":"))
        text += "/";
#endif
    ui->lineEdit_pubKey->setText(text);
}

void COTPDialog::on_lineEdit_encryptKey_editingFinished()
{
    QString text = ui->lineEdit_encryptKey->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
    if(text.endsWith(":"))
        text += "/";
#endif
    ui->lineEdit_encryptKey->setText(text);
}

void COTPDialog::updateProgressPercent(int percent)
{
    ui->progressBar->setValue(percent);
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

void COTPDialog::on_pushButton_pubKeyExplorer_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), "", tr(u8"IMG (*.img)"));
    if(str.isEmpty())
        return;

    ui->lineEdit_pubKey->setText(str);
    on_lineEdit_pubKey_editingFinished();
}

void COTPDialog::on_pushButton_encryptKeyExplorer_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), "", tr(u8"IMG (*.img)"));
    if(str.isEmpty())
        return;

    ui->lineEdit_encryptKey->setText(str);
    on_lineEdit_encryptKey_editingFinished();
}


void COTPDialog::on_pushButton_refresh_clicked()
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
        ui->lineEdit_pubKey->setEnabled(false);
        ui->pushButton_pubKeyExplorer->setEnabled(false);
        ui->pushButton_encryptKeyExplorer->setEnabled(false);
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
