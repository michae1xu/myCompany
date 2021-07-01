#include <QDragEnterEvent>
#include <QMimeData>
#include "clistview.h"

CListView::CListView(QWidget *parent) :
    QListView(parent)
{
    setAcceptDrops(true);
}

void CListView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
//        event->acceptProposedAction();
        event->accept();
    }

}

void CListView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->setDropAction(Qt::CopyAction);
//        event->acceptProposedAction();
        event->accept();
    }
}

void CListView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QStringList xmlList, unSupportList;
        QList<QUrl> urlList = mimeData->urls();
        for(int i = 0; i < urlList.count(); ++i)
        {
            QString fileName = urlList.at(i).toLocalFile();
            if(fileName.endsWith("xml", Qt::CaseInsensitive))
            {
                xmlList.append(fileName);
            }
            else
            {
                unSupportList.append(fileName);
            }
        }
        emit dropXml(xmlList, unSupportList);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}
