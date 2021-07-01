#ifndef CLISTVIEW_H
#define CLISTVIEW_H

#include <QListView>

class CListView : public QListView
{
    Q_OBJECT
public:
    explicit CListView(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void dropXml(QStringList xmlList, QStringList unSupportList);
};

#endif // CLISTVIEW_H
