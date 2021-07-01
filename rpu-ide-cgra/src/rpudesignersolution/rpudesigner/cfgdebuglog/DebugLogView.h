#ifndef DEBUGLOGVIEW_H
#define DEBUGLOGVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class DebugLogView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DebugLogView(QWidget *parent = 0);
    ~DebugLogView();

signals:
    void debugLogViewClose();

protected:
    void wheelEvent(QWheelEvent *event);
    void closeEvent(QCloseEvent *);
};

#endif // DEBUGLOGVIEW_H
