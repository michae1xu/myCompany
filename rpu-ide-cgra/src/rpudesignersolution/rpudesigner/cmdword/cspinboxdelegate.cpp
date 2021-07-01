/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    cspinboxdelegate.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "cspinboxdelegate.h"
#include <QSpinBox>

/**
 * CSpinBoxDelegate::CSpinBoxDelegate
 * \brief   constructor of CSpinBoxDelegate
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CSpinBoxDelegate::CSpinBoxDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

/**
 * CSpinBoxDelegate::createEditor
 * \brief   
 * \param   parent
 * \param   option
 * \param   index
 * \return  QWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
QWidget *CSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() > 0 && index.column() < 4)
    {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setFrame(false);
        spinBox->setMinimum(0);
        spinBox->setMaximum(1);
        return spinBox;
    }
    else if((index.column() > 6 && index.column() < 10) || index.column() == 4)
    {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setFrame(false);
        spinBox->setMinimum(0);
        spinBox->setMaximum(255);
        return spinBox;
    }
    return  QItemDelegate::createEditor(parent, option, index);
}

/**
 * CSpinBoxDelegate::setEditorData
 * \brief   
 * \param   editor
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if((index.column() > 0 && index.column() < 5) || (index.column() > 6 && index.column() < 10))
    {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(index.model()->data(index,Qt::DisplayRole).toInt());
    }
    else
        QItemDelegate::setEditorData(editor, index);
}

/**
 * CSpinBoxDelegate::setModelData
 * \brief   
 * \param   editor
 * \param   model
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if((index.column() > 0 && index.column() < 5) || (index.column() > 6 && index.column() < 10))
    {
        QSpinBox *spinBox  = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }
    else
        QItemDelegate::setModelData(editor, model, index);
}

/**
 * CSpinBoxDelegate::updateEditorGeometry
 * \brief   
 * \param   editor
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if((index.column() > 0 && index.column() < 5) || (index.column() > 6 && index.column() < 10))
        editor->setGeometry(option.rect);
    else
        QItemDelegate::updateEditorGeometry(editor, option, index);
}
