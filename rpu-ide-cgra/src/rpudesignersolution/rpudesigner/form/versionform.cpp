/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    versionform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QIODevice>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include "versionform.h"
#include "ui_versionform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

/**
 * VersionForm::VersionForm
 * \brief   constructor of VersionForm
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
VersionForm::VersionForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionForm)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    QImage image;
    image.load(CAppEnv::getImageDirPath() + "/128/Product-documentation128.png");
    image = image.scaled(128, 128, Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(image));

    QFile file(CAppEnv::getAppDirPath() + "/version.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        ui->textBrowser->setText(tr(u8"没有找到版本信息文件！"));
    }
    else
    {

        QTextStream in(&file);
        ui->textBrowser->setText(in.readAll());
        file.close();
    }

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

/**
 * VersionForm::~VersionForm
 * \brief   destructor of VersionForm
 * \author  zhangjun
 * \date    2016-10-12
 */
VersionForm::~VersionForm()
{
    delete ui;
}
