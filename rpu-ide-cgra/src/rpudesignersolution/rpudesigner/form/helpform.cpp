/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    helpform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QFile>
#include <QTextStream>
#include "helpform.h"
#include "ui_helpform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

/**
 * HelpForm::HelpForm
 * \brief   constructor of HelpForm
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
HelpForm::HelpForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpForm)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    QImage image;
    image.load(CAppEnv::getImageDirPath() + "/128/Help-desk128.png");
    image = image.scaled(128, 128, Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(image));

    QFile file(CAppEnv::getDocDirPath() + "/manual.mht");
    if(!file.open(QFile::ReadOnly | QFile::Text))
        ui->textBrowser->setText(tr(u8"未找到帮助文件，重新安装可解决此问题！"));
    else
    {
        QTextStream in(&file);
        ui->textBrowser->setHtml(in.readAll());
//        ui->textBrowser->setText(in.readAll());
    }
    file.close();

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

/**
 * HelpForm::~HelpForm
 * \brief   destructor of HelpForm
 * \author  zhangjun
 * \date    2016-10-12
 */
HelpForm::~HelpForm()
{
    delete ui;
}
