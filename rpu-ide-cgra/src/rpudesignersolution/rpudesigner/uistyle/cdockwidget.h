#ifndef CDOCKWIDGET_H
#define CDOCKWIDGET_H

#include <QDockWidget>

class CDockWidget : public QDockWidget
{
public:
    explicit CDockWidget(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event);
};

#endif // CDOCKWIDGET_H
