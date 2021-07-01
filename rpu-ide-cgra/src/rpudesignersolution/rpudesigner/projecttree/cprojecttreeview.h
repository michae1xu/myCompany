#ifndef CPROJECTTREEVIEW_H
#define CPROJECTTREEVIEW_H

#include <QTreeView>

class CProjectTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CProjectTreeView(QWidget *parent = nullptr);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void dropProject(QStringList algList, QStringList unSupportList);
};

#endif // CPROJECTTREEVIEW_H
