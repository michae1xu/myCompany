#include "cprojectsettingwidget.h"
#include <QKeyEvent>

CProjectSettingWidget::CProjectSettingWidget(QWidget *parent) : QWidget(parent)
{

}

void CProjectSettingWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::CTRL && event->key() == Qt::Key_S)
    {
        emit save();
        return;
    }
    QWidget::keyPressEvent(event);
}
