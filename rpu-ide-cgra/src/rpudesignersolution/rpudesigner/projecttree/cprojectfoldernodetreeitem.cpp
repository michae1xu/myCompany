#include "common/debug.h"
#include "fileresolver/cprojectresolver.h"
#include "cprojectfoldernodetreeitem.h"

CProjectFolderNodeTreeItem::CProjectFolderNodeTreeItem(EItemType itemType, const QString &path, CProjectResolver *projectResolver, bool isCurPro) :
    CProjectTreeItem(itemType, path, isCurPro),
    m_projectResolver(projectResolver)
{

}

CProjectFolderNodeTreeItem::~CProjectFolderNodeTreeItem()
{
    delete m_projectResolver;
}

void CProjectFolderNodeTreeItem::setCurPro()
{
    CProjectTreeItem *item = static_cast<CProjectTreeItem *>(parent());
    if(item)
        item->setCurPro();
    CProjectTreeItem::setCurPro();
}

