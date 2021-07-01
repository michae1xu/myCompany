/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    debugwindow.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "debugwindow.h"
#include "ui_debugwindow.h"
#include "cfgdebuglog/CfgDebugLog.h"

/**
 * DebugWindow::DebugWindow
 * \brief   constructor of DebugWindow
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
DebugWindow::DebugWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);

    ui->menubar->setVisible(false);
    ui->statusbar->setVisible(false);
}

/**
 * DebugWindow::~DebugWindow
 * \brief   destructor of DebugWindow
 * \author  zhangjun
 * \date    2016-10-12
 */
DebugWindow::~DebugWindow()
{
    delete ui;
}
