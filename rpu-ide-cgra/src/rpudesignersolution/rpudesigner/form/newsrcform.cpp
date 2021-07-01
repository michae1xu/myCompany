#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QRegExp>
#include <QCompleter>
#include <QRegExpValidator>
#include <QDirModel>
#include "newsrcform.h"
#include "ui_newsrcform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

NewSrcForm::NewSrcForm(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSrcForm),
    m_defaultPath(path)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

#if defined(Q_OS_WIN)
    m_defaultPath.replace("\\", "/");
    while(m_defaultPath.endsWith("/"))
    {
        m_defaultPath.chop(1);
    }
#endif

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_error->setPalette(pe);
    ui->lineEdit_path->setText(m_defaultPath);
    ui->lineEdit_path->setCursorPosition(0);
    ui->pushButton->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Folder128.png"));

    QRegExp regExp("[^\\\\/:*?\"<>|]+(,[^\\\\/:*?\"<>|]+)*");
    ui->lineEdit_name->setValidator(new QRegExpValidator(regExp, this));

    QCompleter *completer = new QCompleter(this);
#ifndef Q_WS_X11
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    completer->setModel(new CDirModel(completer));
#else
    completer->setModel(new QDirModel(completer));
#endif
    ui->lineEdit_path->setCompleter(completer);

    ui->lineEdit_name->setFocus();

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

NewSrcForm::~NewSrcForm()
{
    delete ui;
}

QString NewSrcForm::getFilePath() const
{
    QString str = ui->lineEdit_path->text().trimmed() + "/" + ui->lineEdit_name->text().trimmed();
    if(ui->radioButton_sbox->isChecked())
    {
        if(QFileInfo(str).suffix().compare("sbox", Qt::CaseInsensitive) != 0)
        {
            str += u8".sbox";
        }
    }
    else if(ui->radioButton_benes->isChecked())
    {
        if(QFileInfo(str).suffix().compare("benes", Qt::CaseInsensitive) != 0)
        {
            str += u8".benes";
        }
    }
    else if(ui->radioButton_fifo->isChecked())
    {
        if(QFileInfo(str).suffix().compare("fifo", Qt::CaseInsensitive) != 0)
        {
            str += u8".fifo";
        }
    }
    else if(ui->radioButton_memory->isChecked())
    {
        if(QFileInfo(str).suffix().compare("memory", Qt::CaseInsensitive) != 0)
        {
            str += u8".memory";
        }
    }
    else if(ui->radioButton_imd->isChecked())
    {
        if(QFileInfo(str).suffix().compare("imd", Qt::CaseInsensitive) != 0)
        {
            str += u8".imd";
        }
    }
    return str;
}

void NewSrcForm::on_pushButton_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, tr(u8"打开"), m_defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(str.isEmpty())
        return;

    ui->lineEdit_path->setText(str);
    on_lineEdit_path_editingFinished();
}

void NewSrcForm::on_lineEdit_path_textChanged(const QString &arg1)
{
    if(QDir(arg1).exists())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ui->lineEdit_path->setPalette(palette);
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        ui->lineEdit_path->setPalette(palette);
    }
}

void NewSrcForm::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->radioButton_sbox->isChecked())
    {
        m_type = "sbox";
    }
    else if(ui->radioButton_benes->isChecked())
    {
        m_type = "benes";
    }
    else if(ui->radioButton_fifo->isChecked())
    {
        m_type = "fifo";
    }
    else if(ui->radioButton_imd->isChecked())
    {
        m_type = "imd";
    }
    else if(ui->radioButton_memory->isChecked())
    {
        m_type = "memory";
    }

    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        if(QDir(ui->lineEdit_path->text()).exists() && !ui->lineEdit_path->text().trimmed().isEmpty())
        {
            if(checkFileName())
                this->accept();
        }
        else
        {
            QMessageBox::information(this, tr(u8"提示"), tr(u8"路径不存在，请先创建！"));
            return;
        }
    }
    else
        this->reject();
}

void NewSrcForm::on_lineEdit_name_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    checkFileName();
}

bool NewSrcForm::checkFileName()
{
    QString name = ui->lineEdit_name->text().trimmed();
    if(name.isEmpty())
    {
        ui->label_error->setText(tr(u8"不能为空"));
        return false;
    }

    QString str = ui->lineEdit_path->text().trimmed() + "/" + name;
    if(QFileInfo(str).exists())
    {
        ui->label_error->setText(tr(u8"文件已存在"));
        return false;
    }
    else
    {
        ui->label_error->setText("");
        return true;
    }
}

void NewSrcForm::on_lineEdit_path_editingFinished()
{
    QString text = ui->lineEdit_path->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
    if(text.endsWith(":"))
        text += "/";
#endif
    ui->lineEdit_path->setText(text);
    ui->lineEdit_path->setCursorPosition(0);
}
