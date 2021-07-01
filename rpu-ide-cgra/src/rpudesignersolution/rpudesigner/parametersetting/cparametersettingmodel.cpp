/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    cparametersettingmodel.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "cparametersettingmodel.h"
#include "xmlresolver\xmlresolver.h"
#include "capppathenv.h"
#include <QIcon>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QBrush>
/**
 * CParameterSettingModel::CParameterSettingModel
 * \brief   constructor of CParameterSettingModel
 * \param   list
 * \param   QString
 * \param   sourceMap
 * \author  zhangjun
 * \date    2016-10-12
 */
CParameterSettingModel::CParameterSettingModel(QList<SParaInfo *> *list, QMap<QString, QString> sourceMap)
{
    m_sourceMap = sourceMap;
    m_paraInfoList = list;
}

/**
 * CParameterSettingModel::~CParameterSettingModel
 * \brief   destructor of CParameterSettingModel
 * \author  zhangjun
 * \date    2016-10-12
 */
CParameterSettingModel::~CParameterSettingModel()
{
    qDebug() << "~CParameterSettingModel()";
}

/**
 * CParameterSettingModel::data
 * \brief   
 * \param   index
 * \param   role
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant CParameterSettingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignVCenter | Qt::AlignLeft);
    }
    if (role == Qt::ForegroundRole)
    {
        QString fileName = index.data(Qt::EditRole).toString();
        if((index.column() > INTERVAL || index.column() == XMLFNP) && !checkFileExists(fileName))
        {
            return QBrush(QColor(255, 0, 0));
        }
//        else
//            return QBrush(Qt::black);
    }
//    else if(role == Qt::DecorationRole)
//    {
//        QVariant v;
//        int column = index.column();
//        if(column == 0)
//        {
//            QIcon icon(":images/xml.png");
//            v = icon;
//        }
//        return v;

//    }
    else if (role == Qt::DisplayRole)
    {
        QVariant current;
        if(index.column() == PROJECT || index.column() == CYCLE || index.column() == INTERVAL)
        {
            current = currencyAt(index.row(), index.column());
            if(index.column() == PROJECT && current.toBool())
                current = tr(u8"是");
            else if(index.column() == PROJECT)
                current = tr(u8"否");
        }
        else
        {
            QString str = currencyAt(index.row(), index.column()).toString();
            str = CAppPathEnv::stpToNm(str);
            current = str;
        }
        return current;
    }
    else if (role == Qt::EditRole)
    {
        QVariant current = currencyAt(index.row(), index.column());
        return current;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == XMLFNP)
        {
            return m_paraInfoList->at(index.row())->selected == Qt::Checked ? Qt::Checked : Qt::Unchecked;
        }
    }
    else if(role == Qt::BackgroundColorRole )
    {
        if(index.row() % 2 == 1)
            return QColor(240, 240, 240);

    }
    return QVariant();

}

/**
 * CParameterSettingModel::setData
 * \brief   
 * \param   index
 * \param   value
 * \param   role
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CParameterSettingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        SParaInfo *paraInfo = m_paraInfoList->at(row);
        switch (index.column()) {
        case CYCLE:
            paraInfo->cycle = value.toInt();
            break;
        case INTERVAL:
            paraInfo->triggerInterval = value.toInt();
//            for(int i = 0; i < m_paraInfoList->count(); ++i)
//            {
//                SParaInfo *paraInfo2 = m_paraInfoList->at(i);
//                paraInfo2->triggerInterval = value.toInt();
//            }
            break;
        case INFIFOFNP0:
            paraInfo->inputFifoFNP0 = value.toString().trimmed();
            break;
        case INFIFOFNP1:
            paraInfo->inputFifoFNP1 = value.toString().trimmed();
            break;
        case INFIFOFNP2:
            paraInfo->inputFifoFNP2 = value.toString().trimmed();
            break;
        case INFIFOFNP3:
            paraInfo->inputFifoFNP3 = value.toString().trimmed();
            break;
        case INMEMFNP0:
            paraInfo->inMemFNP0 = value.toString().trimmed();
            break;
        case INMEMFNP1:
            paraInfo->inMemFNP1 = value.toString().trimmed();
            break;
        case INMEMFNP2:
            paraInfo->inMemFNP2 = value.toString().trimmed();
            break;
        case INMEMFNP3:
            paraInfo->inMemFNP3 = value.toString().trimmed();
            break;
        case INMEMFNP4:
            paraInfo->inMemFNP4 = value.toString().trimmed();
            break;
        default:
            break;
        }

//        m_paraInfoList->replace(row, paraInfo);
        emit dataChanged(index, index);
        return true;
    }

    if (index.isValid() && role == Qt::CheckStateRole)
    {
        SParaInfo *paraInfo = m_paraInfoList->at(index.row());
        if(value == Qt::Checked)
            paraInfo->selected = Qt::Checked;
        else
            paraInfo->selected = Qt::Unchecked;

//        m_paraInfoList->replace(index.row(), paraInfo);
        emit dataChanged(index, index);
        return true;
    }
    return false;

}

/**
 * CParameterSettingModel::rowCount
 * \brief   
 * \param   parent
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int CParameterSettingModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_paraInfoList->count();

}

/**
 * CParameterSettingModel::columnCount
 * \brief   
 * \param   parent
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int CParameterSettingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return INMEMFNP4 + 1;
}



/**
 * CParameterSettingModel::flags
 * \brief   
 * \param   index
 * \return  Qt::ItemFlags
 * \author  zhangjun
 * \date    2016-10-12
 */
Qt::ItemFlags CParameterSettingModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    flags |= Qt::ItemIsEnabled;
    flags |= Qt::ItemIsSelectable;
    if(index.column() == XMLFNP)
    {
        flags |= Qt::ItemIsUserCheckable;
        return flags;
    }
    if(index.column() > PROJECT)
        flags |= Qt::ItemIsEditable;
    return flags;
}

//void CParameterSettingModel::beginReset()
//{
//    beginResetModel();
//}

//void CParameterSettingModel::endReset()
//{
//    endResetModel();
//}

/**
 * CParameterSettingModel::currencyAt
 * \brief   
 * \param   row
 * \param   column
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant CParameterSettingModel::currencyAt(int row, int column) const
{
    QVariant v;
    switch (column) {
    case XMLFNP:
        v = m_paraInfoList->at(row)->xmlFNP;
        break;
    case PROJECT:
        v = m_paraInfoList->at(row)->project;
        break;
    case CYCLE:
        v = m_paraInfoList->at(row)->cycle;
        break;
    case INTERVAL:
        v = m_paraInfoList->at(row)->triggerInterval;
        break;
    case INFIFOFNP0:
        v = m_paraInfoList->at(row)->inputFifoFNP0;
        break;
    case INFIFOFNP1:
        v = m_paraInfoList->at(row)->inputFifoFNP1;
        break;
    case INFIFOFNP2:
        v = m_paraInfoList->at(row)->inputFifoFNP2;
        break;
    case INFIFOFNP3:
        v = m_paraInfoList->at(row)->inputFifoFNP3;
        break;
    case INMEMFNP0:
        v = m_paraInfoList->at(row)->inMemFNP0;
        break;
    case INMEMFNP1:
        v = m_paraInfoList->at(row)->inMemFNP1;
        break;
    case INMEMFNP2:
        v = m_paraInfoList->at(row)->inMemFNP2;
        break;
    case INMEMFNP3:
        v = m_paraInfoList->at(row)->inMemFNP3;
        break;
    case INMEMFNP4:
        v = m_paraInfoList->at(row)->inMemFNP4;
        break;
    default:
        break;
    }
    return v;
}

/**
 * CParameterSettingModel::checkFileExists
 * \brief   
 * \param   fileName
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CParameterSettingModel::checkFileExists(const QString fileName) const
{
    if(fileName.startsWith(":"))
    {
        return QFileInfo(m_sourceMap.value(fileName)).exists();
    }
    else
    {
        return QFileInfo(fileName).exists();
    }
}

/**
 * CParameterSettingModel::stpToNm
 * \brief   
 * \param   fullFileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
//QString CParameterSettingModel::stpToNm(const QString &fullFileName) const
//{
//    return QFileInfo(fullFileName).fileName();
//}

/**
 * CParameterSettingModel::headerData
 * \brief   
 * \param   section
 * \param   orientation
 * \param   role
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant CParameterSettingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal)
    {
        switch (section) {
        case XMLFNP:
            return tr(u8"XML文件");
            break;
        case PROJECT:
            return tr(u8"项目");
            break;
        case CYCLE:
            return tr(u8"周期");
            break;
        case INTERVAL:
            return tr(u8"触发周期");
            break;
        case INFIFOFNP0:
            return tr(u8"IN FIFO 0");
            break;
        case INFIFOFNP1:
            return tr(u8"IN FIFO 1");
            break;
        case INFIFOFNP2:
            return tr(u8"IN FIFO 2");
            break;
        case INFIFOFNP3:
            return tr(u8"IN FIFO 3");
            break;
        case INMEMFNP0:
            return tr(u8"IN MEM 0");
            break;
        case INMEMFNP1:
            return tr(u8"IN MEM 1");
            break;
        case INMEMFNP2:
            return tr(u8"IN MEM 2");
            break;
        case INMEMFNP3:
            return tr(u8"IN MEM 3");
            break;
        case INMEMFNP4:
            return tr(u8"IN MEM 4");
            break;
        default:
            break;
        }
    }
    else
    {
        return section + 1;
    }
    return QVariant();

}
