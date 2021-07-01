#ifndef CFILESYSTEMTREEVIEW_H
#define CFILESYSTEMTREEVIEW_H

#include <QTreeView>

class CFileSystemTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CFileSystemTreeView(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void performDrag();

    QPoint m_startPos;
};

#endif // CFILESYSTEMTREEVIEW_H
