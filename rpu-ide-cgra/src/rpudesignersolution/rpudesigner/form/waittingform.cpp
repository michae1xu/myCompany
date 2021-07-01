/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    waittingform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QTimerEvent>
#include <QMovie>
#include <QMutexLocker>
#include <QKeyEvent>
#include "waittingform.h"
#include "ui_waittingform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CWaitDialog* CWaitDialog::m_inst = nullptr;
QMutex CWaitDialog::m_mutex;

/**
 * dlg_wait::dlg_wait
 * \brief   constructor of dlg_wait
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
dlg_wait::dlg_wait(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint/* | Qt::WindowStaysOnTopHint*/),
    ui(new Ui::dlg_wait)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
//    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(windowFlags() | Qt::Tool);
    this->setWindowModality(Qt::WindowModal);
//    this->setModal(true);
    this->setWindowOpacity(0.9);

    QMovie *mv = new QMovie(CAppEnv::getImageDirPath() + "/waitting.gif", "", ui->label_pic);
    ui->label_pic->setMovie(mv);
//    ui->label_text->setVisible(false);
    mv->start();
}

/**
 * dlg_wait::~dlg_wait
 * \brief   destructor of dlg_wait
 * \author  zhangjun
 * \date    2016-10-12
 */
dlg_wait::~dlg_wait()
{
    delete ui;
}

void dlg_wait::setText(const QString &str)
{
    if(str.isEmpty())
        ui->label_info->clear();
    else
        ui->label_info->setText(str);
}

void dlg_wait::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void dlg_wait::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();
}

void dlg_wait::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

/**
 * CWaitDialog::CWaitDialog
 * \brief   constructor of CWaitDialog
 * \author  zhangjun
 * \date    2016-10-12
 */
CWaitDialog::CWaitDialog(QWidget *parent) :
    QObject(parent)
{
    m_dlg = new dlg_wait(parent);
    m_currTimerId = -1;
}

/**
 * CWaitDialog::~CWaitDialog
 * \brief   destructor of CWaitDialog
 * \author  zhangjun
 * \date    2016-10-12
 */
CWaitDialog::~CWaitDialog()
{
    if(!m_dlg.isNull())
        m_dlg.data()->close();
}

void CWaitDialog::setText(const QString &str)
{
    if(!m_dlg.isNull())
    {
        m_dlg->setText(str);
    }
}

/**
 * CWaitDialog::timerEvent
 * \brief   
 * \param   ev
 * \author  zhangjun
 * \date    2016-10-12
 */
void CWaitDialog::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == m_currTimerId)
    {
//        m_dlg.data()->setWindowModality(Qt::WindowModal);
        if(!m_dlg.isNull())
        {
            m_dlg->show();
//            m_dlg->raise();
//            m_dlg->activateWindow();
        }
    }
}

/**
 * CWaitDialog::getInstance
 * \brief   
 * \return  CWaitDialog *
 * \author  zhangjun
 * \date    2016-10-12
 */
CWaitDialog *CWaitDialog::getInstance(QWidget *parent)
{
    if(m_inst == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if(m_inst == nullptr)
            m_inst = new CWaitDialog(parent);
    }
    return m_inst;
}

/**
 * CWaitDialog::releaseInstance
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void CWaitDialog::releaseInstance()
{
    if(m_inst)
    {
        QMutexLocker locker(&m_mutex);
        if(m_inst)
        {
            delete m_inst;
            m_inst = nullptr;
        }
    }
}

/**
 * CWaitDialog::readyToShowDlg
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void CWaitDialog::readyToShowDlg()
{
    if(getInstance()->m_currTimerId > 0)
    {
//        finishShowing();
        return;
    }
    if(!getInstance()->m_dlg.isNull())
    {
        getInstance()->setText();
    }
    getInstance()->m_currTimerId = getInstance()->startTimer(800);
}

/**
 * CWaitDialog::finishShowing
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void CWaitDialog::finishShowing()
{
    if(getInstance()->m_currTimerId < 0)
        return;
    getInstance()->killTimer(getInstance()->m_currTimerId);
    getInstance()->m_currTimerId = -1;
    if(!getInstance()->m_dlg.isNull())
    {
        getInstance()->m_dlg.data()->setVisible(false);
//        getInstance()->m_dlg.data()->close();
        getInstance()->m_dlg.data()->setWindowModality(Qt::NonModal);
    }
}
