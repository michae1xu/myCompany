#include <QApplication>
#include <QMouseEvent>
#include "cwidget.h"

CWidget::CWidget(QWidget *parent) : QWidget(parent)
{

}

void CWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    qApp->setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    m_moveFlag = true;
    m_mainPos = this->pos();
    m_mousePos = mouseEvent->globalPos();
}

void CWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    m_moveFlag = false;
    qApp->restoreOverrideCursor();
}

void CWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    QPoint pos = mouseEvent->globalPos();

    if (m_moveFlag)
    {
        int x = pos.x() - m_mousePos.x();
        int y = pos.y() - m_mousePos.y();
        this->move(m_mainPos.x() + x ,m_mainPos.y() + y);
    }
}
