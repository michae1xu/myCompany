#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include "csolutionnodetreeitem.h"
#include "common/cappenv.hpp"

CSolutionNodeTreeItem::CSolutionNodeTreeItem(EItemType itemType, const QString &path, bool isCurPro) :
    CProjectTreeItem(itemType, path, isCurPro)
{
    QFile file(getFileName());
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while(!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            QString absFileName = CAppEnv::relToAbs(getFileName(), line);
            if(QFileInfo(absFileName).exists() && absFileName.endsWith("rpupro", Qt::CaseInsensitive))
            {
                m_projectList.insert(CAppEnv::relToAbs(getFileName(), line));
            }
        }
    }

}

CSolutionNodeTreeItem::~CSolutionNodeTreeItem()
{
    save();
}

void CSolutionNodeTreeItem::lostCurPro()
{
    for(int i = 0; i < rowCount(); ++i)
    {
        CProjectTreeItem *projectFolderItem = static_cast<CProjectTreeItem *>(child(i));
        if(projectFolderItem && projectFolderItem->getItemType() == ITEMPROJECT
                && projectFolderItem->isCurPro())
        {
            projectFolderItem->lostCurPro();
        }
    }
    CProjectTreeItem::lostCurPro();
}

void CSolutionNodeTreeItem::addProject(const QString &fileName)
{
    Q_ASSERT(m_itemType == ITEMSOLUTION);
    m_projectList.insert(fileName);
    save();
}

void CSolutionNodeTreeItem::removeProject(const QString &fileName)
{
    Q_ASSERT(getItemType() == ITEMSOLUTION);
    m_projectList.remove(fileName);
    save();
}

void CSolutionNodeTreeItem::renameProject(const QString &oldName, const QString &newName)
{
    Q_ASSERT(getItemType() == ITEMSOLUTION);
    m_projectList.remove(oldName);
    m_projectList.insert(newName);
    save();
}

QStringList CSolutionNodeTreeItem::getProjectList() const
{
    Q_ASSERT(getItemType() == ITEMSOLUTION);
    QStringList list;
    list = list.fromSet(m_projectList);
    qStableSort(list.begin(), list.end());
    return list;
}

void CSolutionNodeTreeItem::save()
{
    QFile file(getFileName());
    QStringList tempList;
    tempList = tempList.fromSet(m_projectList);

    qStableSort(tempList.begin(), tempList.end());
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        foreach (QString str, tempList) {
            out << CAppEnv::absToRel(getFileName(), str) << endl;
        }
    }
    file.close();
}

