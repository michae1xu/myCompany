#ifndef CPROJECTTREEITEM_H
#define CPROJECTTREEITEM_H

#include <QCoreApplication>
#include <QStandardItem>
#include "datafile.h"

class CProjectResolver;

class CProjectTreeItem : public QStandardItem
{
    Q_DECLARE_TR_FUNCTIONS(CProjectTreeItem)

public:
    CProjectTreeItem(EItemType itemType, const QString &path, bool isCurPro = false);
    ~CProjectTreeItem();

    virtual CProjectResolver* getProjectResolver() const {return 0;}
    virtual void setCurPro();
    virtual void lostCurPro();
    virtual void addProject(const QString &fileName);
    virtual void removeProject(const QString &fileName);
    virtual void renameProject(const QString &oldName, const QString &newName);
    virtual void save();
    virtual QStringList getProjectList() const;
    bool isCurPro() const;
    void setFileName(const QString &str);
    QString getFileName() const;
    EItemType getItemType() const;
    CProjectTreeItem *clone() const;

protected:
    EItemType m_itemType;
    QString m_fileName;
    bool m_isCurPro = false;

};

#endif // CPROJECTTREEITEM_H
