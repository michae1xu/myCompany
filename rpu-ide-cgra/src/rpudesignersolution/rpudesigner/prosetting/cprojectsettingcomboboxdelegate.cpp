/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    ccomboboxdelegate.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "cprojectsettingcomboboxdelegate.h"
#include "datafile.h"
#include <QComboBox>

/**
 * CProjectSettingComboBoxDelegate::CComboBoxDelegate
 * \brief   constructor of CComboBoxDelegate
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CProjectSettingComboBoxDelegate::CProjectSettingComboBoxDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

/**
 * CProjectSettingComboBoxDelegate::createEditor
 * \brief   
 * \param   parent
 * \param   option
 * \param   index
 * \return  QWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
QWidget *CProjectSettingComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == ProjectSettingSource || index.column() == ProjectSettingDest)
    {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->setFrame(false);
        QStringList strList;
        for(int i = 0; i < 16; ++i)
            strList.append(tr(u8"MemoryPort%0").arg(i));
        if(index.column() == ProjectSettingSource)
        {
            for(int i = 0; i < 4; ++i)
                strList.append(tr(u8"InFIFO%0").arg(i));
        }
        else if(index.column() == ProjectSettingDest)
        {
            for(int i = 0; i < 4; ++i)
                strList.append(tr(u8"OutFIFO%0").arg(i));
        }
        comboBox->addItems(strList);
        return comboBox;
    }
    else if(index.column() == projectSettingLFSRGroup)
    {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->setFrame(false);
        QStringList strList = {"未分组", "Group0", "Group1", "Group2", "Group3"};
        comboBox->addItems(strList);
        return comboBox;
    }
    return  QItemDelegate::createEditor(parent, option, index);
}

/**
 * CProjectSettingComboBoxDelegate::setEditorData
 * \brief   
 * \param   editor
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectSettingComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == ProjectSettingSource || index.column() == ProjectSettingDest)
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString str = index.model()->data(index, Qt::DisplayRole).toString();
        comboBox->setCurrentText(str);
    }
    else if(index.column() == projectSettingLFSRGroup)
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString str = index.model()->data(index, Qt::DisplayRole).toString();
        comboBox->setCurrentText(str);
    }
    else
        QItemDelegate::setEditorData(editor, index);
}

/**
 * CProjectSettingComboBoxDelegate::setModelData
 * \brief   
 * \param   editor
 * \param   model
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectSettingComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == ProjectSettingSource || index.column() == ProjectSettingDest)
    {
        QComboBox *comboBox  = static_cast<QComboBox*>(editor);
        if (comboBox)
        {
            model->setData(index, comboBox->currentText(), Qt::EditRole);
        }
    }
    else if(index.column() == projectSettingLFSRGroup)
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString str = index.model()->data(index, Qt::DisplayRole).toString();
        comboBox->setCurrentText(str);
    }
    else
        QItemDelegate::setModelData(editor, model, index);
}

/**
 * CProjectSettingComboBoxDelegate::updateEditorGeometry
 * \brief   
 * \param   editor
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectSettingComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == ProjectSettingSource || index.column() == ProjectSettingDest || index.column() == projectSettingLFSRGroup)
        editor->setGeometry(option.rect);
    else
        QItemDelegate::updateEditorGeometry(editor, option, index);
}
