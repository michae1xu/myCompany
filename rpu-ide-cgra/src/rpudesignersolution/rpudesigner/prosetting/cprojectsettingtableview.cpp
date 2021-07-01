#include <QDragEnterEvent>
#include <QMimeData>
#include <QHeaderView>
#include "cprojectsettingtableview.h"
#include "datafile.h"
#include "prosetting/cfilesystemtreeview.h"
#include "prosetting/cresourcelistwidget.h"

CProjectSettingTableView::CProjectSettingTableView(QWidget *parent) :
    QTableView(parent)
{
    setAcceptDrops(true);
//    setDragDropOverwriteMode(false);
}

void CProjectSettingTableView::dragEnterEvent(QDragEnterEvent *event)
{
    CResourceListWidget *source1 = qobject_cast<CResourceListWidget *>(event->source());
    CFileSystemTreeView *source2 = qobject_cast<CFileSystemTreeView *>(event->source());
    if(source1 || source2)
    {
        QModelIndex index = indexAt(event->pos());
        if(index.isValid())
        {
            int column = index.column();
            if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
            {
                event->setDropAction(Qt::IgnoreAction);
            }
            else
            {
                event->setDropAction(Qt::CopyAction);
            }
            event->accept();
            return QTableView::dragEnterEvent(event);
        }
    }
//    event->setDropAction(Qt::IgnoreAction);
    event->accept();
    QTableView::dragEnterEvent(event);
}

void CProjectSettingTableView::dragMoveEvent(QDragMoveEvent *event)
{
    CResourceListWidget *source1 = qobject_cast<CResourceListWidget *>(event->source());
    CFileSystemTreeView *source2 = qobject_cast<CFileSystemTreeView *>(event->source());
    if(source1 || source2)
    {
        QModelIndex index = indexAt(event->pos());
        if(index.isValid())
        {
            int column = index.column();
            if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
            {
                event->ignore();
                event->setDropAction(Qt::IgnoreAction);
            }
            else
            {
                event->accept();
                event->setDropAction(Qt::CopyAction);
            }
            return QTableView::dragMoveEvent(event);
        }
    }
//    event->setDropAction(Qt::IgnoreAction);
//    event->ignore();
    QTableView::dragMoveEvent(event);
}

void CProjectSettingTableView::dropEvent(QDropEvent *event)
{
    CResourceListWidget *source1 = qobject_cast<CResourceListWidget *>(event->source());
    CFileSystemTreeView *source2 = qobject_cast<CFileSystemTreeView *>(event->source());
    if(!source1 && !source2)
    {
        event->ignore();
        return QTableView::dropEvent(event);
    }

    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
    {
        event->ignore();
        return QTableView::dropEvent(event);
    }

    int column = index.column();
    if(column == ProjectSettingXmlName || column > ProjectSettingInMemoryPath4)
    {
        event->ignore();
        return QTableView::dropEvent(event);
    }

    this->model()->setData(index, event->mimeData()->text(), Qt::EditRole);
    event->setDropAction(Qt::CopyAction);
    event->accept();
}

void CProjectSettingTableView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        if(currentIndex().isValid())
        {
            model()->setData(currentIndex(), "", Qt::EditRole);
        }
        return;
    }
    QTableView::keyPressEvent(event);
}

