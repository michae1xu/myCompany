/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    toolbar.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "toolbar.h"
#include "spinbox.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include <QMouseEvent>
#include <QFile>
#include <QLayout>
#include <QSpinBox>
#include <QImage>
#include <QLabel>
#include <QComboBox>
#include <QList>
#include <QFileInfo>

/**
 * ToolBar::ToolBar
 * \brief   constructor of ToolBar
 * \param   paraInfoList
 * \author  zhangjun
 * \date    2016-10-12
 */
ToolBar::ToolBar(QWidget *parent) :
    QToolBar(parent)
{
//#if defined(Q_OS_LINUX)
//    this->setWindowFlags(Qt::Tool);
//#elif defined(Q_OS_WIN)
//    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
//#endif
    setIconSize(iconSize()*2/3);
    QFile file(":qss/toolbar.qss");
    file.open(QFile::ReadOnly);
    this->setStyleSheet(file.readAll());
//    setFixedHeight(22);

//    layout()->setSpacing(10);

//    QImage image;
//    image.load(CAppEnv::getImageDirPath() + "/debug.png");
//    image = image.scaled(18, 18, Qt::KeepAspectRatio);
//    QLabel *labelPic = new QLabel(this);
////    labelPic->setToolTip(tr(u8"调试悬浮窗"));
//    labelPic->setPixmap(QPixmap::fromImage(image));
//    this->addWidget(labelPic);

    QLabel *labelText = new QLabel(this);
    labelText->setText(tr(u8"Debugger"));
    labelText->setIndent(10);
    this->addWidget(labelText);

//    this->addSeparator();

    m_comboBox = new QComboBox(this);
    m_comboBox->setToolTip(tr(u8"xml文件"));
    m_comboBox->setMaxVisibleItems(10);
    m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    this->addWidget(m_comboBox);

//    QLabel *label = new QLabel(this);
//    this->addWidget(label);

    m_spinBox = new SpinBox(this);
    m_spinBox->setToolTip(tr(u8"周期数"));
    this->addWidget(m_spinBox);
}

ToolBar::~ToolBar()
{
}

/**
 * ToolBar::getSpinBox
 * \brief   
 * \return  SpinBox *
 * \author  zhangjun
 * \date    2016-10-12
 */
SpinBox *ToolBar::getSpinBox() const
{
    return m_spinBox;
}

/**
 * ToolBar::getComboBox
 * \brief   
 * \return  QComboBox *
 * \author  zhangjun
 * \date    2016-10-12
 */
QComboBox *ToolBar::getComboBox() const
{
    return m_comboBox;
}

/**
 * ToolBar::getComboBoxStrList
 * \brief   
 * \return  QStringList
 * \author  zhangjun
 * \date    2016-10-12
 */
QStringList ToolBar::getComboBoxStrList() const
{
    return m_comboBoxStrList;
}

//void ToolBar::setParaInfo(const QStringList &strList)
//{
//    m_comboBoxStrList = strList;
//    m_comboBox->clear();
//    m_comboBox->addItems(m_comboBoxStrList);
//    m_comboBox->setCurrentIndex(0);
//}

///**
// * ToolBar::mousePressEvent
// * \brief
// * \param   mouseEvent
// * \author  zhangjun
// * \date    2016-10-12
// */
//void ToolBar::mousePressEvent(QMouseEvent *mouseEvent)
//{
//    if (mouseEvent->button() != Qt::LeftButton)
//        return;
//    m_moveFlag = true;
//    m_mainPos = this->pos();
//    m_mousePos = mouseEvent->globalPos();

//}

///**
// * ToolBar::mouseReleaseEvent
// * \brief
// * \param   mouseEvent
// * \author  zhangjun
// * \date    2016-10-12
// */
//void ToolBar::mouseReleaseEvent(QMouseEvent *mouseEvent)
//{
//    if (mouseEvent->button() != Qt::LeftButton)
//        return;
//    if(!m_moved)
//        emit toolBarClicked();
//    m_moved = false;
//    m_moveFlag = false;
////    this->adjustSize();
//}

///**
// * ToolBar::mouseMoveEvent
// * \brief
// * \param   mouseEvent
// * \author  zhangjun
// * \date    2016-10-12
// */
//void ToolBar::mouseMoveEvent(QMouseEvent *mouseEvent)
//{
//    QPoint pos = mouseEvent->globalPos();

//    if (m_moveFlag)
//    {
//        int x = pos.x() - m_mousePos.x();
//        int y = pos.y() - m_mousePos.y();
//        this->move(m_mainPos.x() + x ,m_mainPos.y() + y);
//        m_moved = true;
//    }
//}
