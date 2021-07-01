#ifndef CRESOURCELISTWIDGET_H
#define CRESOURCELISTWIDGET_H

#include <QListWidget>

class CResourceListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CResourceListWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void dropped(QString str);

private:
    void performDrag();

    QPoint m_startPos;
};

#endif // CRESOURCELISTWIDGET_H
