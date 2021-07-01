/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    spinbox.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "spinbox.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include <QLineEdit>
#include <QAction>

/**
 * SpinBox::SpinBox
 * \brief   constructor of SpinBox
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
SpinBox::SpinBox(QWidget *parent)
    :QSpinBox(parent)
{
    m_jumpAction = new QAction(lineEdit());
    m_jumpAction->setIcon(QIcon(CAppEnv::getImageDirPath() + "/enter.png"));
    this->lineEdit()->addAction(m_jumpAction, QLineEdit::TrailingPosition);
    connect(this->lineEdit(), SIGNAL(returnPressed()), m_jumpAction, SIGNAL(triggered()));
    this->setButtonSymbols(QAbstractSpinBox::NoButtons);
    this->setToolTip(tr(u8"运行至周期"));
    this->setFixedSize(80, 20);
    this->setFont(QFont("Arial", 9));
    this->setMaximum(9999999);
    this->setMinimum(0);
}

SpinBox::~SpinBox()
{
}

/**
 * SpinBox::getAction
 * \brief   
 * \return  QAction *
 * \author  zhangjun
 * \date    2016-10-12
 */
QAction *SpinBox::getAction()
{
    return m_jumpAction;
}

void SpinBox::updateValue(int value)
{
    setValue(value);
    setMinimum(value);
}
