/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    ccreatecommandworddetegate.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "ccreatecommandworddetegate.h"
#include <QApplication>
#include <QProgressBar>
#include <QPainter>

/**
 * CCreateCommandWordDetegate::CCreateCommandWordDetegate
 * \brief   constructor of CCreateCommandWordDetegate
 * \param   cmdWordInfoList
 * \author  zhangjun
 * \date    2016-10-12
 */
CCreateCommandWordDetegate::CCreateCommandWordDetegate(QList<SCMDWordInfo *> *cmdWordInfoList)
{
    m_cmdWordInfoList = cmdWordInfoList;
}

/**
 * CCreateCommandWordDetegate::createEditor
 * \brief   
 * \param   parent
 * \param   option
 * \param   index
 * \return  QWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
QWidget *CCreateCommandWordDetegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

/**
 * CCreateCommandWordDetegate::setEditorData
 * \brief   
 * \param   editor
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCreateCommandWordDetegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

/**
 * CCreateCommandWordDetegate::setModelData
 * \brief   
 * \param   editor
 * \param   model
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCreateCommandWordDetegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

/**
 * CCreateCommandWordDetegate::updateEditorGeometry
 * \brief   
 * \param   editor
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCreateCommandWordDetegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

/**
 * CCreateCommandWordDetegate::paint
 * \brief   
 * \param   painter
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCreateCommandWordDetegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 10)
    {
        int value = index.model()->data(index).toInt();
        QStyleOptionProgressBarV2 progressBarOption;
        progressBarOption.rect = option.rect.adjusted(1, 2, -1, -2);
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textAlignment = Qt::AlignHCenter;
        progressBarOption.textVisible = true;
        progressBarOption.progress = value;
        progressBarOption.text = tr(u8"%1%").arg(progressBarOption.progress);

        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setBrush(option.palette.highlightedText());
        }
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
        painter->restore();

    }
    else
    {
        QStyledItemDelegate::paint (painter, option, index);
    }
}
