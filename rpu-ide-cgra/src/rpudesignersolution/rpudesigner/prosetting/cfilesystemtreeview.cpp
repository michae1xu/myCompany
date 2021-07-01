#include <QApplication>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QFileSystemModel>
#include "cfilesystemtreeview.h"
#include "common/cappenv.hpp"

CFileSystemTreeView::CFileSystemTreeView(QWidget *parent) :
    QTreeView(parent)
{

}

void CFileSystemTreeView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_startPos = event->pos();
    }
    QTreeView::mousePressEvent(event);
}

void CFileSystemTreeView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - m_startPos).manhattanLength();
        if(distance >= QApplication::startDragDistance())
        {
            performDrag();
        }
    }
    QTreeView::mouseMoveEvent(event);
}

void CFileSystemTreeView::performDrag()
{
    QModelIndex index = indexAt(m_startPos);
    if(!index.isValid())
    {
        return;
    }
    QFileSystemModel *model = static_cast<QFileSystemModel*>(this->model());
    if(!model)
        return;
    QFileInfo fileInfo(model->fileInfo(index));
    if(!fileInfo.isFile())
        return;

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(fileInfo.absoluteFilePath());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(QPixmap(CAppEnv::getImageDirPath() + "/128/Complete-file128.png").scaled(20, 20));
    drag->exec(Qt::CopyAction);
}
