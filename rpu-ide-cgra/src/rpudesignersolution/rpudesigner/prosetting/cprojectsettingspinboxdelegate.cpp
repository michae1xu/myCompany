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
#include <QSpinBox>
#include "datafile.h"
#include "cprojectsettingspinboxdelegate.h"

/**
 * CProjectSettingSpinBoxDelegate::CProjectSettingSpinBoxDelegate
 * \brief   constructor of CProjectSettingSpinBoxDelegate
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CProjectSettingSpinBoxDelegate::CProjectSettingSpinBoxDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

/**
 * CProjectSettingSpinBoxDelegate::createEditor
 * \brief   
 * \param   parent
 * \param   option
 * \param   index
 * \return  QWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
QWidget *CProjectSettingSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() > ProjectSettingInMemoryPath4 && index.column() < ProjectSettingGap)
    {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setFrame(false);
        spinBox->setMinimum(0);
        spinBox->setMaximum(1);
        return spinBox;
    }
    else if((index.column() > ProjectSettingDest && index.column() <= projectSettingLFSRGroup) || index.column() == ProjectSettingGap)
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
 * CProjectSettingSpinBoxDelegate::setEditorData
 * \brief   
 * \param   editor
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectSettingSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if((index.column() > ProjectSettingInMemoryPath4 && index.column() < ProjectSettingSource)
            || (index.column() > ProjectSettingDest && index.column() <= projectSettingLFSRGroup))
    {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(index.model()->data(index,Qt::DisplayRole).toInt());
    }
    else
        QItemDelegate::setEditorData(editor, index);
}

/**
 * CProjectSettingSpinBoxDelegate::setModelData
 * \brief   
 * \param   editor
 * \param   model
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectSettingSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if((index.column() > ProjectSettingInMemoryPath4 && index.column() < ProjectSettingSource)
            || (index.column() > ProjectSettingDest && index.column() <= projectSettingLFSRGroup))
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
 * CProjectSettingSpinBoxDelegate::updateEditorGeometry
 * \brief   
 * \param   editor
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectSettingSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if((index.column() > ProjectSettingInMemoryPath4 && index.column() < ProjectSettingSource)
            || (index.column() > ProjectSettingDest && index.column() <= projectSettingLFSRGroup))
        editor->setGeometry(option.rect);
    else
        QItemDelegate::updateEditorGeometry(editor, option, index);
}
