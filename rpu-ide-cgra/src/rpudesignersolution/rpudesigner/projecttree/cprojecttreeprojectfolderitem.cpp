#include "cprojecttreeprojectfolderitem.h"

CProjectTreeProjectFolderItem::CProjectTreeProjectFolderItem(QString projectName, CProjectResolver *projectResolver, QString filePath, bool isCurPro) :
    CAbstractProjectTreeItem(projectName, projectResolver, filePath, isCurPro)
{
    m_showName = projectName;
    setText(m_showName);
    setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/Briefcase128.png"));
}
