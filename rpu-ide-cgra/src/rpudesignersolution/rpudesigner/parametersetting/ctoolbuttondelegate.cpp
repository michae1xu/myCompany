/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    ctoolbuttondelegate.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "ctoolbuttondelegate.h"
#include <QApplication>
#include <QStyleOptionButton>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayOut>
#include <QToolButton>

/**
 * CToolButtonDelegate::CToolButtonDelegate
 * \brief   constructor of CToolButtonDelegate
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CToolButtonDelegate::CToolButtonDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

/**
 * CToolButtonDelegate::~CToolButtonDelegate
 * \brief   destructor of CToolButtonDelegate
 * \author  zhangjun
 * \date    2016-10-12
 */
CToolButtonDelegate::~CToolButtonDelegate()
{
    qDebug() << "~CToolButtonDelegate()";
}

/**
 * CToolButtonDelegate::setModelData
 * \brief   
 * \param   editor
 * \param   model
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CToolButtonDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() > 3)
    {
        model->setData(index, model->data(index, Qt::EditRole), Qt::EditRole);
    }
    else
        QItemDelegate::setModelData(editor, model, index);
}

/**
 * CToolButtonDelegate::createEditor
 * \brief   
 * \param   parent
 * \param   option
 * \param   index
 * \return  QWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
QWidget *CToolButtonDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() > 3)
    {
        QToolButton *toolButton = new QToolButton(parent);
        toolButton->setText(tr(u8"..."));
        QMenu *pMenu = m_menuMap.value(index);
        if(!pMenu)
        {
            pMenu = new QMenu;
            QAction *actionSrc = new QAction(tr(u8"选择资源..."), pMenu);
            QAction *actionFile = new QAction(tr(u8"选择文件..."), pMenu);
            pMenu->addAction(actionSrc);
            pMenu->addAction(actionFile);

            connect(actionSrc, SIGNAL(triggered()), this, SLOT(srcTriggered()));
            connect(actionFile, SIGNAL(triggered()), this, SLOT(fileTriggered()));
            //            toolButton->setMenu(pMenu);

            (const_cast<CToolButtonDelegate *>(this))->m_menuMap.insert(index, pMenu);
        }
        toolButton->setMenu(pMenu);
        connect(toolButton, SIGNAL(clicked(bool)), toolButton, SLOT(showMenu()));
        return toolButton;
    }
    return  QItemDelegate::createEditor(parent, option, index);
}

/**
 * CToolButtonDelegate::updateEditorGeometry
 * \brief   
 * \param   editor
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CToolButtonDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() > 3)
    {
        QRect rect = option.rect;

        int x = rect.x();
        int y = rect.y();
        int width = rect.width();
        int height = rect.height();
        rect.setRect(x + width - 20, y, 20, height);
//        rect.setRect(x + width * 0.7, y, width * 0.3, height);
        editor->setGeometry(rect);
    }
    else
        QItemDelegate::updateEditorGeometry(editor, option, index);
}

/**
 * CToolButtonDelegate::srcTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void CToolButtonDelegate::srcTriggered()
{
    QDir dir(qApp->applicationDirPath());
    dir.cdUp();
    QString defaultPath = dir.absolutePath() + "/etc";
    QString fileName = QFileDialog::getOpenFileName(0, tr(u8"打开"), defaultPath);
    if(fileName.isEmpty())
        return;

    QMenu *menu = static_cast<QMenu *>(sender()->parent());
    QModelIndex index = m_menuMap.key(menu);
    emit itemChanged(index.row(), index.column(), fileName);

}
