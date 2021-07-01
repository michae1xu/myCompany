/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    addtoprojectform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "addtoprojectform.h"
#include "ui_addtoprojectform.h"

/**
 * AddToProjectForm::AddToProjectForm
 * \brief   constructor of AddToProjectForm
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
AddToProjectForm::AddToProjectForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddToProjectForm)
{
    ui->setupUi(this);
}

/**
 * AddToProjectForm::~AddToProjectForm
 * \brief   destructor of AddToProjectForm
 * \author  zhangjun
 * \date    2016-10-12
 */
AddToProjectForm::~AddToProjectForm()
{
    delete ui;
}
