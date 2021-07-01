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
#include "ccomboboxdelegate.h"
#include <QComboBox>

/**
 * CComboBoxDelegate::CComboBoxDelegate
 * \brief   constructor of CComboBoxDelegate
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CComboBoxDelegate::CComboBoxDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

/**
 * CComboBoxDelegate::createEditor
 * \brief   
 * \param   parent
 * \param   option
 * \param   index
 * \return  QWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
QWidget *CComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 5 || index.column() == 6)
    {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->setFrame(false);
        QStringList strList;
        for(int i = 0; i < 16; ++i)
            strList.append(tr(u8"MemoryPort%1").arg(i));
        if(index.column() == 5)
        {
            for(int i = 0; i < 4; ++i)
                strList.append(tr(u8"InFIFO%1").arg(i));
        }
        else if(index.column() == 6)
        {
            for(int i = 0; i < 4; ++i)
                strList.append(tr(u8"OutFIFO%1").arg(i));
        }
        comboBox->addItems(strList);
        return comboBox;
    }
    return  QItemDelegate::createEditor(parent, option, index);
}

/**
 * CComboBoxDelegate::setEditorData
 * \brief   
 * \param   editor
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() > 4 && index.column() < 7)
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString str = index.model()->data(index, Qt::DisplayRole).toString();
        comboBox->setCurrentText(str);
    }
    else
        QItemDelegate::setEditorData(editor, index);
}

/**
 * CComboBoxDelegate::setModelData
 * \brief   
 * \param   editor
 * \param   model
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() > 4 && index.column() < 7)
    {
        QComboBox *comboBox  = static_cast<QComboBox*>(editor);
        if (comboBox)
        {
            model->setData(index, comboBox->currentText(), Qt::EditRole);
        }
    }
    else
        QItemDelegate::setModelData(editor, model, index);
}

/**
 * CComboBoxDelegate::updateEditorGeometry
 * \brief   
 * \param   editor
 * \param   option
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void CComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() > 4 && index.column() < 7)
        editor->setGeometry(option.rect);
    else
        QItemDelegate::updateEditorGeometry(editor, option, index);
}
