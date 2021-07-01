/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    bodyiddialog.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QDebug>
#include "form/bodyiddialog.h"
#include "ui_bodyiddialog.h"

/**
 * bodyIdDialog::bodyIdDialog
 * \brief   constructor of bodyIdDialog
 * \param   body
 * \author  zhangjun
 * \date    2016-10-12
 */
bodyIdDialog::bodyIdDialog(ModuleBody *body):
    body(body),
    ui(new Ui::bodyIdDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    setWindowModality(Qt::WindowModal);
    ui->bcuid->setCurrentIndex(body->bcuID);
    ui->rcuid->setCurrentIndex(body->rcuID);
    switch(body->element)
    {
    case ELEMENT_BFU:
    {
        setWindowTitle("BFU");
        ui->eleid->clear();
        for(int i = 0; i<BFUMAX; i++)
        {
            ui->eleid->addItem(QString::number(i));
        }
        break;
    }
    case ELEMENT_SBOX:
    {
        setWindowTitle("SBOX");
        ui->eleid->clear();
        for(int i = 0; i<SBOXMAX; i++)
        {
            ui->eleid->addItem(QString::number(i));
        }
        break;
    }
    case ELEMENT_BENES:
    {
        setWindowTitle("BENES");
        ui->eleid->clear();
        for(int i = 0; i<BENESMAX; i++)
        {
            ui->eleid->addItem(QString::number(i));
        }
        break;
    }
    default:break;
    }

    ui->eleid->setCurrentIndex(body->ID);
}

/**
 * bodyIdDialog::~bodyIdDialog
 * \brief   destructor of bodyIdDialog
 * \author  zhangjun
 * \date    2016-10-12
 */
bodyIdDialog::~bodyIdDialog()
{
    delete ui;
    qDebug() << "~bodyIdDialog()";
}

/**
 * bodyIdDialog::on_button_accepted
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void bodyIdDialog::on_button_accepted()
{
    emit changeID(body, ui->bcuid->currentIndex(), ui->rcuid->currentIndex(), ui->eleid->currentIndex());
}
