/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    deletexmlform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "ui_deletexmlform.h"
#include "common/debug.h"
#include "deletexmlform.h"
#include "common/cappenv.hpp"

/**
 * DeleteXmlForm::DeleteXmlForm
 * \brief   constructor of DeleteXmlForm
 * \param   fileName
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
DeleteXmlForm::DeleteXmlForm(const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteXmlForm)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    ui->label->setText(fileName);
    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

/**
 * DeleteXmlForm::~DeleteXmlForm
 * \brief   destructor of DeleteXmlForm
 * \author  zhangjun
 * \date    2016-10-12
 */
DeleteXmlForm::~DeleteXmlForm()
{
    delete ui;
//    DBG << "~DeleteXmlForm()";
}

/**
 * DeleteXmlForm::getChecked
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool DeleteXmlForm::getChecked() const
{
    return ui->checkBox->isChecked();
}
