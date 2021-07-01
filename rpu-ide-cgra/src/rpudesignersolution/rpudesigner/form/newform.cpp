/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    newform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QRegExp>
#include <QDirModel>
#include <QCompleter>
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "newform.h"
#include "ui_newform.h"

/**
 * NewForm::NewForm
 * \brief   constructor of NewForm
 * \param   path
 * \param   suffix
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
NewForm::NewForm(QString path,  ENewFileType fileType, QWidget *parent, ECipherType cipherType) :
    QDialog(parent),
    ui(new Ui::NewForm)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    switch (fileType) {
    case NEWSOLUTION:
        ui->label_type->setVisible(false);
        ui->comboBox->setVisible(false);
        ui->label_name->setText(tr(u8"解决方案名称："));
        ui->label_path->setText(tr(u8"解决方案路径："));
        m_suffix = ".rpusln";
        break;
    case NEWPROJECT:
        ui->label_type->setVisible(true);
        ui->comboBox->setVisible(true);
        ui->label_name->setText(tr(u8"项目名称："));
        ui->label_path->setText(tr(u8"项目路径："));
        m_suffix = ".rpupro";
        break;
    case NEWXML:
        ui->label_type->setVisible(false);
        ui->comboBox->setVisible(false);
        ui->label_name->setText(tr(u8"算法文件名称："));
        ui->label_path->setText(tr(u8"算法文件路径："));
        m_suffix = ".xml";
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    ui->comboBox->setCurrentIndex(cipherType);
    m_defaultPath = path;
    m_fileType = fileType;
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

/**
 * NewForm::~NewForm
 * \brief   destructor of NewForm
 * \author  zhangjun
 * \date    2016-10-12
 */
NewForm::~NewForm()
{
    delete ui;
}


/**
 * NewForm::on_pushButton_clicked
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void NewForm::on_pushButton_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, tr(u8"打开"), m_defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(str.isEmpty())
        return;

    ui->lineEdit_path->setText(str);
}

/**
 * NewForm::on_lineEdit_path_textChanged
 * \brief   
 * \param   arg1
 * \author  zhangjun
 * \date    2016-10-12
 */
bool NewForm::on_lineEdit_path_textChanged(const QString &arg1)
{
    if(arg1.trimmed().isEmpty())
    {
        ui->label_error->setText(tr(u8"路径不能为空"));
        return false;
    }
    else if(QDir(arg1).exists())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ui->lineEdit_path->setPalette(palette);
        ui->label_error->setText(tr(u8""));
        return true;
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        ui->lineEdit_path->setPalette(palette);
        ui->label_error->setText(tr(u8"路径不存在"));
        return false;
    }
}

/**
 * NewForm::on_buttonBox_clicked
 * \brief   
 * \param   button
 * \author  zhangjun
 * \date    2016-10-12
 */
void NewForm::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        if(on_lineEdit_path_textChanged(ui->lineEdit_path->text()) && checkFileName())
        {
            switch (m_fileType) {
            case NEWSOLUTION:
            case NEWPROJECT:
            {
                QDir dir(m_path);
                if(!dir.mkdir(m_baseName))
                {
                    m_fileName.clear();
                }
            }
                break;
            case NEWXML:
                break;
            default:
                Q_ASSERT(false);
                break;
            }
            this->accept();
        }
    }
    else
        this->reject();

}

/**
 * NewForm::on_lineEdit_name_textChanged
 * \brief   
 * \param   arg1
 * \author  zhangjun
 * \date    2016-10-12
 */
void NewForm::on_lineEdit_name_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    checkFileName();
}

/**
 * NewForm::checkFileName
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool NewForm::checkFileName()
{
    m_baseName = ui->lineEdit_name->text().trimmed();
    m_path = ui->lineEdit_path->text().trimmed();
    if(m_baseName.isEmpty())
    {
        ui->label_error->setText(tr(u8"名称不能为空"));
        return false;
    }
    if(m_baseName.endsWith(m_suffix, Qt::CaseInsensitive))
    {
        m_suffix = m_baseName.right(m_suffix.length());
        m_baseName.remove(m_baseName.length() - m_suffix.length(), m_suffix.length());
    }
    switch (m_fileType) {
    case NEWSOLUTION:
    case NEWPROJECT:
        if(QDir(m_path + "/" + m_baseName).exists())
        {
            ui->label_error->setText(tr(u8"文件已存在"));
            return false;
        }
        m_fileName = m_path + "/" + m_baseName + "/" + m_baseName + m_suffix;
        m_cipherType = (ECipherType)ui->comboBox->currentIndex();
        break;
    case NEWXML:
        if(QFileInfo(m_path + "/" + m_baseName + m_suffix).exists())
        {
            ui->label_error->setText(tr(u8"文件已存在"));
            return false;
        }
        m_fileName = m_path + "/" + m_baseName + m_suffix;
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    ui->label_error->setText("");
    return true;
}

void NewForm::on_lineEdit_path_editingFinished()
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
