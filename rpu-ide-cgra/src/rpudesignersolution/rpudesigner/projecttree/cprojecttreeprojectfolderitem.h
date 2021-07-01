#ifndef CPROJECTTREEPROJECTFOLDERITEM_H
#define CPROJECTTREEPROJECTFOLDERITEM_H
#include "cabstractprojecttreeitem.h"

class CProjectResolver;

class CProjectTreeProjectFolderItem : public CAbstractProjectTreeItem
{
public:
    CProjectTreeProjectFolderItem(QString projectName, CProjectResolver *projectResolver, QString path, bool isCurPro = false);

    QString getProjectName() Q_DECL_OVERRIDE {return m_projectName;}
    CProjectResolver * getProjectResolver() Q_DECL_OVERRIDE {return m_projectResolver;}
    bool getIsCurPro() Q_DECL_OVERRIDE {return m_isCurPro;}
    QString getPath() Q_DECL_OVERRIDE {return m_path;}
    void setProjectName(QString projectName) Q_DECL_OVERRIDE {m_projectName = projectName;}
    void setProjectResolver(CProjectResolver *projectResolver) Q_DECL_OVERRIDE {m_projectResolver = projectResolver;}
    void setIsCurPro(bool iscurPro) Q_DECL_OVERRIDE {m_isCurPro = iscurPro;}
    void setPath(QString path) Q_DECL_OVERRIDE {m_path = path;}
};

#endif // CPROJECTTREEPROJECTFOLDERITEM_H
