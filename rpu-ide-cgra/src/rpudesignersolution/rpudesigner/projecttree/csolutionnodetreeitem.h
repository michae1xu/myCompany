#ifndef CSOLUTIONNODETREEITEM_H
#define CSOLUTIONNODETREEITEM_H

#include <QSet>
#include <projecttree/cprojecttreeitem.h>

class CSolutionNodeTreeItem : public CProjectTreeItem
{
public:
    CSolutionNodeTreeItem(EItemType itemType, const QString &path, bool isCurPro = false);
    ~CSolutionNodeTreeItem();
    void lostCurPro();
    void addProject(const QString &fileName);
    void removeProject(const QString &fileName);
    void renameProject(const QString &oldName, const QString &newName);
    void save();
    QStringList getProjectList() const;

private:
    QSet<QString> m_projectList;

};

#endif // CSOLUTIONNODETREEITEM_H
