#ifndef CPROJECTFOLDERNODETREEITEM_H
#define CPROJECTFOLDERNODETREEITEM_H

#include <projecttree/cprojecttreeitem.h>

class CProjectResolver;

class CProjectFolderNodeTreeItem : public CProjectTreeItem
{
public:
    CProjectFolderNodeTreeItem(EItemType itemType, const QString &path, CProjectResolver *projectResolver = 0, bool isCurPro = false);
    ~CProjectFolderNodeTreeItem();

    CProjectResolver* getProjectResolver() const {return m_projectResolver;}
    void setCurPro();

private:
    CProjectResolver *m_projectResolver;

};

#endif // CPROJECTFOLDERNODETREEITEM_H
