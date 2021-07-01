#ifndef CPROJECTSETTINGTABLEVIEW_H
#define CPROJECTSETTINGTABLEVIEW_H

#include <QTableView>

class CProjectSettingTableView : public QTableView
{
    Q_OBJECT
public:
    explicit CProjectSettingTableView(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // CPROJECTSETTINGTABLEVIEW_H
