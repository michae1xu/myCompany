#include <QKeyEvent>
#include "clineedit.h"

CLineEdit::CLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_cur(-1)
{

}

void CLineEdit::appendCmd(const QString &cmd)
{
    m_historyCmd.insert(0, cmd);
}

void CLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Up)
    {
        if(m_historyCmd.count() > m_cur + 1)
        {
            ++m_cur;
            setText(m_historyCmd.at(m_cur));
        }
        return;
    }
    else if(event->key() == Qt::Key_Down)
    {
        if(m_historyCmd.count() > m_cur - 1 && m_cur > 0)
        {
            --m_cur;
            setText(m_historyCmd.at(m_cur));
        }
        else if(m_cur == 0)
        {
            clear();
        }
        return;
    }
    m_cur = -1;
    QLineEdit::keyReleaseEvent(event);
}
