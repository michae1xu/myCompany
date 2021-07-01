#include <QEvent>
#include "cdockwidget.h"

CDockWidget::CDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
}

void CDockWidget::changeEvent(QEvent *event)
{
    if(isActiveWindow())
        setWindowOpacity(1);
    else
        setWindowOpacity(0.5);
    QDockWidget::changeEvent(event);
}
