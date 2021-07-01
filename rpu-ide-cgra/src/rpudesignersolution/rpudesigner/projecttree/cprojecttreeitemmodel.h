#ifndef CPROJECTTREEITEMMODEL_H
#define CPROJECTTREEITEMMODEL_H

#include <QStandardItemModel>
#include "datafile.h"

class CProjectResolver;
class CProjectTreeItem;

class CProjectTreeItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit CProjectTreeItemModel(QObject *parent = nullptr);
    ~CProjectTreeItemModel();

    bool appendSolution(const QString &fileName);
    void closeSolution(const QModelIndex &index);
//    void renameSolution();

    bool appendProject(const QString &fileName, CProjectTreeItem *parent, const ECipherType &cipherType = CIPHER_TYPE_NONE);
    bool appendProject(const QString &fileName, const QModelIndex &index);
    bool addProject(const QString &fileName, const ECipherType &cipherType, const QModelIndex &index);
    void deleteProject(const QModelIndex &index);
    void deleteProject(CProjectTreeItem *projectItem);
    void deleteProject(const QString &projectName);
    void renameProject(const QString &oldName, const QString &newName);

    bool appendXml(const QString &fileName, CProjectTreeItem *parent);
    void removeXml(const QString &fileName, CProjectTreeItem *parent);
    bool addXml(const QString &fileName, const QModelIndex &index);
    void deleteXml(const QModelIndex &index);
    void deleteXml(const QString &fileName);
    void deleteXml(CProjectTreeItem *xmlItem);
    bool appendSrc(const QString &fileName, CProjectTreeItem *parent);
    void removeSrc(const QString &fileName, CProjectTreeItem *parent);
    bool addSrc(const QString &fileName, const QModelIndex &index);
    bool activeProjectAddSrc(const QString &fileName);
    void deleteSrc(const QModelIndex &index);
    void deleteSrc(const QString &fileName);
    void deleteSrc(CProjectTreeItem *srcItem);
    void activeProjectDeleteSrc(const QString &fileName);
    void renameXml(const QString &oldName, const QString &newName);
    void renameSrc(const QString &oldName, const QString &newName);

    QString getSolutionFileName(const QModelIndex &index) const;
    QList<CProjectResolver *> getAllProjectResolver() const;
    CProjectResolver* getProjectResolver(const QModelIndex &index) const; //just for ITEMPROJECT or his child
    CProjectResolver* getNearestProjectResolverFromXml(const QString &xmlFileName) const;      //查找xml文件所属的项目解析器，可能存在多个结果，返回第一个符合条件的
    QString getProjectFileName(const QModelIndex &index) const;           //just for ITEMPROJECT or his child
    QString getNearestProjectFileNameFromSrc(const QString &fileName) const;
    void setActiveProject(const QString &solutionName, const QString &projectName);           //just for ITEMPROJECT
    void setActiveProject(const QModelIndex &index);                //just for ITEMPROJECT
    void setActiveProject(CProjectTreeItem *projectItem);           //just for ITEMPROJECT
//    void setActiveSolution(const QModelIndex &index);             //just for ITEMSOLUTION
    QString getActiveSolutionName() const;
    CProjectResolver* getActiveProjectResolver() const;//just for ITEMPROJECT or his child
    QString getActiveProjectFileName() const;
    bool isActiveProject(const QModelIndex &index) const;
    bool isActiveProject(CProjectTreeItem *item) const;       //just for ITEMPROJECT or his child
    bool isActiveSolution(const QModelIndex &index) const;
    QString getItemFileName(const QModelIndex &index) const;
    EItemType getItemType(const QModelIndex &index) const;
    bool hasProject(const QModelIndex &index) const;
    bool hasActiveSolution() const;
    QStringList getActiveXmlList() const;
    QStringList getAllXmlList() const;
    QStringList getActiveSrcList() const;
    QStringList getAllSrcList() const;
    QStringList getSolutionList() const;
    QStringList getProjectList(const CProjectTreeItem *solutionItem) const;
    QStringList getProjectList(const QModelIndex &index) const;
    QStringList getAllProjectList() const;
    QStringList getXmlList(const CProjectTreeItem *projectItem) const;
    QStringList getSrcList(const CProjectTreeItem *projectItem) const;


private:
    CProjectTreeItem *getActiveSolutionItem() const;
    CProjectTreeItem *getSolutionItemByName(const QString &solutionName) const;
    CProjectTreeItem *getSolutionItemFromChild(QStandardItem *item) const;

    CProjectTreeItem *getActiveProjectItem() const;
    CProjectTreeItem *getProjectItemByName(const QString &solutionName, const QString &projectName) const;
    CProjectTreeItem *getProjectItemFromChild(QStandardItem *item) const;
    CProjectTreeItem *getImpreciseProjectItemByName(const QString &projectFileName) const;
    QList<CProjectTreeItem *> getProjectItemList(CProjectTreeItem *solutionItem) const;

    CProjectTreeItem *getActiveProjectFileItem() const;
    CProjectTreeItem *getProjectFileItemByName(const QString &solutionName, const QString &projectName) const;
    CProjectTreeItem *getProjectFileItemFromNeighborChild(QStandardItem *item) const;

    CProjectTreeItem *getActiveXmlFolderItem() const;
    CProjectTreeItem *getXmlFolderItemByName(const QString &solutionName, const QString &projectName) const;
    CProjectTreeItem *getXmlFolderItemFromProject(const QStandardItem *item) const;
    CProjectTreeItem *getXmlFolderItemFromChild(const QStandardItem *item) const;

    CProjectTreeItem *getActiveSrcFolderItem() const;
    CProjectTreeItem *getSrcFolderItemByName(const QString &solutionName, const QString &projectName) const;
    CProjectTreeItem *getSrcFolderItemFromProject(const QStandardItem *item) const;
    CProjectTreeItem *getSrcFolderItemFromChild(const QStandardItem *item) const;

    CProjectTreeItem *getActiveXmlItem(const QString &fileName) const;
    CProjectTreeItem *getXmlItemByName(const QString &solutionName, const QString &projectName, const QString &fileName) const;
    CProjectTreeItem *getImpreciseXmlItemByName(const QString &xmlFileName) const;

    CProjectTreeItem *getActiveSrcItem(const QString &fileName) const;
    CProjectTreeItem *getSrcItemByName(const QString &solutionName, const QString &projectName, const QString &fileName) const;
    CProjectTreeItem *getImpreciseSrcItemByName(const QString &srcFileName) const;

signals:
    void setProjectResolverForXml(QString, CProjectResolver *);
    void projectSettingSaved(QString);
    void projectSettingReset();
    void projectSettingChanged();
//    void projectSettingModified();
    void resourceSelectReset();
    void activeProjectChanged();
    void xmlFileRenamed(QString oldName, QString newName);
    void srcFileRenamed(QString oldName, QString newName);
    void projectFileRenamed(QString oldName, QString newName);
    void solutionFileRenamed(QString oldName, QString newName);
    void renameFileFinished(QModelIndex index);


private slots:
    void projectTreeItemChanged(QStandardItem *stdItem);
};

#endif // CPROJECTTREEITEMMODEL_H
