/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    sourceform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "sourceform.h"
#include "ui_sourceform.h"
#include "capppathenv.h"
#include <QListWidget>
#include <QMapIterator>
#include <QFileIconProvider>
#include <QDebug>

/**
 * SourceForm::SourceForm
 * \brief   constructor of SourceForm
 * \param   QString
 * \param   sourceMap
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
SourceForm::SourceForm(QMap<QString, QString> &sourceMap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SourceForm)
{
    ui->setupUi(this);

    QImage image;
    image.load(CAppPathEnv::getImageDirPath() + "/128/addons128.png");
    image = image.scaled(128, 128, Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(image));


    m_sourceMap = sourceMap;

    QMapIterator<QString, QString> i(m_sourceMap);
    while(i.hasNext())
    {
        i.next();
        m_listViewList.append(i.key());
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        QString showName = i.key().mid(2);

        item->setText(showName);

        QFile file(i.value());
        QFileIconProvider fileIcon;
        QIcon icon = fileIcon.icon(file);
        item->setIcon(icon);
    }
//    ui->listWidget->addItems(m_listViewList);
}

/**
 * SourceForm::~SourceForm
 * \brief   destructor of SourceForm
 * \author  zhangjun
 * \date    2016-10-12
 */
SourceForm::~SourceForm()
{
    delete ui;
    qDebug() << "~SourceForm()";
}

/**
 * SourceForm::on_pushButton_choice_clicked
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void SourceForm::on_pushButton_choice_clicked()
{
    emit choice(":/" + ui->listWidget->currentItem()->text());
}
