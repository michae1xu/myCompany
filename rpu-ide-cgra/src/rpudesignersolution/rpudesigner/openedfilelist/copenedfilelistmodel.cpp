/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    copenedfilelistmodel.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "copenedfilelistmodel.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include <QFileInfo>
#include <QIcon>
#include <QVariant>

/**
 * COpenedFileListModel::COpenedFileListModel
 * \brief   constructor of COpenedFileListModel
 * \param   openedFileList
 * \author  zhangjun
 * \date    2016-10-12
 */
COpenedFileListModel::COpenedFileListModel(QStringList *openedFileList, QObject *parent) :
    QStringListModel(parent)
{
    m_openedFileList = openedFileList;
}

COpenedFileListModel::~COpenedFileListModel()
{
}

/**
 * COpenedFileListModel::data
 * \brief   
 * \param   index
 * \param   role
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant COpenedFileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignVCenter | Qt::AlignLeft);
    }
    else if(role == Qt::DecorationRole)
    {
        QVariant v;
        QIcon icon(CAppEnv::getImageDirPath() + "/Xml-tool128.png");
        v = icon;

        return v;
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        QVariant current;
        current = CAppEnv::stpToNm(m_openedFileList->at(index.row()));
        return current;
    }
    else if(role == Qt::BackgroundColorRole )
    {
        if(index.row() % 2 == 1)
            return QColor(240, 240, 240);

    }
    return QVariant();

}

/**
 * COpenedFileListModel::rowCount
 * \brief   
 * \param   parent
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int COpenedFileListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_openedFileList->count();
}

/**
 * COpenedFileListModel::flags
 * \brief   
 * \param   index
 * \return  Qt::ItemFlags
 * \author  zhangjun
 * \date    2016-10-12
 */
Qt::ItemFlags COpenedFileListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags/* = QAbstractItemModel::flags(index)*/;

    flags |= Qt::ItemIsEnabled;
    flags |= Qt::ItemIsSelectable;

    return flags;
}

/**
 * COpenedFileListModel::stpToNm
 * \brief   
 * \param   fullFileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
//QString COpenedFileListModel::stpToNm(QString fullFileName) const
//{
//    return QFileInfo(fullFileName).fileName();
//}
