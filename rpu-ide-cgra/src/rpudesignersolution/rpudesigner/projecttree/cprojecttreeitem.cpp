/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    cprojecttreeitem.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QIcon>
#include <QFileInfo>
#include "cprojecttreeitem.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

/**
 * CProjectTreeItem::CProjectTreeItem
 * \brief   constructor of CProjectTreeItem
 * \param   itemType
 * \param   path
 * \param   isCurPro
 * \author  zhangjun
 * \date    2016-10-12
 */
CProjectTreeItem::CProjectTreeItem(EItemType itemType, const QString &path, bool isCurPro)
    : m_itemType(itemType),
      m_fileName(path),
      m_isCurPro(isCurPro)
{
    QIcon icon;
    QString name;
    switch (m_itemType) {
    case ITEMSOLUTION:
        name = QFileInfo(path).completeBaseName();
        icon = QIcon(CAppEnv::getImageDirPath() + "/128/Inventory-maintenance128.png");
        break;
    case ITEMSOLUTIONFILE:
        name = QFileInfo(path).fileName();
//        icon = QIcon(CAppEnv::getImageDirPath() + "/128/paperclip128.png");
        icon = QIcon(CAppEnv::getImageDirPath() + "/128/Reports128.png");
        break;
    case ITEMPROJECT:
        name = QFileInfo(path).completeBaseName();
        icon = QIcon(CAppEnv::getImageDirPath() + "/project.png");
        break;
    case ITEMPROJECTFILE:
        name = QFileInfo(path).fileName();
        icon = QIcon(CAppEnv::getImageDirPath() + "/128/Product-sale-report128.png");
        this->setToolTip(path);
        break;
    case ITEMXMLFOLDER:
        name = tr(u8"XML文件");
        icon = QIcon(CAppEnv::getImageDirPath() + "/xmldir.png");
        break;
    case ITEMXMLFILE:
        name = QFileInfo(path).fileName();
        icon = QIcon(CAppEnv::getImageDirPath() + "/128/Xml-tool128.png");
        this->setToolTip(path);
        break;
    case ITEMSRCFOLDER:
        name = tr(u8"资源文件");
//        icon = QIcon(CAppEnv::getImageDirPath() + "/128/addons128.png");
        icon = QIcon(CAppEnv::getImageDirPath() + "/srcdir.png");
        break;
    case ITEMSRCFILE:
    {
        name = QFileInfo(path).fileName();
        QString suffix = QFileInfo(path).suffix();
        if(QFileInfo(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower()).exists())
            icon = QIcon(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower());
        else
            icon = QIcon(CAppEnv::getImageDirPath() + "/128/puzzle128.png");
        this->setToolTip(path);
    }
        break;
    default:
        break;
    }
    this->setIcon(icon);
    this->setText(name);
}

/**
 * CProjectTreeItem::~CProjectTreeItem
 * \brief   destructor of CProjectTreeItem
 * \author  zhangjun
 * \date    2016-10-12
 */
CProjectTreeItem::~CProjectTreeItem()
{
}

/**
 * CProjectTreeItem::setCurPro
 * \brief   
 * \param   flag
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectTreeItem::setCurPro()
{
    m_isCurPro = true;
    if(m_itemType == ITEMSOLUTION || m_itemType == ITEMPROJECT)
    {
        QFont font = this->font();
        font.setBold(m_isCurPro);
        this->setFont(font);
    }
}

void CProjectTreeItem::lostCurPro()
{
    m_isCurPro = false;
    if(m_itemType == ITEMSOLUTION || m_itemType == ITEMPROJECT)
    {
        QFont font = this->font();
        font.setBold(m_isCurPro);
        this->setFont(font);
    }
}

void CProjectTreeItem::addProject(const QString &fileName)
{
    Q_UNUSED(fileName);
    if(m_itemType != ITEMSOLUTION)
        return;
}

void CProjectTreeItem::removeProject(const QString &fileName)
{
    Q_UNUSED(fileName);
    if(m_itemType != ITEMSOLUTION)
        return;
}

void CProjectTreeItem::renameProject(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);
    Q_UNUSED(newName);
    if(m_itemType != ITEMSOLUTION)
        return;
}

void CProjectTreeItem::save()
{
    if(m_itemType != ITEMSOLUTION)
        return;
}

QStringList CProjectTreeItem::getProjectList() const
{
    if(m_itemType != ITEMSOLUTION)
        return QStringList();
    return QStringList();
}

/**
 * CProjectTreeItem::getCurPro
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CProjectTreeItem::isCurPro() const
{
    return m_isCurPro;
}

/**
 * CProjectTreeItem::setPath
 * \brief   
 * \param   str
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectTreeItem::setFileName(const QString &str)
{
    m_fileName = str;
}

/**
 * CProjectTreeItem::getFileName
 * \brief   
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString CProjectTreeItem::getFileName() const
{
    return m_fileName;
}

/**
 * CProjectTreeItem::getItemType
 * \brief   
 * \return  EItemType
 * \author  zhangjun
 * \date    2016-10-12
 */
EItemType CProjectTreeItem::getItemType() const
{
    return m_itemType;
}

CProjectTreeItem* CProjectTreeItem::clone() const
{
    CProjectTreeItem *item = new CProjectTreeItem(m_itemType, m_fileName, m_isCurPro);
    return item;
}
