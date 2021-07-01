#include <QApplication>
#include <QMouseEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDrag>
#include "cresourcelistwidget.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "prosetting/cfilesystemtreeview.h"

CResourceListWidget::CResourceListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);
}

void CResourceListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_startPos = event->pos();
    }
    QListWidget::mousePressEvent(event);
}

void CResourceListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - m_startPos).manhattanLength();
        if(distance >= QApplication::startDragDistance())
        {
            performDrag();
        }
    }
    QListWidget::mouseMoveEvent(event);
}

void CResourceListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    CFileSystemTreeView *source = qobject_cast<CFileSystemTreeView *>(event->source());
    if(source)
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void CResourceListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    CFileSystemTreeView *source = qobject_cast<CFileSystemTreeView *>(event->source());
    if(source)
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void CResourceListWidget::dropEvent(QDropEvent *event)
{
    CFileSystemTreeView *source = qobject_cast<CFileSystemTreeView *>(event->source());
    if(source)
    {
        emit dropped(event->mimeData()->text());
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void CResourceListWidget::performDrag()
{
    QListWidgetItem *item = currentItem();
    if(item)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(":/" + item->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(CAppEnv::getImageDirPath() + "/128/puzzle128.png").scaled(20, 20));
        drag->exec(Qt::CopyAction);
    }
}
