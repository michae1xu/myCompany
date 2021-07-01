#include <QDragEnterEvent>
#include <QMimeData>
#include "cprojecttreeview.h"

CProjectTreeView::CProjectTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
}

void CProjectTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
//        event->acceptProposedAction();
        event->accept();
    }
}

void CProjectTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
//        event->acceptProposedAction();
        event->accept();
    }
}

void CProjectTreeView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QStringList projectList, unSupportList;
        QList<QUrl> urlList = mimeData->urls();
        for(int i = 0; i < urlList.count(); ++i)
        {
            QString fileName = urlList.at(i).toLocalFile();
            if(fileName.endsWith("rpusln", Qt::CaseInsensitive))
            {
                projectList.append(fileName);
            }
            else
            {
                unSupportList.append(fileName);
            }
        }
        emit dropProject(projectList, unSupportList);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}
