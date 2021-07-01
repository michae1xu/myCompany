/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    ccreatecommandwordmodel.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "ccreatecommandwordmodel.h"
#include "capppathenv.h"
#include <QFileInfo>
#include <QColor>

/**
 * CCreateCommandWordModel::CCreateCommandWordModel
 * \brief   constructor of CCreateCommandWordModel
 * \param   cmdWordInfoList
 * \author  zhangjun
 * \date    2016-10-12
 */
CCreateCommandWordModel::CCreateCommandWordModel(QList<SCMDWordInfo *> *cmdWordInfoList)
{
    m_cmdWordInfoList = cmdWordInfoList;
}

/**
 * CCreateCommandWordModel::data
 * \brief   
 * \param   index
 * \param   role
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant CCreateCommandWordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (role == Qt::TextAlignmentRole)
    {
        if(index.column() == STATUS)
            return int(Qt::AlignCenter);
        else
            return int(Qt::AlignVCenter | Qt::AlignLeft);
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
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        QVariant current = currencyAt(index.row(), index.column());
        return current;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == XMLFILE)
        {
            return m_cmdWordInfoList->at(index.row())->selected == Qt::Checked ? Qt::Checked : Qt::Unchecked;
        }
    }
//    else if(role == Qt::BackgroundColorRole )
//    {
//        if(index.row() % 2 == 1)
//            return QColor(240, 240, 240);

//    }
    return QVariant();
}

/**
 * CCreateCommandWordModel::setData
 * \brief   
 * \param   index
 * \param   value
 * \param   role
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CCreateCommandWordModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        SCMDWordInfo *cmdWordInfo = m_cmdWordInfoList->at(row);
        switch (index.column()) {
        case ONLYSIM:
            cmdWordInfo->onlySim = value.toInt();
            break;
        case RMODE0:
            cmdWordInfo->rMode0 = value.toInt();
            break;
        case RMODE1:
            cmdWordInfo->rMode1 = value.toInt();
            break;
        case GAP:
            cmdWordInfo->gap = value.toInt();
            break;
        case SOURCE:
            cmdWordInfo->source = value.toString();
            break;
        case DEST:
            cmdWordInfo->dest = value.toString();
            break;
        case BURSTNUM:
            cmdWordInfo->burstNum = value.toInt();
            break;
        case LOOPNUM:
            cmdWordInfo->loopNum = value.toInt();
            break;
        case REPEATNUM:
            cmdWordInfo->repeatNum = value.toInt();
            break;
        default:
            break;
        }

        m_cmdWordInfoList->replace(row, cmdWordInfo);
        emit dataChanged(index, index);
        return true;
    }

    if (index.isValid() && role == Qt::CheckStateRole)
    {
        SCMDWordInfo *cmdWordInfo = m_cmdWordInfoList->at(index.row());
        if(value == Qt::Checked)
            cmdWordInfo->selected = Qt::Checked;
        else
            cmdWordInfo->selected = Qt::Unchecked;

        m_cmdWordInfoList->replace(index.row(), cmdWordInfo);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

/**
 * CCreateCommandWordModel::rowCount
 * \brief   
 * \param   parent
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int CCreateCommandWordModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_cmdWordInfoList->count();
}

/**
 * CCreateCommandWordModel::columnCount
 * \brief   
 * \param   parent
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int CCreateCommandWordModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 12;
}

/**
 * CCreateCommandWordModel::headerData
 * \brief   
 * \param   section
 * \param   orientation
 * \param   role
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant CCreateCommandWordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal)
    {
        switch (section) {
        case XMLFILE:
            return tr(u8"XML文件");
            break;
        case ONLYSIM:
            return tr(u8"ONLYSIM");
            break;
        case RMODE0:
            return tr(u8"RMODE0");
            break;
        case RMODE1:
            return tr(u8"RMODE1");
            break;
        case GAP:
            return tr(u8"GAP");
            break;
        case SOURCE:
            return tr(u8"SOURCE");
            break;
        case DEST:
            return tr(u8"DEST");
            break;
        case BURSTNUM:
            return tr(u8"BURSTNUM");
            break;
        case LOOPNUM:
            return tr(u8"LOOPNUM");
            break;
        case REPEATNUM:
            return tr(u8"REPEATNUM");
            break;
        case PROGRESS:
            return tr(u8"进度");
            break;
        case STATUS:
            return tr(u8"当前状态");
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

/**
 * CCreateCommandWordModel::flags
 * \brief   
 * \param   index
 * \return  Qt::ItemFlags
 * \author  zhangjun
 * \date    2016-10-12
 */
Qt::ItemFlags CCreateCommandWordModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if(index.column() == XMLFILE)
    {
        flags |= Qt::ItemIsUserCheckable;
        return flags;
    }
    if(index.column() > XMLFILE && index.column() < PROGRESS)
    {
        flags |= Qt::ItemIsEditable;
        return flags;
    }
    return flags;
}

/**
 * CCreateCommandWordModel::currencyAt
 * \brief   
 * \param   row
 * \param   column
 * \return  QVariant
 * \author  zhangjun
 * \date    2016-10-12
 */
QVariant CCreateCommandWordModel::currencyAt(int row, int column) const
{
    QVariant v;
    switch (column) {
    case XMLFILE:
        v = CAppPathEnv::stpToNm(m_cmdWordInfoList->at(row)->xmlFile);
        break;
    case ONLYSIM:
        v = m_cmdWordInfoList->at(row)->onlySim;
        break;
    case RMODE0:
        v = m_cmdWordInfoList->at(row)->rMode0;
        break;
    case RMODE1:
        v = m_cmdWordInfoList->at(row)->rMode1;
        break;
    case GAP:
        v = m_cmdWordInfoList->at(row)->gap;
        break;
    case SOURCE:
        v = m_cmdWordInfoList->at(row)->source;
        break;
    case DEST:
        v = m_cmdWordInfoList->at(row)->dest;
        break;
    case BURSTNUM:
        v = m_cmdWordInfoList->at(row)->burstNum;
        break;
    case LOOPNUM:
        v = m_cmdWordInfoList->at(row)->loopNum;
        break;
    case REPEATNUM:
        v = m_cmdWordInfoList->at(row)->repeatNum;
        break;
    case PROGRESS:
        v = m_cmdWordInfoList->at(row)->progress;
        break;
    case STATUS:
        v = m_cmdWordInfoList->at(row)->status;
        break;
    default:
        break;
    }
    return v;
}

/**
 * CCreateCommandWordModel::stpToNm
 * \brief   
 * \param   fullFileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
//QString CCreateCommandWordModel::stpToNm(const QString &fullFileName) const
//{
//    return QFileInfo(fullFileName).fileName();
//}
