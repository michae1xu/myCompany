#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "cprojecttreeitemmodel.h"
#include "fileresolver/cprojectresolver.h"
#include "projecttree/cprojecttreeitem.h"
#include "projecttree/csolutionnodetreeitem.h"
#include "projecttree/cprojectfoldernodetreeitem.h"

CProjectTreeItemModel::CProjectTreeItemModel(QObject *parent) :
    QStandardItemModel(parent)
{
    connect(this, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(projectTreeItemChanged(QStandardItem*)));
}

CProjectTreeItemModel::~CProjectTreeItemModel()
{
    DBG << "~CProjectTreeItemModel()";
}

bool CProjectTreeItemModel::appendSolution(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

    foreach (QString str, getSolutionList()) {
#if defined(Q_OS_WIN)
            if(QFileInfo(str).completeBaseName().toLower()
                    == QFileInfo(fileName).completeBaseName().toLower())
#elif defined(Q_OS_UNIX)
            if(QFileInfo(str).completeBaseName()
                    == QFileInfo(fileName).completeBaseName())
#endif
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的解决方案已打开，不能打开相同名称的解决方案，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(fileName).fileName()));
                return false;
            }
    }

    CProjectTreeItem *solutionItem = new CSolutionNodeTreeItem(ITEMSOLUTION, fileName);
//    disconnect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    int row = rowCount();
    for(int i = 0; i < rowCount(); ++i) //按名称排序
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && QFileInfo(fileName).fileName().toLower() <
                QFileInfo(solutionItem->getFileName()).fileName().toLower())
        {
            row = i;
            break;
        }
    }
    invisibleRootItem()->insertRow(row, solutionItem);
    CProjectTreeItem *solutionFileItem = new CProjectTreeItem(ITEMSOLUTIONFILE, fileName);
    solutionItem->appendRow(solutionFileItem);
//    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));

    foreach (QString str, solutionItem->getProjectList()) {
        appendProject(str, solutionItem);
    }

    return true;
}

bool CProjectTreeItemModel::appendProject(const QString &fileName, CProjectTreeItem *parent, const ECipherType &cipherType)
{
    if (fileName.isEmpty())
        return false;

    foreach (QString existFileName, getAllProjectList()) {
        if(QFileInfo(existFileName) == QFileInfo(fileName))
        {
            QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                     tr(u8"项目“%0”已在其他解决方案中打开，同一个项目不能多次打开！").arg(fileName));
            return false;
        }
    }

    QString solutionName = QFileInfo(parent->getFileName()).completeBaseName();
    CProjectResolver *projectResolver = new CProjectResolver(fileName, solutionName, cipherType);
    //TODO:是否有更合理的解决办法
//    MainWindow *mw = qobject_cast<MainWindow *>(m_parent);
    connect(projectResolver, SIGNAL(refreshed()), this, SIGNAL(projectSettingReset()));
    connect(projectResolver, SIGNAL(projectSaved(QString)),
            this, SIGNAL(projectSettingSaved(QString)));
    if(!projectResolver->isStarted())
    {
        delete projectResolver;
        QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                 tr(u8"解析项目“%0”失败！").arg(fileName));
        return false;
    }

    if(solutionName != projectResolver->getProjectParameter()->solutionName)
    {
        int &&ret = QMessageBox::warning(CAppEnv::m_mainWgt, qApp->applicationName(),
                                       tr(u8"<P>项目“%0”可能不属于解决方案“%1”，是否确定添加？</P>"
                                          "<P>点击“确定”将修改项目所属的解决方案！</P>")
                                       .arg(fileName).arg(solutionName),
                                       QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            projectResolver->getProjectParameter()->solutionName = solutionName;
            projectResolver->paraChanged();
            projectResolver->save();

        }
        else if(ret == QMessageBox::No) {
            return false;
        }
    }

    foreach (QString str, getProjectList(parent)) {
#if defined(Q_OS_WIN)
            if(QFileInfo(str).completeBaseName().toLower()
                    == QFileInfo(fileName).completeBaseName().toLower())
#elif defined(Q_OS_UNIX)
            if(QFileInfo(str).completeBaseName()
                    == QFileInfo(fileName).completeBaseName())
#endif
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的项目已打开，不能在同一个解决方案中打开相同名称的项目，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(fileName).fileName()));
                delete projectResolver;
                return false;
            }
    }

//    disconnect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    CProjectTreeItem *projectItem = new CProjectFolderNodeTreeItem(ITEMPROJECT, fileName, projectResolver);

    int row = parent->rowCount();
    for(int i = 1; i < parent->rowCount(); ++i) //按文件名称排序
    {
        CProjectTreeItem *item = static_cast<CProjectTreeItem *>(parent->child(i));
        if(item && QFileInfo(fileName).completeBaseName().toLower() <
                QFileInfo(item->getFileName()).completeBaseName().toLower())
        {
            row = i;
            break;
        }
    }
    parent->addProject(fileName);
    parent->insertRow(row, projectItem);
    if(!hasActiveSolution())
    {
        setActiveProject(projectItem);
    }

    CProjectTreeItem *projectFileItem = new CProjectTreeItem(ITEMPROJECTFILE, fileName);
    projectItem->appendRow(projectFileItem);
    CProjectTreeItem *xmlFolderItem = new CProjectTreeItem(ITEMXMLFOLDER, fileName);
    projectItem->appendRow(xmlFolderItem);
    CProjectTreeItem *srcFolderItem = new CProjectTreeItem(ITEMSRCFOLDER, fileName);
    projectItem->appendRow(srcFolderItem);
//    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));

    SProjectParameter *projectParameter = projectResolver->getProjectParameter();
    foreach (SXmlParameter xmlParameter, *projectParameter->xmlParameterList)
    {
        appendXml(xmlParameter.xmlPath, xmlFolderItem);
    }

    QMapIterator<QString, QString> itor(projectParameter->resourceMap);
    while (itor.hasNext()) {
        itor.next();
        appendSrc(itor.value(), srcFolderItem);
    }
    return true;
}

bool CProjectTreeItemModel::appendProject(const QString &fileName, const QModelIndex &index)
{
    if(!index.isValid())
        return false;
    CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    return appendProject(fileName, solutionItem);
}

bool CProjectTreeItemModel::addProject(const QString &fileName, const ECipherType &cipherType, const QModelIndex &index)
{
    if(!index.isValid())
        return false;
    CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    solutionItem->addProject(fileName);
    return appendProject(fileName, solutionItem, cipherType);

}

bool CProjectTreeItemModel::appendXml(const QString &fileName, CProjectTreeItem *parent)
{
    if (fileName.isEmpty())
        return false;

    if(!QFileInfo(fileName).exists())
        return false;

//    CProjectTreeItem *solutionItem = getSolutionItemFromChild(parent);
    CProjectTreeItem *projectItem = getProjectItemFromChild(parent);
//    Q_ASSERT(solutionItem);
    Q_ASSERT(projectItem);

    foreach (QString str, getXmlList(projectItem)) {
#if defined(Q_OS_WIN)
            if(QFileInfo(str).completeBaseName().toLower()
                    == QFileInfo(fileName).completeBaseName().toLower())
#elif defined(Q_OS_UNIX)
            if(QFileInfo(str).completeBaseName()
                    == QFileInfo(fileName).completeBaseName())
#endif
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的XML文件已在项目中，不能添加相同名称的XML文件到同一个项目中，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(fileName).fileName()));
                return false;
            }
    }

//    disconnect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    CProjectTreeItem *itemXmlFile = new CProjectTreeItem(ITEMXMLFILE, fileName);
    int row = parent->rowCount();
    for(int i = 0; i < parent->rowCount(); ++i) //按文件名称排序
    {
        CProjectTreeItem *xmlFile = static_cast<CProjectTreeItem *>(parent->child(i));
        if(QFileInfo(fileName).fileName().toLower() <
                QFileInfo(xmlFile->getFileName()).fileName().toLower())
        {
            row = i;
            break;
        }
    }
    parent->insertRow(row, itemXmlFile);
//    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    // TODO:       已打开的非项目xml改为项目内文件
    emit setProjectResolverForXml(fileName, parent->getProjectResolver());
//    QList<QMdiSubWindow *> winList = ui->mdiArea->subWindowList();
//    for(int i = 0; i < winList.count(); ++i)
//    {
//        RcaGraphView *rcaGraphView = qobject_cast<RcaGraphView *>(winList.at(i)->widget());
//        if(rcaGraphView->getCurFile() == xmlParameter.xmlPath)
//        {
//            DBG << tr(u8"已打开的非项目xml改为项目内文件");
//            rcaGraphView->setProject(m_curProjectResolver);
//        }
    //    }

    return true;
}

void CProjectTreeItemModel::removeXml(const QString &fileName, CProjectTreeItem *parent)
{
//    disconnect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    for(int i = 0; i < parent->rowCount(); ++i)
    {
        CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(parent->child(i));
        if(QFileInfo(fileName) == QFileInfo(xmlItem->getFileName()))
        {
            parent->removeRow(i);
            break;
        }
    }
//    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
}

bool CProjectTreeItemModel::addXml(const QString &fileName, const QModelIndex &index)
{
    if(!index.isValid())
        return false;
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *projectItem = getProjectItemFromChild(stdItem);
    if(!projectItem)
        return false;
    CProjectTreeItem *xmlFolderItem = getXmlFolderItemFromProject(projectItem);
    if(!xmlFolderItem)
        return false;

    if(!appendXml(fileName, xmlFolderItem))
        return false;

    CProjectResolver *projectResolver = projectItem->getProjectResolver();
    Q_ASSERT(projectResolver);
    if(!projectResolver->isStarted())
    {
        qInfo() << tr(u8"项目中添加XML：添加XML时发生错误，未能写入到项目文件！");
        QMessageBox::warning(CAppEnv::m_mainWgt, qApp->applicationName(), tr(u8"添加XML时发生错误，未能写入到项目文件！"));
        removeXml(fileName, xmlFolderItem);
        return false;
    }
    projectResolver->appendXmlElement(fileName);
    if(isActiveProject(projectItem))
        emit projectSettingChanged();
    return true;
}

void CProjectTreeItemModel::deleteXml(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(stdItem);
    deleteXml(xmlItem);
}

void CProjectTreeItemModel::deleteXml(const QString &fileName)
{
    CProjectTreeItem *xmlItem = getImpreciseXmlItemByName(fileName);
    while(xmlItem && xmlItem->getItemType() == ITEMXMLFILE)
    {
        deleteXml(xmlItem);
        xmlItem = getImpreciseXmlItemByName(fileName);
    }
}

void CProjectTreeItemModel::deleteXml(CProjectTreeItem *xmlItem)
{
    if(!xmlItem || xmlItem->getItemType() != ITEMXMLFILE)
        return;
    CProjectTreeItem *xmlFolderItem = getXmlFolderItemFromChild(xmlItem);
    if(!xmlFolderItem || xmlFolderItem->getItemType() != ITEMXMLFOLDER)
        return;
    CProjectTreeItem *projectItem = getProjectItemFromChild(xmlFolderItem);
    if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        return;
    CProjectResolver *projectResolver = projectItem->getProjectResolver();
    if(!projectResolver || !projectResolver->isStarted())
    {
        qInfo() << tr(u8"项目中移除XML：移除XML文件时发生错误，未能写入到项目文件！");
        QMessageBox::warning(CAppEnv::m_mainWgt, tr(u8"写入错误"), tr(u8"移除XML文件时发生错误，未能写入到项目文件！"));
        return;
    }

    projectResolver->removeXmlElement(xmlItem->getFileName());
//    this->removeRow(xmlItem->row(), indexFromItem(xmlFolderItem));
    xmlFolderItem->removeRow(xmlItem->row());
    if(isActiveProject(projectItem))
        emit projectSettingChanged();
}

bool CProjectTreeItemModel::appendSrc(const QString &fileName, CProjectTreeItem *parent)
{
    if (fileName.isEmpty())
        return false;

    if(!QFileInfo(fileName).exists())
    {
        return false;
    }

//    CProjectTreeItem *solutionItem = getSolutionItemFromChild(parent);
    CProjectTreeItem *projectItem = getProjectItemFromChild(parent);
//    Q_ASSERT(solutionItem);
    Q_ASSERT(projectItem);

    foreach (QString str, getSrcList(projectItem)) {
#if defined(Q_OS_WIN)
            if(QFileInfo(str).fileName().toLower()
                    == QFileInfo(fileName).fileName().toLower())
#elif defined(Q_OS_UNIX)
            if(QFileInfo(str).completeBaseName()
                    == QFileInfo(fileName).completeBaseName())
#endif
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的资源文件已在项目中，不能添加相同名称的资源文件到同一个项目中，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(fileName).fileName()));
                return false;
            }
    }

//    disconnect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    CProjectTreeItem *itemSrcFile = new CProjectTreeItem(ITEMSRCFILE, fileName);
    int row = parent->rowCount();
    for(int i = 0; i < parent->rowCount(); ++i) //按文件名称排序
    {
        CProjectTreeItem *srcFile = static_cast<CProjectTreeItem *>(parent->child(i));
        if(QFileInfo(fileName).fileName().toLower() <
                QFileInfo(srcFile->getFileName()).fileName().toLower())
        {
            row = i;
            break;
        }
    }
    parent->insertRow(row, itemSrcFile);
//    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    return true;
}

void CProjectTreeItemModel::removeSrc(const QString &fileName, CProjectTreeItem *parent)
{
//    disconnect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
    for(int i = 0; i < parent->rowCount(); ++i)
    {
        CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(parent->child(i));
        if(QFileInfo(fileName) == QFileInfo(srcItem->getFileName()))
        {
            parent->removeRow(i);
            break;
        }
    }
//    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(projectTreeItemChanged(QStandardItem*)));
}

bool CProjectTreeItemModel::addSrc(const QString &fileName, const QModelIndex &index)
{
    if(!index.isValid())
        return false;
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *projectItem = getProjectItemFromChild(stdItem);
    if(!projectItem)
        return false;
    CProjectTreeItem *srcFolderItem = getSrcFolderItemFromProject(projectItem);
    if(!srcFolderItem)
        return false;

    if(!appendSrc(fileName, srcFolderItem))
        return false;

    CProjectResolver *projectResolver = projectItem->getProjectResolver();
    Q_ASSERT(projectResolver);
    if(!projectResolver->isStarted())
    {
        qInfo() << tr(u8"添加资源文件：添加资源时发生错误，未能写入到项目文件！");
        QMessageBox::warning(CAppEnv::m_mainWgt, qApp->applicationName(), tr(u8"添加资源时发生错误，未能写入到项目文件！"));
        removeSrc(fileName, srcFolderItem);
        return false;
    }
    projectResolver->appendSrcElement(fileName);
    if(isActiveProject(projectItem))
        emit resourceSelectReset();
    return true;
}

bool CProjectTreeItemModel::activeProjectAddSrc(const QString &fileName)
{
    return addSrc(fileName, indexFromItem(getActiveProjectItem()));
}

void CProjectTreeItemModel::deleteSrc(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(stdItem);
    deleteSrc(srcItem);
}

void CProjectTreeItemModel::deleteSrc(const QString &fileName)
{
    CProjectTreeItem *srcItem = getImpreciseSrcItemByName(fileName);
    while(srcItem/* && srcItem->getItemType() == ITEMSRCFILE*/)
    {
        deleteSrc(srcItem);
        srcItem = getImpreciseSrcItemByName(fileName);
    }
}

void CProjectTreeItemModel::deleteSrc(CProjectTreeItem *srcItem)
{
    if(!srcItem || srcItem->getItemType() != ITEMSRCFILE)
        return;
    CProjectTreeItem *srcFolderItem = getSrcFolderItemFromChild(srcItem);
    if(!srcFolderItem || srcFolderItem->getItemType() != ITEMSRCFOLDER)
        return;
    CProjectTreeItem *projectItem = getProjectItemFromChild(srcFolderItem);
    if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        return;
    CProjectResolver *projectResolver = getProjectItemFromChild(projectItem)->getProjectResolver();
    if(!projectResolver || !projectResolver->isStarted())
    {
        qInfo() << tr(u8"移除资源文件：移除资源文件时发生错误，未能写入到项目文件！");
        QMessageBox::warning(CAppEnv::m_mainWgt, tr(u8"写入错误"), tr(u8"移除资源文件时发生错误，未能写入到项目文件！"));
        return;
    }
    projectResolver->removeSrcElement(srcItem->getFileName());
    srcFolderItem->removeRow(srcItem->row());
    if(isActiveProject(srcFolderItem))
        emit resourceSelectReset();
}

void CProjectTreeItemModel::activeProjectDeleteSrc(const QString &fileName)
{
    CProjectTreeItem *srcFolderItem = getActiveSrcFolderItem();
    if(!srcFolderItem)
        return;
    for(int i = 0; i < srcFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(i));
        if(srcItem && srcItem->getItemType() == ITEMSRCFILE && QFileInfo(fileName) == QFileInfo(srcItem->getFileName()))
        {
            deleteSrc(indexFromItem(srcItem));
        }
    }
}

QList<CProjectResolver *> CProjectTreeItemModel::getAllProjectResolver() const
{
    QList<CProjectResolver *> list;
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT)
                {
                    list.append(projectItem->getProjectResolver());
                }
            }
        }
    }
    return list;
}

QList<CProjectTreeItem *> CProjectTreeItemModel::getProjectItemList(CProjectTreeItem *solutionItem) const
{
    QList<CProjectTreeItem *> list;
    solutionItem = getSolutionItemFromChild(solutionItem);
    if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
    {
        for(int i = 1; i < solutionItem->rowCount(); ++i)
        {
            CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
            if(projectItem && projectItem->getItemType() == ITEMPROJECT)
            {
                list.append(projectItem);
            }
        }
    }
    return list;
}

void CProjectTreeItemModel::closeSolution(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(stdItem);
    if(!solutionItem)
        return;
    if(solutionItem->getItemType() == ITEMSOLUTION)
    {
        for(int i = 0; i < solutionItem->rowCount(); ++i)
        {
            CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
            if(projectItem && projectItem->getItemType() == ITEMPROJECT)
            {
                if(!projectItem->getProjectResolver()->okToContinue())
                    return;
            }
        }
    }
    int row = solutionItem->row();
    if(isActiveSolution(index))
        getActiveProjectItem()->lostCurPro();
    removeRow(solutionItem->row());

    if(!hasActiveSolution() && rowCount() > 0)
    {
//        disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
//                   this, SLOT(projectTreeItemChanged(QStandardItem*)));
        if(row == rowCount())
            --row;
        solutionItem = static_cast<CProjectTreeItem *>(item(row));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            if(solutionItem->rowCount() > 1)
                setActiveProject(indexFromItem(item(row)->child(1)));
            else
            {
                if(row > 0)
                {
                    for(int i = row - 1; i >= 0; --i)
                    {
                        if(item(i)->rowCount() > 1)
                        {
                            setActiveProject(indexFromItem(item(i)->child(1)));
                            break;
                        }
                    }
                }
                if(!hasActiveSolution() && row < rowCount() - 1)
                {
                    for(int i = row + 1; i < rowCount(); ++i)
                    {
                        if(item(i)->rowCount() > 1)
                        {
                            setActiveProject(indexFromItem(item(i)->child(1)));
                            break;
                        }
                    }
                }
            }
        }



//        connect(this, SIGNAL(itemChanged(QStandardItem*)),
//                this, SLOT(projectTreeItemChanged(QStandardItem*)));
    }
}

void CProjectTreeItemModel::deleteProject(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(stdItem);
    deleteProject(projectItem);
}

void CProjectTreeItemModel::deleteProject(CProjectTreeItem *projectItem)
{
    Q_ASSERT(projectItem);
    if(projectItem->getItemType() != ITEMPROJECT || !projectItem->getProjectResolver()->okToContinue())
        return;

    int row = projectItem->row();
    QStandardItem *parentItem = projectItem->parent();
    CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(parentItem);
    if(!solutionItem || solutionItem->getItemType() != ITEMSOLUTION)
        return;
    if(isActiveProject(projectItem))
        solutionItem->lostCurPro();
    solutionItem->removeProject(projectItem->getFileName());
    solutionItem->removeRow(projectItem->row());

    if(!hasActiveSolution())
    {
//        disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
//                   this, SLOT(projectTreeItemChanged(QStandardItem*)));
        if(solutionItem->rowCount() > 1)
        {
            if(row == solutionItem->rowCount())
                --row;
            setActiveProject(indexFromItem(solutionItem->child(row)));
        }
        else if(rowCount() > 1)
        {
            int slnRow = solutionItem->row();
            if(slnRow > 0)
            {
                for(int i = slnRow - 1; i >= 0; --i)
                {
                    if(item(i)->rowCount() > 1)
                    {
                        setActiveProject(indexFromItem(item(i)->child(1)));
                        break;
                    }
                }
            }
            if(!hasActiveSolution() && slnRow < rowCount() - 1)
            {
                for(int i = slnRow + 1; i < rowCount(); ++i)
                {
                    if(item(i)->rowCount() > 1)
                    {
                        setActiveProject(indexFromItem(item(i)->child(1)));
                        break;
                    }
                }
            }
        }
//        connect(this, SIGNAL(itemChanged(QStandardItem*)),
//                this, SLOT(projectTreeItemChanged(QStandardItem*)));
    }
}

void CProjectTreeItemModel::deleteProject(const QString &projectName)
{
    CProjectTreeItem *projectItem = getImpreciseProjectItemByName(projectName);
    while(projectItem && projectItem->getItemType() == ITEMPROJECT)
    {
        deleteProject(projectItem);
        projectItem = getImpreciseProjectItemByName(projectName);
    }
}

void CProjectTreeItemModel::setActiveProject(const QString &solutionName, const QString &projectName)
{
    CProjectTreeItem *projectItem = getProjectItemByName(solutionName, projectName);
    setActiveProject(projectItem);
}

void CProjectTreeItemModel::setActiveProject(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(stdItem);
    setActiveProject(projectItem);
}

void CProjectTreeItemModel::setActiveProject(CProjectTreeItem *projectItem)
{
    CProjectTreeItem *activeItem = getActiveProjectItem();
    if(activeItem && activeItem == projectItem)
        return;
    if(activeItem && !activeItem->getProjectResolver()->okToContinue())
        return;
//可能需要disconnect
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->isCurPro())
        {
            solutionItem->lostCurPro();
        }
    }
    if(projectItem && projectItem->getItemType() == ITEMPROJECT)
        projectItem->setCurPro();

    emit activeProjectChanged();
    emit projectSettingReset();
    emit resourceSelectReset();
}

QString CProjectTreeItemModel::getActiveSolutionName() const
{
    CProjectTreeItem *solutionItem = getActiveSolutionItem();
    if(solutionItem)
        return solutionItem->getFileName();
    else
        return QString();
}

CProjectResolver* CProjectTreeItemModel::getProjectResolver(const QModelIndex &index) const
{
    if(!index.isValid())
        return nullptr;
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *projectItem = getProjectItemFromChild(stdItem);
    if(projectItem && projectItem->getItemType() == ITEMPROJECT)
        return projectItem->getProjectResolver();
    return nullptr;
}

CProjectResolver* CProjectTreeItemModel::getNearestProjectResolverFromXml(const QString &xmlFileName) const
{
    CProjectTreeItem *xmlItem = getImpreciseXmlItemByName(xmlFileName);
    if(!xmlItem || xmlItem->getItemType() != ITEMXMLFILE)
        return nullptr;
    CProjectTreeItem *projectItem = getProjectItemFromChild(xmlItem);
    if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        return nullptr;
    return projectItem->getProjectResolver();
}

QString CProjectTreeItemModel::getProjectFileName(const QModelIndex &index) const
{
    if(!index.isValid())
        return QString();
    QStandardItem *stdItem = itemFromIndex(index);
    CProjectTreeItem *projectItem = getProjectItemFromChild(stdItem);
    if(projectItem && projectItem->getItemType() == ITEMPROJECT)
        return projectItem->getFileName();
    return QString();
}

QString CProjectTreeItemModel::getNearestProjectFileNameFromSrc(const QString &fileName) const
{
    CProjectTreeItem *srcItem = getImpreciseSrcItemByName(fileName);
    if(!srcItem || srcItem->getItemType() != ITEMSRCFILE)
        return QString();

    CProjectTreeItem *projectItem = getProjectItemFromChild(srcItem);
    if(projectItem && projectItem->getItemType() == ITEMPROJECT)
    {
        return projectItem->getFileName();
    }
    return QString();
}

CProjectResolver *CProjectTreeItemModel::getActiveProjectResolver() const
{
    CProjectTreeItem *projectItem = getActiveProjectItem();
    if(projectItem)
        return projectItem->getProjectResolver();
    else
        return nullptr;
}

QString CProjectTreeItemModel::getActiveProjectFileName() const
{
    CProjectTreeItem *projectItem = getActiveProjectItem();
    if(projectItem)
        return projectItem->getFileName();
    else
        return QString();
}

bool CProjectTreeItemModel::isActiveProject(const QModelIndex &index) const
{
    if(!index.isValid())
        return false;

    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    return isActiveProject(tempItem);
}

bool CProjectTreeItemModel::isActiveProject(CProjectTreeItem *item) const
{
    const CProjectTreeItem *projectItem = getProjectItemFromChild(item);
    if(projectItem && projectItem->getItemType() == ITEMPROJECT)
    {
        const CProjectTreeItem *activeProjectItem = getActiveProjectItem();
        if(!activeProjectItem)
            return false;
        return (projectItem == activeProjectItem);
    }
    return false;
}

bool CProjectTreeItemModel::isActiveSolution(const QModelIndex &index) const
{
    if(!index.isValid())
        return false;
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    if(tempItem && tempItem->getItemType() == ITEMSOLUTION )
        return tempItem->isCurPro();
    return false;
}

QString CProjectTreeItemModel::getItemFileName(const QModelIndex &index) const
{
    if(!index.isValid())
        return QString();
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    return tempItem->getFileName();
}

EItemType CProjectTreeItemModel::getItemType(const QModelIndex &index) const
{
    if(!index.isValid())
        return ITEMNONE;
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    return tempItem->getItemType();
}

bool CProjectTreeItemModel::hasProject(const QModelIndex &index) const
{
    if(!index.isValid())
        return false;
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    if(tempItem && tempItem->getItemType() == ITEMSOLUTION)
    {
        return tempItem->rowCount();
    }
    return false;
}

bool CProjectTreeItemModel::hasActiveSolution() const
{
    return getActiveSolutionItem();
}

QStringList CProjectTreeItemModel::getActiveXmlList() const
{
    QStringList list;
    CProjectTreeItem *projectTreeItem = getActiveProjectItem();
    if(!projectTreeItem || projectTreeItem->getItemType() != ITEMPROJECT)
        return list;
    CProjectTreeItem *xmlFolderItem = getXmlFolderItemFromProject(projectTreeItem);
    if(!xmlFolderItem || xmlFolderItem->getItemType() != ITEMXMLFOLDER)
        return list;
    for(int i = 0; i < xmlFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(xmlFolderItem->child(i));
        if(xmlItem && xmlItem->getItemType() == ITEMXMLFILE)
            list.append(xmlItem->getFileName());
    }
    return list;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveSolutionItem() const
{
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION && solutionItem->isCurPro())
        {
            return solutionItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getSolutionItemFromChild(QStandardItem *item) const
{
    if(!item)
        return nullptr;

    if(item->parent())
    {
        return getSolutionItemFromChild(item->parent());
    }
    else
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item);
        return solutionItem;
    }
}

CProjectTreeItem *CProjectTreeItemModel::getSolutionItemByName(const QString &solutionName) const
{
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION && QFileInfo(solutionName) == QFileInfo(solutionItem->getFileName()))
        {
            return solutionItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveProjectItem() const
{
    CProjectTreeItem *solutionItem = getActiveSolutionItem();
    if(!solutionItem)
        return nullptr;
    for(int i = 0; i < solutionItem->rowCount(); ++i)
    {
        CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
        if(projectItem && projectItem->getItemType() == ITEMPROJECT && projectItem->isCurPro())
        {
            return projectItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getProjectItemByName(const QString &solutionName, const QString &projectName) const
{
    CProjectTreeItem *solutionItem = getSolutionItemByName(solutionName);
    if(!solutionItem)
        return nullptr;

    for(int i = 0; i < solutionItem->rowCount(); ++i)
    {
        CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
        if(projectItem && projectItem->getItemType() == ITEMPROJECT && QFileInfo(projectName) == QFileInfo(projectItem->getFileName()))
        {
            return projectItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getProjectItemFromChild(QStandardItem *item) const
{
    if(!item)
        return nullptr;
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(item);
    if(tempItem)
    {
        if(tempItem->getItemType() == ITEMPROJECT)
            return tempItem;
        else
            return getProjectItemFromChild(tempItem->parent());
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getImpreciseProjectItemByName(const QString &projectFileName) const
{
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT
                        && QFileInfo(projectItem->getFileName()) == QFileInfo(projectFileName))
                {
                    return projectItem;
                }
            }
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveProjectFileItem() const
{
    CProjectTreeItem *projectItem = getActiveProjectItem();
    if(!projectItem)
        return nullptr;
    CProjectTreeItem *projectFileItem = static_cast<CProjectTreeItem *>(projectItem->child(0));
    if(projectFileItem && projectFileItem->getItemType() == ITEMPROJECTFILE)
    {
        return projectFileItem;
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getProjectFileItemByName(const QString &solutionName, const QString &projectName) const
{
    CProjectTreeItem *projectItem = getProjectItemByName(solutionName, projectName);
    if(projectItem && projectItem->rowCount() > 0)
    {
        CProjectTreeItem *projectFileItem = static_cast<CProjectTreeItem *>(projectItem->child(0));
        if(projectFileItem && projectFileItem->getItemType() == ITEMPROJECTFILE && QFileInfo(projectName) == QFileInfo(projectFileItem->getFileName()))
        {
            return projectFileItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getProjectFileItemFromNeighborChild(QStandardItem *item) const
{
    if(!item)
        return nullptr;

    CProjectTreeItem *projectItem = getProjectItemFromChild(item);

    if(projectItem && projectItem->getItemType() == ITEMPROJECT)
    {
        return static_cast<CProjectTreeItem *>(projectItem->child(0));
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveXmlFolderItem() const
{
    CProjectTreeItem *projectItem = getActiveProjectItem();
    if(!projectItem)
        return nullptr;
    CProjectTreeItem *xmlFolderItem = static_cast<CProjectTreeItem *>(projectItem->child(1));
    if(xmlFolderItem && xmlFolderItem->getItemType() == ITEMXMLFOLDER)
    {
        return xmlFolderItem;
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getXmlFolderItemByName(const QString &solutionName, const QString &projectName) const
{
    CProjectTreeItem *projectItem = getProjectItemByName(solutionName, projectName);
    if(projectItem && projectItem->rowCount() > 1)
    {
        CProjectTreeItem *xmlFolderItem = static_cast<CProjectTreeItem *>(projectItem->child(1));
        if(xmlFolderItem && xmlFolderItem->getItemType() == ITEMXMLFOLDER)
        {
            return xmlFolderItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getXmlFolderItemFromProject(const QStandardItem *item) const
{
    if(!item)
        return nullptr;
    const CProjectTreeItem *tempItem = static_cast<const CProjectTreeItem *>(item);
    if(tempItem && tempItem->getItemType() == ITEMPROJECT)
    {
        return static_cast<CProjectTreeItem *>(tempItem->child(1));
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getXmlFolderItemFromChild(const QStandardItem *item) const
{
    if(!item)
        return nullptr;
    const CProjectTreeItem *tempItem = static_cast<const CProjectTreeItem *>(item);
    if(tempItem && tempItem->getItemType() == ITEMXMLFILE)
    {
        return static_cast<CProjectTreeItem *>(tempItem->parent());
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveSrcFolderItem() const
{
    CProjectTreeItem *projectItem = getActiveProjectItem();
    if(!projectItem)
        return nullptr;
    CProjectTreeItem *srcFolderItem = static_cast<CProjectTreeItem *>(projectItem->child(2));
    if(srcFolderItem && srcFolderItem->getItemType() == ITEMSRCFOLDER)
    {
        return srcFolderItem;
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getXmlItemByName(const QString &solutionName, const QString &projectName, const QString &fileName) const
{
    CProjectTreeItem *xmlFolderItem = getXmlFolderItemByName(solutionName, projectName);
    if(!xmlFolderItem)
        return nullptr;

    for(int i = 0; i < xmlFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(xmlFolderItem->child(i));
        if(xmlItem && xmlItem->getItemType() == ITEMXMLFILE && QFileInfo(fileName) == QFileInfo(xmlItem->getFileName()))
        {
            return xmlItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getImpreciseXmlItemByName(const QString &xmlFileName) const
{
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT)
                {
                    CProjectTreeItem *xmlFolderItem = getXmlFolderItemFromProject(projectItem);
                    if(xmlFolderItem && xmlFolderItem->getItemType() == ITEMXMLFOLDER)
                    {
                        for(int k = 0; k < xmlFolderItem->rowCount(); ++k)
                        {
                            CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(xmlFolderItem->child(k));
                            if(xmlItem && xmlItem->getItemType() == ITEMXMLFILE && QFileInfo(xmlItem->getFileName()) == QFileInfo(xmlFileName))
                            {
                                return xmlItem;
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveSrcItem(const QString &fileName) const
{
    CProjectTreeItem* srcFolderItem = getActiveSrcFolderItem();
    if(!srcFolderItem)
        return nullptr;

    for(int i = 0; i < srcFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(i));
        if(srcItem && srcItem->getItemType() == ITEMXMLFILE && QFileInfo(fileName) == QFileInfo(srcItem->getFileName()))
        {
            return srcItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getSrcFolderItemByName(const QString &solutionName, const QString &projectName) const
{
    CProjectTreeItem *projectItem = getProjectItemByName(solutionName, projectName);
    if(projectItem && projectItem->rowCount() > 2)
    {
        CProjectTreeItem *srcFolderItem = static_cast<CProjectTreeItem *>(projectItem->child(2));
        if(srcFolderItem && srcFolderItem->getItemType() == ITEMSRCFOLDER)
        {
            return srcFolderItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getSrcFolderItemFromProject(const QStandardItem *item) const
{
    if(!item)
        return nullptr;
    const CProjectTreeItem *tempItem = static_cast<const CProjectTreeItem *>(item);
    if(tempItem && tempItem->getItemType() == ITEMPROJECT)
    {
        return static_cast<CProjectTreeItem *>(tempItem->child(2));
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getSrcFolderItemFromChild(const QStandardItem *item) const
{
    if(!item)
        return nullptr;
    const CProjectTreeItem *tempItem = static_cast<const CProjectTreeItem *>(item);
    if(tempItem && tempItem->getItemType() == ITEMSRCFILE)
    {
        return static_cast<CProjectTreeItem *>(tempItem->parent());
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getActiveXmlItem(const QString &fileName) const
{
    CProjectTreeItem* xmlFolderItem = getActiveXmlFolderItem();
    if(!xmlFolderItem)
        return nullptr;

    for(int i = 0; i < xmlFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(xmlFolderItem->child(i));
        if(xmlItem && xmlItem->getItemType() == ITEMXMLFILE && QFileInfo(fileName) == QFileInfo(xmlItem->getFileName()))
        {
            return xmlItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getSrcItemByName(const QString &solutionName, const QString &projectName, const QString &fileName) const
{
    CProjectTreeItem *srcFolderItem = getSrcFolderItemByName(solutionName, projectName);
    if(!srcFolderItem)
        return nullptr;

    for(int i = 0; i < srcFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(i));
        if(srcItem && srcItem->getItemType() == ITEMSRCFILE && QFileInfo(fileName) == QFileInfo(srcItem->getFileName()))
        {
            return srcItem;
        }
    }
    return nullptr;
}

CProjectTreeItem *CProjectTreeItemModel::getImpreciseSrcItemByName(const QString &srcFileName) const
{
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT)
                {
                    CProjectTreeItem *srcFolderItem = getSrcFolderItemFromProject(projectItem);
                    if(srcFolderItem && srcFolderItem->getItemType() == ITEMSRCFOLDER)
                    {
                        for(int k = 0; k < srcFolderItem->rowCount(); ++k)
                        {
                            CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(k));
                            if(srcItem && srcItem->getItemType() == ITEMSRCFILE && QFileInfo(srcItem->getFileName()) == QFileInfo(srcFileName))
                            {
                                return srcItem;
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

QStringList CProjectTreeItemModel::getSolutionList() const
{
    QStringList list;
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem)
        {
            list.append(solutionItem->getFileName());
        }
    }
    return list;
}

QStringList CProjectTreeItemModel::getProjectList(const CProjectTreeItem *solutionItem) const
{
    QStringList list;
    if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
    {
        for(int i = 0; i < solutionItem->rowCount(); ++i)
        {
            CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
            if(projectItem && projectItem->getItemType() == ITEMPROJECT)
            {
                list.append(projectItem->getFileName());
            }
        }
    }
    return list;
}

QStringList CProjectTreeItemModel::getProjectList(const QModelIndex &index) const
{
    if(!index.isValid())
        return QStringList();
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(itemFromIndex(index));
    if(tempItem && tempItem->getItemType() == ITEMSOLUTION)
    {
        return getProjectList(tempItem);
    }
    return QStringList();
}

QStringList CProjectTreeItemModel::getAllProjectList() const
{
    QStringList list;
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT)
                {
                    list.append(projectItem->getFileName());
                }
            }
        }
    }
    return list;
}

QStringList CProjectTreeItemModel::getXmlList(const CProjectTreeItem *projectItem) const
{
    QStringList list;
    if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        return list;
    const CProjectTreeItem *xmlFolderItem = getXmlFolderItemFromProject(projectItem);
    if(xmlFolderItem && xmlFolderItem->getItemType() == ITEMXMLFOLDER)
    {
        for(int i = 0; i < xmlFolderItem->rowCount(); ++i)
        {
            CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(xmlFolderItem->child(i));
            if(xmlItem && xmlItem->getItemType() == ITEMXMLFILE)
            {
                list.append(xmlItem->getFileName());
            }
        }
    }
    return list;
}

QStringList CProjectTreeItemModel::getSrcList(const CProjectTreeItem *projectItem) const
{
    QStringList list;
    if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        return list;
    CProjectTreeItem *srcFolderItem = getSrcFolderItemFromProject(projectItem);
    if(srcFolderItem && srcFolderItem->getItemType() == ITEMSRCFOLDER)
    {
        for(int i = 0; i < srcFolderItem->rowCount(); ++i)
        {
            CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(i));
            if(srcItem && srcItem->getItemType() == ITEMSRCFILE)
            {
                list.append(srcItem->getFileName());
            }
        }
    }
    return list;
}

QStringList CProjectTreeItemModel::getAllXmlList() const
{
    QStringList list;
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT)
                {
                    CProjectTreeItem *xmlFolderItem = getXmlFolderItemFromProject(projectItem);
                    if(xmlFolderItem && xmlFolderItem->getItemType() == ITEMXMLFOLDER)
                    {
                        for(int k = 0; k < xmlFolderItem->rowCount(); ++k)
                        {
                            CProjectTreeItem *xmlItem = static_cast<CProjectTreeItem *>(xmlFolderItem->child(k));
                            if(xmlItem && xmlItem->getItemType() == ITEMXMLFILE)
                            {
                                list.append(xmlItem->getFileName());
                            }
                        }
                    }
                }
            }
        }
    }
    return list;
}

QStringList CProjectTreeItemModel::getActiveSrcList() const
{
    QStringList list;
    CProjectTreeItem *projectTreeItem = getActiveProjectItem();
    if(!projectTreeItem || projectTreeItem->getItemType() != ITEMPROJECT)
        return list;
    CProjectTreeItem *srcFolderItem = getSrcFolderItemFromProject(projectTreeItem);
    if(!srcFolderItem || srcFolderItem->getItemType() != ITEMSRCFOLDER)
        return list;
    for(int i = 0; i < srcFolderItem->rowCount(); ++i)
    {
        CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(i));
        if(srcItem && srcItem->getItemType() == ITEMSRCFILE)
            list.append(srcItem->getFileName());
    }
    return list;
}

QStringList CProjectTreeItemModel::getAllSrcList() const
{
    QStringList list;
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *solutionItem = static_cast<CProjectTreeItem *>(item(i));
        if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        {
            for(int j = 1; j < solutionItem->rowCount(); ++j)
            {
                CProjectTreeItem *projectItem = static_cast<CProjectTreeItem *>(solutionItem->child(j));
                if(projectItem && projectItem->getItemType() == ITEMPROJECT)
                {
                    CProjectTreeItem *srcFolderItem = getSrcFolderItemFromProject(projectItem);
                    if(srcFolderItem && srcFolderItem->getItemType() == ITEMXMLFOLDER)
                    {
                        for(int k = 0; k < srcFolderItem->rowCount(); ++k)
                        {
                            CProjectTreeItem *srcItem = static_cast<CProjectTreeItem *>(srcFolderItem->child(k));
                            if(srcItem && srcItem->getItemType() == ITEMXMLFILE)
                            {
                                list.append(srcItem->getFileName());
                            }
                        }
                    }
                }
            }
        }
    }
    return list;
}

void CProjectTreeItemModel::projectTreeItemChanged(QStandardItem *stdItem)
{
    CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(stdItem);
    QModelIndex index = indexFromItem(stdItem);
    if(!tempItem)
    {
        emit renameFileFinished(index);
        return;
    }
    if(tempItem->getItemType() == ITEMXMLFILE || tempItem->getItemType() == ITEMSRCFILE)
    {
        QString oldName = tempItem->getFileName();
        QString newName = QFileInfo(oldName).absolutePath() + "/" + tempItem->text();
        bool suffixWrong = false;
        if(tempItem->getItemType() == ITEMXMLFILE && !newName.endsWith(".xml", Qt::CaseInsensitive))
        {
            suffixWrong = true;
            newName += ".xml";
        }
        if(oldName == newName)
        {
            if(suffixWrong)
            {
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(QFileInfo(oldName).fileName());
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
            }
            emit renameFileFinished(index);
            return;
        }

        CProjectTreeItem *projectItem = getProjectItemFromChild(tempItem);//已存在相同名称
        Q_ASSERT(projectItem);
        Q_ASSERT(projectItem->getItemType() == ITEMPROJECT);
        foreach (QString fileName, getXmlList(projectItem)) {
            if(QFileInfo(fileName).completeBaseName() == QFileInfo(newName).completeBaseName())
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的文件已在项目中，不能重命名为重复的文件名称，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(newName).completeBaseName()));
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(QFileInfo(oldName).fileName());
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
                emit renameFileFinished(index);
                return;
            }
        }

        if((tempItem->getItemType() == ITEMXMLFILE && QFileInfo(newName).completeBaseName().isEmpty())
                || (tempItem->getItemType() == ITEMSRCFILE && QFileInfo(newName).fileName().isEmpty()))
        {
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setText(QFileInfo(oldName).fileName());
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            emit renameFileFinished(index);
            return;
        }

        if(!QFile::rename(oldName, newName))
        {
            qInfo() << tr(u8"重命名：重命名文件%0为%1失败").arg(oldName).arg(newName);
            QMessageBox::critical(CAppEnv::m_mainWgt, tr(u8"错误"), tr(u8"重命名失败，请核对文件是否被占用！"));
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setText(QFileInfo(oldName).fileName());
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            emit renameFileFinished(index);
            return;
        }
        else
        {
            qInfo() << tr(u8"重命名：重命名文件%0为%1成功").arg(oldName).arg(newName);
            if(suffixWrong)
            {
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(tempItem->text() + ".xml");
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
            }
            tempItem->setFileName(newName);
            QStandardItem *folderItem = tempItem->parent();
            QList<QStandardItem *> itemList = folderItem->takeRow(tempItem->row());
            tempItem = static_cast<CProjectTreeItem *>(itemList.at(0));
            if(!tempItem || (tempItem->getItemType() != ITEMXMLFILE && tempItem->getItemType() != ITEMSRCFILE))
            {
                emit renameFileFinished(index);
                return;
            }
            int i;
            for(i = 0; i < folderItem->rowCount(); ++i) //按文件名称排序
            {
                CProjectTreeItem *xmlFile = static_cast<CProjectTreeItem *>(folderItem->child(i));
                if(QFileInfo(tempItem->getFileName()).fileName().toLower() <
                        QFileInfo(xmlFile->getFileName()).fileName().toLower())
                {
                    folderItem->insertRow(i, tempItem);
                    break;
                }
            }
            if(i == folderItem->rowCount())
            {
                folderItem->appendRow(tempItem);
            }
        }

        CProjectResolver *projectResolver = projectItem->getProjectResolver();
        if(projectResolver && !projectResolver->isStarted())
        {
            qInfo() << tr(u8"重命名：重命名文件时发生错误，未能写入到项目文件！");
            QMessageBox::warning(CAppEnv::m_mainWgt, tr(u8"写入错误"), tr(u8"重命名文件时发生错误，未能写入到项目文件！"));
            emit renameFileFinished(index);
            return;
        }
        if(tempItem->getItemType() == ITEMXMLFILE)
        {
            projectResolver->changeXmlElement(oldName, newName);
            if(isActiveProject(tempItem))
            {
                emit projectSettingChanged();
            }
            emit xmlFileRenamed(oldName, newName);  //更新最近打开列表名称
            renameXml(oldName, newName);
        }
        else if(tempItem->getItemType() == ITEMSRCFILE)
        {
            projectResolver->changeSrcElement(oldName, newName);
            if(isActiveProject(tempItem))
            {
                emit resourceSelectReset();
            }
            emit srcFileRenamed(oldName, newName);
            renameSrc(oldName, newName);
        }
    }
    else if(tempItem->getItemType() == ITEMPROJECTFILE)
    {
        QString oldName = tempItem->getFileName();
        QString newName = QFileInfo(oldName).absolutePath() + "/" + tempItem->text();
        bool suffixWrong = false;
        if(!newName.endsWith(".rpupro", Qt::CaseInsensitive))
        {
            suffixWrong = true;
            newName += ".rpupro";
        }
        if(oldName == newName) //名称未修改
        {
            if(suffixWrong)
            {
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(QFileInfo(oldName).fileName());
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
            }
            emit renameFileFinished(index);
            return;
        }

        CProjectTreeItem *projectItem = getProjectItemFromChild(tempItem);//已存在相同名称
        CProjectTreeItem *solutionItem = getSolutionItemFromChild(projectItem);
        Q_ASSERT(projectItem);
        Q_ASSERT(projectItem->getItemType() == ITEMPROJECT);
        Q_ASSERT(solutionItem);
        Q_ASSERT(solutionItem->getItemType() == ITEMSOLUTION);
        foreach (QString fileName, getProjectList(solutionItem)) {
            if(QFileInfo(fileName).completeBaseName() == QFileInfo(newName).completeBaseName())
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的项目已打开，不能重命名为相同名称的项目，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(newName).completeBaseName()));
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(QFileInfo(oldName).fileName());
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
                emit renameFileFinished(index);
                return;
            }
        }

        if(QFileInfo(newName).completeBaseName().isEmpty()) //重命名为空
        {
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setText(QFileInfo(oldName).fileName());
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            emit renameFileFinished(index);
            return;
        }

        CProjectResolver *projectResolver = projectItem->getProjectResolver();
        if(!projectResolver || !projectResolver->isStarted())
        {
            qInfo() << tr(u8"重命名：重命名文件时发生错误，未能写入到项目文件！");
            QMessageBox::warning(CAppEnv::m_mainWgt, tr(u8"写入错误"), tr(u8"重命名文件时发生错误，未能写入到项目文件！"));
            emit renameFileFinished(index);
            return;
        }
        if(!projectResolver->renameProject(oldName, newName))
        {
            qInfo() << tr(u8"重命名：重命名文件%0为%1失败").arg(oldName).arg(newName);
            QMessageBox::critical(CAppEnv::m_mainWgt, tr(u8"错误"), tr(u8"重命名失败，请核对文件是否被占用。"));
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setText(QFileInfo(oldName).fileName());
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            emit renameFileFinished(index);
            return;
        }

        qInfo() << tr(u8"重命名：重命名文件%0为%1成功").arg(oldName).arg(newName);
        disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                   this, SLOT(projectTreeItemChanged(QStandardItem*)));
        if(suffixWrong)
        {
            tempItem->setText(tempItem->text() + ".rpupro");
        }
        projectItem->setText(QFileInfo(newName).baseName());
        tempItem->setToolTip(newName);
        connect(this, SIGNAL(itemChanged(QStandardItem*)),
                   this, SLOT(projectTreeItemChanged(QStandardItem*)));
        projectItem->setFileName(newName);
        for(int i = 0; i < projectItem->rowCount(); ++i)   //更新所有item的path信息
        {
            CProjectTreeItem *childItem = static_cast<CProjectTreeItem *>(projectItem->child(i));
//            if(childItem->getItemType() == ITEMPROJECTFILE || childItem->getItemType() == ITEMXMLFOLDER
//                    || childItem->getItemType() == ITEMSRCFOLDER)
//            {
                childItem->setFileName(newName);
//            }
        }


        solutionItem->renameProject(oldName, newName);

        QList<QStandardItem *> itemList = solutionItem->takeRow(projectItem->row());
        projectItem = static_cast<CProjectTreeItem *>(itemList.at(0));
        if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        {
            emit renameFileFinished(index);
            return;
        }
        int i;
        for(i = 1; i < solutionItem->rowCount(); ++i) //按文件名称排序
        {
            CProjectTreeItem *tempProjectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
            if(QFileInfo(projectItem->getFileName()).fileName().toLower() <
                    QFileInfo(tempProjectItem->getFileName()).fileName().toLower())
            {
                solutionItem->insertRow(i, projectItem);
                break;
            }
        }
        if(i == solutionItem->rowCount())
        {
            solutionItem->appendRow(projectItem);
        }
        index = indexFromItem(projectItem);
//        renameProject(oldName, newName);//同一个项目不能多次打开，暂注释
        emit projectFileRenamed(oldName, newName);  //更新最近打开列表名称
    }
    else if(tempItem->getItemType() == ITEMSOLUTIONFILE)
    {
        QString oldName = tempItem->getFileName();
        QString newName = QFileInfo(oldName).absolutePath() + "/" + tempItem->text();
        bool suffixWrong = false;
        if(!newName.endsWith(".rpusln", Qt::CaseInsensitive))
        {
            suffixWrong = true;
            newName += ".rpusln";
        }
        if(oldName == newName) //名称未修改
        {
            if(suffixWrong)
            {
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(QFileInfo(oldName).fileName());
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
            }
            emit renameFileFinished(index);
            return;
        }

        CProjectTreeItem *solutionItem = getSolutionItemFromChild(tempItem);
        Q_ASSERT(solutionItem);
        Q_ASSERT(solutionItem->getItemType() == ITEMSOLUTION);
        foreach (QString fileName, getSolutionList()) {
            if(QFileInfo(fileName).completeBaseName() == QFileInfo(newName).completeBaseName())
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"名为“%0”的解决方案已打开，不能重命名为相同名称的解决方案，尽管他们处于不同文件夹中！")
                                         .arg(QFileInfo(newName).completeBaseName()));
                disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                           this, SLOT(projectTreeItemChanged(QStandardItem*)));
                tempItem->setText(QFileInfo(oldName).fileName());
                connect(this, SIGNAL(itemChanged(QStandardItem*)),
                        this, SLOT(projectTreeItemChanged(QStandardItem*)));
                emit renameFileFinished(index);
                return;
            }
        }

        if(QFileInfo(newName).completeBaseName().isEmpty()) //重命名为空
        {
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setText(QFileInfo(oldName).fileName());
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            emit renameFileFinished(index);
            return;
        }

        if(!QFile::rename(oldName, newName))
        {
            qInfo() << tr(u8"重命名：重命名文件%0为%1失败").arg(oldName).arg(newName);
            QMessageBox::critical(CAppEnv::m_mainWgt, tr(u8"错误"), tr(u8"重命名失败，请核对文件是否被占用。"));
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setText(QFileInfo(oldName).fileName());
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            emit renameFileFinished(index);
            return;
        }
        else
        {
            disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                       this, SLOT(projectTreeItemChanged(QStandardItem*)));
            if(suffixWrong)
            {
                tempItem->setText(tempItem->text() + ".rpusln");
            }
            solutionItem->setText(QFileInfo(newName).baseName());
            tempItem->setToolTip(newName);
            connect(this, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(projectTreeItemChanged(QStandardItem*)));
            tempItem->setFileName(newName);
            solutionItem->setFileName(newName);
            solutionItem->save();

            QList<QStandardItem *> itemList = takeRow(tempItem->row());
            solutionItem = static_cast<CProjectTreeItem *>(itemList.at(0));
            if(!solutionItem || solutionItem->getItemType() != ITEMSOLUTION)
            {
                emit renameFileFinished(index);
                return;
            }
            int i;
            for(i = 0; i < rowCount(); ++i) //按文件名称排序
            {
                CProjectTreeItem *tempSolutionItem = static_cast<CProjectTreeItem *>(item(i));
                if(QFileInfo(solutionItem->getFileName()).fileName().toLower() <
                        QFileInfo(tempSolutionItem->getFileName()).fileName().toLower())
                {
                    insertRow(i, solutionItem);
                    break;
                }
            }
            if(i == rowCount())
            {
                appendRow(solutionItem);
            }
            index = indexFromItem(solutionItem);

            foreach (CProjectTreeItem *projectItem, getProjectItemList(solutionItem)) {
                CProjectResolver *projectResolver = projectItem->getProjectResolver();
                projectResolver->getProjectParameter()->solutionName = QFileInfo(solutionItem->getFileName()).completeBaseName();
                if(!isActiveProject(projectItem) || !projectResolver->getIsDirty())
                {
                    projectResolver->paraChanged();
                    projectResolver->save();
                }
//                else
//                {
//                    projectResolver->setDirty();
//                    emit projectSettingModified();
//                }
            }
            emit solutionFileRenamed(oldName, newName);  //更新最近打开列表名称
        }
    }
    emit renameFileFinished(index);
}

void CProjectTreeItemModel::renameXml(const QString &oldName, const QString &newName)
{
    CProjectTreeItem *xmlItem = getImpreciseXmlItemByName(oldName);
    while(xmlItem && xmlItem->getItemType() == ITEMXMLFILE)
    {
        CProjectTreeItem *projectItem = getProjectItemFromChild(xmlItem);//已存在相同名称
        Q_ASSERT(projectItem);
        Q_ASSERT(projectItem->getItemType() == ITEMPROJECT);
        bool repeated = false;
        foreach (QString fileName, getXmlList(projectItem)) {
            if(QFileInfo(fileName).completeBaseName() == QFileInfo(newName).completeBaseName())
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"<p>名为“%0”的文件已在项目“%1”中，不能重命名为重复的文件名称，尽管他们处于不同文件夹中！</P>"
                                            "<p>文件“%2”将从项目“%1”中删除。</P>")
                                         .arg(QFileInfo(newName).completeBaseName()).arg(QFileInfo(projectItem->getFileName()).completeBaseName()).arg(oldName));
                repeated = true;
                break;
            }
        }
        if(repeated)
        {
            deleteXml(xmlItem);
            xmlItem = getImpreciseXmlItemByName(oldName);
            continue;
        }

        disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(projectTreeItemChanged(QStandardItem*)));
        xmlItem->setText(QFileInfo(newName).fileName());
        connect(this, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(projectTreeItemChanged(QStandardItem*)));
        xmlItem->setFileName(newName);
        QStandardItem *folderItem = xmlItem->parent();
        QList<QStandardItem *> itemList = folderItem->takeRow(xmlItem->row());
        xmlItem = static_cast<CProjectTreeItem *>(itemList.at(0));
        if(!xmlItem || xmlItem->getItemType() != ITEMXMLFILE)
        {
            xmlItem = getImpreciseXmlItemByName(oldName);
            continue;
        }

        int i;
        for(i = 0; i < folderItem->rowCount(); ++i) //按文件名称排序
        {
            CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(folderItem->child(i));
            if(QFileInfo(xmlItem->getFileName()).fileName().toLower() <
                    QFileInfo(tempItem->getFileName()).fileName().toLower())
            {
                folderItem->insertRow(i, xmlItem);
                break;
            }
        }

        if(i == folderItem->rowCount())
        {
            folderItem->appendRow(xmlItem);
        }

        CProjectResolver *projectResolver = projectItem->getProjectResolver();
        if(!projectResolver || !projectResolver->isStarted())
        {
            qInfo() << tr(u8"重命名：重命名文件时发生错误，未能写入到项目文件");
            QMessageBox::warning(CAppEnv::m_mainWgt, tr(u8"写入错误"), tr(u8"重命名文件时发生错误，未能写入到项目文件！"));
            xmlItem = getImpreciseXmlItemByName(oldName);
            continue;
        }

        projectResolver->changeXmlElement(oldName, newName);
        if(isActiveProject(projectItem))
        {
            emit projectSettingChanged();
        }
        xmlItem = getImpreciseXmlItemByName(oldName);
    }
}

void CProjectTreeItemModel::renameSrc(const QString &oldName, const QString &newName)
{
    CProjectTreeItem *srcItem = getImpreciseSrcItemByName(oldName);
    while(srcItem && srcItem->getItemType() == ITEMSRCFILE)
    {
        CProjectTreeItem *projectItem = getProjectItemFromChild(srcItem);//已存在相同名称
        Q_ASSERT(projectItem);
        Q_ASSERT(projectItem->getItemType() == ITEMPROJECT);
        bool repeated = false;
        foreach (QString fileName, getSrcList(projectItem)) {
            if(QFileInfo(fileName).completeBaseName() == QFileInfo(newName).completeBaseName())
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"<p>名为“%0”的文件已在项目“%1”中，不能重命名为重复的文件名称，尽管他们处于不同文件夹中！</P>"
                                            "<p>文件“%2”将从项目“%1”中删除。</P>")
                                         .arg(QFileInfo(newName).completeBaseName()).arg(QFileInfo(projectItem->getFileName()).completeBaseName()).arg(oldName));
                repeated = true;
                break;
            }
        }
        if(repeated)
        {
            deleteSrc(srcItem);
            srcItem = getImpreciseSrcItemByName(oldName);
            continue;
        }

        disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(projectTreeItemChanged(QStandardItem*)));
        srcItem->setText(QFileInfo(newName).fileName());
        connect(this, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(projectTreeItemChanged(QStandardItem*)));
        srcItem->setFileName(newName);
        QStandardItem *folderItem = srcItem->parent();
        QList<QStandardItem *> itemList = folderItem->takeRow(srcItem->row());
        srcItem = static_cast<CProjectTreeItem *>(itemList.at(0));
        if(!srcItem || srcItem->getItemType() != ITEMSRCFILE)
        {
            srcItem = getImpreciseSrcItemByName(oldName);
            continue;
            return;
        }
        int i;
        for(i = 0; i < folderItem->rowCount(); ++i) //按文件名称排序
        {
            CProjectTreeItem *tempItem = static_cast<CProjectTreeItem *>(folderItem->child(i));
            if(QFileInfo(srcItem->getFileName()).fileName().toLower() <
                    QFileInfo(tempItem->getFileName()).fileName().toLower())
            {
                folderItem->insertRow(i, srcItem);
                break;
            }
        }
        if(i == folderItem->rowCount())
        {
            folderItem->appendRow(srcItem);
        }

        CProjectResolver *projectResolver = projectItem->getProjectResolver();
        if(!projectResolver || !projectResolver->isStarted())
        {
            qInfo() << tr(u8"重命名：重命名文件时发生错误，未能写入到项目文件！");
            QMessageBox::warning(CAppEnv::m_mainWgt, tr(u8"写入错误"), tr(u8"重命名文件时发生错误，未能写入到项目文件!"));
            srcItem = getImpreciseSrcItemByName(oldName);
            continue;
        }

        projectResolver->changeSrcElement(oldName, newName);
        if(isActiveProject(srcItem))
        {
            emit resourceSelectReset();
        }
        emit srcFileRenamed(oldName, newName);

        srcItem = getImpreciseSrcItemByName(oldName);
    }
}

QString CProjectTreeItemModel::getSolutionFileName(const QModelIndex &index) const
{
    if(!index.isValid())
        return QString();
    QStandardItem *stdItem = itemFromIndex(index);
    CSolutionNodeTreeItem *solutionItem = static_cast<CSolutionNodeTreeItem *>(stdItem);
    if(solutionItem && solutionItem->getItemType() == ITEMSOLUTION)
        return solutionItem->getFileName();
    return QString();
}

void CProjectTreeItemModel::renameProject(const QString &oldName, const QString &newName)
{
    CProjectTreeItem *projectItem = getImpreciseProjectItemByName(oldName);
    while(projectItem && projectItem->getItemType() == ITEMPROJECT)
    {
        CProjectTreeItem *solutionItem = getSolutionItemFromChild(projectItem);
        bool repeated = false;
        foreach (QString fileName, getProjectList(solutionItem)) {
            if(QFileInfo(fileName).completeBaseName() == QFileInfo(newName).completeBaseName())
            {
                QMessageBox::information(CAppEnv::m_mainWgt, qApp->applicationName(),
                                         tr(u8"<p>名为“%0”的文件已在项目“%1”中，不能重命名为重复的文件名称，尽管他们处于不同文件夹中！</P>"
                                            "<p>文件“%2”将从项目“%1”中删除。</P>")
                                         .arg(QFileInfo(newName).completeBaseName()).arg(QFileInfo(projectItem->getFileName()).completeBaseName()).arg(oldName));
                repeated = true;
                break;
            }
        }
        if(repeated)
        {
            deleteProject(projectItem);
            projectItem = getImpreciseProjectItemByName(oldName);
            continue;
        }

        disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                this, SLOT(projectTreeItemChanged(QStandardItem*)));
        projectItem->setText(QFileInfo(newName).baseName());
        CProjectTreeItem *projectFileItem = static_cast<CProjectTreeItem *>(projectItem->child(0));
        projectFileItem->setText(QFileInfo(newName).fileName());
        projectFileItem->setToolTip(newName);
        connect(this, SIGNAL(itemChanged(QStandardItem*)),
                   this, SLOT(projectTreeItemChanged(QStandardItem*)));
        projectItem->setFileName(newName);
        for(int i = 0; i < projectItem->rowCount(); ++i)   //更新所有item的path信息
        {
            CProjectTreeItem *childItem = static_cast<CProjectTreeItem *>(projectItem->child(i));
//            if(childItem->getItemType() == ITEMPROJECTFILE || childItem->getItemType() == ITEMXMLFOLDER
//                    || childItem->getItemType() == ITEMSRCFOLDER)
//            {
                childItem->setFileName(newName);
//            }
        }
        solutionItem->renameProject(oldName, newName);

        QList<QStandardItem *> itemList = solutionItem->takeRow(projectItem->row());
        projectItem = static_cast<CProjectTreeItem *>(itemList.at(0));
        if(!projectItem || projectItem->getItemType() != ITEMPROJECT)
        {
            return;
        }
        int i;
        for(i = 1; i < solutionItem->rowCount(); ++i) //按文件名称排序
        {
            CProjectTreeItem *tempProjectItem = static_cast<CProjectTreeItem *>(solutionItem->child(i));
            if(QFileInfo(projectItem->getFileName()).fileName().toLower() <
                    QFileInfo(tempProjectItem->getFileName()).fileName().toLower())
            {
                solutionItem->insertRow(i, projectItem);
                break;
            }
        }
        if(i == solutionItem->rowCount())
        {
            solutionItem->appendRow(projectItem);
        }

        projectItem = getImpreciseProjectItemByName(oldName);
    }
}
